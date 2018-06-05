

#ifndef _MADARA_FILTERS_COUNTER_FILTER_H_
#define _MADARA_FILTERS_COUNTER_FILTER_H_

/**
 * @file CounterFilter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter for counting filter invocations
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
#include "madara/knowledge/containers/Integer.h"

#include "AggregateFilter.h"

namespace madara
{
  namespace filters
  {
    /**
     * Filter for discovering neighboring peers
     **/
    class MADARA_EXPORT CounterFilter : public AggregateFilter
    {
    public:
      /**
      * Constructor
      **/
      CounterFilter ();

      /**
       * Destructor
       **/
      virtual ~CounterFilter ();

      /**
       * Adds to the counter
       * @param   records           the aggregate records vector
       * @param   transport_context context for querying transport state
       * @param   vars              context for querying current program state
       **/
      virtual void filter (knowledge::KnowledgeMap & records,
        const transport::TransportContext & transport_context,
        knowledge::Variables & vars);

      /**
       * Gets the number of packets filtered
       * @return the number of packets
       **/
      inline knowledge::KnowledgeRecord::Integer get_count ();

      /**
      * Returns the duration of filtering from first to
      * last message in nanoseconds
      * @return test duration in nanoseconds
      **/
      inline knowledge::KnowledgeRecord::Integer get_elapsed ();

      /**
      * Returns the throughput in terms of packets/second
      * @return packet throughput
      **/
      inline double get_throughput ();

    protected:

      /**
       * Tracks if the first timer has been initialized
       **/
      bool initialized_;

      /**
       * A map of discovered peers
       **/
      knowledge::KnowledgeRecord::Integer packets_;

      /**
       * The time to keep record of a peer
       **/
      knowledge::KnowledgeRecord::Integer first_message_;

      /**
      * The time of the last clear of the peer_list
      **/
      knowledge::KnowledgeRecord::Integer last_message_;
    };
  }
}

#include "CounterFilter.inl"

#endif  // _MADARA_FILTERS_COUNTER_FILTER_H_
