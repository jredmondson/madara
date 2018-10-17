
#ifndef _MADARA_NO_KARL_

#include "madara/utility/Utility.h"
#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallExpandEnv.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"

madara::expression::SystemCallExpandEnv::SystemCallExpandEnv(
    madara::knowledge::ThreadSafeContext& context, const ComponentNodes& nodes)
  : SystemCallNode(context, nodes)
{
}

// Dtor
madara::expression::SystemCallExpandEnv::~SystemCallExpandEnv(void) {}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallExpandEnv::item(void) const
{
  return madara::knowledge::KnowledgeRecord(nodes_.size());
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallExpandEnv::prune(bool& can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;

  madara::knowledge::KnowledgeRecord result;

  if(nodes_.size() == 1)
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
        "madara::expression::SystemCallExpandEnv: "
        "KARL COMPILE ERROR:"
        "System call expand_env requires 1 argument,"
        "e.g., #expand_env ('path=$(MADARA_ROOT')\n");

    throw exceptions::KarlException(
        "madara::expression::SystemCallExpandEnv: "
        "KARL COMPILE ERROR: "
        "System call expand_env requires 1 argument,"
        "e.g., #expand_env ('path=$(MADARA_ROOT')\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallExpandEnv::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  knowledge::KnowledgeRecord return_value;

  if(nodes_.size() == 1)
  {
    std::string statement(nodes_[0]->evaluate(settings).to_string());

    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "madara::expression::SystemCallExpandEnv: "
        "System call expand_var is returning the expansion "
        "of %s.\n",
        statement.c_str());

    return knowledge::KnowledgeRecord(madara::utility::expand_envs(statement));
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallExpandEnv: "
        "KARL RUNTIME ERROR:"
        "System call expand_env requires 1 argument,"
        "e.g., #expand_env ('path=$(MADARA_ROOT')\n");

    throw exceptions::KarlException(
        "madara::expression::SystemCallExpandEnv: "
        "KARL RUNTIME ERROR: "
        "System call expand_env requires 1 argument,"
        "e.g., #expand_env ('path=$(MADARA_ROOT')\n");
  }

  return return_value;
}

// accept a visitor
void madara::expression::SystemCallExpandEnv::accept(
    madara::expression::Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_
