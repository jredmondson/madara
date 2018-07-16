/* -*- C++ -*- */
#ifndef _MADARA_EXCEPTIONS_THREADEXCEPTION_H_
#define _MADARA_EXCEPTIONS_THREADEXCEPTION_H_

#include <string>
#include "MadaraException.h"

namespace madara
{
  namespace exceptions
  {
    /**
     * An exception for general thread-related errors
     **/
    class ThreadException : public MadaraException
    {
    public:
      /// inherit the base constructor
      using MadaraException::MadaraException;
    };
  }
}

#endif /* _MADARA_EXCEPTIONS_THREADEXCEPTION_H_ */
