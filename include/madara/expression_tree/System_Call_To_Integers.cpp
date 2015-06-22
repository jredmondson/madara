
#ifndef _MADARA_NO_KARL_


#include "madara/expression_tree/Leaf_Node.h"
#include "madara/expression_tree/System_Call_To_Integers.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/expression_tree/Visitor.h"


Madara::Expression_Tree::System_Call_To_Integers::System_Call_To_Integers (
  Madara::Knowledge_Engine::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
Madara::Expression_Tree::System_Call_To_Integers::~System_Call_To_Integers (void)
{
}

Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_To_Integers::item (void) const
{
  return Madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_To_Integers::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  Madara::Knowledge_Record result;

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
Madara::Knowledge_Record 
Madara::Expression_Tree::System_Call_To_Integers::evaluate (
const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  if (nodes_.size () > 0)
  {
    logger_->log (Logger::LOG_MINOR,
      "System call to_integers is converting an argument\n");

    return nodes_[0]->evaluate (settings).to_integers ();
  }
  else
  {
    Knowledge_Record return_value;

    logger_->log (Logger::LOG_MINOR,
      "System call to_integers is converting 0\n");
    
    return return_value.to_integers ();
  }
}

// accept a visitor
void 
Madara::Expression_Tree::System_Call_To_Integers::accept (
  Madara::Expression_Tree::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
