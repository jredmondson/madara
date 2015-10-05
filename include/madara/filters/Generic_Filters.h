

#ifndef _GENERIC_FILTERS_H_
#define _GENERIC_FILTERS_H_

/**
 * @file Generic_Filters.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains generic filters that developers can use to filter
 * Knowledge_Record classes in common ways.
 **/

#include <string>
#include <vector>
#include <map>
#include <list>
#include "madara/knowledge/Knowledge_Record.h"
#include "madara/knowledge/Functions.h"
#include "madara/utility/stdint.h"
#include "madara/MADARA_export.h"
#include "madara/transport/Transport_Context.h"

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
    MADARA_Export Knowledge_Record
    discard     (knowledge::Function_Arguments & args,
                 knowledge::Variables & vars);

    /**
     * Filter for discarding a non-primitive record. Resulting record will
     * have a status of UNCREATED. Primitives are INTEGER, DOUBLE, STRING,
     * INTEGER_ARRAY, and DOUBLE_ARRAY.
     * @param   args        filter arguments (only first argument is used)
     * @param   vars        variable context (unused)
     * @return  record with status set to UNCREATED if arg is non-primitive
     **/
    MADARA_Export Knowledge_Record
    discard_nonprimitives (knowledge::Function_Arguments & args,
                           knowledge::Variables & vars);

    /**
     * Filter for discarding a non-file record. Resulting record will
     * have a status of UNCREATED. Files are IMAGE_JPEG, TEXT, XML, and
     * UNKNOWN_FILE_TYPE.
     * @param   args        filter arguments (only first argument is used)
     * @param   vars        variable context
     * @return  record with status set to UNCREATED if arg is non-file
     **/
    MADARA_Export Knowledge_Record
    discard_nonfiles      (knowledge::Function_Arguments & args,
                           knowledge::Variables & vars);

    /**
     * Filter for logging args
     * @param   args        filter arguments (only first argument is used)
     * @param   vars        variable context
     * @return  record with status set to UNCREATED if arg is non-file
     **/
    MADARA_Export Knowledge_Record
    log_args              (knowledge::Function_Arguments & args,
                           knowledge::Variables & vars);
    
    /**
     * Filter for logging aggregate update and context
     * @param   records           aggregate update
     * @param   transport_context the transport context
     * @param   vars              variable context
     **/
    MADARA_Export void
    log_aggregate         (
      Knowledge_Map & records,
      const transport::Transport_Context & transport_context,
      knowledge::Variables & vars);
  }
}

#endif  // _GENERIC_FILTERS_H_
