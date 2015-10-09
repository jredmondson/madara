
#ifndef _MADARA_NO_KARL_


#include "madara/expression/LeafNode.h"
#include "madara/expression/VariableNode.h"
#include "madara/expression/SystemCallSleep.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"
#include "madara/utility/Utility.h"


madara::expression::SystemCallSleep::SystemCallSleep (
  madara::knowledge::ThreadSafeContext & context,
  const ComponentNodes & nodes)
  : SystemCallNode (context, nodes)
{

}

// Dtor
madara::expression::SystemCallSleep::~SystemCallSleep (void)
{
}

madara::KnowledgeRecord
madara::expression::SystemCallSleep::item (void) const
{
  return madara::KnowledgeRecord::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::KnowledgeRecord
madara::expression::SystemCallSleep::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  madara::KnowledgeRecord result;

  for (ComponentNodes::iterator i = nodes_.begin (); i != nodes_.end ();
       ++i)
  {
    bool arg_can_change = false;
    result = (*i)->prune (arg_can_change);
    
    if (!arg_can_change && dynamic_cast <LeafNode *> (*i) == 0)
    {
      delete *i;
      *i = new LeafNode (*(this->logger_), result);
    }
  }

  if (nodes_.size () > 2 || nodes_.size () == 0)
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: System call set_clock requires 1-2 arguments, "
      "e.g., set_clock (5) or set_clock (var, 5)\n");
  }


  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::KnowledgeRecord 
madara::expression::SystemCallSleep::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  KnowledgeRecord return_value;

  if (nodes_.size () == 1)
  {
    double sleep_time = nodes_[0]->evaluate (settings).to_double ();
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call sleep is sleeping for %f.\n", sleep_time);

    return_value = madara::utility::sleep (sleep_time);
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL RUNTIME ERROR: System call set_clock requires 1-2 arguments, "
      "e.g., set_clock (5) or set_clock (var, 5).\n");
  }

  return return_value;
}

// accept a visitor
void 
madara::expression::SystemCallSleep::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
