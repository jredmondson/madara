
#ifndef _MADARA_NO_KARL_


#include "madara/expression/Leaf_Node.h"
#include "madara/expression/System_Call_To_Integer.h"
#include "madara/knowledge/Thread_Safe_Context.h"
#include "madara/expression/Visitor.h"


madara::expression::System_Call_To_Integer::System_Call_To_Integer (
  madara::knowledge::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
madara::expression::System_Call_To_Integer::~System_Call_To_Integer (void)
{
}

madara::Knowledge_Record
madara::expression::System_Call_To_Integer::item (void) const
{
  return madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression::System_Call_To_Integer::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  madara::Knowledge_Record result;

  if (nodes_.size () > 0)
  {
    bool arg_can_change = false;
    result = nodes_[0]->prune (arg_can_change);
    
    if (!arg_can_change && dynamic_cast <Leaf_Node *> (nodes_[0]) == 0)
    {
      delete nodes_[0];
      nodes_[0] = new Leaf_Node (*(this->logger_), result);
    }
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression::System_Call_To_Integer::evaluate (
const madara::knowledge::Knowledge_Update_Settings & settings)
{
  if (nodes_.size () > 0)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call to_integer is converting an argument\n");

    return nodes_[0]->evaluate (settings).to_integer ();
  }
  else
  {
    Knowledge_Record return_value;

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call to_integer is converting 0\n");
    
    return return_value.to_integer ();
  }
}

// accept a visitor
void 
madara::expression::System_Call_To_Integer::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
