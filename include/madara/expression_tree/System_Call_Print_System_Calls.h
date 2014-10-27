/* -*- C++ -*- */
#ifndef _SYSTEM_CALL_PRINT_SYSTEM_CALLS_H_
#define _SYSTEM_CALL_PRINT_SYSTEM_CALLS_H_

#include <string>
#include <map>
#include <stdexcept>
#include "madara/utility/stdint.h"
#include "madara/expression_tree/System_Call_Node.h"


namespace Madara
{
  namespace Expression_Tree
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
        Madara::Knowledge_Engine::Thread_Safe_Context & context,
        const Component_Nodes & nodes);
      
      /**
       * Destructor
       **/
      virtual ~System_Call_Print_System_Calls (void);

      /**
       * Returns the value of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of current contained expression tree
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @return    value of current contained expression tree
       **/
      virtual Madara::Knowledge_Record evaluate (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings);

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

#endif /* _SYSTEM_CALL_PRINT_SYSTEM_CALLS_H_ */
