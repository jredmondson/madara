

#ifndef _MADARA_FILTERS_CLEAR_RECORDS_H_
#define _MADARA_FILTERS_CLEAR_RECORDS_H_

/**
 * @file ClearRecords.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the endpoint discovery filter, which
 * is useful for anyone wanting to keep a list of remote hosts
 * that have messaged an agent.
 **/

#include <string>
#include <vector>
#include <map>
#include <list>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/Functions.h"
#include "madara/utility/StdInt.h"
#include "madara/MadaraExport.h"
#include "madara/transport/TransportContext.h"
#include "madara/knowledge/ExternFunctionVariables.h"
#include "madara/knowledge/containers/Map.h"

#include "AggregateFilter.h"

namespace madara
{
  namespace filters
  {
    /**
     * Filter for clearing records from a filter
     **/
    class MADARA_EXPORT ClearRecords : public AggregateFilter
    {
    public:
      /**
      * Constructor
      **/
      ClearRecords ();

      /**
       * Destructor
       **/
      virtual ~ClearRecords ();

      /**
       * Calls the main logic of the filter
       * @param   records           the aggregate records vector
       * @param   transport_context context for querying transport state
       * @param   vars              context for querying current program state
       **/
      virtual void filter (knowledge::KnowledgeMap & records,
        const transport::TransportContext & transport_context,
        knowledge::Variables & vars);
    };
  }
}

#endif  // _MADARA_FILTERS_CLEAR_RECORDS_H_
