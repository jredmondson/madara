
#ifndef _MADARA_NO_KARL_

#include "madara/utility/Log_Macros.h"
#include "madara/utility/Utility.h"
#include "madara/expression_tree/Leaf_Node.h"
#include "madara/expression_tree/System_Call_Rand_Double.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/expression_tree/Visitor.h"


Madara::Expression_Tree::System_Call_Rand_Double::System_Call_Rand_Double (
  Madara::Knowledge_Engine::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
Madara::Expression_Tree::System_Call_Rand_Double::~System_Call_Rand_Double (void)
{
}

Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_Rand_Double::item (void) const
{
  return Madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_Rand_Double::prune (bool & can_change)
{
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

  if (nodes_.size () > 3)
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_ERROR, 
      "KARL COMPILE ERROR: System call rand_double"
      " can have up to three arguments, 1) floor, "
      "2) ceiling and 3) whether to set the random seed'\n"));
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
Madara::Knowledge_Record 
Madara::Expression_Tree::System_Call_Rand_Double::evaluate (
const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  double floor (0.0), ceiling (1.0);
  bool update_srand = true;
  
  if (nodes_.size () > 0)
  {
    floor = nodes_[0]->evaluate (settings).to_double ();

    if (nodes_.size () > 1)
    {
      ceiling = nodes_[1]->evaluate (settings).to_double ();

      if (nodes_.size () > 2)
      {
        update_srand = bool (nodes_[2]->evaluate (settings).to_integer ());
      }
    }
  }
  
  MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
    "System call rand_double called with %f, %f, %d.\n",
    floor, ceiling, update_srand));

  return Utility::rand_double (floor, ceiling, update_srand);
}

// accept a visitor
void 
Madara::Expression_Tree::System_Call_Rand_Double::accept (
  Madara::Expression_Tree::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
