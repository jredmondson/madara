/* -*- C++ -*- */
#ifndef _MADARA_EXCEPTION_BADANYACCESS_H_
#define _MADARA_EXCEPTION_BADANYACCESS_H_

#include <string>
#include <stdexcept>

#include <boost/type_index.hpp>

#include "MadaraException.h"
#include "madara/utility/SupportTest.h"

namespace madara
{
  namespace exceptions
  {
    /**
     * An exception for misuse of the Any class
     **/
    class BadAnyAccess : public MadaraException
    {
    public:
      using MadaraException::MadaraException;

      template<typename Got>
      BadAnyAccess(type<Got>, boost::typeindex::type_index expected)
        : BadAnyAccess(std::string("Bad Any access: expected ") +
            expected.pretty_name() + ", got " +
            boost::typeindex::type_id<Got>().pretty_name()) {}
    };
  }
}

#endif /* _MADARA_EXCEPTION_BADANYACCESS_H_ */
