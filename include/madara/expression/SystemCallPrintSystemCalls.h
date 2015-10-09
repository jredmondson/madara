/* -*- C++ -*- */
#ifndef _MADARA_SYSTEM_CALL_PRINT_SYSTEM_CALLS_H_
#define _MADARA_SYSTEM_CALL_PRINT_SYSTEM_CALLS_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <map>
#include <stdexcept>
#include "madara/utility/stdint.h"
#include "madara/expression/SystemCallNode.h"


namespace madara
{
  namespace expression
  {
    typedef std::map <std::string, std::string> SystemCallsHelp;  

    // Forward declaration.
    class Visitor;

    /**
     * @class SystemCallPrintSystemCalls
     * @brief Prints all supported system calls
     */
    class SystemCallPrintSystemCalls : public SystemCallNode
    {
    public:
      /**
       * Constructor
       **/
      SystemCallPrintSystemCalls (
        madara::knowledge::ThreadSafeContext & context,
        const ComponentNodes & nodes);
      
      /**
       * Destructor
       **/
      virtual ~SystemCallPrintSystemCalls (void);

      /**
       * Returns the value of the node
       * @return    value of the node
       **/
      virtual madara::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of current contained expression tree
       **/
      virtual madara::KnowledgeRecord prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @return    value of current contained expression tree
       **/
      virtual madara::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;
    private:
      // calls help menu
      static SystemCallsHelp calls_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_SYSTEM_CALL_PRINT_SYSTEM_CALLS_H_ */
