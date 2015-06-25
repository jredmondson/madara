
#include "madara_jni.h"

#include "madara/logger/Global_Logger.h"

namespace logger = Madara::Logger;

static JavaVM * madara_JVM = NULL;

static jobject madara_class_loader;
static jmethodID madara_find_class_method;

jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
  JNIEnv* env;
  if (vm->GetEnv ((void**)&env, JNI_VERSION_1_6) != JNI_OK)
  {
    return JNI_ERR;
  }

  madara_JVM = vm;

  /**
   * we need to keep a handle open to the class loader because Android
   * uses a Dex system that is broken when it comes to native thread
   * creation. Dex/Android essentially complicate the class path
   * in a way that is not POSIX and Java intent compliant.
   **/
  
  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_EMERGENCY,
    "Madara:JNI_OnLoad: "
    "Getting transient class handles for class loader\n");
  jclass static_class = env->FindClass ("com/madara/KnowledgeBase");
  jclass kb_class = env->GetObjectClass (static_class);
  jclass cl_class = env->FindClass ("java/lang/ClassLoader");

  /**
   * because class loader is abstract, we have to use the class loader method
   * of a class that we know exists. So, we use the KnowledgeBase class
   **/
  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_EMERGENCY,
    "Madara:JNI_OnLoad: "
    "Getting class specific class loader method\n");
  jmethodID kb_cl_method = env->GetMethodID (kb_class, "getClassLoader",
    "()Ljava/lang/ClassLoader;");

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_EMERGENCY,
    "Madara:JNI_OnLoad: "
    "Obtaining class specific class loader object\n");
  madara_class_loader = env->NewGlobalRef (
    env->CallObjectMethod (kb_class, kb_cl_method));

  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_EMERGENCY,
    "Madara:JNI_OnLoad: "
    "Saving global class loader for later usage in "
    "Madara::Utility::Java::find_class\n");
  madara_find_class_method = 
    env->GetMethodID (cl_class, "findClass",
    "(Ljava/lang/String;)Ljava/lang/Class;");

  madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_EMERGENCY,
    "Madara:JNI_OnLoad: "
    "Leaving OnLoad\n");

  return env->GetVersion();
}

void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
  madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_EMERGENCY,
    "Madara:JNI_OnUnload: "
    "Entering Unload\n");

  JNIEnv* env;
  vm->GetEnv ((void**)&env, JNI_VERSION_1_6);

  if (env)
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_EMERGENCY,
      "Madara:JNI_OnUnload: "
      "Deleting global ref to class loader\n");

    env->DeleteGlobalRef (madara_class_loader);
  }

  madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_EMERGENCY,
    "Madara:JNI_OnUnload: "
    "Leaving OnUnload\n");

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

JNIEnv* madara_jni_get_env ()
{
  JNIEnv* env (0);
  if (madara_JVM)
  {
    madara_JVM->GetEnv ((void**)&env, JNI_VERSION_1_6);

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

jclass Madara::Utility::Java::find_class (JNIEnv * env, const char * name)
{
  jclass result (0);

  if (env != 0)
  {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_WARNING,
      "Madara::Utility::Java::find_class: "
      "Attempting default findClass. "
      "Unlikely to work in multi-threaded Android.\n", name);

    result = (jclass)env->NewWeakGlobalRef (env->FindClass (name));

    if (result == 0)
    {
      madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_EMERGENCY,
        "Madara::Utility::Java::find_class: "
        "Attempting to find class %s via ClassLoader\n", name);

      result = (jclass)env->NewWeakGlobalRef (env->CallObjectMethod (
        madara_class_loader,
        madara_find_class_method,
        env->NewStringUTF (name)));
      
      if (result == 0)
      {
        madara_logger_ptr_log (logger::global_logger.get (),
          logger::LOG_EMERGENCY,
          "Madara::Utility::Java::find_class: "
          "Class %s was not found. Returning zero. Expect segfault.\n", name);
      }
      else
      {
        madara_logger_ptr_log (logger::global_logger.get (),
          logger::LOG_EMERGENCY,
          "Madara::Utility::Java::find_class: "
          "Class was found. Returning.\n", name);
      }
    }
  }

  return result;
}
