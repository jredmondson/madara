/* -*- C++ -*- */
#ifndef _MADARA_EXCEPTION_THREADEXCEPTION_H_
#define _MADARA_EXCEPTION_THREADEXCEPTION_H_

#include <string>
#include <stdexcept>

namespace madara
{
  namespace exceptions
  {
    /**
     * An exception for general thread-related errors
     **/
    class ThreadException : public std::runtime_error
    {
    public:
      /// inherit the base constructor
      using std::runtime_error::runtime_error;
    };
  }
}

#endif /* _MADARA_EXCEPTION_THREADEXCEPTION_H_ */
