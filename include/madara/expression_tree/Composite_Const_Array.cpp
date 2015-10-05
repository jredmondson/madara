/* -*- C++ -*- */
#ifndef _FUNCTION_NODE_CPP_
#define _FUNCTION_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>
#include <sstream>

#include "madara/knowledge/Knowledge_Record.h"
#include "madara/expression_tree/Component_Node.h"
#include "madara/expression_tree/Composite_Unary_Node.h"
#include "madara/expression_tree/Visitor.h"
#include "madara/expression_tree/Composite_Const_Array.h"
#include "madara/expression_tree/Leaf_Node.h"


#include "madara/knowledge/Functions.h"
#include "madara/knowledge/Extern_Function_Variables.h"

#ifdef _MADARA_PYTHON_CALLBACKS_

  #include <boost/python/call.hpp> 

#endif

#ifdef _MADARA_JAVA_

#include "madara_jni.h"

#endif

// Ctor

madara::expression_tree::Composite_Const_Array::Composite_Const_Array (
  logger::Logger & logger, 
  const Component_Nodes & nodes)
: madara::expression_tree::Composite_Ternary_Node (logger, nodes)
{
  
}

// Dtor
madara::expression_tree::Composite_Const_Array::~Composite_Const_Array (void)
{
}

madara::Knowledge_Record
madara::expression_tree::Composite_Const_Array::item (void) const
{
  madara::Knowledge_Record record;
  return record;
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::Knowledge_Record
madara::expression_tree::Composite_Const_Array::prune (bool & can_change)
{
  can_change = false;
  
  Knowledge_Record result;
  madara::knowledge::Function_Arguments args;
  args.resize (nodes_.size ());

  bool is_double (false);
  Component_Nodes::size_type i = 0;

  for (; i < nodes_.size (); ++i)
  {
    bool arg_can_change = false;
    args[i] = nodes_[i]->prune (arg_can_change);

    // we cannot initialize the array until we know if there are any doubles
    if (args[i].type () == Knowledge_Record::DOUBLE)
    {
      is_double = true;
    }

    if (!arg_can_change && dynamic_cast <Leaf_Node *> (nodes_[i]) == 0)
    {
      delete nodes_[i];
      nodes_[i] = new Leaf_Node (*(this->logger_), result);
    }
    else if (arg_can_change)
    {
      can_change = true;
    }
  }
  
  // if we are looking at a static const array, then leafize it
  if (i > 0)
  {
    result.clear_value ();

    if (is_double)
      result.set_index (i - 1, args[i - 1].to_double ());
    else
      result.set_index (i - 1, args[i - 1].to_integer ());

    for (i = 0; i < args.size () - 1; ++i)
    {
      if (is_double)
        result.set_index (i, args[i].to_double ());
      else
        result.set_index (i, args[i].to_integer ());
    }
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: Array initialized with no elements\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::Knowledge_Record 
madara::expression_tree::Composite_Const_Array::evaluate (
const madara::knowledge::Knowledge_Update_Settings & settings)
{
  madara::knowledge::Function_Arguments args;
  madara::Knowledge_Record result;

  args.resize (nodes_.size ());

  bool is_double (false);

  int j = 0;

  for (Component_Nodes::iterator i = nodes_.begin (); i != nodes_.end ();
       ++i, ++j)
  {
    args[j] = (*i)->evaluate (settings);

    // we cannot initialize the array until we know if there are any doubles
    if (args[j].type () == Knowledge_Record::DOUBLE)
    {
      is_double = true;
    }
    else if (args[j].type () != Knowledge_Record::INTEGER)
    {
      args[j] = (*i)->evaluate (settings).to_integer ();
    }
  }

  // check if we have any args or if this was a poorly done array
  if (args.size () > 0)
  {
    // set last element first so there is only one resize
    if (is_double)
      result.set_index (j - 1, args[j - 1].to_double ());
    else
      result.set_index (j - 1, args[j - 1].to_integer ());

    for (size_t i = 0; i < args.size () - 1; ++i)
    {
      if (is_double)
        result.set_index (i, args[i].to_double ());
      else
        result.set_index (i, args[i].to_integer ());
    }
  }

  return result;
}

// accept a visitor
void 
madara::expression_tree::Composite_Const_Array::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _FUNCTION_NODE_CPP_ */
