

#ifndef _MADARA_THREADS_THREADER_INL_
#define _MADARA_THREADS_THREADER_INL_

/**
 * @file Threader.inl
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains inline methods for the Threader class
 **/

#include "Threader.h"


inline void
madara::threads::Threader::change_hertz (const std::string name, double hertz)
{
  control_->set (name + ".hertz", hertz);
}


#endif  // _MADARA_THREADS_THREADER_INL_
