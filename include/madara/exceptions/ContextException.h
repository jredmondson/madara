/* -*- C++ -*- */
#ifndef _MADARA_EXCEPTIONS_CONTEXTEXCEPTION_H_
#define _MADARA_EXCEPTIONS_CONTEXTEXCEPTION_H_

#include <string>
#include "MadaraException.h"

namespace madara
{
namespace exceptions
{
/**
 * An exception for attempting to access an invalid context1
 **/
class ContextException : public MadaraException
{
public:
  using MadaraException::MadaraException;
};
}
}

#endif /* _MADARA_EXCEPTIONS_CONTEXTEXCEPTION_H_ */
