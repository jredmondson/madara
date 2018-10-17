
#ifndef _MADARA_NO_KARL_

#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallPrint.h"
#include "madara/expression/Visitor.h"
#include "madara/knowledge/ThreadSafeContext.h"

madara::expression::SystemCallPrint::SystemCallPrint(
    madara::knowledge::ThreadSafeContext& context, const ComponentNodes& nodes)
  : SystemCallNode(context, nodes)
{
}

// Dtor
madara::expression::SystemCallPrint::~SystemCallPrint(void) {}

madara::knowledge::KnowledgeRecord madara::expression::SystemCallPrint::item(
    void) const
{
  return madara::knowledge::KnowledgeRecord(nodes_.size());
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord madara::expression::SystemCallPrint::prune(
    bool& can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;

  madara::knowledge::KnowledgeRecord result;

  if(nodes_.size() > 0)
  {
    bool arg_can_change = false;
    result = nodes_[0]->prune(arg_can_change);

    if(!arg_can_change && dynamic_cast<LeafNode*>(nodes_[0]) == 0)
    {
      delete nodes_[0];
      nodes_[0] = new LeafNode(*(this->logger_), result);
    }
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallPrint: "
        "KARL COMPILE ERROR:"
        "System call size requires an argument\n");

    throw exceptions::KarlException("madara::expression::SystemCallPrint: "
                                    "KARL COMPILE ERROR: "
                                    "System call size requires an argument\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallPrint::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  knowledge::KnowledgeRecord return_value;

  if(nodes_.size() == 1)
  {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "madara::expression::SystemCallPrint: "
        "System call print is printing the first argument and returning the"
        " size of the first argument\n");

    std::string statement(nodes_[0]->evaluate(settings).to_string());

    context_.print(statement, 0);

    return_value = knowledge::KnowledgeRecord(0);
  }
  else if(nodes_.size() == 2)
  {
    std::string statement(nodes_[0]->evaluate(settings).to_string());
    unsigned int log_level =
        (unsigned int)nodes_[1]->evaluate(settings).to_integer();

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "madara::expression::SystemCallPrint: "
        "System call print is printing the first argument at log level %d.\n",
        log_level);

    context_.print(statement, log_level);

    return_value = knowledge::KnowledgeRecord(0);
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallPrint: "
        "KARL RUNTIME ERROR:"
        "System call size requires an argument\n");

    throw exceptions::KarlException("madara::expression::SystemCallPrint: "
                                    "KARL RUNTIME ERROR: "
                                    "System call size requires an argument\n");
  }

  return return_value;
}

// accept a visitor
void madara::expression::SystemCallPrint::accept(
    madara::expression::Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_
