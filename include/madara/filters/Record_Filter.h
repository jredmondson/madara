

#ifndef _MADARA_FILTERS_RECORD_FILTER_H_
#define _MADARA_FILTERS_RECORD_FILTER_H_

/**
 * @file Record_Filter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter functor for individual records
 **/

#include <string>
#include <vector>
#include <map>
#include <list>
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/utility/stdint.h"
#include "madara/MADARA_export.h"
#include "madara/knowledge_engine/Function_Arguments.h"
#include "madara/knowledge_engine/Extern_Function_Variables.h"

namespace Madara
{
  namespace Filters
  {
    /**
     * Abstract base class for implementing individual record filters via
     * a functor interface. When subclassing this class, create a new
     * instance with the new operator, and the pointer will be managed
     * by the underlying MADARA infrastructure.
     **/
    class Record_Filter
    {
    public:
      /**
       * User-implementable method for performing a filter on network
       * data. This is a pure abstract function that must be overridden
       * when implementing a subclass.
       * @param   args        arguments to the filter
       *                      args[0] is the value of the record
       *                      args[1] is the name of the record
       *                      args[2] is the type of operation 
       *                      args[3] is the send bandwidth usage in B/s
       *                      args[4] is the total bandwidth usage in B/s
       *                      args[5] is the send timestamp in seconds
       *                      args[6] is the receive timestamp in seconds
       *                      args[7] is the knowledge domain
       *                      args[8] is the originator id
       * @param   vars        variable context for querying current state
       * @return  value that the args[1] named variable should be set to.
       *          ON_SEND/REBROADCAST will mean this is the value that will be
       *          sent. ON_RECEIVE will mean this is the value that will be
       *          set in the knowledge base. If a Knowledge Record is returned
       *          in a default state (i.e., with a default constructor), then
       *          the variable is removed from the operation.
       **/
      virtual Knowledge_Record filter (
        Knowledge_Engine::Function_Arguments & args,
        Knowledge_Engine::Variables & vars) = 0;
    };
  }
}

#endif  // _MADARA_FILTERS_RECORD_FILTER_H_
