
#ifndef _MADARA_NO_KARL_


#include "madara/utility/Utility.h"
#include "madara/expression_tree/Leaf_Node.h"
#include "madara/expression_tree/System_Call_Rand_Int.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/expression_tree/Visitor.h"


Madara::Expression_Tree::System_Call_Rand_Int::System_Call_Rand_Int (
  Madara::Knowledge_Engine::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
Madara::Expression_Tree::System_Call_Rand_Int::~System_Call_Rand_Int (void)
{
}

Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_Rand_Int::item (void) const
{
  return Madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
Madara::Knowledge_Record
Madara::Expression_Tree::System_Call_Rand_Int::prune (bool & can_change)
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
      *i = new Leaf_Node (*(this->logger_), result);
    }
  }

  if (nodes_.size () > 3)
  {
    logger_->log (Logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: System call rand_int"
      " can have up to three arguments, 1) floor, "
      "2) ceiling and 3) whether to set the random seed");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
Madara::Knowledge_Record 
Madara::Expression_Tree::System_Call_Rand_Int::evaluate (
const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  Knowledge_Record::Integer floor (0), ceiling (1);
  bool update_srand = true;
  
  if (nodes_.size () > 0)
  {
    floor = nodes_[0]->evaluate (settings).to_integer ();

    if (nodes_.size () > 1)
    {
      ceiling = nodes_[1]->evaluate (settings).to_integer ();

      if (nodes_.size () > 2)
      {
        update_srand = bool (nodes_[2]->evaluate (settings).to_integer ());
      }
    }
  }

  logger_->log (Logger::LOG_MINOR,
    "System call rand_int called with %" PRId64 ", %" PRId64 ", %d.\n",
    floor, ceiling, update_srand);

  return Utility::rand_int (floor, ceiling, update_srand);
}

// accept a visitor
void 
Madara::Expression_Tree::System_Call_Rand_Int::accept (
  Madara::Expression_Tree::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
