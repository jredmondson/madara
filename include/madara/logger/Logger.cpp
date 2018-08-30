#include "Logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <madara/utility/Utility.h>
#include <boost/lexical_cast.hpp>


thread_local int madara::logger::Logger::thread_level_(madara::logger::TLS_THREAD_LEVEL_DEFAULT);
thread_local std::string madara::logger::Logger::thread_name_("");
thread_local double madara::logger::Logger::thread_hertz_(madara::logger::TLS_THREAD_HZ_DEFAULT);

madara::logger::Logger::Logger (bool log_to_terminal)
: mutex_ (), level_ (LOG_ERROR),
  term_added_ (log_to_terminal), syslog_added_ (false), tag_ ("madara"),
  timestamp_format_ ("")
{
  
  if (log_to_terminal)
  {
    add_term ();
  }
}

madara::logger::Logger::~Logger ()
{
  clear ();
}



std::string
madara::logger::Logger::strip_custom_tstamp(const std::string instr,const std::string tsstr)
{
  std::string retstr(instr);
  retstr.replace(retstr.find(tsstr),tsstr.length(),tsstr);
  return retstr;
}

std::string 
madara::logger::Logger::search_and_insert_custom_tstamp(const std::string & buf,
                                                        const std::string & tsstr)
{
  bool done = false;
  std::size_t found = 0;
  std::size_t offset = 0;
  std::string retstring = buf;
  
  while ( !done )
  {
    found = retstring.find(tsstr.c_str(),found+offset,tsstr.length());
    if ( found == std::string::npos )
    {
      done = true;
      continue;
    }

    offset = 1;
    if ( tsstr == MADARA_GET_TIME_MGT )
    {
      // insert mgt text here
      //get_time returns nsecs. need to convert into seconds.
      int64_t mgt_time = (madara::utility::get_time () / (int64_t)100000) / (int64_t)10000;

      //insert this value into the buffer
      std::string mgtstr = boost::lexical_cast<std::string>(mgt_time);
      retstring.replace(retstring.find(tsstr),tsstr.length(),mgtstr);
      continue;
    }else
    if ( tsstr == MADARA_THREAD_NAME )
    {
      //insert thread name into buffer
      retstring.replace(retstring.find(tsstr),tsstr.length(),
                        madara::logger::Logger::get_thread_name());
      continue;
    }else
    if ( tsstr == MADARA_THREAD_HERTZ )
    {
      // insert thread hertz value into buffer
      std::string hzstr = boost::lexical_cast<std::string>(madara::logger::Logger::get_thread_hertz());
      retstring.replace(retstring.find(tsstr),tsstr.length(),hzstr);
      continue;
    }
  }
  
  return retstring;
}


void
madara::logger::Logger::log (int level, const char * message, ...)
{
  if (level <= level_)
  {
    va_list argptr;
    va_start (argptr, message);

    // Android seems to not handle printf arguments correctly as best I can tell
    char buffer[10240];
    char * begin = (char *)buffer;
    size_t remaining_buffer = sizeof (buffer);

    if (this->timestamp_format_.size () > 0)
    {
      std::string madstr = message;
      madstr = search_and_insert_custom_tstamp(madstr,MADARA_GET_TIME_MGT);
      madstr = search_and_insert_custom_tstamp(madstr,MADARA_THREAD_NAME);
      madstr = search_and_insert_custom_tstamp(madstr,MADARA_THREAD_HERTZ);
    
      char custom_buffer[10240];
      std::strcpy(custom_buffer, madstr.c_str());

      time_t rawtime;
      struct tm * timeinfo;

      time (&rawtime);
      timeinfo = localtime (&rawtime);

      madstr = search_and_insert_custom_tstamp(timestamp_format_,MADARA_GET_TIME_MGT);
      madstr = search_and_insert_custom_tstamp(madstr,MADARA_THREAD_NAME);
      madstr = search_and_insert_custom_tstamp(madstr,MADARA_THREAD_HERTZ);
      
      size_t chars_written = strftime (
        begin, remaining_buffer, madstr.c_str (), timeinfo);

      remaining_buffer -= chars_written;
      begin += chars_written;
      vsnprintf (begin, remaining_buffer, custom_buffer, argptr);
    }else
      vsnprintf (begin, remaining_buffer, message, argptr);

    va_end (argptr);

    MADARA_GUARD_TYPE guard (mutex_);

#ifdef _MADARA_ANDROID_
    if (this->term_added_ || this->syslog_added_)
    {
      if (level == LOG_ERROR)
      {
        __android_log_write (ANDROID_LOG_ERROR, tag_.c_str (), buffer);
      }
      else if (level == LOG_WARNING)
      {
        __android_log_write (ANDROID_LOG_WARN, tag_.c_str (), buffer);
      }
      else
      {
        __android_log_write (ANDROID_LOG_INFO, tag_.c_str (), buffer);
      }
    }
#else // end if _USING_ANDROID_
    if (this->term_added_ || this->syslog_added_)
    {
      fprintf (stderr, "%s", buffer);
    }
#endif

    int file_num = 0;
    for (FileVectors::iterator i = files_.begin ();
      i != files_.end (); ++i)
    {
      if (level >= LOG_DETAILED)
      {
        fprintf (stderr, "Logger::log: writing to file num %d", file_num);

        // file_num is only important if logging is detailed
        ++file_num;
      }
      fprintf (*i, "%s", buffer);
    }
  }
}
