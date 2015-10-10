
#ifndef _MADARA_NO_KARL_


#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallWriteFile.h"
#include "madara/expression/Visitor.h"


madara::expression::SystemCallWriteFile::SystemCallWriteFile (
  madara::knowledge::ThreadSafeContext & context,
  const ComponentNodes & nodes)
  : SystemCallNode (context, nodes)
{

}

// Dtor
madara::expression::SystemCallWriteFile::~SystemCallWriteFile (void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallWriteFile::item (void) const
{
  return madara::knowledge::KnowledgeRecord::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallWriteFile::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  madara::knowledge::KnowledgeRecord result;
  
  for (ComponentNodes::iterator i = nodes_.begin (); i != nodes_.end ();
       ++i)
  {
    bool arg_can_change = false;
    result = (*i)->prune (arg_can_change);
    
    if (!arg_can_change && dynamic_cast <LeafNode *> (*i) == 0)
    {
      delete *i;
      *i = new LeafNode (*(this->logger_), result);
    }
  }

  if (nodes_.size () != 2)
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL ERROR: System call write_file requires 2 arguments: "
      "a knowledge record and a file name\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::SystemCallWriteFile::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  knowledge::KnowledgeRecord return_value;

  if (nodes_.size () == 2)
  {
    // copying strings wastes execution time, so we hold the knowledge::KnowledgeRecord
    // instead of the resulting string filename.
    knowledge::KnowledgeRecord arg1 = nodes_[0]->evaluate (settings);
    knowledge::KnowledgeRecord arg2 = nodes_[1]->evaluate (settings);

    knowledge::KnowledgeRecord * filename = &arg1;
    knowledge::KnowledgeRecord * contents = &arg2;

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call write_file is attempting to open %s.\n",
      filename->to_string ().c_str ());

    ssize_t bytes_written = contents->to_file (filename->to_string ());

    if (bytes_written <= 0)
    {
      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "KARL ERROR: System call write_file could not write to %s\n",
        filename->to_string ().c_str ());

      return madara::knowledge::KnowledgeRecord::Integer (bytes_written);
    }
    else
    {
      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "System call write_file wrote %zd bytes to %s\n",
        bytes_written, filename->to_string ().c_str ());
    }
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL ERROR: System call write_file requires 2 arguments: "
      "a knowledge record and a file name\n");
  }

  return return_value;
}

// accept a visitor
void 
madara::expression::SystemCallWriteFile::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
