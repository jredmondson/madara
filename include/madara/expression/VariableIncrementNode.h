/* -*- C++ -*- */
#ifndef _MADARA_VARIABLE_INCREMENT_NODE_H_
#define _MADARA_VARIABLE_INCREMENT_NODE_H_

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
 * @class VariableIncrementNode
 * @brief Defines a terminal node of that references the current
 *        value stored in a variable.
 */

class VariableIncrementNode : public ComponentNode
{
public:
  /// Ctor.
  VariableIncrementNode(ComponentNode* lhs,
      madara::knowledge::KnowledgeRecord value, ComponentNode* rhs,
      madara::knowledge::ThreadSafeContext& context);

  /// Dtor.
  virtual ~VariableIncrementNode(void);

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

  /// Expands the key (if necessary). This allow for keys to be defined
  /// with other variables inserted (e.g. var{.id} with .id = 2 expands
  /// to var2)
  std::string expand_key(void) const;

  /// Return the variable key.
  const std::string& key(void) const;

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

  madara::knowledge::ThreadSafeContext& context_;

  /// Reference to context for variable retrieval
};
}
}

#endif  // _MADARA_NO_KARL_

#endif /* _MADARA_VARIABLE_INCREMENT_NODE_H_ */
