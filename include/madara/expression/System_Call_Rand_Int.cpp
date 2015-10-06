
#ifndef _MADARA_NO_KARL_

#include "madara/utility/Utility.h"
#include "madara/expression/Leaf_Node.h"
#include "madara/expression/System_Call_Rand_Int.h"
#include "madara/knowledge/Thread_Safe_Context.h"
#include "madara/expression/Visitor.h"
#include "madara/utility/inttypes.h"


madara::expression::System_Call_Rand_Int::System_Call_Rand_Int (
  madara::knowledge::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
madara::expression::System_Call_Rand_Int::~System_Call_Rand_Int (void)
{
}

madara::Knowledge_Record
madara::expression::System_Call_Rand_Int::item (void) const
{
  return madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression::System_Call_Rand_Int::prune (bool & can_change)
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
      "KARL COMPILE ERROR: System call rand_int"
      " can have up to three arguments, 1) floor, "
      "2) ceiling and 3) whether to set the random seed");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression::System_Call_Rand_Int::evaluate (
const madara::knowledge::Knowledge_Update_Settings & settings)
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

  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
    "System call rand_int called with %" PRId64 ", %" PRId64 ", %d.\n",
    floor, ceiling, update_srand);

  return utility::rand_int (floor, ceiling, update_srand);
}

// accept a visitor
void 
madara::expression::System_Call_Rand_Int::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
