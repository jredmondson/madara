#include "madara/knowledge_engine/Compiled_Expression.h"
#include "madara/expression_tree/Expression_Tree.h"

#ifndef _MADARA_NO_KARL_

Madara::Knowledge_Engine::Compiled_Expression::Compiled_Expression ()
{

}

Madara::Knowledge_Engine::Compiled_Expression::Compiled_Expression (
  const Compiled_Expression & ce)
: logic (ce.logic), expression (ce.expression)
{

}

Madara::Knowledge_Engine::Compiled_Expression::~Compiled_Expression ()
{

}


/**
  * Gets the root of the compiled expression
  **/
Madara::Expression_Tree::Component_Node *
Madara::Knowledge_Engine::Compiled_Expression::get_root (void)
{
  return expression.get_root ();
}


void
Madara::Knowledge_Engine::Compiled_Expression::operator= (
  const Compiled_Expression & ce)
{
  if (this != &ce)
  {
    logic = ce.logic;
    expression = ce.expression;
  }
}

#endif // _MADARA_NO_KARL_
