/* -*- C++ -*- */
#ifndef _MADARA_SYSTEM_CALL_PRINT_H_
#define _MADARA_SYSTEM_CALL_PRINT_H_

#ifndef _MADARA_NO_KARL_

/**
 * @file SystemCallPrint.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the SystemCallPrint class which prints a
 * Knowledge Record to stderr.
 **/

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
 * @class SystemCallPrint
 * @brief Prints a Knowledge Record
 */
class SystemCallPrint : public SystemCallNode
{
public:
  /**
   * Constructor
   **/
  SystemCallPrint(madara::knowledge::ThreadSafeContext& context,
      const ComponentNodes& nodes);

  /**
   * Destructor
   **/
  virtual ~SystemCallPrint(void);

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

#endif /* _MADARA_SYSTEM_CALL_PRINT_H_ */
