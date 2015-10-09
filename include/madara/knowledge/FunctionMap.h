#ifndef _MADARA_FUNCTION_MAP_H_
#define _MADARA_FUNCTION_MAP_H_

#include <map>
#include <string>

#include "madara/knowledge/Functions.h"

namespace madara
{
  namespace knowledge
  {    
    /**
     * @class FunctionMap
     * @brief This class stores external functions
     */
    typedef     std::map <std::string, Function>   FunctionMap;
  }
}

#endif // _MADARA_FUNCTION_MAP_H_
