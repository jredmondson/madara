#include "madara/knowledge/Compiled_Expression.h"
#include "madara/expression/Expression_Tree.h"
#include "madara/logger/Global_Logger.h"

#ifndef _MADARA_NO_KARL_

madara::knowledge::Compiled_Expression::Compiled_Expression ()
  : expression (*logger::global_logger.get ())
{

}

madara::knowledge::Compiled_Expression::Compiled_Expression (
  const Compiled_Expression & ce)
: logic (ce.logic), expression (ce.expression)
{

}

madara::knowledge::Compiled_Expression::~Compiled_Expression ()
{

}


/**
  * Gets the root of the compiled expression
  **/
madara::expression::Component_Node *
madara::knowledge::Compiled_Expression::get_root (void)
{
  return expression.get_root ();
}


void
madara::knowledge::Compiled_Expression::operator= (
  const Compiled_Expression & ce)
{
  if (this != &ce)
  {
    logic = ce.logic;
    expression = ce.expression;
  }
}

#endif // _MADARA_NO_KARL_
