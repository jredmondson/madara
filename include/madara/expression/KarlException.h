/* -*- C++ -*- */
#ifndef _MADARA_EXPRESSION_KARLEXCEPTION_H_
#define _MADARA_EXPRESSION_KARLEXCEPTION_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <vector>
#include <stdexcept>

namespace madara
{
  namespace expression
  {
    /**
     * An exception for unrecoverable KaRL compilation issues
     **/
    class KarlException : public std::runtime_error
    {
    public:
      /**
       * Constructor
       **/
      explicit KarlException (const std::string & what)
      : std::runtime_error (what)
      {

      }
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _MADARA_EXPRESSION_KARLEXCEPTION_H_ */
