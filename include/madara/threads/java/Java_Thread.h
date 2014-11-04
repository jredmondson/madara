

#ifndef _MADARA_THREADS_JAVA_THREAD_H_
#define _MADARA_THREADS_JAVA_THREAD_H_

/**
 * @file Base_Thread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a base thread implementation for
 * use when running MADARA threads
 **/

#include <jni.h>
#include "madara_jni.h"

#include "madara/threads/Base_Thread.h"

namespace Madara
{
  namespace Threads
  {
    /**
     * Abstract base class for implementing threads
     **/
    class Java_Thread : public Base_Thread
    {
    public:
      /**
       * Constructor
       **/
      Java_Thread (jobject obj);

      /**
       * Destructor
       **/
      ~Java_Thread ();

      /**
       * Assignment operator
       **/
      void operator= (const Java_Thread & rhs);

      /**
       * Initializes thread with MADARA context
       * @param   context   context for querying current program state
       **/
      virtual void init (Knowledge_Engine::Knowledge_Base & context);

      /**
       * Executes the main thread logic. In a Base_Thread, the
       * execute is called once. Afterwards, cleanup is called.
       **/
      virtual void run (void);
      
      /**
       * Calls the 
       **/
      virtual void cleanup (void);

    protected:
      
      /// the Java thread implementation of com.madara.threads.BaseThread
      jobject obj_;
      
      /// the class of the Java object obj_
      jclass class_;

      /// handle to the run_method_
      jmethodID run_method_;
    };
  }
}

#endif  // _MADARA_THREADS_JAVA_THREAD_H_
