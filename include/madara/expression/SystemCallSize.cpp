
#ifndef _MADARA_NO_KARL_

#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallSize.h"
#include "madara/expression/Visitor.h"

madara::expression::SystemCallSize::SystemCallSize(
    madara::knowledge::ThreadSafeContext& context, const ComponentNodes& nodes)
  : SystemCallNode(context, nodes)
{
}

// Dtor
madara::expression::SystemCallSize::~SystemCallSize(void) {}

madara::knowledge::KnowledgeRecord madara::expression::SystemCallSize::item(
    void) const
{
  return madara::knowledge::KnowledgeRecord(nodes_.size());
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord madara::expression::SystemCallSize::prune(
    bool& can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;

  madara::knowledge::KnowledgeRecord result;

  if (nodes_.size() > 0)
  {
    bool arg_can_change = false;
    result = nodes_[0]->prune(arg_can_change);

    if (!arg_can_change && dynamic_cast<LeafNode*>(nodes_[0]) == 0)
    {
      delete nodes_[0];
      nodes_[0] = new LeafNode(*(this->logger_), result);
    }
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallSize: "
        "KARL COMPILE ERROR:"
        "System call size requires an argument\n");

    throw exceptions::KarlException("madara::expression::SystemCallSize: "
                                    "KARL COMPILE ERROR: "
                                    "System call size requires an argument\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord madara::expression::SystemCallSize::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  knowledge::KnowledgeRecord return_value;

  if (nodes_.size() > 0)
  {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "madara::expression::SystemCallSize: "
        "System call size is returning the size of its first argument\n");

    return madara::knowledge::KnowledgeRecord(
        nodes_[0]->evaluate(settings).size());
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallSize: "
        "KARL RUNTIME ERROR:"
        "System call size requires an argument\n");

    throw exceptions::KarlException("madara::expression::SystemCallSize: "
                                    "KARL RUNTIME ERROR: "
                                    "System call size requires an argument\n");
  }

  return return_value;
}

// accept a visitor
void madara::expression::SystemCallSize::accept(
    madara::expression::Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_
