

#ifndef _MADARA_FILTERS_RECORD_FILTER_H_
#define _MADARA_FILTERS_RECORD_FILTER_H_

/**
 * @file RecordFilter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter functor for individual records
 **/

#include <string>
#include <vector>
#include <map>
#include <list>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/utility/stdint.h"
#include "madara/MADARA_export.h"
#include "madara/knowledge/FunctionArguments.h"
#include "madara/knowledge/ExternFunctionVariables.h"

namespace madara
{
  namespace filters
  {
    /**
     * Abstract base class for implementing individual record filters via
     * a functor interface. When subclassing this class, create a new
     * instance with the new operator, and the pointer will be managed
     * by the underlying MADARA infrastructure.
     **/
    class RecordFilter
    {
    public:
      /**
       * Destructor
       **/
      virtual ~RecordFilter ()
      {
      }

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
       *                      args[5] is the send timestamp in nanoseconds
       *                      args[6] is the receive timestamp in nanoseconds
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
      virtual knowledge::KnowledgeRecord filter (
        knowledge::FunctionArguments & args,
        knowledge::Variables & vars) = 0;
    };
  }
}

#endif  // _MADARA_FILTERS_RECORD_FILTER_H_
