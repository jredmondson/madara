

#ifndef _MADARA_FILTERS_PEER_DISCOVERY_H_
#define _MADARA_FILTERS_PEER_DISCOVERY_H_

/**
 * @file PeerDiscovery.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a filter functor for aggregate records
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
#include "madara/knowledge/Variables.h"
#include "madara/knowledge/containers/Map.h"

#include "AggregateFilter.h"

namespace madara
{
namespace filters
{
/**
 * Filter for discovering neighboring peers
 **/
class MADARA_EXPORT PeerDiscovery : public AggregateFilter
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
  PeerDiscovery(const std::string& prefix = ".peers",
      knowledge::KnowledgeRecord::Integer heart_beat = -1);

  /**
   * Destructor
   **/
  virtual ~PeerDiscovery();

  /**
   * User-implementable method for performing a filter on network
   * data. This is a pure abstract function that must be overridden
   * when implementing a subclass.
   * @param   records           the aggregate records vector
   * @param   transport_context context for querying transport state
   * @param   vars              context for querying current program state
   **/
  virtual void filter(knowledge::KnowledgeMap& records,
      const transport::TransportContext& transport_context,
      knowledge::Variables& vars);

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
  knowledge::containers::Map peers_;

  /**
   * The time to keep record of a peer
   **/
  knowledge::KnowledgeRecord::Integer heart_beat_;

  /**
   * The time of the last clear of the peer_list
   **/
  knowledge::KnowledgeRecord::Integer last_clear_;
};
}
}

#endif  // _MADARA_FILTERS_PEER_DISCOVERY_H_
