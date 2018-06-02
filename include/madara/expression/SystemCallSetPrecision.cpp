
#ifndef _MADARA_NO_KARL_


#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallSetPrecision.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"


madara::expression::SystemCallSetPrecision::SystemCallSetPrecision (
  madara::knowledge::ThreadSafeContext & context,
  const ComponentNodes & nodes)
  : SystemCallNode (context, nodes)
{

}

// Dtor
madara::expression::SystemCallSetPrecision::~SystemCallSetPrecision (void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallSetPrecision::item (void) const
{
  return madara::knowledge::KnowledgeRecord (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallSetPrecision::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  madara::knowledge::KnowledgeRecord result;

  if (nodes_.size () > 0)
  {
    bool arg_can_change = false;
    result = nodes_[0]->prune (arg_can_change);
    
    if (!arg_can_change && dynamic_cast <LeafNode *> (nodes_[0]) == 0)
    {
      delete nodes_[0];
      nodes_[0] = new LeafNode (*(this->logger_), result);
    }
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::SystemCallSetPrecision::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  knowledge::KnowledgeRecord return_value;

  if (nodes_.size () > 0)
  {
    int64_t new_precision = nodes_[0]->evaluate (settings).to_integer ();
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "madara::expression::SystemCallSetPrecision: "
      "System call precision is setting the precision to %" PRId64 ".\n",
      new_precision);

    knowledge::KnowledgeRecord::set_precision ((int)new_precision);

    return knowledge::KnowledgeRecord (new_precision);
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "madara::expression::SystemCallSetPrecision: "
      "System call precision is returning the double precision.\n");

    return knowledge::KnowledgeRecord (
      knowledge::KnowledgeRecord::get_precision ());
  }

  return return_value;
}

// accept a visitor
void 
madara::expression::SystemCallSetPrecision::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
