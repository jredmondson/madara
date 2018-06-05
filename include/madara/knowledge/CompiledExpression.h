#ifndef _MADARA_COMPILED_EXPRESSION_H_
#define _MADARA_COMPILED_EXPRESSION_H_

#ifndef _MADARA_NO_KARL_

/**
 * @file CompiledExpression.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the CompiledExpression class which is used by
 * the madara::knowledge::KnowledgeBase class
 */

#include <string>
#include "madara/MadaraExport.h"
#include "madara/expression/ExpressionTree.h"

namespace madara
{
  namespace expression
  {
    class SystemCallEval;
  }
  namespace knowledge
  {
    class ThreadSafeContext;
    class KnolwedgeBaseImpl;
    class Variables;
    class KnowledgeBase;

    /**
     * @class CompiledExpression
     * @brief Compiled, optimized KaRL logic
     **/
    class MADARA_EXPORT CompiledExpression
    {
    public:
      //class KnowledgeBaseImpl;
      friend class ThreadSafeContext;
      friend class KnowledgeBaseImpl;
      friend class Variables;
      friend class KnowledgeBase;
      friend class expression::SystemCallEval;

      /**
       * Constructor
       **/
      CompiledExpression ();

      /**
       * Copy Constructor
       * @param   ce      Compiled expression to copy from
       **/
      CompiledExpression (const CompiledExpression & ce);

      /**
       * Destructor
       **/
      ~CompiledExpression ();

      /**
       * Assignment operator
       * @param   ce      Compiled expression to copy from
       **/
      void operator= (const CompiledExpression & ce);
      
      /**
       * Gets the root of the compiled expression
       **/
      expression::ComponentNode * get_root (void);

    private:
      /// the logic that was compiled
      std::string logic;

      /// the expression tree
      madara::expression::ExpressionTree expression;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif // _MADARA_COMPILED_EXPRESSION_H_
