
#include "madara_jni.h"

static JavaVM* madara_JVM = NULL;
static jclass madara_JNI = NULL;
static jclass jni_string = NULL;

MADARA_Export jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
  JNIEnv* env;
  if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK)
  {
    return JNI_ERR;
  }

  madara_JNI = (jclass)env->NewGlobalRef(env->FindClass("com/madara/MadaraJNI"));
  jni_string = (jclass)env->NewGlobalRef(env->FindClass("java/lang/String"));

  madara_JVM = vm;

  return env->GetVersion();
}

MADARA_Export void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
  JNIEnv* env;
  vm->GetEnv((void**)&env, JNI_VERSION_1_6);
  env->DeleteGlobalRef(madara_JNI);
  madara_JVM = NULL;
}

bool madara_jni_is_attached ()
{
  JNIEnv * env (0);

  if (madara_JVM)
  {
    madara_JVM->GetEnv ( (void**)&env, JNI_VERSION_1_6);
  }

  return env != 0;
}

MADARA_Export JNIEnv* madara_jni_get_env()
{
  JNIEnv* env;
  if (madara_JVM)
  {
    madara_JVM->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (env == 0) //Thread is not attached
    {
      madara_JVM->AttachCurrentThread((void**)&env, NULL);
    }
  }
  return env;
}

MADARA_Export JavaVM* madara_jni_jvm()
{
  return madara_JVM;
}

MADARA_Export jclass madara_jni_class()
{
  return madara_JNI;
}

MADARA_Export jclass jni_string_cls()
{
  return jni_string;
}

MADARA_Export JNIEnv* jni_attach()
{
  return madara_jni_get_env();
}

MADARA_Export void jni_detach()
{
  madara_JVM->DetachCurrentThread();
}