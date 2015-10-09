
#ifndef _MADARA_KE_FUNCTION_ARGUMENTS_H_
#define _MADARA_KE_FUNCTION_ARGUMENTS_H_

#include <vector>
#include "madara/knowledge/KnowledgeRecord.h"

/**
 * @file Functions.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Function Arguments definition. Currently
 * this is just a typedef of a vector of knowledge records
 **/

namespace madara
{
  namespace knowledge
  {
    typedef  std::vector <KnowledgeRecord>   FunctionArguments;
  }
}

#endif // _MADARA_KE_FUNCTION_ARGUMENTS_H_
