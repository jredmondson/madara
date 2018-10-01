#ifndef _MADARA_COMPOSITE_ASSIGNMENT_NODE_H_
#define _MADARA_COMPOSITE_ASSIGNMENT_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression/CompositeBinaryNode.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/expression/VariableNode.h"
#include "madara/expression/CompositeArrayReference.h"

namespace madara
{
namespace expression
{
// Forward declaration.
class ComponentNode;
class Visitor;

/**
 * @class CompositeAssignmentNode
 * @brief A composite node that allows for variable assignment.
 *        Left child should be a variable and right child should
 *        be an expression to evaluate
 */
class CompositeAssignmentNode : public CompositeUnaryNode
{
public:
  /**
   * Constructor
   * @param   logger the logger to use for printing
   * @param   left   variable to assign the right expression to
   * @param   right  right expression to assign to the variable
   **/
  CompositeAssignmentNode(
      logger::Logger& logger, ComponentNode* left, ComponentNode* right);

  /**
   * Returns the printable character of the node
   * @return    value of the node
   **/
  virtual madara::knowledge::KnowledgeRecord item(void) const;

  /**
   * Prunes the expression tree of unnecessary nodes.
   * @param     can_change   set to true if variable nodes are contained
   * @return    value of evaluated right expression
   **/
  virtual madara::knowledge::KnowledgeRecord prune(bool& can_change);

  /**
   * Evaluates the expression tree.
   * @param     settings     settings for evaluating the node
   * @return    value of evaluated right expression
   **/
  virtual madara::knowledge::KnowledgeRecord evaluate(
      const madara::knowledge::KnowledgeUpdateSettings& settings);

  /**
   * Accepts a visitor subclassed from the Visitor class
   * @param    visitor   visitor instance to use
   **/
  virtual void accept(Visitor& visitor) const;

private:
  /**
   * Left should always be a variable node. Using VariableNode
   * instead of having to dynamic cast a Symbol * should be much
   * fater.
   **/
  VariableNode* var_;
  CompositeArrayReference* array_;
};
}
}

#endif  // _MADARA_NO_KARL_

#endif  //_MADARA_COMPOSITE_ASSIGNMENT_NODE_H_
