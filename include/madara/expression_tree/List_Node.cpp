
#ifndef _MADARA_NO_KARL_

#include "madara/expression_tree/Visitor.h"
#include "madara/expression_tree/List_Node.h"
#include "madara/utility/Utility.h"


#include <string>
#include <sstream>

madara::expression_tree::List_Node::List_Node (
  madara::knowledge::Thread_Safe_Context & context)
: Component_Node (context.get_logger ()), context_ (context), list_ ()
{

}

madara::expression_tree::List_Node::~List_Node ()
{
  // do not clean up record_. Let the context clean that up.
}


void
madara::expression_tree::List_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

madara::Knowledge_Record
madara::expression_tree::List_Node::item () const
{
  return madara::Knowledge_Record (
    madara::Knowledge_Record::Integer (list_.size ()));
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression_tree::List_Node::prune (bool & can_change)
{
  // a variable is one of very few nodes that can change over time and
  // cannot be pruned
  can_change = true;
  
  return madara::Knowledge_Record (
    madara::Knowledge_Record::Integer (list_.size ()));
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression_tree::List_Node::evaluate (
  const madara::knowledge::Knowledge_Update_Settings & settings)
{
  return madara::Knowledge_Record (
    madara::Knowledge_Record::Integer (list_.size ()));
}


#endif // _MADARA_NO_KARL_
