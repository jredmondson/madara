#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"

namespace knowledge = madara::knowledge;
namespace logger = madara::logger;

int main (int argc, char ** argv)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_ALWAYS,
    "kats_sleep: KATS has been temporarily suspended\n");

  return 0;
}
