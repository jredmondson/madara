
#ifndef _MADARA_NO_KARL_


#include "madara/expression_tree/Leaf_Node.h"
#include "madara/expression_tree/Variable_Node.h"
#include "madara/expression_tree/System_Call_Sleep.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/expression_tree/Visitor.h"
#include "madara/utility/Utility.h"


Madara::Expression_Tree::System_Call_Sleep::System_Call_Sleep (
  Madara::Knowledge_Engine::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
Madara::Expression_Tree::System_Call_Sleep::~System_Call_Sleep (void)
{
}

Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_Sleep::item (void) const
{
  return Madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_Sleep::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  Madara::Knowledge_Record result;

  for (Component_Nodes::iterator i = nodes_.begin (); i != nodes_.end ();
       ++i)
  {
    bool arg_can_change = false;
    result = (*i)->prune (arg_can_change);
    
    if (!arg_can_change && dynamic_cast <Leaf_Node *> (*i) == 0)
    {
      delete *i;
      *i = new Leaf_Node (*(this->logger_), result);
    }
  }

  if (nodes_.size () > 2 || nodes_.size () == 0)
  {
    madara_logger_ptr_log (logger_, Logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: System call set_clock requires 1-2 arguments, "
      "e.g., set_clock (5) or set_clock (var, 5)\n");
  }


  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
Madara::Knowledge_Record 
Madara::Expression_Tree::System_Call_Sleep::evaluate (
const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  Knowledge_Record return_value;

  if (nodes_.size () == 1)
  {
    double sleep_time = nodes_[0]->evaluate (settings).to_double ();
    madara_logger_ptr_log (logger_, Logger::LOG_MINOR,
      "System call sleep is sleeping for %f.\n", sleep_time);

    return_value = Madara::Utility::sleep (sleep_time);
  }
  else
  {
    madara_logger_ptr_log (logger_, Logger::LOG_EMERGENCY,
      "KARL RUNTIME ERROR: System call set_clock requires 1-2 arguments, "
      "e.g., set_clock (5) or set_clock (var, 5).\n");
  }

  return return_value;
}

// accept a visitor
void 
Madara::Expression_Tree::System_Call_Sleep::accept (
  Madara::Expression_Tree::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
