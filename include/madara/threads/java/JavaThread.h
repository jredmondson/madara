

#ifndef _MADARA_THREADS_JAVA_THREAD_H_
#define _MADARA_THREADS_JAVA_THREAD_H_

/**
 * @file BaseThread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a base thread implementation for
 * use when running MADARA threads
 **/

#include <jni.h>
#include "madara_jni.h"

#include "madara/threads/BaseThread.h"

namespace madara
{
  namespace threads
  {
    /**
     * A facade for a user-defined Java thread class
     **/
    class JavaThread : public BaseThread
    {
    public:

      /**
       * Destructor
       **/
      ~JavaThread ();

      /**
       * Assignment operator
       **/
      void operator= (const JavaThread & rhs);

      /**
       * Initializes Java thread with MADARA context
       * @param   context   context for querying current program state
       **/
      virtual void init (knowledge::KnowledgeBase & context);

      /**
       * Executes the Java thread's logic.
       **/
      virtual void run (void);
      
      /**
       * Calls the Java cleanup method
       **/
      virtual void cleanup (void);

      /**
       * Creates a JavaThread. The obj is checked for compliance before
       * returning a JavaThread.
       * @param obj  a user-defined com.madara.threads.BaseThread object
       **/
      static JavaThread * create (jobject obj);

      /**
       * Checks a Java object for compliance with
       * com.madara.threads.BaseThread. Fills in class members if compliant.
       * @param obj  a user-defined com.madara.threads.BaseThread object
       **/
      bool check_compliance (jobject obj);

    protected:
      
      /**
       * Constructor
       **/
      JavaThread ();
      
      /**
       * Initializes the Java thread implementation's control plane variables
       * @param control the control plane between Threader and threads
       **/
      virtual void init_control_vars (
        knowledge::KnowledgeBase & control);

      /// the Java thread implementation of com.madara.threads.BaseThread
      jobject obj_;
      
      /// the class of the Java object obj_
      jclass class_;

      /// handle to the run method
      jmethodID run_method_;

      /// handle to the init method
      jmethodID init_method_;
      
      /// handle to the cleanup method
      jmethodID cleanup_method_;
    };
  }
}

#endif  // _MADARA_THREADS_JAVA_THREAD_H_
