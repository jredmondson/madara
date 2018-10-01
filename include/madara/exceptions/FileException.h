/* -*- C++ -*- */
#ifndef _MADARA_EXCEPTIONS_FILEEXCEPTION_H_
#define _MADARA_EXCEPTIONS_FILEEXCEPTION_H_

#include <string>
#include "MadaraException.h"

namespace madara
{
namespace exceptions
{
/**
 * An exception for bad file interactions
 **/
class FileException : public MadaraException
{
public:
  using MadaraException::MadaraException;
};
}
}

#endif /* _MADARA_EXCEPTIONS_FILEEXCEPTION_H_ */
