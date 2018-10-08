/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_CONST_ARRAY_H_
#define _MADARA_COMPOSITE_CONST_ARRAY_H_

#ifndef _MADARA_NO_KARL_

#include <vector>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/expression/CompositeTernaryNode.h"
#include "madara/knowledge/Functions.h"
#include "madara/knowledge/ThreadSafeContext.h"

namespace madara
{
namespace expression
{
class ComponentNode;
class Visitor;

/**
 * @class CompositeConstArray
 * @brief A composite node that contains an array of values
 */
class CompositeConstArray : public CompositeTernaryNode
{
public:
  /**
   * Constructor
   * @param   logger the logger to use for printing
   * @param   nodes    arguments to the function
   **/
  CompositeConstArray(logger::Logger& logger, const ComponentNodes& nodes);

  /**
   * Destructor
   **/
  virtual ~CompositeConstArray(void);

  /**
   * Returns the printable character of the node
   * @return    value of the node
   **/
  virtual madara::knowledge::KnowledgeRecord item(void) const;

  /**
   * Prunes the expression tree of unnecessary nodes.
   * @param     can_change   set to true if variable nodes are contained
   * @return    negation of the right expression
   **/
  virtual madara::knowledge::KnowledgeRecord prune(bool& can_change);

  /**
   * Evaluates the expression tree.
   * @param     settings     settings for evaluating the node
   * @return    negation of the right expression
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

#endif /* _MADARA_COMPOSITE_CONST_ARRAY_H_ */
