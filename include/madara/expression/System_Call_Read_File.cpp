
#ifndef _MADARA_NO_KARL_


#include "madara/expression/Leaf_Node.h"
#include "madara/expression/System_Call_Read_File.h"
#include "madara/expression/Visitor.h"


madara::expression::System_Call_Read_File::System_Call_Read_File (
  madara::knowledge::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
madara::expression::System_Call_Read_File::~System_Call_Read_File (void)
{
}

madara::Knowledge_Record
madara::expression::System_Call_Read_File::item (void) const
{
  return madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression::System_Call_Read_File::prune (bool & can_change)
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

  if (nodes_.size () == 0 || nodes_.size () > 2)
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: System call read_file"
      " requires at least a filename to read, e.g."
      " #read_file (filename), #read_file (filename, 'text')."
      " Second argument is to force a file type when the filename"
      " does not end with .txt, .xml, .jpg, etc. Can be 'text',"
      " 'jpeg', 'xml'\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression::System_Call_Read_File::evaluate (
const madara::knowledge::Knowledge_Update_Settings & settings)
{
  Knowledge_Record return_value;

  if (nodes_.size () > 0)
  {
    // copying strings wastes execution time, so we hold the Knowledge_Record
    // instead of the resulting string filename.
    Knowledge_Record filename_eval = nodes_[0]->evaluate (settings);
    uint32_t read_as_type_uint (0);

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call read_file is attempting to open %s.\n",
      filename_eval.to_string ().c_str ());

    if (nodes_.size () == 2)
    {
      Knowledge_Record read_as_type = nodes_[1]->evaluate (settings);
      if (read_as_type.type () == Knowledge_Record::INTEGER)
      {
        read_as_type_uint = (uint32_t) read_as_type.to_integer ();
      }
      else if (read_as_type.is_string_type ())
      {
        std::string type = read_as_type.to_string ();
        if (type == "text")
        {
          read_as_type_uint = Knowledge_Record::TEXT_FILE;
        }
        else if (type == "jpeg")
        {
          read_as_type_uint = Knowledge_Record::IMAGE_JPEG;
        }
        else if (type == "xml")
        {
          read_as_type_uint = Knowledge_Record::XML;
        }
      }
    }

    if (0 != return_value.read_file (filename_eval.to_string ()))
    {
      madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
        "KARL RUNTIME ERROR: System call read_file could not open %s.\n",
        filename_eval.to_string ().c_str ());
    }
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL RUNTIME ERROR: System call read_file"
      " requires at least a filename to read, e.g."
      " #read_file (filename), #read_file (filename, 'text')."
      " Second argument is to force a file type when the filename"
      " does not end with .txt, .xml, .jpg, etc. Can be 'text',"
      " 'jpeg', 'xml'");
  }

  return return_value;
}

// accept a visitor
void 
madara::expression::System_Call_Read_File::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
