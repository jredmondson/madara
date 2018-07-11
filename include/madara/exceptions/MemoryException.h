/* -*- C++ -*- */
#ifndef _MADARA_EXCEPTION_MEMORYEXCEPTION_H_
#define _MADARA_EXCEPTION_MEMORYEXCEPTION_H_

#include <string>
#include <stdexcept>

namespace madara
{
  namespace exceptions
  {
    /**
     * An exception for general memory errors like out-of-memory
     **/
    class MemoryException : public std::runtime_error
    {
    public:
      /**
       * Constructor
       **/
      explicit MemoryException (const std::string & what)
      : std::runtime_error (what)
      {

      }
    };
  }
}

#endif /* _MADARA_EXCEPTION_MEMORYEXCEPTION_H_ */
