/* -*- C++ -*- */
#ifndef _MADARA_SYSTEM_CALL_RAND_DOUBLE_H_
#define _MADARA_SYSTEM_CALL_RAND_DOUBLE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <stdexcept>
#include "madara/utility/stdint.h"
#include "madara/expression/SystemCallNode.h"


namespace madara
{
  namespace expression
  {
    // Forward declaration.
    class Visitor;

    /**
     * @class SystemCallRandDouble
     * @brief Returns a random double
     */
    class SystemCallRandDouble : public SystemCallNode
    {
    public:
      /**
       * Constructor
       **/
      SystemCallRandDouble (
        madara::knowledge::ThreadSafeContext & context,
        const ComponentNodes & nodes);
      
      /**
       * Destructor
       **/
      virtual ~SystemCallRandDouble (void);

      /**
       * Returns the value of the node
       * @return    value of the node
       **/
      virtual madara::knowledge::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of current contained expression tree
       **/
      virtual madara::knowledge::KnowledgeRecord prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @return    value of current contained expression tree
       **/
      virtual madara::knowledge::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_SYSTEM_CALL_RAND_DOUBLE_H_ */
