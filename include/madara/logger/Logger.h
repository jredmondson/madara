#ifndef _MADARA_LOGGER_LOGGER_H_
#define _MADARA_LOGGER_LOGGER_H_

#include "madara/MadaraExport.h"
#include "madara/LockType.h"
#include <vector>
#include <atomic>
#include <string>
#include <stdio.h>
#include "madara/utility/IntTypes.h"

/**
 * Fast version of the madara::logger::log method. This macro
 * uses compiler optimizations to ensure that args are not
 * evaluated unless the level is appropriate for the loggers level.
* This makes logging transparent and minimally invasive, performance wise
 * @param  logger  the logger instance to use
 * @param  level   the logging level
 **/
#define madara_logger_log(loggering, level, ...) \
  if (madara::logger::Logger::get_thread_level() >= 0 ) \
 { \
   if ( level <= madara::logger::Logger::get_thread_level() )   \
     loggering.log (level, __VA_ARGS__);                    \
 }else  \
    if ( level <= loggering.get_level() )         \
     loggering.log (level, __VA_ARGS__);

/**
* Fast version of the madara::logger::log method for Logger pointers.
* This macro uses compiler optimizations to ensure that args are not
* evaluated unless the level is appropriate for the loggers level.
* This makes logging transparent and minimally invasive, performance wise
* @param  logger  the logger pointer to use
* @param  level   the logging level
**/
#define madara_logger_ptr_log(loggering, level, ...) \
  if (loggering && (madara::logger::Logger::get_thread_level() >= 0) )  \
 { \
   if ( level <= madara::logger::Logger::get_thread_level() )\
     loggering->log (level, __VA_ARGS__);\
 }else  \
   if ( loggering && (level <= loggering->get_level()) )  \
     loggering->log (level, __VA_ARGS__);


/**
* High-performance logger that performs conditional logging based on first arg
* @param  conditional     the primary logger pointer to use (if not null)
* @param  logger_ptr      the logger that will be used if conditional is true
* @param  alt_logger_ptr  the secondary logger pointer to use (should be not null)
* @param  level           the logging level
**/
#define madara_logger_cond_log_ptrs(conditional, logger_ptr, alt_logger_ptr, level, ...) \
  if (conditional && logger_ptr && (madara::logger::Logger::get_thread_level() >= 0)) \
  { \
      if ( level <= madara::logger::Logger::get_thread_level()) \
      {   \
            logger_ptr->log (level, __VA_ARGS__); \
      }else \
            alt_logger_ptr->log (level, __VA_ARGS__); \
  }else if (conditional && logger_ptr && level <= logger_ptr->get_level ()) \
            logger_ptr->log (level, __VA_ARGS__); \
          else  alt_logger_ptr->log (level, __VA_ARGS__);

/**
* High-performance logger that performs conditional logging based on first arg
* @param  conditional     the primary logger pointer to use (if not null)
* @param  logger          the logger that will be used if conditional is true
* @param  alt_logger_ptr  the secondary logger pointer to use (should be not null)
* @param  level           the logging level
**/
#define madara_logger_cond_log(conditional, loggering, alt_logger_ptr, level, ...) \
  if (conditional && (madara::logger::Logger::get_thread_level() >= 0)) \
  { \
      if ( level <= madara::logger::Logger::get_thread_level()) \
      {   \
            loggering.log (level, __VA_ARGS__); \
      }else  alt_logger_ptr->log (level, __VA_ARGS__); \
  }else \
  if (conditional && level <= loggering.get_level ()) \
    loggering.log (level, __VA_ARGS__);                         \
  else \
    alt_logger_ptr->log (level, __VA_ARGS__);


namespace madara
{
  namespace logger
  {

    /**
    * Logging levels available for MADARA library
    **/
    enum LogLevels
    {
      LOG_EMERGENCY = 0,
      LOG_ALWAYS = 0,
      LOG_ERROR = 1,
      LOG_WARNING = 2,
      LOG_MAJOR = 3,
      LOG_MINOR = 4,
      LOG_TRACE = 5,
      LOG_DETAILED = 6,
      LOG_MADARA_MAX = 6
    };


    const int TLS_THREAD_LEVEL_DEFAULT = -1;
    const double TLS_THREAD_HZ_DEFAULT = 0.0;

    /**
     * A multi-threaded logger for logging to one or more destinations.
     * A default logger is provided via madara::logger::global_logger in
     * GlobalLogger.h
     **/
    class MADARA_EXPORT Logger
    {
    public:
      /**
       * Constructor
       * @param  log_to_stderr  if true, log to stderr by default
       **/
      Logger (bool log_to_stderr = true);

      /**
       * Destructor
       **/
      ~Logger ();

      /**
       * Logs a message to all available loggers
       * @param  level   the logging level @see LogLevels
       * @param  message the message to log
       **/
      void log (int level, const char * message, ...);

      /**
       * Adds a file to the logger
       * @param  filename the file to open and add to writing
       **/
      void add_file (const std::string & filename);

      /**
       * Adds terminal to logger outputs
       **/
      void add_term (void);

      /**
       * Adds the system log
       **/
      void add_syslog (void);

      /**
       * Sets the maximum logging detail level
       * @param  level  the maximum level of log detail
       **/
      void set_level (int level);

      /**
      * Gets the tag used for syslogs
      * @return  the tag used for any system logging
      **/
      std::string get_tag (void);

      /**
      * Sets the tag used for syslogs (e.g. Android Log)
      * @param  tag  the tag to use for any system logging
      **/
      void set_tag (const std::string & tag);

      /**
      * Gets the maximum logging detail level
      * @return the maximum level of log detail
      **/
      int get_level (void);

      /**
       * Clears all log targets
       **/
      void clear (void);

      /**
       * Sets timestamp format. Uses 
       * <a href="http://www.cplusplus.com/reference/ctime/strftime/">strftime</a>
       * for formatting time.
       * @param  format  the format of the timestamp. See C++
       *                 strftime definition for common usage.
       **/
      void set_timestamp_format (const std::string & format = "%x %X: ");

      /**
       * Fetches thread local storage value for thread level
       **/
      static int get_thread_level(void);

#ifndef MADARA_NO_THREAD_LOCAL
      /**
       * Fetches thread local storage value for thread name
       **/
      static std::string get_thread_name(void);

      /**
       * Fetch thread local storage value for hertz
       **/
      static double get_thread_hertz(void);

      /**
       * Set thread local storage value for thread level
       **/
      static void set_thread_level(int level);

      /**
       * Set thread local storage value for thread name
       **/
      static void set_thread_name(const std::string name);

      /**
       * Set thread local storage value for hertz
       **/
      static void set_thread_hertz(double hertz);
#endif

    private:

#ifndef MADARA_NO_THREAD_LOCAL
      static thread_local int thread_level_;
      static thread_local std::string thread_name_;
      static thread_local double thread_hertz_;
#endif

      std::string search_and_insert_custom_tstamp(const std::string & buf,
                                                  const std::string & tsstr);

      std::string strip_custom_tstamp(const std::string instr,const std::string tsstr);

      /// guard for access and changes
      

      /// vector of file handles
      typedef std::vector <FILE *>   FileVectors;

      /// mutex for changes
      mutable MADARA_LOCK_TYPE mutex_;

      /// list of all log outputs
      FileVectors files_;

      /// the maximum detail level for logging
      std::atomic<int> level_;

      /// tracks whether terminal output has been added
      bool term_added_;

      /// tracks whether the system log has been added
      bool syslog_added_;

      /// the tag used for logging to system logs
      std::string tag_;

      /// the timestamp format. Default is "" for no timestamp
      std::string timestamp_format_;

      const char * MADARA_GET_TIME_MGT = "%MGT";
      const char * MADARA_THREAD_NAME = "%MTN";
      const char * MADARA_THREAD_HERTZ = "%MTZ";
    };


  }//end logger namespace
}//end madara namespace

#include "Logger.inl"

#endif // _MADARA_LOGGER_LOGGER_H_
