

#ifndef _MADARA_FILTERS_AGGREGATE_FILTER_H_
#define _MADARA_FILTERS_AGGREGATE_FILTER_H_

/**
 * @file Aggregate_Filter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter functor for aggregate records
 **/

#include <string>
#include <vector>
#include <map>
#include <list>
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/knowledge_engine/Functions.h"
#include "madara/utility/stdint.h"
#include "madara/MADARA_export.h"
#include "madara/transport/Transport_Context.h"
#include "madara/knowledge_engine/Extern_Function_Variables.h"

namespace Madara
{
  namespace Filters
  {
    /**
     * Abstract base class for implementing aggregate record filters via
     * a functor interface. When subclassing this class, create a new
     * instance with the new operator, and the pointer will be managed
     * by the underlying MADARA infrastructure.
     **/
    class Aggregate_Filter
    {
    public:
      /**
       * User-implementable method for performing a filter on network
       * data. This is a pure abstract function that must be overridden
       * when implementing a subclass.
       * @param   records           the aggregate records vector
       * @param   transport_context context for querying transport state
       * @param   vars              context for querying current program state
       **/
      virtual void filter (Knowledge_Map & records,
        const Transport::Transport_Context & transport_context,
        Knowledge_Engine::Variables & vars) = 0;
    };
  }
}

#endif  // _MADARA_FILTERS_AGGREGATE_FILTER_H_
