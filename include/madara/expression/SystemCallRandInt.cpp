
#ifndef _MADARA_NO_KARL_

#include "madara/utility/Utility.h"
#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallRandInt.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"
#include "madara/utility/inttypes.h"


madara::expression::SystemCallRandInt::SystemCallRandInt (
  madara::knowledge::ThreadSafeContext & context,
  const ComponentNodes & nodes)
  : SystemCallNode (context, nodes)
{

}

// Dtor
madara::expression::SystemCallRandInt::~SystemCallRandInt (void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallRandInt::item (void) const
{
  return madara::knowledge::KnowledgeRecord (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallRandInt::prune (bool & can_change)
{
  can_change = true;
  
  madara::knowledge::KnowledgeRecord result;
  
  for (ComponentNodes::iterator i = nodes_.begin (); i != nodes_.end ();
       ++i)
  {
    bool arg_can_change = false;
    result = (*i)->prune (arg_can_change);
    
    if (!arg_can_change && dynamic_cast <LeafNode *> (*i) == 0)
    {
      delete *i;
      *i = new LeafNode (*(this->logger_), result);
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
madara::knowledge::KnowledgeRecord 
madara::expression::SystemCallRandInt::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  int64_t floor (0), ceiling (1);
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

  return knowledge::KnowledgeRecord (
      utility::rand_int (floor, ceiling, update_srand));
}

// accept a visitor
void 
madara::expression::SystemCallRandInt::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
