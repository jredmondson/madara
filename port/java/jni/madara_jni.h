
#ifndef _Included_ai_madara_JNI
#define _Included_ai_madara_JNI

#include <jni.h>
#include "madara/MadaraExport.h"

MADARA_EXPORT jint JNICALL JNI_OnLoad (JavaVM *vm, void *reserved);
MADARA_EXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved);

MADARA_EXPORT bool madara_jni_is_attached ();
MADARA_EXPORT JNIEnv* madara_jni_get_env();

MADARA_EXPORT JavaVM* madara_jni_jvm();

MADARA_EXPORT jclass madara_jni_class();
MADARA_EXPORT jclass jni_string_cls();

MADARA_EXPORT JNIEnv* jni_attach();
MADARA_EXPORT void jni_detach();

namespace madara
{
  namespace utility
  {
    namespace java
    {
      /**
       * Android-safe, Thread-safe find class method
       * @param  env   Java environment
       * @param  name  name of the class to find
       **/
      jclass MADARA_EXPORT find_class (JNIEnv * env, const char * name);

      /**
       * Throws an exception when developers use JNI objects after being deleted
       * @param  env   Java environment
       * @param  name  Message to embed in exception
       **/
      void MADARA_EXPORT throw_dead_obj_exception (JNIEnv * env, const char * message);

      /**
             * Throws a runtime exception when developers use features that are not explicitly supported by MADARA
             * @param  env   Java environment
             * @param  name  Message to embed in exception
             **/
     void MADARA_EXPORT throw_feature_not_impl_exception (JNIEnv * env, const char * message);

    }
  }
}

#endif
