#ifndef _MADARA_COMPILED_EXPRESSION_H_
#define _MADARA_COMPILED_EXPRESSION_H_

#ifndef _MADARA_NO_KARL_

/**
 * @file Compiled_Expression.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Compiled_Expression class which is used by
 * the madara::knowledge::Knowledge_Base class
 */

#include <string>
#include "madara/MADARA_export.h"
#include "madara/expression/Expression_Tree.h"

namespace madara
{
  namespace expression
  {
    class System_Call_Eval;
  }
  namespace knowledge
  {
    class Thread_Safe_Context;
    class Knolwedge_Base_Impl;
    class Variables;
    class Knowledge_Base;

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
      friend class Knowledge_Base;
      friend class expression::System_Call_Eval;

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
      expression::Component_Node * get_root (void);

    private:
      /// the logic that was compiled
      std::string logic;

      /// the expression tree
      madara::expression::Expression_Tree expression;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif // _MADARA_COMPILED_EXPRESSION_H_
