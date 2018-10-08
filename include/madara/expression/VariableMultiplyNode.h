/* -*- C++ -*- */
#ifndef _MADARA_VARIABLE_MULTIPLY_NODE_H_
#define _MADARA_VARIABLE_MULTIPLY_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <vector>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/VariableNode.h"
#include "madara/expression/CompositeArrayReference.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
namespace expression
{
// Forward declarations.
class Visitor;

/**
 * @class VariableMultiplyNode
 * @brief Composite node that multiplies a variable by some right hand side
 */

class VariableMultiplyNode : public ComponentNode
{
public:
  /// Ctor.
  VariableMultiplyNode(ComponentNode* lhs,
      madara::knowledge::KnowledgeRecord value, ComponentNode* rhs,
      madara::knowledge::ThreadSafeContext& context);

  /// Dtor.
  virtual ~VariableMultiplyNode(void);

  /// Return the item stored in the node.
  virtual madara::knowledge::KnowledgeRecord item(void) const;

  /// Prune the tree of unnecessary nodes.
  /// Returns evaluation of the node and sets can_change appropriately.
  /// if this node can be changed, that means it shouldn't be pruned.
  virtual madara::knowledge::KnowledgeRecord prune(bool& can_change);

  /// Evaluates the node and its children. This does not prune any of
  /// the expression tree, and is much faster than the prune function
  virtual madara::knowledge::KnowledgeRecord evaluate(
      const madara::knowledge::KnowledgeUpdateSettings& settings);

  /// Define the @a accept() operation used for the Visitor pattern.
  virtual void accept(Visitor& visitor) const;

private:
  /// variable holder
  VariableNode* var_;

  /// variable index holder
  CompositeArrayReference* array_;

  /// amount to increment by. Note that this can also do decrement.
  madara::knowledge::KnowledgeRecord value_;

  /// holds a right hand side argument if it is not value_
  ComponentNode* rhs_;
};
}
}

#endif  // _MADARA_NO_KARL_

#endif /* _MADARA_VARIABLE_MULTIPLY_NODE_H_ */
