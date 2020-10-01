
#ifndef _MADARA_NO_KARL_

#include <math.h>

#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallCbrt.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"

madara::expression::SystemCallCbrt::SystemCallCbrt(
    madara::knowledge::ThreadSafeContext& context, const ComponentNodes& nodes)
  : SystemCallNode(context, nodes)
{
}

// Dtor
madara::expression::SystemCallCbrt::~SystemCallCbrt(void) {}

madara::knowledge::KnowledgeRecord madara::expression::SystemCallCbrt::item(
    void) const
{
  return madara::knowledge::KnowledgeRecord(nodes_.size());
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord madara::expression::SystemCallCbrt::prune(
    bool& can_change)
{
  madara::knowledge::KnowledgeRecord result;

  if (nodes_.size() == 1)
  {
    bool arg_can_change = false;
    result = knowledge::KnowledgeRecord(
        cbrt(nodes_[0]->prune(arg_can_change).to_double()));

    if (!arg_can_change && dynamic_cast<LeafNode*>(nodes_[0]) == 0)
    {
      delete nodes_[0];
      nodes_[0] = new LeafNode(*(this->logger_), result);
    }

    can_change = can_change || arg_can_change;
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallCbrt: "
        "KARL COMPILE ERROR:"
        "System call cbrt requires 1 argument,"
        "e.g., #cbrt (8), which would return 2\n");

    throw exceptions::KarlException("madara::expression::SystemCallSleep: "
                                    "KARL COMPILE ERROR: "
                                    "System call cbrt requires 1 argument,"
                                    "e.g., #cbrt (8), which would return 2\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord madara::expression::SystemCallCbrt::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  knowledge::KnowledgeRecord return_value;

  if (nodes_.size() == 1)
  {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "madara::expression::SystemCallCbrt: "
        "System call cbrt is returning the cube root "
        "of its first argument\n");

    return_value = knowledge::KnowledgeRecord(
        cbrt(nodes_[0]->evaluate(settings).to_double()));
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallCbrt: "
        "KARL RUNTIME ERROR:"
        "System call cbrt requires 1 argument,"
        "e.g., #cbrt (8), which would return 2\n");

    throw exceptions::KarlException("madara::expression::SystemCallCbrt: "
                                    "KARL RUNTIME ERROR: "
                                    "System call cbrt requires 1 argument,"
                                    "e.g., #cbrt (8), which would return 2\n");
  }

  return return_value;
}

// accept a visitor
void madara::expression::SystemCallCbrt::accept(
    madara::expression::Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_
