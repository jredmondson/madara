/* -*- C++ -*- */
#ifndef _MADARA_SYSTEM_CALL_DELETE_VARIABLE_H_
#define _MADARA_SYSTEM_CALL_DELETE_VARIABLE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <stdexcept>
#include "madara/utility/StdInt.h"
#include "madara/expression/SystemCallNode.h"

namespace madara
{
namespace expression
{
// Forward declaration.
class Visitor;

/**
 * @class SystemCallDeleteVariable
 * @brief Attempts to delete a variable. This is an extremely
 *        dangerous function call that will break any compiled
 *        expression. Do not use this unless you are not going
 *        to use compiled expressions that use this variable.
 */
class SystemCallDeleteVariable : public SystemCallNode
{
public:
  /**
   * Constructor
   **/
  SystemCallDeleteVariable(madara::knowledge::ThreadSafeContext& context,
      const ComponentNodes& nodes);

  /**
   * Destructor
   **/
  virtual ~SystemCallDeleteVariable(void);

  /**
   * Returns the value of the node
   * @return    value of the node
   **/
  virtual madara::knowledge::KnowledgeRecord item(void) const;

  /**
   * Prunes the expression tree of unnecessary nodes.
   * @param     can_change   set to true if variable nodes are contained
   * @return    value of current contained expression tree
   **/
  virtual madara::knowledge::KnowledgeRecord prune(bool& can_change);

  /**
   * Evaluates the expression tree.
   * @return    value of current contained expression tree
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

#endif /* _MADARA_SYSTEM_CALL_DELETE_VARIABLE_H_ */
