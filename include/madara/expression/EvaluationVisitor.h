/* -*- C++ -*- */
#ifndef _MADARA_EVALUATION_VISITOR_H_
#define _MADARA_EVALUATION_VISITOR_H_

#ifdef _USE_VISITORS_

#include "madara/expression/Visitor.h"
#include "madara/utility/LStack.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
namespace expression
{
/**
 * @class EvaluationVisitor
 * @brief This plays the role of a visitor for evaluating
 *        nodes in an expression tree that is being iterated in
 *        post-order fashion (and does not work correctly with any
 *        other iterator).
 */
class EvaluationVisitor : public Visitor
{
public:
  /**
   * Evaluates a constant
   * @param     node  the node to evaluate
   **/
  virtual void visit(const LeafNode& node);

  /**
   * Evaluates a variable
   * @param     node  the node to evaluate
   **/
  virtual void visit(const VariableNode& node);

  /**
   * Evaluates a variable decrementer node
   * @param     node  the node to evaluate
   **/
  virtual void visit(const VariableDecrementNode& node);

  /**
   * Evaluates a variable divider node
   * @param     node  the node to evaluate
   **/
  virtual void visit(const VariableDivideNode& node);

  /**
   * Evaluates a variable incrementer node
   * @param     node  the node to evaluate
   **/
  virtual void visit(const VariableIncrementNode& node);

  /**
   * Evaluates a variable multiplier node
   * @param     node  the node to evaluate
   **/
  virtual void visit(const VariableMultiplyNode& node);

  /**
   * Evaluates a variable comparison node
   * @param     node  the node to evaluate
   **/
  virtual void visit(const VariableCompareNode& node);

  /**
   * Evaluates a list
   * @param     node  the node to evaluate
   **/
  virtual void visit(const ListNode& node);

  /**
   * Evaluates an integer negation
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeNegateNode& node);

  /**
   * Evaluates a predecrement
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositePredecrementNode& node);

  /**
   * Evaluates a preincrement
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositePreincrementNode& node);

  /**
   * Evaluates a logical not
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeNotNode& node);

  /**
   * Evaluates an addition
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeAddNode& node);

  /**
   * Evaluates an assignment to a variable
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeAssignmentNode& node);

  /**
   * Evaluates a logical and
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeAndNode& node);

  /**
   * Evaluates a logical or
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeOrNode& node);

  /**
   * Evaluates an equality operation
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeEqualityNode& node);

  /**
   * Evaluates an inequality operation
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeInequalityNode& node);

  /**
   * Evaluates a greater-than-or-equal-to operation
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeGreaterThanEqualNode& node);

  /**
   * Evaluates a less-than operation
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeGreaterThanNode& node);

  /**
   * Evaluates a less-than-or-equal-to operation
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeLessThanEqualNode& node);

  /**
   * Evaluates a less-than operation
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeLessThanNode& node);

  /**
   * Evaluates a subtraction
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeSubtractNode& node);

  /**
   * Evaluates a division
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeDivideNode& node);

  /**
   * Evaluates a multiplication
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeMultiplyNode& node);

  /**
   * Evaluates a remainder operation
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeModulusNode& node);

  /**
   * Evaluates a left and right expression
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeBothNode& node);

  /**
   * Evaluates a left and right expression
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeSequentialNode& node);

  /**
   * Evaluates a function
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeFunctionNode& node);

  /**
   * Evaluates a for loop
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeForLoop& node);

  /**
   * Evaluates an inference rule
   * @param     node  the node to evaluate
   **/
  virtual void visit(const CompositeImpliesNode& node);

  /**
   * Returns the result of the evaluation
   * @return    result of the evaluation
   **/
  madara::knowledge::KnowledgeRecord::Integer total(void);

  /**
   * Resets the evaluation
   **/
  void reset(void);

private:
  /// Stack used for temporarily storing evaluations.
  madara::utility::LStack<madara::knowledge::KnowledgeRecord> stack_;
};
}
}

#endif  // _USE_VISITORS_

#endif /* _MADARA_EVALUATION_VISITOR_H_ */
