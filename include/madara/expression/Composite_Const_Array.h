/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_CONST_ARRAY_H_
#define _MADARA_COMPOSITE_CONST_ARRAY_H_

#ifndef _MADARA_NO_KARL_

#include <vector>
#include "madara/knowledge/Knowledge_Record.h"
#include "madara/expression/Composite_Ternary_Node.h"
#include "madara/knowledge/Functions.h"
#include "madara/knowledge/Thread_Safe_Context.h"

namespace madara
{
  namespace expression
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
       * @param   logger the logger to use for printing
       * @param   nodes    arguments to the function
       **/
      Composite_Const_Array (logger::Logger & logger,
        const Component_Nodes & nodes);

      /**
       * Destructor
       **/
      virtual ~Composite_Const_Array (void);

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
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_COMPOSITE_CONST_ARRAY_H_ */
