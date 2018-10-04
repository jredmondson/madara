

#ifndef _MADARA_THREADS_THREADER_INL_
#define _MADARA_THREADS_THREADER_INL_

/**
 * @file Threader.inl
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains inline methods for the Threader class
 **/

#include "Threader.h"

inline void madara::threads::Threader::change_hertz(
    const std::string name, double hertz)
{
  control_.set(name + ".hertz", hertz);
}

inline void madara::threads::Threader::enable_debug(const std::string name)
{
  control_.set(name + ".debug", true);
}

inline void madara::threads::Threader::disable_debug(const std::string name)
{
  control_.set(name + ".debug", false);
}

inline void madara::threads::Threader::debug_to_kb(const std::string prefix)
{
  debug_ = true;
  control_.set(".debug_to_kb", prefix);
}

inline void madara::threads::Threader::enable_debug(void)
{
  debug_ = true;
}

inline void madara::threads::Threader::disable_debug(void)
{
  debug_ = false;
}

inline madara::knowledge::KnowledgeBase
madara::threads::Threader::get_data_plane(void)
{
  return data_;
}

inline madara::knowledge::KnowledgeBase
madara::threads::Threader::get_control_plane(void)
{
  return control_;
}

#endif  // _MADARA_THREADS_THREADER_INL_
