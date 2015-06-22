
#ifndef _MADARA_LOGGER_LOGGER_INL_
#define _MADARA_LOGGER_LOGGER_INL_

#include "Logger.h"
#include <stdarg.h>

#ifdef _MADARA_ANDROID_
#include <android/log.h>
#endif


inline void
Madara::Logger::Logger::add_file (const std::string & filename)
{
  if (filename != "")
  {
    FILE * new_file = fopen (filename.c_str (), "a+");

    if (new_file)
    {
      Guard guard (mutex_);

      files_.push_back (new_file);
    }
  }
}

inline void
Madara::Logger::Logger::set_level (int level)
{
  Guard guard (mutex_);

  level_ = level;
}

inline std::string
Madara::Logger::Logger::get_tag (void)
{
  Guard guard (mutex_);

  return tag_;
}

inline void
Madara::Logger::Logger::set_tag (const std::string & tag)
{
  Guard guard (mutex_);

  tag_ = tag;
}

inline int
Madara::Logger::Logger::get_level (void)
{
  Guard guard (mutex_);

  return level_;
}

inline void
Madara::Logger::Logger::add_term (void)
{
  Guard guard (mutex_);

  this->term_added_ = true;
}

inline void
Madara::Logger::Logger::add_syslog (void)
{
  Guard guard (mutex_);

  this->syslog_added_ = true;
}

inline void
Madara::Logger::Logger::clear (void)
{
  Guard guard (mutex_);

  this->term_added_ = false;
  this->syslog_added_ = false;

  for (File_Vectors::iterator i = files_.begin ();
    i != files_.end (); ++i)
  {
    if (*i != stderr)
    {
      fclose (*i);
    }
  }

  files_.clear ();
}

#endif // _MADARA_LOGGER_LOGGER_INL_
