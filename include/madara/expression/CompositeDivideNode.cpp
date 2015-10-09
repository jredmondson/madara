/* -*- C++ -*- */
#ifndef _DIVIDE_NODE_CPP_
#define _DIVIDE_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression/CompositeBinaryNode.h"
#include "madara/expression/CompositeDivideNode.h"
#include "madara/expression/Visitor.h"
#include "madara/expression/LeafNode.h"


// Ctor
madara::expression::CompositeDivideNode::CompositeDivideNode (
  logger::Logger & logger,
  ComponentNode *left,
  ComponentNode *right)
: CompositeBinaryNode (logger, left, right)
{    
}

// Dtor
madara::expression::CompositeDivideNode::~CompositeDivideNode (void)
{
}

madara::KnowledgeRecord
madara::expression::CompositeDivideNode::item (void) const
{
  madara::KnowledgeRecord record;
  record.set_value ("/");
  return record;
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
madara::KnowledgeRecord
madara::expression::CompositeDivideNode::prune (bool & can_change)
{
  bool left_child_can_change = false;
  bool right_child_can_change = false;
  madara::KnowledgeRecord left_value;
  madara::KnowledgeRecord right_value;
  madara::KnowledgeRecord zero;

  if (this->left_)
  {
    left_value = this->left_->prune (left_child_can_change);
    if (!left_child_can_change && dynamic_cast <LeafNode *> (left_) == 0)
    {
      delete this->left_;
      this->left_ = new LeafNode (*(this->logger_), left_value);
    }
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: Division has no left expression\n");
    exit (-1);  
  }

  if (this->right_)
  {
    right_value = this->right_->prune (right_child_can_change);

    if (!right_child_can_change && dynamic_cast <LeafNode *> (right_) == 0)
    {
      // leave this check which is important
      if (right_value.is_false ())
      {
        madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
          "KARL COMPILE ERROR: Division results in permanent divide by zero\n");

        exit (-1);
      }
      // the only time we should delete right is if we have a clean division
      if (!left_child_can_change && 
          left_value % right_value == zero)
      {
        // don't worry about allocating anything. This is about to be
        // reclaimed anyway since !right_can_change and !left_can_change
        delete this->right_;
        this->right_ = 0;
      }
      else
        right_child_can_change = true;
    }

    //if (!right_child_can_change)
    //  right_child_can_change = true;
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: Division has no right expression (divide by zero)\n");
    exit (-1);
  }

  can_change = left_child_can_change || right_child_can_change;

  return left_value / right_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
madara::KnowledgeRecord 
madara::expression::CompositeDivideNode::evaluate (
const madara::knowledge::KnowledgeUpdateSettings & settings)
{
  // only evaluate right if left evaluates to non-zero (0/{any_number} = 0)
  madara::KnowledgeRecord lvalue (left_->evaluate (settings));
  madara::KnowledgeRecord zero;
  if (lvalue.is_true ())
    return lvalue / right_->evaluate (settings);

  // note that we are not handling divide by zero. Still unsure whether I
  // want to use exceptions throughout evaluation or not. This should be as
  // quick as possible. We should probably only try to catch exception like
  // things in prune

  return zero;
}

// accept a visitor
void 
madara::expression::CompositeDivideNode::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _DIVIDE_NODE_CPP_ */
