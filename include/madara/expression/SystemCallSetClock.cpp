
#ifndef _MADARA_NO_KARL_


#include "madara/expression/LeafNode.h"
#include "madara/expression/VariableNode.h"
#include "madara/expression/SystemCallSetClock.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"


madara::expression::SystemCallSetClock::SystemCallSetClock (
  madara::knowledge::ThreadSafeContext & context,
  const ComponentNodes & nodes)
  : SystemCallNode (context, nodes)
{

}

// Dtor
madara::expression::SystemCallSetClock::~SystemCallSetClock (void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallSetClock::item (void) const
{
  return madara::knowledge::KnowledgeRecord (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallSetClock::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  madara::knowledge::KnowledgeRecord result;

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
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "madara::expression::SystemCallSetClock: "
      "KARL COMPILE ERROR: System call set_clock requires 1-2 arguments, "
      "e.g., set_clock (5) or set_clock (var, 5)\n");
  }


  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::SystemCallSetClock::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  knowledge::KnowledgeRecord return_value;

  if (nodes_.size () == 1)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "madara::expression::SystemCallSetClock: "
      "System call set_clock is setting the system clock\n");

    context_.set_clock (nodes_[0]->evaluate (settings).to_integer ());
    
    return madara::knowledge::KnowledgeRecord (context_.get_clock ());
  }
  else if (nodes_.size () == 2)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "madara::expression::SystemCallSetClock: "
      "System call set_clock is setting a variable clock\n");

    VariableNode * variable = dynamic_cast <VariableNode *> (nodes_[0]);
    if (variable)
    {
      variable->get_record ()->clock = 
        (uint64_t) nodes_[1]->evaluate (settings).to_integer ();
      
      return madara::knowledge::KnowledgeRecord (
        variable->get_record ()->clock);
    }
    else
    {
      madara_logger_ptr_log (logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallSetClock: "
        "KARL RUNTIME ERROR:"
        "System call set_clock with 2 arguments "
        "requires the first argument to be a variable\n");

      throw KarlException ("madara::expression::SystemCallSetClock: "
        "KARL RUNTIME ERROR: "
        "System call set_clock with 2 arguments "
        "requires the first argument to be a variable\n");
    }

  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "madara::expression::SystemCallSetClock: "
      "KARL RUNTIME ERROR:"
      "System call set_clock requires 1-2 arguments, "
      "e.g., set_clock (5) or set_clock (var, 5)\n");

    throw KarlException ("madara::expression::SystemCallSetClock: "
      "KARL RUNTIME ERROR: "
      "System call set_clock requires 1-2 arguments, "
      "e.g., set_clock (5) or set_clock (var, 5)\n");
  }

  return return_value;
}

// accept a visitor
void 
madara::expression::SystemCallSetClock::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
