
#ifndef _MADARA_NO_KARL_

#include "madara/utility/Utility.h"
#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallRandDouble.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"

madara::expression::SystemCallRandDouble::SystemCallRandDouble(
    madara::knowledge::ThreadSafeContext& context, const ComponentNodes& nodes)
  : SystemCallNode(context, nodes)
{
}

// Dtor
madara::expression::SystemCallRandDouble::~SystemCallRandDouble(void) {}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallRandDouble::item(void) const
{
  return madara::knowledge::KnowledgeRecord(nodes_.size());
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallRandDouble::prune(bool& can_change)
{
  can_change = true;

  madara::knowledge::KnowledgeRecord result;

  for (ComponentNodes::iterator i = nodes_.begin(); i != nodes_.end(); ++i) {
    bool arg_can_change = false;
    result = (*i)->prune(arg_can_change);

    if (!arg_can_change && dynamic_cast<LeafNode*>(*i) == 0) {
      delete *i;
      *i = new LeafNode(*(this->logger_), result);
    }
  }

  if (nodes_.size() > 3) {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "madara::expression::SystemCallRandDouble: "
        "KARL COMPILE ERROR:"
        "System call rand_double"
        " can have up to three arguments, 1) floor, "
        "2) ceiling and 3) whether to set the random seed\n");

    throw exceptions::KarlException(
        "madara::expression::SystemCallRandDouble: "
        "KARL COMPILE ERROR: "
        "System call rand_double"
        " can have up to three arguments, 1) floor, "
        "2) ceiling and 3) whether to set the random seeds\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallRandDouble::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  double floor(0.0), ceiling(1.0);
  bool update_srand = true;

  if (nodes_.size() > 0) {
    floor = nodes_[0]->evaluate(settings).to_double();

    if (nodes_.size() > 1) {
      ceiling = nodes_[1]->evaluate(settings).to_double();

      if (nodes_.size() > 2) {
        update_srand = nodes_[2]->evaluate(settings).to_integer() != 0;
      }
    }
  }

  madara_logger_ptr_log(logger_, logger::LOG_MINOR,
      "madara::expression::SystemCallRandDouble: "
      "System call rand_double called with %f, %f, %d\n",
      floor, ceiling, update_srand);

  return knowledge::KnowledgeRecord(
      utility::rand_double(floor, ceiling, update_srand));
}

// accept a visitor
void madara::expression::SystemCallRandDouble::accept(
    madara::expression::Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_
