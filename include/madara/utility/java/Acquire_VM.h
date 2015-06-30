
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
         * Constructor
         * @param  no_detach  if true, do not try to detach, regardless
         *                    of results of JNI Env returned
         **/
        Acquire_VM (bool no_detach = false)
        {
          if (!no_detach)
          {
            needs_detach = !madara_jni_is_attached ();
          }
          else
          {
            needs_detach = false;
          }

          env = madara_jni_get_env ();
        }

        /**
        * Destructor
        **/
        ~Acquire_VM()
        {
          if (needs_detach)
            jni_detach();
        }

        /**
         * The Java environment
         **/
        JNIEnv * env;

      protected:
        /**
        * Flag for determining whether to call detach in destructor
        **/
        bool needs_detach;
      };
    }
  }
}

#endif // not defined _MADARA_JAVA_ACQUIRE_VM_H_
