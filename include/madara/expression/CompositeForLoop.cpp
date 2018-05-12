/* -*- C++ -*- */
#ifndef _FOR_LOOP_CPP_
#define _FOR_LOOP_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/CompositeUnaryNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/CompositeForLoop.h"
#include "madara/expression/LeafNode.h"
#include "madara/expression/CompositeAssignmentNode.h"



// Ctor

madara::expression::CompositeForLoop::CompositeForLoop (
       ComponentNode * precondition,
        ComponentNode * condition, 
        ComponentNode * postcondition,
        ComponentNode * body,
        madara::knowledge::ThreadSafeContext & context)
: ComponentNode (context.get_logger ()),
    precondition_ (precondition), condition_ (condition), 
    postcondition_ (postcondition), body_ (body)
{
  
}

// Dtor
madara::expression::CompositeForLoop::~CompositeForLoop (void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::CompositeForLoop::item (void) const
{
  madara::knowledge::KnowledgeRecord record;
  record.set_value ("for (;;)");
  return record;
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::CompositeForLoop::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  madara::knowledge::KnowledgeRecord zero;
  return zero;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::CompositeForLoop::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
    "CompositeForLoop::evaluate: Executing precondition\n");

  precondition_->evaluate (settings);

  madara::knowledge::KnowledgeRecord::Integer count = 0;
  while (condition_->evaluate (settings).is_true ())
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "CompositeForLoop::evaluate: Executing loop body\n");

    body_->evaluate (settings);

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "CompositeForLoop::evaluate: Executing postcondition\n");

    postcondition_->evaluate (settings);
    ++count;
  }

  // return is the number of successful body executions
  
  madara::knowledge::KnowledgeRecord evaluations;
  evaluations.set_value (count);
  return evaluations;
}

// accept a visitor
void 
madara::expression::CompositeForLoop::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _FOR_LOOP_CPP_ */
