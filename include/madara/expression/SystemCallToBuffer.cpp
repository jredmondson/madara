
#ifndef _MADARA_NO_KARL_

#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallToBuffer.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"

madara::expression::SystemCallToBuffer::SystemCallToBuffer(
    madara::knowledge::ThreadSafeContext& context, const ComponentNodes& nodes)
  : SystemCallNode(context, nodes)
{
}

// Dtor
madara::expression::SystemCallToBuffer::~SystemCallToBuffer(void) {}

madara::knowledge::KnowledgeRecord madara::expression::SystemCallToBuffer::item(
    void) const
{
  return madara::knowledge::KnowledgeRecord(nodes_.size());
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallToBuffer::prune(bool& can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
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
madara::expression::SystemCallToBuffer::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  knowledge::KnowledgeRecord return_value;

  if (nodes_.size() > 0) {
    madara_logger_ptr_log(logger_, logger::LOG_MINOR,
        "madara::expression::SystemCallToBuffer: "
        "System call to_buffer is converting an argument to an "
        "unmanaged buffer\n");

    size_t size;
    unsigned char* buffer =
        nodes_[0]->evaluate(settings).to_unmanaged_buffer(size);

    return_value.set_file(buffer, size);
  } else {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallToBuffer: "
        "KARL RUNTIME ERROR:"
        "System call to_buffer requires an argument\n");

    throw exceptions::KarlException(
        "madara::expression::SystemCallToBuffer: "
        "KARL RUNTIME ERROR: "
        "System call to_buffer requires an argument\n");
  }

  return return_value;
}

// accept a visitor
void madara::expression::SystemCallToBuffer::accept(
    madara::expression::Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_
