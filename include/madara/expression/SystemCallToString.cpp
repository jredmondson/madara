
#ifndef _MADARA_NO_KARL_


#include "madara/expression/LeafNode.h"
#include "madara/expression/SystemCallToString.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/expression/Visitor.h"


madara::expression::SystemCallToString::SystemCallToString (
  madara::knowledge::ThreadSafeContext & context,
  const ComponentNodes & nodes)
  : SystemCallNode (context, nodes)
{

}

// Dtor
madara::expression::SystemCallToString::~SystemCallToString (void)
{
}

madara::knowledge::KnowledgeRecord
madara::expression::SystemCallToString::item (void) const
{
  return madara::knowledge::KnowledgeRecord (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::SystemCallToString::prune (bool & can_change)
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

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord 
madara::expression::SystemCallToString::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  madara::knowledge::KnowledgeRecord result;

  if (nodes_.size () > 0)
  {
    std::string delimiter = ", ";

    if (nodes_.size () > 1)
    {
      // user wants to use a special delimiter
      delimiter = nodes_[1]->evaluate (settings).to_string (delimiter);

      if (nodes_.size () > 2)
      {
        madara_logger_ptr_log (logger_, logger::LOG_ERROR,
          "madara::expression::SystemCallToString: "
          "KARL RUNTIME ERROR:"
          "System call to_string"
          " may have up to 2 arguments. First is a value to change to string."
          " An optional second is a delimiter for array stringification\n");

        throw KarlException ("madara::expression::SystemCallToString: "
          "KARL RUNTIME ERROR: "
          "System call to_string"
          " may have up to 2 arguments. First is a value to change to string."
          " An optional second is a delimiter for array stringification\n");
      }
    }

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "madara::expression::SystemCallToString: "
      "System call to_string is converting an argument\n");

    result = knowledge::KnowledgeRecord (
        nodes_[0]->evaluate (settings).to_string (delimiter));
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "madara::expression::SystemCallToString: "
      "KARL RUNTIME ERROR:"
      "System call to_string requires an argument\n");

    throw KarlException ("madara::expression::SystemCallToString: "
      "KARL RUNTIME ERROR: "
      "System call to_string requires an argument\n");

    result.set_value ("");
  }

  return result;
}

// accept a visitor
void 
madara::expression::SystemCallToString::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
