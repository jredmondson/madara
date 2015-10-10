/* -*- C++ -*- */
#ifndef _MADARA_LEAF_NODE_H_
#define _MADARA_LEAF_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include "madara/expression/ComponentNode.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
  namespace expression
  {
    // Forward declarations.
    class Visitor;

    /**
     * @class LeafNode
     * @brief Defines a node that contains a madara::knowledge::KnowledgeRecord::Integer value
     */

    class LeafNode : public ComponentNode
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   item    value of the node
       **/
      LeafNode (logger::Logger & logger,
        const knowledge::KnowledgeRecord & item);
      
      /**
       * Integer Constructor
       * @param   logger the logger to use for printing
       * @param   item    value of the node
       **/
      LeafNode (logger::Logger & logger,
        madara::knowledge::KnowledgeRecord::Integer item);
      
      /**
       * Double Constructor
       * @param   logger the logger to use for printing
       * @param   item    value of the node
       **/
      LeafNode (logger::Logger & logger,
        double item);

      /**
       * String Constructor
       * @param   logger the logger to use for printing
       * @param   item    value of the node
       **/
      LeafNode (logger::Logger & logger,
        const std::string &item);

      /**
       * Destructor
       **/
      virtual ~LeafNode (void);

      /**
       * Returns the printable value of the node
       * @return    value of the node
       **/
      virtual madara::knowledge::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    value of this node
       **/
      virtual madara::knowledge::KnowledgeRecord prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @return    value of this node
       **/
      virtual madara::knowledge::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const; 

    private:
      /// Integer value associated with the operand.
      madara::knowledge::KnowledgeRecord item_;
    };

  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_LEAF_NODE_H_ */
