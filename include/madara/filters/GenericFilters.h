

#ifndef _MADARA_GENERIC_FILTERS_H_
#define _MADARA_GENERIC_FILTERS_H_

/**
 * @file GenericFilters.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains generic filters that developers can use to filter
 * knowledge::KnowledgeRecord classes in common ways.
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

namespace madara
{
  namespace filters
  {
    /**
     * Filter for discarding a record. Resulting record will
     * have a status of UNCREATED.
     * @param   args        arguments to the filter (unused)
     * @param   vars        variable context (unused)
     * @return  record with status set to UNCREATED
     **/
    MADARA_Export knowledge::KnowledgeRecord
    discard     (knowledge::FunctionArguments & args,
                 knowledge::Variables & vars);

    /**
     * Filter for discarding a non-primitive record. Resulting record will
     * have a status of UNCREATED. Primitives are INTEGER, DOUBLE, STRING,
     * INTEGER_ARRAY, and DOUBLE_ARRAY.
     * @param   args        filter arguments (only first argument is used)
     * @param   vars        variable context (unused)
     * @return  record with status set to UNCREATED if arg is non-primitive
     **/
    MADARA_Export knowledge::KnowledgeRecord
    discard_nonprimitives (knowledge::FunctionArguments & args,
                           knowledge::Variables & vars);

    /**
     * Filter for discarding a non-file record. Resulting record will
     * have a status of UNCREATED. Files are IMAGE_JPEG, TEXT, XML, and
     * UNKNOWN_FILE_TYPE.
     * @param   args        filter arguments (only first argument is used)
     * @param   vars        variable context
     * @return  record with status set to UNCREATED if arg is non-file
     **/
    MADARA_Export knowledge::KnowledgeRecord
    discard_nonfiles      (knowledge::FunctionArguments & args,
                           knowledge::Variables & vars);

    /**
     * Filter for logging args
     * @param   args        filter arguments (only first argument is used)
     * @param   vars        variable context
     * @return  record with status set to UNCREATED if arg is non-file
     **/
    MADARA_Export knowledge::KnowledgeRecord
    log_args              (knowledge::FunctionArguments & args,
                           knowledge::Variables & vars);
    
    /**
     * Filter for logging aggregate update and context
     * @param   records           aggregate update
     * @param   transport_context the transport context
     * @param   vars              variable context
     **/
    MADARA_Export void
    log_aggregate         (
     knowledge::KnowledgeMap& records,
      const transport::TransportContext & transport_context,
      knowledge::Variables & vars);
  }
}

#endif  // _MADARA_GENERIC_FILTERS_H_
