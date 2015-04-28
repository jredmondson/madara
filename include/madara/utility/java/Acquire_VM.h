
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
        Acquire_VM ()
        {
          needs_detach = !madara_jni_is_attached();
          env = madara_jni_get_env ();
        }

        ~Acquire_VM()
        {
          if (needs_detach)
            jni_detach();
        }

        JNIEnv * env;
        bool needs_detach;
      };
    }
  }
}

#endif // not defined _MADARA_JAVA_ACQUIRE_VM_H_
