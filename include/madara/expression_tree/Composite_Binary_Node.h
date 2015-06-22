/* -*- C++ -*- */
#ifndef _COMPOSITE_BINARY_NODE_H_
#define _COMPOSITE_BINARY_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <string>

#include "madara/expression_tree/Composite_Unary_Node.h"

namespace Madara
{
  namespace Expression_Tree
  {
    /**
     * @class Composite_Binary_Node
     * @brief Defines a left and right node (via inheritance from 
     *        Composite_Unary_Node).
     */
    class Composite_Binary_Node : public Composite_Unary_Node
    {
    public:
      /**
       * Constructor
       * @param   logger the logger to use for printing
       * @param   left   left expression
       * @param   right  right expression
       **/
      Composite_Binary_Node (
        Logger::Logger & logger,
        Component_Node *left,
        Component_Node *right);

      /**
       * Destructor
       **/
      virtual ~Composite_Binary_Node (void);

      /** 
       * Returns the left expression. 
       * @return    a pointer to the left expression
       **/
      virtual Component_Node *left (void) const;

    protected:
      /// left expression
      Component_Node *left_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPOSITE_BINARY_NODE_H_ */
