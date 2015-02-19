/* -*- C++ -*- */
#ifndef _SYSTEM_CALL_H_
#define _SYSTEM_CALL_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <stdexcept>
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/utility/stdint.h"
#include "madara/expression_tree/Composite_Ternary_Node.h"


namespace Madara
{
  namespace Expression_Tree
  {
    // Forward declaration.
    class Visitor;

    /**
     * @class System_Call_Node
     * @brief Interface for a MADARA system call
     */
    class System_Call_Node : public Composite_Ternary_Node
    {
    public:
      /**
       * Constructor
       **/
      System_Call_Node (
        Madara::Knowledge_Engine::Thread_Safe_Context & context);

      /**
       * Constructor
       **/
      System_Call_Node (
        Madara::Knowledge_Engine::Thread_Safe_Context & context,
        const Component_Nodes & nodes);
      
      /**
       * Destructor
       **/
      virtual ~System_Call_Node (void);

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
      virtual Madara::Knowledge_Record prune (bool & can_change)
        = 0;

      /** 
       * Evaluates the expression tree. 
       * @return    value of current contained expression tree
       **/
      virtual Madara::Knowledge_Record evaluate (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
        = 0;

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

    protected:
      
      Madara::Knowledge_Engine::Thread_Safe_Context & context_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _SYSTEM_CALL_H_ */
