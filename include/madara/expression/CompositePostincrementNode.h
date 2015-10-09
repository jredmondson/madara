/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_POSTINCREMENT_NODE_H_
#define _MADARA_COMPOSITE_POSTINCREMENT_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression/CompositeUnaryNode.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/expression/CompositeArrayReference.h"
#include "madara/expression/VariableNode.h"

namespace madara
{
  namespace expression
  {
    class ComponentNode;
    class Visitor;

    /**
     * @class CompositePostincrementNode
     * @brief A composite node that increments a right expression. If
     *        right expression is a variable, then the variable will
     *        be updated with the new value.
     */
    class CompositePostincrementNode : public CompositeUnaryNode
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   right  right expression
       **/
      CompositePostincrementNode (logger::Logger & logger,
        ComponentNode *right);

      /**
       * Destructor
       **/
      virtual ~CompositePostincrementNode (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    one added to the value returned by the right expression
       **/
      virtual madara::KnowledgeRecord prune (bool & can_change);

      /** 
       * Evaluates the node. 
       * @param     settings     settings for evaluating the node
       * @return    one added to the value returned by the right expression
       **/
      virtual madara::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

    private:

      /// variable holder
      VariableNode * var_;

      /// variable index holder
      CompositeArrayReference * array_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_COMPOSITE_POSTINCREMENT_NODE_H_ */
