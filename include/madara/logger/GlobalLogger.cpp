#include "GlobalLogger.h"

madara::utility::Refcounter<madara::logger::Logger>
    madara::logger::global_logger(new Logger());
