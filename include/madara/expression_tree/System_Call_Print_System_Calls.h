/* -*- C++ -*- */
#ifndef _SYSTEM_CALL_PRINT_SYSTEM_CALLS_H_
#define _SYSTEM_CALL_PRINT_SYSTEM_CALLS_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <map>
#include <stdexcept>
#include "madara/utility/stdint.h"
#include "madara/expression_tree/System_Call_Node.h"


namespace madara
{
  namespace expression_tree
  {
    typedef std::map <std::string, std::string> System_Calls_Help;  

    // Forward declaration.
    class Visitor;

    /**
     * @class System_Call_Print_System_Calls
     * @brief Prints all supported system calls
     */
    class System_Call_Print_System_Calls : public System_Call_Node
    {
    public:
      /**
       * Constructor
       **/
      System_Call_Print_System_Calls (
        madara::knowledge::Thread_Safe_Context & context,
        const Component_Nodes & nodes);
      
      /**
       * Destructor
       **/
      virtual ~System_Call_Print_System_Calls (void);

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
    private:
      // calls help menu
      static System_Calls_Help calls_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _SYSTEM_CALL_PRINT_SYSTEM_CALLS_H_ */
