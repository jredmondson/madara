
#ifndef _MADARA_NO_KARL_

#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallFragment.h"
#include "madara/expression/Visitor.h"

madara::expression::SystemCallFragment::SystemCallFragment(
    madara::knowledge::ThreadSafeContext& context, const ComponentNodes& nodes)
  : SystemCallNode(context, nodes)
{
}

// Dtor
madara::expression::SystemCallFragment::~SystemCallFragment(void) {}

madara::knowledge::KnowledgeRecord madara::expression::SystemCallFragment::item(
    void) const
{
  return madara::knowledge::KnowledgeRecord(nodes_.size());
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallFragment::prune(bool& can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;

  madara::knowledge::KnowledgeRecord result;

  for(ComponentNodes::iterator i = nodes_.begin(); i != nodes_.end(); ++i)
  {
    bool arg_can_change = false;
    result = (*i)->prune(arg_can_change);

    if(!arg_can_change && dynamic_cast<LeafNode*>(*i) == 0)
    {
      delete *i;
      *i = new LeafNode(*(this->logger_), result);
    }
  }

  if(nodes_.size() != 3)
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallFragment: "
        "KARL COMPILE ERROR:"
        "System call fragment"
        " requires three arguments, e.g."
        " #fragment ('hello world', 0, 4) will return 'hello'\n");

    throw exceptions::KarlException(
        "madara::expression::SystemCallFragment: "
        "KARL COMPILE ERROR: "
        "System call fragment"
        " requires three arguments, e.g."
        " #fragment ('hello world', 0, 4) will return 'hello'\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallFragment::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  knowledge::KnowledgeRecord return_value;

  if(nodes_.size() == 3)
  {
    // grab the first and last ranges
    unsigned int first =
        (unsigned int)nodes_[1]->evaluate(settings).to_integer();
    unsigned int last =
        (unsigned int)nodes_[2]->evaluate(settings).to_integer();

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "madara::expression::SystemCallFragment: "
        "System call fragment is returning segment [%d-%d].\n",
        first, last);

    return nodes_[0]->evaluate(settings).fragment(first, last);
  }

  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallFragment: "
        "KARL RUNTIME ERROR:"
        "System call fragment"
        " requires three arguments, e.g."
        " #fragment ('hello world', 0, 4) will return 'hello'\n");

    throw exceptions::KarlException(
        "madara::expression::SystemCallFragment: "
        "KARL RUNTIME ERROR: "
        "System call fragment"
        " requires three arguments, e.g."
        " #fragment ('hello world', 0, 4) will return 'hello'\n");
  }

  return return_value;
}

// accept a visitor
void madara::expression::SystemCallFragment::accept(
    madara::expression::Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_
