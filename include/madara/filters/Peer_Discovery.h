

#ifndef _MADARA_FILTERS_PEER_DISCOVERY_H_
#define _MADARA_FILTERS_PEER_DISCOVERY_H_

/**
 * @file Peer_Discovery.h
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
#include "madara/knowledge_engine/containers/Map.h"

#include "Aggregate_Filter.h"

namespace Madara
{
  namespace Filters
  {
    /**
     * Filter for discovering neighboring peers
     **/
    class MADARA_Export Peer_Discovery : public Aggregate_Filter
    {
    public:
      /**
      * Constructor
      * @param  prefix      the prefix to store discovery information into
      *                     the knowledge base
      * @param  heart_beat  the time, in seconds, before dropping a peer from
      *                     discovery. Negative values indicate that peers
      *                     should never be erased.
      **/
      Peer_Discovery (
        const std::string & prefix = ".peers",
        Knowledge_Record::Integer heart_beat = -1);

      /**
       * Destructor
       **/
      virtual ~Peer_Discovery ();

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
        Knowledge_Engine::Variables & vars);

    protected:

      /**
       * Tracks if the peers_ map has been initialized
       **/
      bool initialized_;

      /**
      * The context
      **/
      std::string prefix_;

      /**
       * A map of discovered peers
       **/
      Knowledge_Engine::Containers::Map peers_;

      /**
       * The time to keep record of a peer
       **/
      Knowledge_Record::Integer heart_beat_;

      /**
      * The time of the last clear of the peer_list
      **/
      Knowledge_Record::Integer last_clear_;
    };
  }
}

#endif  // _MADARA_FILTERS_PEER_DISCOVERY_H_
