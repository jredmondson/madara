
#ifndef _MADARA_NO_KARL_


#include "madara/expression/Leaf_Node.h"
#include "madara/expression/System_Call_To_String.h"
#include "madara/knowledge/Thread_Safe_Context.h"
#include "madara/expression/Visitor.h"


madara::expression::System_Call_To_String::System_Call_To_String (
  madara::knowledge::Thread_Safe_Context & context,
  const Component_Nodes & nodes)
  : System_Call_Node (context, nodes)
{

}

// Dtor
madara::expression::System_Call_To_String::~System_Call_To_String (void)
{
}

madara::Knowledge_Record
madara::expression::System_Call_To_String::item (void) const
{
  return madara::Knowledge_Record::Integer (nodes_.size ());
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression::System_Call_To_String::prune (bool & can_change)
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

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression::System_Call_To_String::evaluate (
const madara::knowledge::Knowledge_Update_Settings & settings)
{
  madara::Knowledge_Record result;

  if (nodes_.size () > 0)
  {
    std::string delimiter = ", ";

    if (nodes_.size () > 1)
    {
      // user wants to use a special delimiter
      delimiter = nodes_[1]->evaluate (settings).to_string (delimiter);

      if (nodes_.size () > 2)
      {
        madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
          "KARL RUNTIME ERROR: System call to_string"
          " may have up to 2 arguments. First is a value to change to string."
          " An optional second is a delimiter for array stringification\n");
      }
    }

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call to_string is converting an argument\n");

    result = nodes_[0]->evaluate (settings).to_string (delimiter);
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "System call to_string is converting 0\n");
  }

  return result;
}

// accept a visitor
void 
madara::expression::System_Call_To_String::accept (
  madara::expression::Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_
