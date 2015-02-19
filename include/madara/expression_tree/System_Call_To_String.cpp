
#ifndef _MADARA_NO_KARL_

#include "madara/utility/Log_Macros.h"
#include "madara/expression_tree/Leaf_Node.h"
#include "madara/expression_tree/System_Call_To_String.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/expression_tree/Visitor.h"


Madara::Expression_Tree::System_Call_To_String::System_Call_To_String (
  Madara::Knowledge_Engine::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
Madara::Expression_Tree::System_Call_To_String::~System_Call_To_String (void)
{
}

Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_To_String::item (void) const
{
  return Madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_To_String::prune (bool & can_change)
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
      *i = new Leaf_Node (result);
    }
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
Madara::Knowledge_Record 
Madara::Expression_Tree::System_Call_To_String::evaluate (
const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  Madara::Knowledge_Record result;

  if (nodes_.size () > 0)
  {
    std::string delimiter = ", ";

    if (nodes_.size () > 1)
    {
      // user wants to use a special delimiter
      delimiter = nodes_[1]->evaluate (settings).to_string (delimiter);

      if (nodes_.size () > 2)
      { 
        MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_ERROR, 
          "KARL RUNTIME ERROR: System call to_string"
          " may have up to 2 arguments. First is a value to change to string."
          " An optional second is a delimiter for array stringification.\n"));
      }
    }

    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      "System call to_string is converting an argument.\n"));

    result = nodes_[0]->evaluate (settings).to_string (delimiter);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      "System call to_string is converting 0.\n"));
  }

  return result;
}

// accept a visitor
void 
Madara::Expression_Tree::System_Call_To_String::accept (
  Madara::Expression_Tree::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
