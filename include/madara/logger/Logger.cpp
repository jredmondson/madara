#include "Logger.h"

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

    Guard guard (mutex_);

#ifdef _MADARA_ANDROID_
    if (this->term_added_ || this->syslog_added_)
    {
      va_list localargs;
      va_copy (localargs, argptr);

      if (level == LOG_ERROR)
      {
        __android_log_print (ANDROID_LOG_ERROR, "madara", message, localargs);
      }
      else if (level == LOG_WARNING)
      {
        __android_log_print (ANDROID_LOG_WARN, "madara", message, localargs);
      }
      else
      {
        __android_log_print (ANDROID_LOG_INFO, "madara", message, localargs);
      }

      va_end (localargs);
    }
#else // end if _USING_ANDROID_
    if (this->term_added_ || this->syslog_added_)
    {
      va_list localargs;
      va_copy (localargs, argptr);

      vfprintf (stderr, message, localargs);

      va_end (localargs);
    }
#endif

    for (File_Vectors::iterator i = files_.begin ();
      i != files_.end (); ++i)
    {
      va_list localargs;
      va_copy (localargs, argptr);

      vfprintf (*i, message, localargs);

      va_end (localargs);
    }

    va_end (argptr);
  }
}
