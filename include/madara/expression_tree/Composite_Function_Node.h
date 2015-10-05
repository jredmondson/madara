/* -*- C++ -*- */
#ifndef _COMPOSITE_FUNCTION_NODE_H_
#define _COMPOSITE_FUNCTION_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <vector>
#include "madara/knowledge/Knowledge_Record.h"
#include "madara/expression_tree/Composite_Ternary_Node.h"
#include "madara/knowledge/Functions.h"
#include "madara/knowledge/Thread_Safe_Context.h"

namespace madara
{
  namespace expression_tree
  {
    class Component_Node;
    class Visitor;

    /**
     * @class Composite_Function_Node
     * @brief A composite node that calls a function
     */
    class Composite_Function_Node : public Composite_Ternary_Node
    {
    public:
      /**
       * Constructor
       * @param   name     function name
       * @param   context  context for accessing variables and functions
       * @param   nodes    arguments to the function
       **/
      Composite_Function_Node (const std::string & name, 
        madara::knowledge::Thread_Safe_Context & context,
        const Component_Nodes & nodes);

      /**
       * Destructor
       **/
      virtual ~Composite_Function_Node (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    negation of the right expression
       **/
      virtual madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    negation of the right expression
       **/
      virtual madara::Knowledge_Record evaluate (
        const madara::knowledge::Knowledge_Update_Settings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

    private:

      // function name
      const std::string name_;

      // variables context
      madara::knowledge::Thread_Safe_Context & context_;

      // function pointer
      madara::knowledge::Function * function_;
      
      // pointers to .1, .2, .3, etc.
      std::vector <Knowledge_Record *> compiled_args_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPOSITE_FUNCTION_NODE_H_ */
