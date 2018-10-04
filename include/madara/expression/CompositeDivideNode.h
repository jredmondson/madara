/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_DIVIDE_NODE_H_
#define _MADARA_COMPOSITE_DIVIDE_NODE_H_

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
 * @class CompositeDivideNode
 * @brief A composite node that divides a left expression by a right one.
 */
class CompositeDivideNode : public CompositeBinaryNode
{
public:
  /**
   * Constructor
   * @param   logger the logger to use for printing
   * @param   left   left expression
   * @param   right  right expression
   **/
  CompositeDivideNode(
      logger::Logger& logger, ComponentNode* left, ComponentNode* right);

  /**
   * Destructor
   **/
  virtual ~CompositeDivideNode(void);

  /**
   * Returns the printable character of the node
   * @return    value of the node
   **/
  virtual madara::knowledge::KnowledgeRecord item(void) const;

  /**
   * Prunes the expression tree of unnecessary nodes.
   * @param     can_change   set to true if variable nodes are contained
   * @return    left expression divided by right expression. If right
   *            expression is zero, then a compile error is logged and
   *            the process exits.
   **/
  virtual madara::knowledge::KnowledgeRecord prune(bool& can_change);

  /**
   * Evaluates the expression tree.
   * @param     settings     settings for evaluating the node
   * @return    left expression divided by right expression. If right
   *            expression is zero, then a compile error is logged and
   *            the process exits.
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

#endif /* _MADARA_COMPOSITE_DIVIDE_NODE_H_ */
