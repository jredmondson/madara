#include "Logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <madara/utility/Utility.h>
#include <boost/lexical_cast.hpp>
#include <iomanip>

#ifndef MADARA_NO_THREAD_LOCAL
thread_local int madara::logger::Logger::thread_level_(
    madara::logger::TLS_THREAD_LEVEL_DEFAULT);

thread_local std::string madara::logger::Logger::thread_name_("");

thread_local double madara::logger::Logger::thread_hertz_(
    madara::logger::TLS_THREAD_HZ_DEFAULT);
#endif

madara::logger::Logger::Logger(bool log_to_terminal)
  : mutex_(),
    level_(LOG_ERROR),
    term_added_(log_to_terminal),
    syslog_added_(false),
    tag_("madara"),
    timestamp_format_("")
{
  if(log_to_terminal)
  {
    add_term();
  }
}

madara::logger::Logger::~Logger()
{
  clear();
}

std::string madara::logger::Logger::strip_custom_tstamp(
    const std::string in_str, const std::string ts_str)
{
  std::string ret_str(in_str);
  ret_str.replace(ret_str.find(ts_str), ts_str.length(), ts_str);
  return ret_str;
}

std::string madara::logger::Logger::search_and_insert_custom_tstamp(
    const std::string& buf, const std::string& ts_str)
{
  bool done = false;
  std::size_t found = 0;
  std::size_t offset = 0;
  std::string ret_string = buf;
  const int MGT_DIGIT_PRECISION = 6;

  while(!done)
  {
    /**
     * Take the ts_str (key string) and repeatedly search the incoming
     * message buf.
     *
     * If not found, npos is returned and exit the loop gracefully.
     *
     * If found, compare via the series of if-else-if statements. Swap
     * out the computed value with the key string using the copied message
     * buffer
     *
     **/
    found = ret_string.find(ts_str.c_str(), found + offset, ts_str.length());
    if(found == std::string::npos)
    {
      done = true;
      continue;
    }

    offset = 1;
    if(ts_str == MADARA_GET_TIME_MGT_)
    {
      /// insert mgt text here
      /// get_time returns nsecs. need to convert into seconds.
      double mgt_time = madara::utility::get_time() / (double)1000000000;

      /// insert this value into the buffer
      std::stringstream mgt_str;

      /// convert the double into string format that can easily be written
      /// into the copy of the message buffer.
      mgt_str << std::setprecision(MGT_DIGIT_PRECISION) << std::fixed
              << mgt_time;
      ret_string.replace(
          ret_string.find(ts_str), ts_str.length(), mgt_str.str());
      continue;
    }
    else if(ts_str == MADARA_THREAD_NAME_)
    {
    // insert thread name into message buffer copy
#ifndef MADARA_NO_THREAD_LOCAL
      ret_string.replace(ret_string.find(ts_str), ts_str.length(),
          madara::logger::Logger::get_thread_name());
#else
      ret_string.replace(
          ret_string.find(ts_str), ts_str.length(), "<DISABLED>");
#endif
      continue;
    }
    else if(ts_str == MADARA_THREAD_HERTZ_)
    {
    // insert thread hertz value into message buffer copy
#ifndef MADARA_NO_THREAD_LOCAL
      std::string hzstr = boost::lexical_cast<std::string>(
          madara::logger::Logger::get_thread_hertz());
      ret_string.replace(ret_string.find(ts_str), ts_str.length(), hzstr);
#else
      ret_string.replace(
          ret_string.find(ts_str), ts_str.length(), "<DISABLED>");
#endif
      continue;
    }
  }

  return ret_string;
}

void madara::logger::Logger::log(int level, const char* message, ...)
{
  if((madara::logger::Logger::get_thread_level() >= 0 &&
       level <= get_thread_level()) ||
      level <= level_)
  {
    va_list argptr;
    va_start(argptr, message);

    /// Android seems to not handle printf arguments correctly as
    /// best I can tell
    char buffer[10240];
    char* begin = (char*)buffer;
    size_t remaining_buffer = sizeof(buffer);

    if(this->timestamp_format_.size() > 0)
    {
      /**
       * Prepare string to log for the timestamp prefix.
       *
       * First, search and replace the custom key string for local thread.
       * The return value is a copy of the potential prefix with the custom
       * key string data embedded the number of times it was used.
       **/
      std::string mad_str = message;
      mad_str = search_and_insert_custom_tstamp(mad_str, MADARA_GET_TIME_MGT_);
      mad_str = search_and_insert_custom_tstamp(mad_str, MADARA_THREAD_NAME_);
      mad_str = search_and_insert_custom_tstamp(mad_str, MADARA_THREAD_HERTZ_);

      char custom_buffer[10240];
      std::strcpy(custom_buffer, mad_str.c_str());

      /**
       * Prepare string to write into copy of the message buffer.
       *
       * Search and insert corresponding data for each of the custom key
       * string.
       *
       **/
      time_t raw_time;
      struct tm* time_info;

      time(&raw_time);
      time_info = localtime(&raw_time);

      mad_str = search_and_insert_custom_tstamp(
          timestamp_format_, MADARA_GET_TIME_MGT_);
      mad_str = search_and_insert_custom_tstamp(mad_str, MADARA_THREAD_NAME_);
      mad_str = search_and_insert_custom_tstamp(mad_str, MADARA_THREAD_HERTZ_);

      /**
       * Process the normal message buffer and write into final copy to
       * return.
       **/
      size_t chars_written =
          strftime(begin, remaining_buffer, mad_str.c_str(), time_info);

      remaining_buffer -= chars_written;
      begin += chars_written;
      vsnprintf(begin, remaining_buffer, custom_buffer, argptr);
    }
    else
    {
      vsnprintf(begin, remaining_buffer, message, argptr);
    }

    va_end(argptr);

    MADARA_GUARD_TYPE guard(mutex_);

#ifdef _MADARA_ANDROID_
    if(this->term_added_ || this->syslog_added_)
    {
      if(level == LOG_ERROR)
      {
        __android_log_write(ANDROID_LOG_ERROR, tag_.c_str(), buffer);
      }
      else if(level == LOG_WARNING)
      {
        __android_log_write(ANDROID_LOG_WARN, tag_.c_str(), buffer);
      }
      else
      {
        __android_log_write(ANDROID_LOG_INFO, tag_.c_str(), buffer);
      }
    }
#else  // end if _USING_ANDROID_
    if(this->term_added_ || this->syslog_added_)
    {
      fprintf(stderr, "%s", buffer);
    }
#endif

    int file_num = 0;
    for(FileVectors::iterator i = files_.begin(); i != files_.end(); ++i)
    {
      if(level >= LOG_DETAILED)
      {
        fprintf(stderr, "Logger::log: writing to file num %d", file_num);

        // file_num is only important if logging is detailed
        ++file_num;
      }
      fprintf(*i, "%s", buffer);
    }
  }
}
