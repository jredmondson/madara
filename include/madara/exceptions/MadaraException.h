/* -*- C++ -*- */
#ifndef _MADARA_EXCEPTIONS_MADARAEXCEPTION_H_
#define _MADARA_EXCEPTIONS_MADARAEXCEPTION_H_

#include <string>
#include <stdexcept>

namespace madara
{
namespace exceptions
{
/**
 * Base exception for all custom Madara exceptions
 **/
class MadaraException : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};
}
}

#endif /* _MADARA_EXCEPTIONS_MADARAEXCEPTION_H_ */
