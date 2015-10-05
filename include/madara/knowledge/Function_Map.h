#ifndef _FUNCTION_MAP_H_
#define _FUNCTION_MAP_H_

#include <map>
#include <string>

#include "madara/knowledge/Functions.h"

namespace madara
{
  namespace knowledge
  {    
    /**
     * @class Function_Map
     * @brief This class stores external functions
     */
    typedef     std::map <std::string, Function>   Function_Map;
  }
}

#endif