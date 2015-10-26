

#ifndef _MADARA_FILTERS_ENDPOINT_CLEAR_H_
#define _MADARA_FILTERS_ENDPOINT_CLEAR_H_

/**
 * @file EndpointClear.h
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
#include "madara/utility/stdint.h"
#include "madara/MADARA_export.h"
#include "madara/transport/TransportContext.h"
#include "madara/knowledge/ExternFunctionVariables.h"
#include "madara/knowledge/containers/Map.h"

#include "AggregateFilter.h"

namespace madara
{
  namespace filters
  {
    /**
     * Filter for deleting endpoints, usually when new endpoints have
     * been discovered.
     **/
    class MADARA_Export EndpointClear : public AggregateFilter
    {
    public:
      /**
      * Constructor
      * @param  prefix      the prefix where the endpoint map can be found
      **/
      EndpointClear (
        const std::string & prefix = ".endpoints");

      /**
       * Destructor
       **/
      virtual ~EndpointClear ();

      /**
       * Adds trusted originators who will be able to prompt deleting
       * @param  originators   the trusted originators to add
       **/
      void add_trusted_originator (
        const std::vector <std::string> & originators);

      /**
      * Adds trusted originator who will be able to prompt deleting
      * @param  originator   the trusted originator to add
      **/
      void add_trusted_originator (const std::string & originator);

      /**
       * Clears all trusted originators who can prompt deleting. A
       * cleared list means all originators who write to the prefix
       * will prompt deletion
       **/
      void clear_trusted_originators (void);

      /**
       * Sets the prefix to use
       * @param  prefix      the prefix where the endpoint map can be found
       **/
      void set_prefix (const std::string & prefix);

      /**
       * Calls the main logic of the filter
       * @param   records           the aggregate records vector
       * @param   transport_context context for querying transport state
       * @param   vars              context for querying current program state
       **/
      virtual void filter (knowledge::KnowledgeMap & records,
        const transport::TransportContext & transport_context,
        knowledge::Variables & vars);

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
       * originators that are trusted to prompt a clear of endpoints
       **/
      std::map <std::string, bool> trusted_originators_;

      /**
       * A map of discovered peers
       **/
      knowledge::containers::Map endpoints_;
    };
  }
}

#endif  // _MADARA_FILTERS_ENDPOINT_CLEAR_H_
