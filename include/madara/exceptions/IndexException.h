/* -*- C++ -*- */
#ifndef _MADARA_EXCEPTIONS_INDEXEXCEPTION_H_
#define _MADARA_EXCEPTIONS_INDEXEXCEPTION_H_

#include <string>
#include "MadaraException.h"

namespace madara
{
  namespace exceptions
  {
    /**
     * An exception for out-of-bounds accessing in arrays/vectors
     **/
    class IndexException : public MadaraException
    {
    public:
      using MadaraException::MadaraException;
    };
  }
}

#endif /* _MADARA_EXCEPTIONS_INDEXEXCEPTION_H_ */
