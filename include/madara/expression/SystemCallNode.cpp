
#ifndef _MADARA_NO_KARL_

#include "madara/expression/SystemCallNode.h"
#include "madara/knowledge/ThreadSafeContext.h"

madara::expression::SystemCallNode::SystemCallNode(
    madara::knowledge::ThreadSafeContext& context)
  : CompositeTernaryNode(context.get_logger()), context_(context)
{
}

madara::expression::SystemCallNode::SystemCallNode(
    madara::knowledge::ThreadSafeContext& context, const ComponentNodes& nodes)
  : CompositeTernaryNode(context.get_logger(), nodes), context_(context)
{
}

// Dtor
madara::expression::SystemCallNode::~SystemCallNode(void) {}

madara::knowledge::KnowledgeRecord madara::expression::SystemCallNode::item(
    void) const
{
  return madara::knowledge::KnowledgeRecord(nodes_.size());
}

// accept a visitor
void madara::expression::SystemCallNode::accept(
    madara::expression::Visitor& visitor) const
{
  (void)visitor;
}

#endif  // _MADARA_NO_KARL_
