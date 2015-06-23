#include "Logger.h"
#include <stdio.h>
#include <stdarg.h>

Madara::Logger::Logger::Logger (bool log_to_terminal)
: mutex_ (), level_ (LOG_EMERGENCY),
  term_added_ (log_to_terminal), syslog_added_ (false), tag_ ("madara")
{
  if (log_to_terminal)
  {
    add_term ();
  }
}

Madara::Logger::Logger::~Logger ()
{
  clear ();
}

void
Madara::Logger::Logger::log (int level, const char * message, ...)
{
  if (level <= level_)
  {
    va_list argptr;
    va_start (argptr, message);

    // Android seems to not handle printf arguments correctly as best I can tell
    char buffer[10240];
    vsnprintf (buffer, sizeof(buffer), message, argptr);

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
      fprintf (stderr, buffer);
    }
#endif

    for (File_Vectors::iterator i = files_.begin ();
      i != files_.end (); ++i)
    {
      fprintf (*i, buffer);
    }
  }
}
