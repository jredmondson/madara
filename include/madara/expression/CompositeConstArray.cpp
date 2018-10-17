/* -*- C++ -*- */
#ifndef _FUNCTION_NODE_CPP_
#define _FUNCTION_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>
#include <sstream>

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/expression/ComponentNode.h"
#include "madara/expression/CompositeUnaryNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/CompositeConstArray.h"
#include "madara/expression/LeafNode.h"

#include "madara/knowledge/Functions.h"
#include "madara/knowledge/Variables.h"

#ifdef _MADARA_PYTHON_CALLBACKS_

#include <boost/python/call.hpp>

#endif

#ifdef _MADARA_JAVA_

#include "madara_jni.h"

#endif

// Ctor

madara::expression::CompositeConstArray::CompositeConstArray(
    logger::Logger& logger, const ComponentNodes& nodes)
  : madara::expression::CompositeTernaryNode(logger, nodes)
{
}

// Dtor
madara::expression::CompositeConstArray::~CompositeConstArray(void) {}

madara::knowledge::KnowledgeRecord
madara::expression::CompositeConstArray::item(void) const
{
  madara::knowledge::KnowledgeRecord record;
  return record;
}

/// Prune the tree of unnecessary nodes.
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::knowledge::KnowledgeRecord
madara::expression::CompositeConstArray::prune(bool& can_change)
{
  can_change = false;

  knowledge::KnowledgeRecord result;
  madara::knowledge::FunctionArguments args;
  args.resize(nodes_.size());

  bool is_double(false);
  ComponentNodes::size_type i = 0;

  for(; i < nodes_.size(); ++i)
  {
    bool arg_can_change = false;
    args[i] = nodes_[i]->prune(arg_can_change);

    // we cannot initialize the array until we know if there are any doubles
    if(args[i].type() == knowledge::KnowledgeRecord::DOUBLE)
    {
      is_double = true;
    }

    if(!arg_can_change && dynamic_cast<LeafNode*>(nodes_[i]) == 0)
    {
      delete nodes_[i];
      nodes_[i] = new LeafNode(*(this->logger_), result);
    }
    else if(arg_can_change)
    {
      can_change = true;
    }
  }

  // if we are looking at a static const array, then leafize it
  if(i > 0)
  {
    result.clear_value();

    if(is_double)
      result.set_index(i - 1, args[i - 1].to_double());
    else
      result.set_index(i - 1, args[i - 1].to_integer());

    for(i = 0; i < args.size() - 1; ++i)
    {
      if(is_double)
        result.set_index(i, args[i].to_double());
      else
        result.set_index(i, args[i].to_integer());
    }
  }
  else
  {
    madara_logger_ptr_log(logger_, logger::LOG_ERROR,
        "KARL COMPILE ERROR: Array initialized with no elements\n");
  }

  return result;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::knowledge::KnowledgeRecord
madara::expression::CompositeConstArray::evaluate(
    const madara::knowledge::KnowledgeUpdateSettings& settings)
{
  madara::knowledge::FunctionArguments args;
  madara::knowledge::KnowledgeRecord result;

  args.resize(nodes_.size());

  bool is_double(false);

  int j = 0;

  for(ComponentNodes::iterator i = nodes_.begin(); i != nodes_.end(); ++i, ++j)
  {
    args[j] = (*i)->evaluate(settings);

    // we cannot initialize the array until we know if there are any doubles
    if(args[j].type() == knowledge::KnowledgeRecord::DOUBLE)
    {
      is_double = true;
    }
    else if(args[j].type() != knowledge::KnowledgeRecord::INTEGER)
    {
      args[j] =
          knowledge::KnowledgeRecord((*i)->evaluate(settings).to_integer());
    }
  }

  // check if we have any args or if this was a poorly done array
  if(args.size() > 0)
  {
    // set last element first so there is only one resize
    if(is_double)
      result.set_index(j - 1, args[j - 1].to_double());
    else
      result.set_index(j - 1, args[j - 1].to_integer());

    for(size_t i = 0; i < args.size() - 1; ++i)
    {
      if(is_double)
        result.set_index(i, args[i].to_double());
      else
        result.set_index(i, args[i].to_integer());
    }
  }

  return result;
}

// accept a visitor
void madara::expression::CompositeConstArray::accept(Visitor& visitor) const
{
  visitor.visit(*this);
}

#endif  // _MADARA_NO_KARL_

#endif /* _FUNCTION_NODE_CPP_ */
