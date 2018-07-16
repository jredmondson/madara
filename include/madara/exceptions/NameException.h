/* -*- C++ -*- */
#ifndef _MADARA_EXCEPTIONS_INVALIDNAMEEXCEPTION_H_
#define _MADARA_EXCEPTIONS_INVALIDNAMEEXCEPTION_H_

#include <string>
#include "MadaraException.h"

namespace madara
{
  namespace exceptions
  {
    /**
     * An exception for setting an invalid name in MADARA
     **/
    class NameException : public MadaraException
    {
    public:
      using MadaraException::MadaraException;
    };
  }
}

#endif /* _MADARA_EXCEPTIONS_INVALIDNAMEEXCEPTION_H_ */
