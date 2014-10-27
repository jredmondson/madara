/* -*- C++ -*- */
#ifndef _SYSTEM_CALL_DELETE_VARIABLE_H_
#define _SYSTEM_CALL_DELETE_VARIABLE_H_

#include <string>
#include <stdexcept>
#include "madara/utility/stdint.h"
#include "madara/expression_tree/System_Call_Node.h"


namespace Madara
{
  namespace Expression_Tree
  {
    // Forward declaration.
    class Visitor;

    /**
     * @class System_Call_Delete_Variable
     * @brief Attempts to delete a variable. This is an extremely
     *        dangerous function call that will break any compiled
     *        expression. Do not use this unless you are not going
     *        to use compiled expressions that use this variable.
     */
    class System_Call_Delete_Variable : public System_Call_Node
    {
    public:
      /**
       * Constructor
       **/
      System_Call_Delete_Variable (
        Madara::Knowledge_Engine::Thread_Safe_Context & context,
        const Component_Nodes & nodes);
      
      /**
       * Destructor
       **/
      virtual ~System_Call_Delete_Variable (void);

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
    };
  }
}

#endif /* _SYSTEM_CALL_DELETE_VARIABLE_H_ */
