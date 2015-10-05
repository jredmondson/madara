
#ifndef _MADARA_NO_KARL_


#include "madara/expression_tree/Leaf_Node.h"
#include "madara/expression_tree/System_Call_Write_File.h"
#include "madara/expression_tree/Visitor.h"


madara::expression_tree::System_Call_Write_File::System_Call_Write_File (
  madara::knowledge::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
madara::expression_tree::System_Call_Write_File::~System_Call_Write_File (void)
{
}

madara::Knowledge_Record
madara::expression_tree::System_Call_Write_File::item (void) const
{
  return madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression_tree::System_Call_Write_File::prune (bool & can_change)
{
  // user can always change a function, and we have no control over
  // what it does. Consequently, a function node cannot be pruned out
  // under any situation
  can_change = true;
  
  madara::Knowledge_Record result;
  
  for (Component_Nodes::iterator i = nodes_.begin (); i != nodes_.end ();
       ++i)
  {
    bool arg_can_change = false;
    result = (*i)->prune (arg_can_change);
    
    if (!arg_can_change && dynamic_cast <Leaf_Node *> (*i) == 0)
    {
      delete *i;
      *i = new Leaf_Node (*(this->logger_), result);
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
madara::Knowledge_Record 
madara::expression_tree::System_Call_Write_File::evaluate (
const madara::knowledge::Knowledge_Update_Settings & settings)
{
  Knowledge_Record return_value;

  if (nodes_.size () == 2)
  {
    // copying strings wastes execution time, so we hold the Knowledge_Record
    // instead of the resulting string filename.
    Knowledge_Record arg1 = nodes_[0]->evaluate (settings);
    Knowledge_Record arg2 = nodes_[1]->evaluate (settings);

    Knowledge_Record * filename = &arg1;
    Knowledge_Record * contents = &arg2;

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call write_file is attempting to open %s.\n",
      filename->to_string ().c_str ());

    ssize_t bytes_written = contents->to_file (filename->to_string ());

    if (bytes_written <= 0)
    {
      madara_logger_ptr_log (logger_, logger::LOG_MINOR,
        "KARL ERROR: System call write_file could not write to %s\n",
        filename->to_string ().c_str ());

      return madara::Knowledge_Record::Integer (bytes_written);
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
madara::expression_tree::System_Call_Write_File::accept (
  madara::expression_tree::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
