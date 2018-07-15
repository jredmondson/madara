/* -*- C++ -*- */
#ifndef _MADARA_EXPRESSION_KARLEXCEPTION_H_
#define _MADARA_EXPRESSION_KARLEXCEPTION_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <vector>
#include <stdexcept>

#include "madara/exceptions/MadaraException.h"

namespace madara
{
  namespace expression
  {
    /**
     * An exception for unrecoverable KaRL compilation issues
     **/
    class KarlException : public exceptions::MadaraException
    {
    public:
      using exceptions::MadaraException::MadaraException;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_EXPRESSION_KARLEXCEPTION_H_ */
