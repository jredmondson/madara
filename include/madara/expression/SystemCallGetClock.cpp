
#ifndef _MADARA_NO_KARL_


#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallGetClock.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"


madara::expression::SystemCallGetClock::SystemCallGetClock (
  madara::knowledge::ThreadSafeContext & context,
  const ComponentNodes & nodes)
  : SystemCallNode (context, nodes)
{

}

// Dtor
madara::expression::SystemCallGetClock::~SystemCallGetClock (void)
{
}

madara::KnowledgeRecord
madara::expression::SystemCallGetClock::item (void) const
{
  return madara::KnowledgeRecord::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::KnowledgeRecord
madara::expression::SystemCallGetClock::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  madara::KnowledgeRecord result;

  if (nodes_.size () > 0)
  {
    bool arg_can_change = false;
    result = nodes_[0]->prune (arg_can_change);
    
    if (!arg_can_change && dynamic_cast <LeafNode *> (nodes_[0]) == 0)
    {
      delete nodes_[0];
      nodes_[0] = new LeafNode (*(this->logger_), result);
    }
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::KnowledgeRecord 
madara::expression::SystemCallGetClock::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  KnowledgeRecord return_value;

  if (nodes_.size () > 0)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call get_clock is returning the clock of its first argument\n");

    return madara::KnowledgeRecord::Integer (
      nodes_[0]->evaluate (settings).clock);
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call get_clock is returning the system clock\n");

    return madara::KnowledgeRecord::Integer (
      context_.get_clock ());
  }

  return return_value;
}

// accept a visitor
void 
madara::expression::SystemCallGetClock::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
