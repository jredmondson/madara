
#include "madara_jni.h"

static JavaVM * madara_JVM = NULL;

jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
  JNIEnv* env;
  if (vm->GetEnv((void**)&env, JNI_VERSION_1_8) != JNI_OK)
  {
    return JNI_ERR;
  }

  madara_JVM = vm;

  return env->GetVersion();
}

void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
  JNIEnv* env;
  vm->GetEnv((void**)&env, JNI_VERSION_1_8);
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

JNIEnv* madara_jni_get_env()
{
  JNIEnv* env;
  if (madara_JVM)
  {
    madara_JVM->GetEnv((void**)&env, JNI_VERSION_1_8);

    if (env == 0)
    {
      madara_JVM->AttachCurrentThread((void**)&env, NULL);
    }
  }
  return env;
}

JavaVM* madara_jni_jvm()
{
  return madara_JVM;
}

JNIEnv* jni_attach()
{
  return madara_jni_get_env();
}

void jni_detach()
{
  if (madara_JVM)
  {
    madara_JVM->DetachCurrentThread ();
  }
}
