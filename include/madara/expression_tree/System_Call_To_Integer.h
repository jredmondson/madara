/* -*- C++ -*- */
#ifndef _SYSTEM_CALL_TO_INTEGER_H_
#define _SYSTEM_CALL_TO_INTEGER_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <stdexcept>
#include "madara/utility/stdint.h"
#include "madara/expression_tree/System_Call_Node.h"


namespace madara
{
  namespace expression_tree
  {
    // Forward declaration.
    class Visitor;

    /**
     * @class System_Call_To_Integer
     * @brief Converts an argument to an integer
     */
    class System_Call_To_Integer : public System_Call_Node
    {
    public:
      /**
       * Constructor
       **/
      System_Call_To_Integer (
        madara::knowledge::Thread_Safe_Context & context,
        const Component_Nodes & nodes);
      
      /**
       * Destructor
       **/
      virtual ~System_Call_To_Integer (void);

      /**
       * Returns the value of the node
       * @return    value of the node
       **/
      virtual madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of current contained expression tree
       **/
      virtual madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @return    value of current contained expression tree
       **/
      virtual madara::Knowledge_Record evaluate (
        const madara::knowledge::Knowledge_Update_Settings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _SYSTEM_CALL_TO_INTEGER_H_ */
