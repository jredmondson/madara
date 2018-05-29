
#ifndef _MADARA_NO_KARL_


#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallReadFile.h"
#include "madara/expression/Visitor.h"


madara::expression::SystemCallReadFile::SystemCallReadFile (
  madara::knowledge::ThreadSafeContext & context,
  const ComponentNodes & nodes)
  : SystemCallNode (context, nodes)
{

}

// Dtor
madara::expression::SystemCallReadFile::~SystemCallReadFile (void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallReadFile::item (void) const
{
  return madara::knowledge::KnowledgeRecord (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallReadFile::prune (bool & can_change)
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

  if (nodes_.size () == 0 || nodes_.size () > 2)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "madara::expression::SystemCallRandInt: "
      "KARL COMPILE ERROR:"
      "System call read_file"
      " requires at least a filename to read, e.g."
      " #read_file (filename), #read_file (filename, 'text')."
      " Second argument is to force a file type when the filename"
      " does not end with .txt, .xml, .jpg, etc. Can be 'text',"
      " 'jpeg', 'xml'\n");

    throw KarlException ("madara::expression::SystemCallRandInt: "
      "KARL COMPILE ERROR: "
      "System call read_file"
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
madara::knowledge::KnowledgeRecord 
madara::expression::SystemCallReadFile::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  knowledge::KnowledgeRecord return_value;

  if (nodes_.size () > 0)
  {
    // copying strings wastes execution time, so we hold the knowledge::KnowledgeRecord
    // instead of the resulting string filename.
    knowledge::KnowledgeRecord filename_eval = nodes_[0]->evaluate (settings);
    uint32_t read_as_type_uint (0);
    (void)read_as_type_uint;

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "madara::expression::SystemCallReadFile: "
      "System call read_file is attempting to open %s.\n",
      filename_eval.to_string ().c_str ());

    if (nodes_.size () == 2)
    {
      knowledge::KnowledgeRecord read_as_type = nodes_[1]->evaluate (settings);
      if (read_as_type.type () == knowledge::KnowledgeRecord::INTEGER)
      {
        read_as_type_uint = (uint32_t) read_as_type.to_integer ();
      }
      else if (read_as_type.is_string_type ())
      {
        std::string type = read_as_type.to_string ();
        if (type == "text")
        {
          read_as_type_uint = knowledge::KnowledgeRecord::TEXT_FILE;
        }
        else if (type == "jpeg")
        {
          read_as_type_uint = knowledge::KnowledgeRecord::IMAGE_JPEG;
        }
        else if (type == "xml")
        {
          read_as_type_uint = knowledge::KnowledgeRecord::XML;
        }
      }
    }

    if (0 != return_value.read_file (filename_eval.to_string ()))
    {
      madara_logger_ptr_log (logger_, logger::LOG_WARNING,
        "madara::expression::SystemCallReadFile: "
        "KARL RUNTIME WARNING: System call read_file could not open %s.\n",
        filename_eval.to_string ().c_str ());
    }
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "madara::expression::SystemCallReadFile: "
      "KARL COMPILE ERROR:"
      "System call read_file"
      " requires at least a filename to read, e.g."
      " #read_file (filename), #read_file (filename, 'text')."
      " Second argument is to force a file type when the filename"
      " does not end with .txt, .xml, .jpg, etc. Can be 'text',"
      " 'jpeg', 'xml'\n");

    throw KarlException ("madara::expression::SystemCallReadFile: "
      "KARL COMPILE ERROR: "
      "System call read_file"
      " requires at least a filename to read, e.g."
      " #read_file (filename), #read_file (filename, 'text')."
      " Second argument is to force a file type when the filename"
      " does not end with .txt, .xml, .jpg, etc. Can be 'text',"
      " 'jpeg', 'xml'\n");
  }

  return return_value;
}

// accept a visitor
void 
madara::expression::SystemCallReadFile::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
