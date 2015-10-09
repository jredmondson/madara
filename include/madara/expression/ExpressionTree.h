/* -*- C++ -*- */

#ifndef _MADARA_KNOWLEDGE_EXPRESSION_TREE_H_
#define _MADARA_KNOWLEDGE_EXPRESSION_TREE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <stdexcept>
#include "madara/utility/Refcounter.h"

#include "madara/logger/GlobalLogger.h"
#include "madara/expression/ComponentNode.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
  namespace expression
  {
    // Forward declarations.
    class ExpressionTreeIterator;
    class ExpressionTreeConstIterator;

    /**
     * @class ExpressionTree
     * @brief Encapsulates a MADARA KaRL expression into an evaluatable tree
     */
    class ExpressionTree
    {
    public:

      // Define a "trait"
      typedef madara::KnowledgeRecord value_type;
      typedef ExpressionTreeIterator iterator;
      typedef ExpressionTreeConstIterator const_iterator;

      /**
       * Constructor
       * @param  logger   logger for printing information
       **/
      ExpressionTree (logger::Logger & logger =
        *logger::global_logger.get ());

      /**
       * Constructor for copying a root node
       * @param    logger          logger for printing information
       * @param    root            root of the tree to copy
       * @param    increase_count  whether or not to increase the ref count
       **/
      ExpressionTree (logger::Logger & logger,
        ComponentNode *root, bool increase_count = false);

      /**
       * Constructor for copying an expression tree
       * @param    logger     logger for printing information
       * @param    tree       expression tree to copy
       **/
      ExpressionTree (logger::Logger & logger, const ExpressionTree &tree);

      /**
       * Destructor
       **/
      ~ExpressionTree (void);

      /**
       * Returns the root node of the expression tree
       * @return   root node of the expression tree
       **/
      ComponentNode * get_root (void);

      /**
       * Non-deep copies an expression tree into this instance
       * @param    tree  tree to copy from
       **/
      void operator= (const ExpressionTree &tree);

     /**
       * Checks if root pointer is null
       * @return    true if root of tree is null
       **/
      bool is_null (void) const;

      /**
       * Returns value of tree
       * @return    value of the expression tree
       **/
      madara::KnowledgeRecord item (void) const;


      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @return    value of expression tree
       **/
      madara::KnowledgeRecord prune (void);

      /** 
       * Evaluates the expression tree. 
       * @param settings        Settings for evaluating and setting knowledge
       * @return    value of expression tree
       **/
      madara::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings =
        knowledge::KnowledgeUpdateSettings ());

      /** 
       * Returns the left expression of this tree
       * @return    left expression
       **/
      ExpressionTree left (void);

      /** 
       * Returns the right expression of this tree
       * @return    right expression
       **/
      ExpressionTree right (void);

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
      madara::utility::Refcounter <ComponentNode> root_;
    };

  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_KNOWLEDGE_EXPRESSION_TREE_H_ */
