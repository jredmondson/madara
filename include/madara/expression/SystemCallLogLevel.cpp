
#ifndef _MADARA_NO_KARL_


#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallLogLevel.h"
#include "madara/expression/Visitor.h"


madara::expression::SystemCallLogLevel::SystemCallLogLevel (
  madara::knowledge::ThreadSafeContext & context,
  const ComponentNodes & nodes)
  : SystemCallNode (context, nodes)
{

}

// Dtor
madara::expression::SystemCallLogLevel::~SystemCallLogLevel (void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallLogLevel::item (void) const
{
  return madara::knowledge::KnowledgeRecord (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallLogLevel::prune (bool & can_change)
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
  
  if (nodes_.size () > 1)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "KARL COMPILE ERROR: System call log_level requires 0 or 1 arguments,"
      " e.g., log_level (10) or log_level ()--which returns the log level\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::SystemCallLogLevel::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  knowledge::KnowledgeRecord return_value;

  if (nodes_.size () == 1)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call log_level is setting log level\n");

    int level = (int)nodes_[0]->evaluate (settings).to_integer ();

    logger_->set_level (level);

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call log_level has set log level to %d\n",
      level);

    return knowledge::KnowledgeRecord (level);
  }
  else if (nodes_.size () == 0)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call log_level is returning log level.\n");
    
    return knowledge::KnowledgeRecord (logger_->get_level ());
  }
  else if (nodes_.size () > 1)
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL RUNTIME ERROR: System call log_level requires 0 or 1 arguments,"
      " e.g., log_level (10) or log_level ()--which returns the log level\n");
  }

  return return_value;
}

// accept a visitor
void 
madara::expression::SystemCallLogLevel::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
