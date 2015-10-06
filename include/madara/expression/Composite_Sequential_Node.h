#ifndef _MADARA_COMPOSITE_SEQUENTIAL_NODE_H_
#define _MADARA_COMPOSITE_SEQUENTIAL_NODE_H_

#ifndef _MADARA_NO_KARL_


#include "madara/expression/Composite_Ternary_Node.h"
#include "madara/knowledge/Knowledge_Record.h"

namespace madara
{
  namespace expression
  {
    // Forward declaration.
    class Component_Node;
    class Visitor;

    /**
     * @class Composite_Sequential_Node
     * @brief A composite node that evaluates both left and right
     *        expressions regardless of their evaluations
     */
    class Composite_Sequential_Node : public Composite_Ternary_Node
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   nodes  2+ nodes to be executed in sequence
       **/
      Composite_Sequential_Node (logger::Logger & logger,
        const Component_Nodes & nodes);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    the maximum of the value returned by the
       *            left and right expressions
       **/
      virtual madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the node. 
       * @param     settings     settings for evaluating the node
       * @return    the maximum of the value returned by the
       *            left and right expressions
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

#endif // _MADARA_COMPOSITE_SEQUENTIAL_NODE_H_
