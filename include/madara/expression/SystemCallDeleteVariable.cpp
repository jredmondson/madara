
#ifndef _MADARA_NO_KARL_

#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallDeleteVariable.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"

madara::expression::SystemCallDeleteVariable::SystemCallDeleteVariable(
    madara::knowledge::ThreadSafeContext& context, const ComponentNodes& nodes)
  : SystemCallNode(context, nodes)
{
}

// Dtor
madara::expression::SystemCallDeleteVariable::~SystemCallDeleteVariable(void) {}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallDeleteVariable::item(void) const
{
  return madara::knowledge::KnowledgeRecord(nodes_.size());
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallDeleteVariable::prune(bool& can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;

  madara::knowledge::KnowledgeRecord result;

  if (nodes_.size() == 1)
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
        "madara::expression::SystemCallDeleteVariable: "
        "KARL COMPILE ERROR:"
        "System call delete_variable requires 1 argument,"
        "e.g., #delete_variable ('var')\n");

    throw exceptions::KarlException(
        "madara::expression::SystemCallDeleteVariable: "
        "KARL COMPILE ERROR: "
        "System call delete_variable requires 1 argument,"
        "e.g., #delete_variable ('var')\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallDeleteVariable::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  knowledge::KnowledgeRecord return_value;

  if (nodes_.size() == 1)
  {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "madara::expression::SystemCallDeleteVariable: "
        "System call delete_variable is returning the clock "
        "of its first argument\n");

    return madara::knowledge::KnowledgeRecord(
        context_.delete_variable(nodes_[0]->evaluate(settings).to_string()));
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallDeleteVariable: "
        "KARL RUNTIME ERROR:"
        "System call delete_variable requires 1 argument,"
        "e.g., #delete_variable ('var')\n");

    throw exceptions::KarlException(
        "madara::expression::SystemCallDeleteVariable: "
        "KARL RUNTIME ERROR: "
        "System call delete_variable requires 1 argument,"
        "e.g., #delete_variable ('var')\n");
  }

  return return_value;
}

// accept a visitor
void madara::expression::SystemCallDeleteVariable::accept(
    madara::expression::Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_
