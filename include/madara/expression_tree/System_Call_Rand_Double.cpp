
#ifndef _MADARA_NO_KARL_


#include "madara/utility/Utility.h"
#include "madara/expression_tree/Leaf_Node.h"
#include "madara/expression_tree/System_Call_Rand_Double.h"
#include "madara/knowledge/Thread_Safe_Context.h"
#include "madara/expression_tree/Visitor.h"


madara::expression_tree::System_Call_Rand_Double::System_Call_Rand_Double (
  madara::knowledge::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
madara::expression_tree::System_Call_Rand_Double::~System_Call_Rand_Double (void)
{
}

madara::Knowledge_Record
madara::expression_tree::System_Call_Rand_Double::item (void) const
{
  return madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression_tree::System_Call_Rand_Double::prune (bool & can_change)
{
  can_change = true;
  
  madara::Knowledge_Record result;
  
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
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: System call rand_double"
      " can have up to three arguments, 1) floor, "
      "2) ceiling and 3) whether to set the random seed\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression_tree::System_Call_Rand_Double::evaluate (
const madara::knowledge::Knowledge_Update_Settings & settings)
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

  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
    "System call rand_double called with %f, %f, %d\n",
    floor, ceiling, update_srand);

  return utility::rand_double (floor, ceiling, update_srand);
}

// accept a visitor
void 
madara::expression_tree::System_Call_Rand_Double::accept (
  madara::expression_tree::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
