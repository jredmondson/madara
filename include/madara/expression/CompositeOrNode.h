#ifndef _MADARA_COMPOSITE_OR_NODE_H_
#define _MADARA_COMPOSITE_OR_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression/CompositeTernaryNode.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
namespace expression
{
// Forward declaration.
class ComponentNode;
class Visitor;

/**
 * @class CompositeOrNode
 * @brief A composite node that performs a logical or
 */
class CompositeOrNode : public CompositeTernaryNode
{
public:
  /**
   * Constructor
   * @param   logger the logger to use for printing
   * @param   nodes  2+ nodes to be executed in sequence
   **/
  CompositeOrNode(logger::Logger& logger, const ComponentNodes& nodes);

  /**
   * Returns the printable character of the node
   * @return    value of the node
   **/
  virtual madara::knowledge::KnowledgeRecord item(void) const;

  /**
   * Prunes the expression tree of unnecessary nodes.
   * @param     can_change   set to true if variable nodes are contained
   * @return    logical or of the left and right expression. Will not
   *            evaluate right expression if left is true.
   **/
  virtual madara::knowledge::KnowledgeRecord prune(bool& can_change);

  /**
   * Evaluates the node.
   * @param     settings     settings for evaluating the node
   * @return    logical or of the left and right expression. Will not
   *            evaluate right expression if left is true.
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

#endif  // _MADARA_COMPOSITE_OR_NODE_H_
