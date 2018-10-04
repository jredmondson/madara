/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_MODULUS_NODE_H_
#define _MADARA_COMPOSITE_MODULUS_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression/CompositeBinaryNode.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
namespace expression
{
class ComponentNode;
class Visitor;

/**
 * @class CompositeModulusNode
 * @brief A composite node that divides a left expression by a right
 *        expression and returns the remainder of the division
 */
class CompositeModulusNode : public CompositeBinaryNode
{
public:
  /**
   * Constructor
   * @param   logger the logger to use for printing
   * @param   left   left expression
   * @param   right  right expression
   **/
  CompositeModulusNode(
      logger::Logger& logger, ComponentNode* left, ComponentNode* right);

  /**
   * Destructor
   **/
  virtual ~CompositeModulusNode(void);

  /**
   * Returns the printable character of the node
   * @return    value of the node
   **/
  virtual madara::knowledge::KnowledgeRecord item(void) const;

  /**
   * Prunes the expression tree of unnecessary nodes.
   * @param     can_change   set to true if variable nodes are contained
   * @return    remainder of left expression divided by right expression.
   *            If right expression is zero, then a compile error is
   *            logged and the process exits.
   **/
  virtual madara::knowledge::KnowledgeRecord prune(bool& can_change);

  /**
   * Evaluates the expression tree.
   * @param     settings     settings for evaluating the node
   * @return    remainder of left expression divided by right expression.
   *            If right expression is zero, then a compile error is
   *            logged and the process exits.
   **/
  virtual madara::knowledge::KnowledgeRecord evaluate(
      const madara::knowledge::KnowledgeUpdateSettings& settings);

  /**
   * Accepts a visitor subclassed from the Visitor class
   * @param    visitor   visitor instance to use
   **/
  virtual void accept(Visitor& visitor) const;
};
}
}

#endif  // _MADARA_NO_KARL_

#endif /* _MADARA_COMPOSITE_MODULUS_NODE_H_ */
