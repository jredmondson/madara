#include "Global_Logger.h"

std::auto_ptr <Madara::Logger::Logger> Madara::Logger::global_logger (
  new Logger ());
