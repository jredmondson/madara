/* -*- C++ -*- */
#ifndef _TREE_ITERATOR_H_
#define _TREE_ITERATOR_H_

#include <string>

#include "madara/utility/Refcounter.h"
#include "madara/expression_tree/Iterator_Impl.h"

namespace madara
{
  namespace expression_tree
  {

    // Forward declarations.
    class Expression_Tree_Command_Factory;
    class Component_Node;

    /**
     * @class Expression_Tree_Iterator
     * @brief Non-const iterator for traversing an expression tree
     *
     * @see   Expression_Tree_Level_Order_Iterator_Impl, Expression_Tree_In_Order_Iterator_Impl, 
     *        Expression_Tree_Pre_Order_Iterator_Impl, and Expression_Tree_Post_Order_Iterator_Impl.
     */
    class Expression_Tree_Iterator
    {
    public:
      /// Copy ctor - needed for reference counting.
      Expression_Tree_Iterator (const Expression_Tree_Iterator &ts);
      
      /// Copy ctor for impl *.
      Expression_Tree_Iterator (Expression_Tree_Iterator_Impl *impl);

      /// Dereference operator returns a reference to the item contained
      /// at the current position
      // @@ Expression_Tree operator* (void);
      Expression_Tree operator* (void);

      /// Returns a const reference to the item contained at the current position
      // @@ Expression_Tree operator* (void);
      const Expression_Tree operator* (void) const;

      /// Preincrement operator
      Expression_Tree_Iterator &operator++ (void);

      /// Postincrement operator. 
      Expression_Tree_Iterator operator++ (int);

      /// Equality operator
      bool operator== (const Expression_Tree_Iterator &rhs);

      /// In-equality operator
      bool operator!= (const Expression_Tree_Iterator &rhs);

      // = Necessary traits
      typedef std::forward_iterator_tag iterator_category;
      typedef long value_type;
      typedef int *pointer;
      typedef int &reference;
      typedef int difference_type;

    private:
      madara::utility::Refcounter<Expression_Tree_Iterator_Impl> impl_;
    };

    /**
     * @class Expression_Tree_Const_Iterator
     * @brief Constant iterator over an expression tree
     *
     * @see   Expression_Tree_Level_Order_Iterator_Impl,
     *        Expression_Tree_In_Order_Iterator_Impl,
     *        Expression_Tree_Pre_Order_Iterator_Impl,
     *        and Expression_Tree_Post_Order_Iterator_Impl
     */
    class Expression_Tree_Const_Iterator
    {
    public:
      /// Copy ctor - needed for reference counting.
      Expression_Tree_Const_Iterator (const Expression_Tree_Const_Iterator &ts);
      
      /// Copy ctor for impl *.
      Expression_Tree_Const_Iterator (Expression_Tree_Iterator_Impl *impl);
      
      /// Returns a const reference to the item contained at the current position
      const Expression_Tree operator* (void) const;

      /// Preincrement operator
      Expression_Tree_Const_Iterator &operator++ (void);

      /// Postincrement operator. 
      Expression_Tree_Const_Iterator operator++ (int);

      /// Equality operator
      bool operator== (const Expression_Tree_Const_Iterator &rhs);

      /// Nonequality operator
      bool operator!= (const Expression_Tree_Const_Iterator &rhs);

      // = Necessary traits
      typedef std::forward_iterator_tag iterator_category;
      typedef long value_type;
      typedef int *pointer;
      typedef int &reference;
      typedef int difference_type;

    private:
      /// Pointer to actual implementation, i.e., the "bridge", which is
      /// reference counted to automate memory management. 
      madara::utility::Refcounter <Expression_Tree_Iterator_Impl> impl_;
    };
}
}
#endif /* _TREE_ITERATOR_H_ */
