
#ifndef _MADARA_NO_KARL_

#include <time.h>
#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallGetTimeSeconds.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"
#include "madara/utility/Utility.h"

madara::expression::SystemCallGetTimeSeconds::SystemCallGetTimeSeconds(
    madara::knowledge::ThreadSafeContext& context, const ComponentNodes& nodes)
  : SystemCallNode(context, nodes)
{
}

// Dtor
madara::expression::SystemCallGetTimeSeconds::~SystemCallGetTimeSeconds(void) {}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallGetTimeSeconds::item(void) const
{
  return madara::knowledge::KnowledgeRecord(nodes_.size());
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallGetTimeSeconds::prune(bool& can_change)
{
  // Time changes. No way to prune this node out.
  can_change = true;

  madara::knowledge::KnowledgeRecord result;

  if (nodes_.size() > 0) {
    bool arg_can_change = false;
    result = nodes_[0]->prune(arg_can_change);

    if (!arg_can_change && dynamic_cast<LeafNode*>(nodes_[0]) == 0) {
      delete nodes_[0];
      nodes_[0] = new LeafNode(*(this->logger_), result);
    }
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallGetTimeSeconds::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& /*settings*/)
{
  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "madara::expression::SystemCallGetTimeSeconds:"
      "System call get_time_seconds is returning the system clock\n");

  return madara::knowledge::KnowledgeRecord(time(NULL));
}

// accept a visitor
void madara::expression::SystemCallGetTimeSeconds::accept(
    madara::expression::Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_
