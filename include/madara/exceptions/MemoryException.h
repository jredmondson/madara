/* -*- C++ -*- */
#ifndef _MADARA_EXCEPTIONS_MEMORYEXCEPTION_H_
#define _MADARA_EXCEPTIONS_MEMORYEXCEPTION_H_

#include <string>
#include "MadaraException.h"

namespace madara
{
  namespace exceptions
  {
    /**
     * An exception for general memory errors like out-of-memory
     **/
    class MemoryException : public MadaraException
    {
    public:
      using MadaraException::MadaraException;
    };
  }
}

#endif /* _MADARA_EXCEPTIONS_MEMORYEXCEPTION_H_ */
