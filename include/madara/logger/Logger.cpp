#include "Logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

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
      time_t rawtime;
      struct tm * timeinfo;

      time (&rawtime);
      timeinfo = localtime (&rawtime);

      size_t chars_written = strftime (
        begin, remaining_buffer, timestamp_format_.c_str (), timeinfo);

      remaining_buffer -= chars_written;
      begin += chars_written;
    }

    vsnprintf (begin, remaining_buffer, message, argptr);

    va_end (argptr);

    Guard guard (mutex_);

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

    for (FileVectors::iterator i = files_.begin ();
      i != files_.end (); ++i)
    {
      fprintf (*i, "%s", buffer);
    }
  }
}
