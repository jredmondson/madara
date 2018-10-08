#include "madara/knowledge/CompiledExpression.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/logger/GlobalLogger.h"

#ifndef _MADARA_NO_KARL_

madara::knowledge::CompiledExpression::CompiledExpression()
  : expression(*logger::global_logger.get())
{
}

madara::knowledge::CompiledExpression::CompiledExpression(
    const CompiledExpression& ce)
  : logic(ce.logic), expression(ce.expression)
{
}

madara::knowledge::CompiledExpression::~CompiledExpression() {}

/**
 * Gets the root of the compiled expression
 **/
madara::expression::ComponentNode*
madara::knowledge::CompiledExpression::get_root(void)
{
  return expression.get_root();
}

void madara::knowledge::CompiledExpression::operator=(
    const CompiledExpression& ce)
{
  if (this != &ce)
  {
    logic = ce.logic;
    expression = ce.expression;
  }
}

#endif  // _MADARA_NO_KARL_
