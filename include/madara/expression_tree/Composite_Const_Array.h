/* -*- C++ -*- */
#ifndef _COMPOSITE_CONST_ARRAY_H_
#define _COMPOSITE_CONST_ARRAY_H_

#ifndef _MADARA_NO_KARL_

#include <vector>
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/expression_tree/Composite_Ternary_Node.h"
#include "madara/knowledge_engine/Functions.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"

namespace Madara
{
  namespace Expression_Tree
  {
    class Component_Node;
    class Visitor;

    /**
     * @class Composite_Const_Array
     * @brief A composite node that contains an array of values
     */
    class Composite_Const_Array : public Composite_Ternary_Node
    {
    public:
      /**
       * Constructor
       * @param   context  context for accessing variables and functions
       * @param   nodes    arguments to the function
       **/
      Composite_Const_Array (const Component_Nodes & nodes);

      /**
       * Destructor
       **/
      virtual ~Composite_Const_Array (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    negation of the right expression
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    negation of the right expression
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

#endif // _MADARA_NO_KARL_

#endif /* _COMPOSITE_CONST_ARRAY_H_ */
