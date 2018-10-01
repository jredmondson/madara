
#ifndef _MADARA_NO_KARL_

#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallIsinf.h"
#include "madara/expression/Visitor.h"
#include <math.h>

madara::expression::SystemCallIsinf::SystemCallIsinf(
    madara::knowledge::ThreadSafeContext& context, const ComponentNodes& nodes)
  : SystemCallNode(context, nodes)
{
}

// Dtor
madara::expression::SystemCallIsinf::~SystemCallIsinf(void) {}

madara::knowledge::KnowledgeRecord madara::expression::SystemCallIsinf::item(
    void) const
{
  return madara::knowledge::KnowledgeRecord(nodes_.size());
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord madara::expression::SystemCallIsinf::prune(
    bool& can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;

  madara::knowledge::KnowledgeRecord result;

  if (nodes_.size() == 1) {
    bool arg_can_change = false;
    result = nodes_[0]->prune(arg_can_change);

    if (!arg_can_change && dynamic_cast<LeafNode*>(nodes_[0]) == 0) {
      delete nodes_[0];
      nodes_[0] = new LeafNode(*(this->logger_), result);
    }
  } else {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallIsinf: "
        "KARL COMPILE ERROR:"
        "System call isinf requires 1 argument\n");

    throw exceptions::KarlException("madara::expression::SystemCallIsinf: "
                                    "KARL COMPILE ERROR: "
                                    "System call isinf requires 1 argument\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallIsinf::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  knowledge::KnowledgeRecord return_value;

  if (nodes_.size() == 1) {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "madara::expression::SystemCallIsinf: "
        "System call isinf is returning isinf of its first argument\n");

    return madara::knowledge::KnowledgeRecord(
        std::isinf(nodes_[0]->evaluate(settings).to_double()));
  } else {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallIsinf: "
        "KARL RUNTIME ERROR:"
        "System call isinf requires 1 argument\n");

    throw exceptions::KarlException("madara::expression::SystemCallIsinf: "
                                    "KARL RUNTIME ERROR: "
                                    "System call isinf requires 1 argument\n");
  }

  return return_value;
}

// accept a visitor
void madara::expression::SystemCallIsinf::accept(
    madara::expression::Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_
