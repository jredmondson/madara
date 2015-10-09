/* -*- C++ -*- */
#ifndef _MADARA_COMPOSITE_FUNCTION_NODE_H_
#define _MADARA_COMPOSITE_FUNCTION_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <vector>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/expression/CompositeTernaryNode.h"
#include "madara/knowledge/Functions.h"
#include "madara/knowledge/ThreadSafeContext.h"

namespace madara
{
  namespace expression
  {
    class ComponentNode;
    class Visitor;

    /**
     * @class CompositeFunctionNode
     * @brief A composite node that calls a function
     */
    class CompositeFunctionNode : public CompositeTernaryNode
    {
    public:
      /**
       * Constructor
       * @param   name     function name
       * @param   context  context for accessing variables and functions
       * @param   nodes    arguments to the function
       **/
      CompositeFunctionNode (const std::string & name, 
        madara::knowledge::ThreadSafeContext & context,
        const ComponentNodes & nodes);

      /**
       * Destructor
       **/
      virtual ~CompositeFunctionNode (void);

      /**
       * Returns the printable character of the node
       * @return    value of the node
       **/
      virtual madara::KnowledgeRecord item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @param     can_change   set to true if variable nodes are contained
       * @return    negation of the right expression
       **/
      virtual madara::KnowledgeRecord prune (bool & can_change);

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    negation of the right expression
       **/
      virtual madara::KnowledgeRecord evaluate (
        const madara::knowledge::KnowledgeUpdateSettings & settings);

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

    private:

      // function name
      const std::string name_;

      // variables context
      madara::knowledge::ThreadSafeContext & context_;

      // function pointer
      madara::knowledge::Function * function_;
      
      // pointers to .1, .2, .3, etc.
      std::vector <KnowledgeRecord *> compiled_args_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_COMPOSITE_FUNCTION_NODE_H_ */
