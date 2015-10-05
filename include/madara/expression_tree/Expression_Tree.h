/* -*- C++ -*- */

#ifndef _EXPRESSION_TREE_H_
#define _EXPRESSION_TREE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <stdexcept>
#include "madara/utility/Refcounter.h"

#include "madara/logger/Global_Logger.h"
#include "madara/expression_tree/Component_Node.h"
#include "madara/knowledge/Knowledge_Record.h"

namespace madara
{
  namespace expression_tree
  {
    // Forward declarations.
    class Expression_Tree_Iterator;
    class Expression_Tree_Const_Iterator;

    /**
     * @class Expression_Tree
     * @brief Encapsulates a MADARA KaRL expression into an evaluatable tree
     */
    class Expression_Tree
    {
    public:

      // Define a "trait"
      typedef madara::Knowledge_Record value_type;
      typedef Expression_Tree_Iterator iterator;
      typedef Expression_Tree_Const_Iterator const_iterator;

      /**
       * Constructor
       * @param  logger   logger for printing information
       **/
      Expression_Tree (logger::Logger & logger =
        *logger::global_logger.get ());

      /**
       * Constructor for copying a root node
       * @param    logger          logger for printing information
       * @param    root            root of the tree to copy
       * @param    increase_count  whether or not to increase the ref count
       **/
      Expression_Tree (logger::Logger & logger,
        Component_Node *root, bool increase_count = false);

      /**
       * Constructor for copying an expression tree
       * @param    logger     logger for printing information
       * @param    tree       expression tree to copy
       **/
      Expression_Tree (logger::Logger & logger, const Expression_Tree &tree);

      /**
       * Destructor
       **/
      ~Expression_Tree (void);

      /**
       * Returns the root node of the expression tree
       * @return   root node of the expression tree
       **/
      Component_Node * get_root (void);

      /**
       * Non-deep copies an expression tree into this instance
       * @param    tree  tree to copy from
       **/
      void operator= (const Expression_Tree &tree);

     /**
       * Checks if root pointer is null
       * @return    true if root of tree is null
       **/
      bool is_null (void) const;

      /**
       * Returns value of tree
       * @return    value of the expression tree
       **/
      madara::Knowledge_Record item (void) const;


      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @return    value of expression tree
       **/
      madara::Knowledge_Record prune (void);

      /** 
       * Evaluates the expression tree. 
       * @param settings        Settings for evaluating and setting knowledge
       * @return    value of expression tree
       **/
      madara::Knowledge_Record evaluate (
        const madara::knowledge::Knowledge_Update_Settings & settings =
        knowledge::Knowledge_Update_Settings ());

      /** 
       * Returns the left expression of this tree
       * @return    left expression
       **/
      Expression_Tree left (void);

      /** 
       * Returns the right expression of this tree
       * @return    right expression
       **/
      Expression_Tree right (void);

      /** 
       * Returns an iterator to the beginning of the expression tree
       * @param     traversal_order   in-order, post-order, pre-order,
       *                              level-order
       * @return    iterator for the specified order
       **/
      iterator begin (const std::string & traversal_order);

      /** 
       * Returns an iterator to the end of the expression tree
       * @param     traversal_order   in-order, post-order, pre-order,
       *                              level-order
       * @return    iterator for the specified order
       **/
      iterator end (const std::string & traversal_order);

      /** 
       * Returns a const iterator to the beginning of the expression tree
       * @param     traversal_order   in-order, post-order, pre-order,
       *                              level-order
       * @return    iterator for the specified order
       **/
      const_iterator begin (const std::string & traversal_order) const;

      /** 
       * Returns a const iterator to the end of the expression tree
       * @param     traversal_order   in-order, post-order, pre-order,
       *                              level-order
       * @return    iterator for the specified order
       **/
      const_iterator end (const std::string & traversal_order) const;

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      void accept (Visitor & visitor) const;

    private:
      /// handle for logging information
      logger::Logger * logger_;

      /// root of the expression tree
      madara::utility::Refcounter <Component_Node> root_;
    };

  }
}

#endif // _MADARA_NO_KARL_

#endif /* _EXPRESSION_TREE_H_ */
