
#ifndef _MADARA_LOGGER_LOGGER_INL_
#define _MADARA_LOGGER_LOGGER_INL_

#include "Logger.h"
#include <stdarg.h>

#ifdef _MADARA_ANDROID_
#include <android/log.h>
#endif

inline void
madara::logger::Logger::add_file (const std::string & filename)
{
  madara_logger_ptr_log (this, LOG_MAJOR,
    "Logger::add_file: attempting to open file %s\n",
    filename.c_str ());

  if (filename != "")
  {
    FILE * new_file = fopen (filename.c_str (), "a+");

    if (new_file)
    {
      madara_logger_ptr_log (this, LOG_MAJOR,
        "Logger::add_file: opened file %s for logging\n",
        filename.c_str ());

      MADARA_GUARD_TYPE guard (mutex_);

      files_.push_back (new_file);
    }
    else
    {
      madara_logger_ptr_log (this, LOG_MAJOR,
        "Logger::add_file: unable to open file %s for logging\n",
        filename.c_str ());
    }
  }
}

inline void
madara::logger::Logger::set_level (int level)
{
  level_ = level;
}

inline std::string
madara::logger::Logger::get_tag (void)
{
  MADARA_GUARD_TYPE guard (mutex_);

  return tag_;
}

inline int 
madara::logger::Logger::get_thread_level(void)
{
  return thread_level_;
}

inline std::string 
madara::logger::Logger::get_thread_name(void)
{
  return thread_name_;
}

inline double 
madara::logger::Logger::get_thread_hertz(void)
{
  return thread_hertz_;
}

inline void 
madara::logger::Logger::set_thread_level(int level)
{
  thread_level_ = level;
}

inline void 
madara::logger::Logger::set_thread_name(const std::string name)
{
  thread_name_ = name;
}

inline void
madara::logger::Logger::set_thread_hertz(double hertz)
{
  thread_hertz_ = hertz;
}

inline void
madara::logger::Logger::set_tag (const std::string & tag)
{
  MADARA_GUARD_TYPE guard (mutex_);

  tag_ = tag;
}

inline int
madara::logger::Logger::get_level (void)
{
  return level_;
}

inline void
madara::logger::Logger::add_term (void)
{
  MADARA_GUARD_TYPE guard (mutex_);

  this->term_added_ = true;
}

inline void
madara::logger::Logger::add_syslog (void)
{
  MADARA_GUARD_TYPE guard (mutex_);

  this->syslog_added_ = true;
}

inline void
madara::logger::Logger::clear (void)
{
  MADARA_GUARD_TYPE guard (mutex_);

  this->term_added_ = false;
  this->syslog_added_ = false;

  for (FileVectors::iterator i = files_.begin ();
    i != files_.end (); ++i)
  {
    if (*i != stderr)
    {
      fclose (*i);
    }
  }

  files_.clear ();
}

inline void
madara::logger::Logger::set_timestamp_format (const std::string & format)
{
  MADARA_GUARD_TYPE guard (mutex_);

  this->timestamp_format_ = format;
}


#endif // _MADARA_LOGGER_LOGGER_INL_
