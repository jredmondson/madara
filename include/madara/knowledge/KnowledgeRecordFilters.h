

#ifndef _MADARA_KNOWLEDGE_RECORD_FILTERS_H_
#define _MADARA_KNOWLEDGE_RECORD_FILTERS_H_

/**
 * @file KnowledgeRecordFilters.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the KnowledgeRecordFilters class, helper typdefs and
 *functions
 **/

#include <string>
#include <vector>
#include <map>
#include <list>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/Functions.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/AggregateFilter.h"
#include "madara/transport/TransportContext.h"
#include "madara/utility/StdInt.h"
#include "madara/MadaraExport.h"
#include "madara/filters/RecordFilter.h"
#include "madara/filters/AggregateFilter.h"
#include "madara/filters/BufferFilter.h"

#ifdef _MADARA_JAVA_
#include <jni.h>
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
#include <boost/python.hpp>
#endif

namespace madara
{
namespace knowledge
{
/// a chain of filters
typedef std::list<Function> FilterChain;

/// a map of types to filter chain
typedef std::map<uint32_t, FilterChain> FilterMap;

/**
 * @class KnowledgeRecordFilters
 * @brief Provides map of data types to a filter chain to apply to the data
 */
class MADARA_EXPORT KnowledgeRecordFilters
{
public:
  /**
   * Default constructor
   **/
  KnowledgeRecordFilters();

  /**
   * Copy constructor
   **/
  KnowledgeRecordFilters(const KnowledgeRecordFilters& filters);

  /**
   * Destructor
   **/
  virtual ~KnowledgeRecordFilters();

  /**
   * Assignment operator
   * @param  rhs   the value to be copied into this class
   **/
  void operator=(const KnowledgeRecordFilters& rhs);

  /**
   * Adds a filter to the list of types
   * @param   types      the types to add the filter to
   * @param   function   the function that will take the knowledge record
   *                     in FunctionArguments.
   **/
  void add(uint32_t types,
      knowledge::KnowledgeRecord (*function)(FunctionArguments&, Variables&));

  /**
   * Adds an aggregate filter
   * @param function     the function that will filter the aggregation
   **/
  void add(void (*function)(
      KnowledgeMap&, const transport::TransportContext&, Variables&));

  /**
   * Adds an aggregate filter functor
   * @param filter     the functor that will filter the aggregation
   **/
  void add(filters::AggregateFilter* filter);

  /**
   * Adds a buffer filter to be applied to the message buffer just before
   * a send, before a receive, and just before a rebroadcast
   * @param filter     the functor that will filter the message buffer
   **/
  void add(filters::BufferFilter* filter);

  /**
   * Adds an individual record filter functor
   * @param   types      the types to add the filter to
   * @param   filter     the functor that will filter the aggregation
   **/
  void add(uint32_t types, filters::RecordFilter* filter);

#ifdef _MADARA_JAVA_

  /**
   * Adds a java filter to the list of types
   * @param   types      the types to add the filter to
   * @param   callable   the function that will take the knowledge record
   *                     in FunctionArguments.
   **/
  void add(uint32_t types, jobject& callable);

  /**
   * Adds an aggregate filter
   * @param callable     the function that will filter the aggregation
   **/
  void add(jobject& callable);

#endif

#ifdef _MADARA_PYTHON_CALLBACKS_

  /**
   * Adds a python filter to the list of types
   * @param   types      the types to add the filter to
   * @param   callable   the function that will take the knowledge record
   *                     in FunctionArguments.
   **/
  void add(uint32_t types, boost::python::object& callable);

  /**
   * Adds an aggregate filter
   * @param callable     the function that will filter the aggregation
   **/
  void add(boost::python::object& callable);

#endif

  /**
   * Attaches a context. If the context ever goes out of scope,
   * a 0 should be passed into this function to reset the context.
   * @param   context     context to be used for Variable lookups
   **/
  void attach(ThreadSafeContext* context);

  /**
   * Clears the list of filters for the specified types
   * @param   types   the types to clear the filters of
   **/
  void clear(uint32_t types);

  /**
   * Clears the aggregate filters
   **/
  void clear_aggregate_filters(void);

  /**
   * Clears the buffer filters
   **/
  void clear_buffer_filters(void);

  /**
   * Filters an input according to its filter chain.
   *
   * <p>The arguments passed to the filter are the following:</p>
   *
   * args[0] : The knowledge record that the filter is acting upon<br />
   * args[1] : The name of the knowledge record, if applicable ("" if
   *unnamed)<br /> args[2] : The type of operation calling the filter (integer
   *valued).<br /> : IDLE_OPERATION (should never see)<br /> :
   *SENDING_OPERATION (transport is about to send the record)<br /> :
   *RECEIVING_OPERATION (transport has received the record and is :
   *about to apply the update)<br /> : REBROADCASTING_OPERATION
   *(transport is about to rebroadcast the record -- only happens if
   *rebroadcast is enabled in Transport
   *Settings)<br /> args[3] : Bandwidth
   *used while sending through this transport<br /> args[4] : Bandwidth used
   *while receiving from this transport<br /> args[5] : Message timestamp (when
   *the message was originally sent)<br /> args[6] : Current timestamp (the
   *result of utility::get_time ()<br /> args[7] : Network domain (partition of
   *the network knowledge)<br /> args[8] : Knowledge originator (where this
   *update comes fromm)<br />
   *
   * @param   input   the argument to the filter chain
   * @param   name    variable name ("" if unnamed)
   * @param   context the context of the transport
   * @return  the result of filtering the input
   **/
  knowledge::KnowledgeRecord filter(const knowledge::KnowledgeRecord& input,
      const std::string& name, transport::TransportContext& context) const;

  /**
   * Calls aggregate filter chain on the provided aggregate records
   * @param  records             the aggregate record map
   * @param  transport_context   the context of the transport
   **/
  void filter(KnowledgeMap& records,
      const transport::TransportContext& transport_context) const;

  /**
   * Calls encode on the the buffer filter chain
   * @param   source           the source and destination buffer
   * @param   size             the amount of data in the buffer in bytes
   * @param   max_size         the amount of bytes the buffer can hold
   * @return  the new size after encoding
   **/
  void filter_encode(char* source, int size, int max_size) const;

  /**
   * Calls decode on the the buffer filter chain
   * @param   source           the source and destination buffer
   * @param   size             the amount of data in the buffer in bytes
   * @param   max_size         the amount of bytes the buffer can hold
   * @return  the new size after encoding
   **/
  void filter_decode(char* source, int size, int max_size) const;

  /**
   * Prints the number of filters chained for each type
   **/
  void print_num_filters(void) const;

  /**
   * Returns the number of types that have filters
   * @return  the number of types that have filters
   **/
  size_t get_number_of_filtered_types(void) const;

  /**
   * Returns the number of aggregate update filters
   * @return  the number of aggregate update filters
   **/
  size_t get_number_of_aggregate_filters(void) const;

  /**
   * Returns the number of buffer filters
   * @return  the number of buffer filters
   **/
  size_t get_number_of_buffer_filters(void) const;

protected:
  /**
   * Container for mapping types to filter chains
   **/
  FilterMap filters_;

  /**
   * List of aggregate filters
   **/
  AggregateFilters aggregate_filters_;

  /**
   * List of buffer filters
   **/
  filters::BufferFilters buffer_filters_;

  /**
   * Context used by this filter
   **/
  ThreadSafeContext* context_;
};
}
}

#endif  // _MADARA_KNOWLEDGE_RECORD_FILTERS_H_
