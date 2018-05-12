
#ifndef _MADARA_NO_KARL_

#include "madara/expression/Visitor.h"
#include "madara/expression/ListNode.h"
#include "madara/utility/Utility.h"


#include <string>
#include <sstream>

madara::expression::ListNode::ListNode (
  madara::knowledge::ThreadSafeContext & context)
: ComponentNode (context.get_logger ()), list_ ()
{

}

madara::expression::ListNode::~ListNode ()
{
  // do not clean up record_. Let the context clean that up.
}


void
madara::expression::ListNode::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

madara::knowledge::KnowledgeRecord
madara::expression::ListNode::item () const
{
  return madara::knowledge::KnowledgeRecord (
    madara::knowledge::KnowledgeRecord::Integer (list_.size ()));
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::ListNode::prune (bool & can_change)
{
  // a variable is one of very few nodes that can change over time and
  // cannot be pruned
  can_change = true;
  
  return madara::knowledge::KnowledgeRecord (
    madara::knowledge::KnowledgeRecord::Integer (list_.size ()));
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::ListNode::evaluate (
  const madara::knowledge::KnowledgeUpdateSettings & /*settings*/)
{
  return madara::knowledge::KnowledgeRecord (
    madara::knowledge::KnowledgeRecord::Integer (list_.size ()));
}


#endif // _MADARA_NO_KARL_
