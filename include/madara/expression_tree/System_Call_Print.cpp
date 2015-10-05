
#ifndef _MADARA_NO_KARL_


#include "madara/expression_tree/Leaf_Node.h"
#include "madara/expression_tree/System_Call_Print.h"
#include "madara/expression_tree/Visitor.h"
#include "madara/knowledge/Thread_Safe_Context.h"


madara::expression_tree::System_Call_Print::System_Call_Print (
  madara::knowledge::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
madara::expression_tree::System_Call_Print::~System_Call_Print (void)
{
}

madara::Knowledge_Record
madara::expression_tree::System_Call_Print::item (void) const
{
  return madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression_tree::System_Call_Print::prune (bool & can_change)
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
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: System call size requires an argument\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression_tree::System_Call_Print::evaluate (
const madara::knowledge::Knowledge_Update_Settings & settings)
{
  Knowledge_Record return_value;

  if (nodes_.size () == 1)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call print is printing the first argument and returning the"
      " size of the first argument\n");
    
    std::string statement (nodes_[0]->evaluate (settings).to_string ());

    context_.print (statement, 0);

    return_value = Knowledge_Record::Integer (0);
  }
  else if (nodes_.size () == 2)
  {
    std::string statement (nodes_[0]->evaluate (settings).to_string ());
    unsigned int log_level = 
      (unsigned int) nodes_[1]->evaluate (settings).to_integer ();

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call print is printing the first argument at log level %d.\n",
      log_level);
    
    context_.print (statement, log_level);

    return_value = Knowledge_Record::Integer (0);
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL RUNTIME ERROR: System call print requires an argument\n");
  }

  return return_value;
}

// accept a visitor
void 
madara::expression_tree::System_Call_Print::accept (
  madara::expression_tree::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
