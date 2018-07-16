
#ifndef _MADARA_NO_KARL_

#include <math.h>

#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallPow.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"


madara::expression::SystemCallPow::SystemCallPow (
  madara::knowledge::ThreadSafeContext & context,
  const ComponentNodes & nodes)
  : SystemCallNode (context, nodes)
{

}

// Dtor
madara::expression::SystemCallPow::~SystemCallPow (void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallPow::item (void) const
{
  return madara::knowledge::KnowledgeRecord (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallPow::prune (bool & can_change)
{
  madara::knowledge::KnowledgeRecord result;

  if (nodes_.size () == 2)
  {
    bool base_can_change = false, power_can_change = false;

    double base = nodes_[0]->prune (base_can_change).to_double ();
    double power = nodes_[1]->prune (power_can_change).to_double ();

    if (!base_can_change && dynamic_cast <LeafNode *> (nodes_[0]) == 0)
    {
      delete nodes_[0];
      nodes_[0] = new LeafNode (*(this->logger_), base);
    }

    if (!power_can_change && dynamic_cast <LeafNode *> (nodes_[1]) == 0)
    {
      delete nodes_[1];
      nodes_[1] = new LeafNode (*(this->logger_), power);
    }

    result = knowledge::KnowledgeRecord (pow (base, power));

    can_change = can_change || base_can_change || power_can_change;
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "madara::expression::SystemCallPow: "
      "KARL COMPILE ERROR:"
      "System call pow requires 2 arguments,"
      "e.g., #pow (4,2), which would return 16.\n");

    throw exceptions::KarlException ("madara::expression::SystemCallPow: "
      "KARL COMPILE ERROR: "
      "System call pow requires 2 arguments,"
      "e.g., #pow (4,2), which would return 16.\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallPow::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  knowledge::KnowledgeRecord return_value;

  if (nodes_.size () == 2)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "madara::expression::SystemCallPow: "
      "System call pow is returning the base taken to the power\n");

    return_value = knowledge::KnowledgeRecord(
        pow (nodes_[0]->evaluate (settings).to_double (),
      nodes_[1]->evaluate (settings).to_double ()));
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "madara::expression::SystemCallPow: "
      "KARL RUNTIME ERROR:"
      "System call pow requires 2 arguments,"
      "e.g., #pow (4,2), which would return 16.\n");

    throw exceptions::KarlException ("madara::expression::SystemCallPow: "
      "KARL RUNTIME ERROR: "
      "System call pow requires 2 arguments,"
      "e.g., #pow (4,2), which would return 16.\n");
  }

  return return_value;
}

// accept a visitor
void
madara::expression::SystemCallPow::accept (
madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
