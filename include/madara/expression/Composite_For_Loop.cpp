/* -*- C++ -*- */
#ifndef _FOR_LOOP_CPP_
#define _FOR_LOOP_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/Component_Node.h"
#include "madara/expression/Composite_Unary_Node.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/Composite_For_Loop.h"
#include "madara/expression/Leaf_Node.h"
#include "madara/expression/Composite_Assignment_Node.h"



// Ctor

madara::expression::Composite_For_Loop::Composite_For_Loop (
       Component_Node * precondition,
        Component_Node * condition, 
        Component_Node * postcondition,
        Component_Node * body,
        madara::knowledge::Thread_Safe_Context & context)
: Component_Node (context.get_logger ()), context_(context),
    precondition_ (precondition), condition_ (condition), 
    postcondition_ (postcondition), body_ (body)
{
  
}

// Dtor
madara::expression::Composite_For_Loop::~Composite_For_Loop (void)
{
}

madara::Knowledge_Record
madara::expression::Composite_For_Loop::item (void) const
{
  madara::Knowledge_Record record;
  record.set_value ("for (;;)");
  return record;
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression::Composite_For_Loop::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  madara::Knowledge_Record zero;
  return zero;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression::Composite_For_Loop::evaluate (
const madara::knowledge::Knowledge_Update_Settings & settings)
{
  madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
    "Composite_For_Loop::evaluate: Executing precondition\n");

  precondition_->evaluate (settings);

  madara::Knowledge_Record::Integer count = 0;
  while (condition_->evaluate (settings).is_true ())
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "Composite_For_Loop::evaluate: Executing loop body\n");

    body_->evaluate (settings);

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "Composite_For_Loop::evaluate: Executing postcondition\n");

    postcondition_->evaluate (settings);
    ++count;
  }

  // return is the number of successful body executions
  
  madara::Knowledge_Record evaluations;
  evaluations.set_value (count);
  return evaluations;
}

// accept a visitor
void 
madara::expression::Composite_For_Loop::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _FOR_LOOP_CPP_ */
