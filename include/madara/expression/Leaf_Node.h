/* -*- C++ -*- */
#ifndef _MADARA_LEAF_NODE_H_
#define _MADARA_LEAF_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include "madara/expression/Component_Node.h"

namespace madara
{
  namespace expression
  {
    // Forward declarations.
    class Visitor;

    /**
     * @class Leaf_Node
     * @brief Defines a node that contains a madara::Knowledge_Record::Integer value
     */

    class Leaf_Node : public Component_Node
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   item    value of the node
       **/
      Leaf_Node (logger::Logger & logger,
        const Knowledge_Record & item);
      
      /**
       * Integer Constructor
       * @param   logger the logger to use for printing
       * @param   item    value of the node
       **/
      Leaf_Node (logger::Logger & logger,
        madara::Knowledge_Record::Integer item);
      
      /**
       * Double Constructor
       * @param   logger the logger to use for printing
       * @param   item    value of the node
       **/
      Leaf_Node (logger::Logger & logger,
        double item);

      /**
       * String Constructor
       * @param   logger the logger to use for printing
       * @param   item    value of the node
       **/
      Leaf_Node (logger::Logger & logger,
        const std::string &item);

      /**
       * Destructor
       **/
      virtual ~Leaf_Node (void);

      /**
       * Returns the printable value of the node
       * @return    value of the node
       **/
      virtual madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of this node
       **/
      virtual madara::Knowledge_Record prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @return    value of this node
       **/
      virtual madara::Knowledge_Record evaluate (
        const madara::knowledge::Knowledge_Update_Settings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const; 

    private:
      /// Integer value associated with the operand.
      madara::Knowledge_Record item_;
    };

  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_LEAF_NODE_H_ */
