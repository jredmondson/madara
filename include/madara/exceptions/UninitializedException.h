/* -*- C++ -*- */
#ifndef _MADARA_EXCEPTIONS_UNINITIALIZEDEXCEPTION_H_
#define _MADARA_EXCEPTIONS_UNINITIALIZEDEXCEPTION_H_

#include <string>
#include "MadaraException.h"

namespace madara
{
namespace exceptions
{
/**
 * An exception for attempting to access an invalid context1
 **/
class UninitializedException : public MadaraException
{
public:
  using MadaraException::MadaraException;
};
}
}

#endif /* _MADARA_EXCEPTIONS_UNINITIALIZEDEXCEPTION_H_ */
