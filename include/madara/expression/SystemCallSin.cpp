
#ifndef _MADARA_NO_KARL_

#include <math.h>

#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallSin.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"

madara::expression::SystemCallSin::SystemCallSin(
    madara::knowledge::ThreadSafeContext& context, const ComponentNodes& nodes)
  : SystemCallNode(context, nodes)
{
}

// Dtor
madara::expression::SystemCallSin::~SystemCallSin(void) {}

madara::knowledge::KnowledgeRecord madara::expression::SystemCallSin::item(
    void) const
{
  return madara::knowledge::KnowledgeRecord(nodes_.size());
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord madara::expression::SystemCallSin::prune(
    bool& can_change)
{
  madara::knowledge::KnowledgeRecord result;

  if (nodes_.size() == 1)
  {
    bool arg_can_change = false;
    result = knowledge::KnowledgeRecord(
        sin(nodes_[0]->prune(arg_can_change).to_double()));

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
        "madara::expression::SystemCallSin: "
        "KARL COMPILE ERROR:"
        "System call sin requires 1 argument,"
        " e.g., #sin (1.5), where 1.5 is radians\n");

    throw exceptions::KarlException(
        "madara::expression::SystemCallSin: "
        "KARL COMPILE ERROR: "
        "System call sin requires 1 argument,"
        " e.g., #sin (1.5), where 1.5 is radians\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord madara::expression::SystemCallSin::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  knowledge::KnowledgeRecord return_value;

  if (nodes_.size() == 1)
  {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "madara::expression::SystemCallSin: "
        "System call sin is returning the sin "
        "of its first argument\n");

    return knowledge::KnowledgeRecord(
        sin(nodes_[0]->evaluate(settings).to_double()));
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallSin: "
        "KARL RUNTIME ERROR:"
        "System call sin requires 1 argument,"
        " e.g., #sin (1.5), where 1.5 is radians\n");

    throw exceptions::KarlException(
        "madara::expression::SystemCallSin: "
        "KARL RUNTIME ERROR: "
        "System call sin requires 1 argument,"
        " e.g., #sin (1.5), where 1.5 is radians\n");
  }

  return return_value;
}

// accept a visitor
void madara::expression::SystemCallSin::accept(
    madara::expression::Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_
