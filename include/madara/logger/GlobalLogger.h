
#ifndef _MADARA_LOGGER_GLOBAL_LOGGER_H_
#define _MADARA_LOGGER_GLOBAL_LOGGER_H_

#include <memory>
#include "madara/MADARA_export.h"
#include "Logger.h"

namespace madara
{
  namespace logger
  {
    extern MADARA_Export std::auto_ptr <logger::Logger> global_logger;
  }
}

#endif // _MADARA_LOGGER_GLOBAL_LOGGER_H_