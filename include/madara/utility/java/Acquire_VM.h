
#ifndef _MADARA_JAVA_ACQUIRE_VM_H_
#define _MADARA_JAVA_ACQUIRE_VM_H_

#include "madara_jni.h"

/**
 * @file Acquire_VM.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains helper classes for attaching and detaching to a VM.
 **/

namespace Madara
{
  namespace Utility
  {
    namespace Java
    {
      /**
       * @class Acquire_VM
       * @brief This class encapsulates attaching and detaching to a VM
       **/
      class Acquire_VM
      {
      public:
        /**
         * Constructor attaches thread to VM if necessary
         **/
        Acquire_VM ()
        {
          env = madara_jni_get_env ();
        }

        /**
         * Destructor detaches from VM
         **/
        ~Acquire_VM()
        {
          jni_detach();
        }

        JNIEnv * env;
      };
    }
  }
}

#endif // not defined _MADARA_JAVA_ACQUIRE_VM_H_
