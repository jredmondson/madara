/* -*- C++ -*- */
#ifndef _MADARA_SYSTEM_CALL_H_
#define _MADARA_SYSTEM_CALL_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <stdexcept>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/utility/stdint.h"
#include "madara/expression/CompositeTernaryNode.h"


namespace madara
{
  namespace expression
  {
    // Forward declaration.
    class Visitor;

    /**
     * @class SystemCallNode
     * @brief Interface for a MADARA system call
     */
    class SystemCallNode : public CompositeTernaryNode
    {
    public:
      /**
       * Constructor
       **/
      SystemCallNode (
        madara::knowledge::ThreadSafeContext & context);

      /**
       * Constructor
       **/
      SystemCallNode (
        madara::knowledge::ThreadSafeContext & context,
        const ComponentNodes & nodes);
      
      /**
       * Destructor
       **/
      virtual ~SystemCallNode (void);

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
      virtual madara::knowledge::KnowledgeRecord prune (bool & can_change)
        = 0;

      /** 
       * Evaluates the expression tree. 
       * @return    value of current contained expression tree
       **/
      virtual madara::knowledge::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings)
        = 0;

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

    protected:
      
      madara::knowledge::ThreadSafeContext & context_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_SYSTEM_CALL_H_ */
