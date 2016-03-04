
#ifndef _MADARA_LOGGER_GLOBAL_LOGGER_H_
#define _MADARA_LOGGER_GLOBAL_LOGGER_H_

#include "madara/MADARA_export.h"
#include "madara/utility/Refcounter.h"
#include "Logger.h"

namespace madara
{
  namespace logger
  {
    extern MADARA_Export utility::Refcounter <logger::Logger> global_logger;
  }
}

#endif // _MADARA_LOGGER_GLOBAL_LOGGER_H_