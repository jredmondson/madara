
#ifndef _MADARA_NO_KARL_


#include "madara/expression/Leaf_Node.h"
#include "madara/expression/System_Call_Get_Time.h"
#include "madara/knowledge/Thread_Safe_Context.h"
#include "madara/expression/Visitor.h"
#include "madara/utility/Utility.h"


madara::expression::System_Call_Get_Time::System_Call_Get_Time (
  madara::knowledge::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
madara::expression::System_Call_Get_Time::~System_Call_Get_Time (void)
{
}

madara::Knowledge_Record
madara::expression::System_Call_Get_Time::item (void) const
{
  return madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression::System_Call_Get_Time::prune (bool & can_change)
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
madara::expression::System_Call_Get_Time::evaluate (
const madara::knowledge::Knowledge_Update_Settings & settings)
{
  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
    "System call get_time is returning the system clock\n");

  return madara::Knowledge_Record::Integer (
    madara::utility::get_time ());
}

// accept a visitor
void 
madara::expression::System_Call_Get_Time::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
