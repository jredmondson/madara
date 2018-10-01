

#ifndef _MADARA_FILTERS_AGGREGATE_FILTER_H_
#define _MADARA_FILTERS_AGGREGATE_FILTER_H_

/**
 * @file AggregateFilter.h
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

namespace madara
{
namespace filters
{
/**
 * Abstract base class for implementing aggregate record filters via
 * a functor interface. When subclassing this class, create a new
 * instance with the new operator, and the pointer will be managed
 * by the underlying MADARA infrastructure.
 **/
class AggregateFilter
{
public:
  /**
   * Destructor
   **/
  virtual ~AggregateFilter() {}

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
      knowledge::Variables& vars) = 0;
};
}
}

#endif  // _MADARA_FILTERS_AGGREGATE_FILTER_H_
