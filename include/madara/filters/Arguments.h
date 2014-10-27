
#ifndef _MADARA_FILTER_ARGUMENTS_
#define _MADARA_FILTER_ARGUMENTS_

/**
 * @file Arguments.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains enumerations and other definitions for filter arguments
 **/

namespace Madara
{
  namespace Filters
  {
    enum
    {
      RECORD = 0,
      RECORD_NAME = 1,
      OPERATION_TYPE = 2,
      SEND_BANDWIDTH = 3,
      RECEIVED_BANDWIDTH = 4,
      UPDATE_TIME = 5,
      CURRENT_TIME = 6,
      KNOWLEDGE_DOMAIN = 7,
      ORIGINATOR = 8,
      TOTAL_ARGUMENTS = 9
    };
  }
}



#endif // _MADARA_FILTER_ARGUMENTS_