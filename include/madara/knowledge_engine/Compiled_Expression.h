#ifndef _COMPILED_EXPRESSION_H_
#define _COMPILED_EXPRESSION_H_

/**
 * @file Compiled_Expression.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Compiled_Expression class which is used by
 * the Madara::Knowledge_Engine::Knowledge_Base class
 */

#include <string>
#include "madara/MADARA_export.h"
#include "madara/expression_tree/Expression_Tree.h"

namespace Madara
{
  namespace Expression_Tree
  {
    class System_Call_Eval;
  }
  namespace Knowledge_Engine
  {
    class Thread_Safe_Context;
    class Knolwedge_Base_Impl;
    class Variables;

    /**
     * @class Compiled_Expression
     * @brief Compiled, optimized KaRL logic
     **/
    class MADARA_Export Compiled_Expression
    {
    public:
      //class Knowledge_Base_Impl;
      friend class Thread_Safe_Context;
      friend class Knowledge_Base_Impl;
      friend class Variables;
      friend class Expression_Tree::System_Call_Eval;

      /**
       * Constructor
       **/
      Compiled_Expression ();

      /**
       * Copy Constructor
       * @param   ce      Compiled expression to copy from
       **/
      Compiled_Expression (const Compiled_Expression & ce);

      /**
       * Destructor
       **/
      ~Compiled_Expression ();

      /**
       * Assignment operator
       * @param   ce      Compiled expression to copy from
       **/
      void operator= (const Compiled_Expression & ce);
      
      /**
       * Gets the root of the compiled expression
       **/
      Expression_Tree::Component_Node * get_root (void);

    private:
      std::string logic;
      Madara::Expression_Tree::Expression_Tree expression;
    };
  }
}

#endif // _COMPILED_EXPRESSION_H_
