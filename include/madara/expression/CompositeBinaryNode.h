/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_BINARY_NODE_H_
#define _MADARA_COMPOSITE_BINARY_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <string>

#include "madara/expression/CompositeUnaryNode.h"

namespace madara
{
  namespace expression
  {
    /**
     * @class CompositeBinaryNode
     * @brief Defines a left and right node (via inheritance from 
     *        CompositeUnaryNode).
     */
    class CompositeBinaryNode : public CompositeUnaryNode
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   left   left expression
       * @param   right  right expression
       **/
      CompositeBinaryNode (
        logger::Logger & logger,
        ComponentNode *left,
        ComponentNode *right);

      /**
       * Destructor
       **/
      virtual ~CompositeBinaryNode (void);

      /** 
       * Returns the left expression. 
       * @return    a pointer to the left expression
       **/
      virtual ComponentNode *left (void) const;

    protected:
      /// left expression
      ComponentNode *left_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_COMPOSITE_BINARY_NODE_H_ */
