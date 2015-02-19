/* -*- C++ -*- */
#ifndef _COMPOSITE_UNARY_NODE_H_
#define _COMPOSITE_UNARY_NODE_H_

#ifndef _MADARA_NO_KARL_

#include "madara/expression_tree/Component_Node.h"
#include "madara/knowledge_engine/Knowledge_Record.h"

namespace Madara
{
  namespace Expression_Tree
  {
    /**
     * @class Composite_Unary_Node
     * @brief Encapsulates a single expression tree
     * @see Composite_Binary_Node
     */
    class Composite_Unary_Node : public Component_Node
    {
    public:
      /**
       * Constructor
       * @param   right  right expression
       **/
      Composite_Unary_Node (Component_Node *right);

      /**
       * Destructor
       **/
      virtual ~Composite_Unary_Node (void);

      /** 
       * Returns the right expression. 
       * @return    a pointer to the right expression
       **/
      virtual Component_Node *right (void) const;

    protected:
      /// Right expression
      Component_Node *right_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPOSITE_UNARY_NODE_H_ */
