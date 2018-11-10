
#include "madara/logger/GlobalLogger.h"
#include "MyThread.h"

namespace knowledge = madara::knowledge;

// constructor
threads::MyThread::MyThread()
{
}

// destructor
threads::MyThread::~MyThread()
{
}

/**
 * Initialization to a knowledge base. If you don't actually need access
 * to the knowledge base, just scheduling things in madara::threads::Threader,
 * then you can decide to delete this function or simply do nothing inside of
 * the function.
 **/
void
threads::MyThread::init(knowledge::KnowledgeBase & kb)
{
  // point our data plane to the knowledge base initializing the thread
  data_ = kb;
}

/**
 * Executes the actual thread logic. Best practice is to simply do one loop
 * iteration. If you want a long running thread that executes something
 * frequently, see the madara::threads::Threader::runHz method in your
 * controller.
 **/
void
threads::MyThread::run(void)
{
  /**
   * the MADARA logger is thread-safe, fast, and allows for specifying
   * various options like output files and multiple output targets(
   * e.g., std::cerr, a system log, and a thread_output.txt file). You
   * can create your own custom log levels or loggers as well.
   **/
  madara_logger_ptr_log(madara::logger::global_logger.get(),
    madara::logger::LOG_MAJOR,
    "threads::MyThread::run:" 
    " executing\n");
}

