/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_UNARY_NODE_H_
#define _MADARA_COMPOSITE_UNARY_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression/ComponentNode.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
  namespace expression
  {
    /**
     * @class CompositeUnaryNode
     * @brief Encapsulates a single expression tree
     * @see CompositeBinaryNode
     */
    class CompositeUnaryNode : public ComponentNode
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   right  right expression
       **/
      CompositeUnaryNode (logger::Logger & logger,
        ComponentNode *right);

      /**
       * Destructor
       **/
      virtual ~CompositeUnaryNode (void);

      /** 
       * Returns the right expression. 
       * @return    a pointer to the right expression
       **/
      virtual ComponentNode *right (void) const;

    protected:
      /// Right expression
      ComponentNode *right_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_COMPOSITE_UNARY_NODE_H_ */
