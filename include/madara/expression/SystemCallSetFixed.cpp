
#ifndef _MADARA_NO_KARL_


#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallSetFixed.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"


madara::expression::SystemCallSetFixed::SystemCallSetFixed (
  madara::knowledge::ThreadSafeContext & context,
  const ComponentNodes & nodes)
  : SystemCallNode (context, nodes)
{

}

// Dtor
madara::expression::SystemCallSetFixed::~SystemCallSetFixed (void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallSetFixed::item (void) const
{
  return madara::knowledge::KnowledgeRecord (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallSetFixed::prune (bool & can_change)
{
  // this node never changes, but we may be able to prune what's inside
  can_change = true;
  
  madara::knowledge::KnowledgeRecord result;

  // this node should never have arguments
  if (nodes_.size () > 0)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "SystemCallSetFixed:prune: no arguments should be supplied to "
      "#set_fixed(). Deleting arguments to reclaim memory.\n");

    // delete any arguments that the user specified
    for (ComponentNodes::iterator i = nodes_.begin (); i != nodes_.end ();
      ++i)
    {
      delete *i;
    }

    nodes_.clear ();
  }


  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::SystemCallSetFixed::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & /*settings*/)
{
  knowledge::KnowledgeRecord return_value;

  madara_logger_ptr_log (logger_, logger::LOG_MINOR,
    "System call set_fixed is setting double output format to std::fixed.\n");

  knowledge::KnowledgeRecord::set_fixed ();

  return return_value;
}

// accept a visitor
void 
madara::expression::SystemCallSetFixed::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
