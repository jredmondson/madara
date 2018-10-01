/* -*- C++ -*- */
#ifndef _MADARA_EXPRESSION_KARLEXCEPTION_H_
#define _MADARA_EXPRESSION_KARLEXCEPTION_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <vector>
#include <stdexcept>

#include "MadaraException.h"

namespace madara
{
namespace exceptions
{
/**
 * An exception for unrecoverable KaRL compilation issues
 **/
class KarlException : public MadaraException
{
public:
  using MadaraException::MadaraException;
};
}
}

#endif  // _MADARA_NO_KARL_

#endif /* _MADARA_EXPRESSION_KARLEXCEPTION_H_ */
