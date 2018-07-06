#ifndef _MADARA_ZEROMQ_CONTEXT_H_
#define _MADARA_ZEROMQ_CONTEXT_H_

/**
* @file ZMQContext.h
* @author James Edmondson <jedmondson@gmail.com>
*
* This file contains the ZMQContext class, which provides a singleton
* instance for a global ZMQ context
**/

#include "madara/MadaraExport.h"
#include <atomic>

namespace madara
{
  namespace transport
  {
    /**
    * @class ZMQContext
    * @brief A class that manages a ZMQ context
    **/
    class MADARA_EXPORT ZMQContext
    {
    public:
      /**
      * Constructor
      * @param   context   the context to manage
      **/
      ZMQContext ();

      /**
      * Destructor
      **/
      ~ZMQContext ();

      /**
      * Destroys the underlying context
      **/
      void destroy_context (void);

      /**
       * Retrieves the underlying ZMQ context
       * @return current ZMQ context
       **/
      inline void * get_context ()
      {
        return context_;
      }

      /**
      * Retrieves the underlying ZMQ context
      * @param   context   the context to manage
      **/
      void set_context (void * context);

      /**
      * Adds a reference to the context. This is useful for
      * very lazy compilers that do not forcefully call
      * destructors of global variables, such as VS apparently does.
      **/
      void add_ref (void);

      /**
       * Removes a reference to the context. This is useful for
      * very lazy compilers that do not forcefully call
      * destructors of global variables, such as VS apparently does.
      **/
      void rem_ref (void);

    private:

      /// the context
      void * context_;

      /// track the number of references to the context
      std::atomic<int> references_;
    };

    extern MADARA_EXPORT ZMQContext zmq_context;
  }
}

#endif // _MADARA_ZEROMQ_CONTEXT_H_