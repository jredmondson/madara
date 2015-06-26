
#include "madara_jni.h"

#include "madara/logger/Global_Logger.h"
#include <algorithm>
#include <string>
#include <assert.h>

namespace logger = Madara::Logger;

static JavaVM * madara_JVM = NULL;

static jobject madara_class_loader;

jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "Madara:JNI_OnLoad: "
    "Entering OnLoad\n");

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
  
  madara_logger_ptr_log (logger::global_logger.get(),
    logger::LOG_MINOR,
    "Madara:JNI_OnLoad: "
    "Retrieving current thread context\n");

  jclass thread_class = env->FindClass ("java/lang/Thread");
  jclass cl_class = env->FindClass ("java/lang/ClassLoader");
  jmethodID current_thread = env->GetStaticMethodID (
    thread_class, "currentThread", "()Ljava/lang/Thread;");

  jmethodID get_class_loader = env->GetMethodID (
    thread_class, "getContextClassLoader", "()Ljava/lang/ClassLoader;");

  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MINOR,
    "Madara:JNI_OnLoad: "
    "Retrieving class loader for current thread\n");

  jobject thread = env->CallStaticObjectMethod (thread_class, current_thread);
  jobject classLoader = env->CallObjectMethod (thread, get_class_loader);


  if (classLoader != NULL)
  {
    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Madara:JNI_OnLoad: "
      "SUCCESS: Class loader found. Storing reference.\n");

    madara_class_loader = env->NewGlobalRef (classLoader);
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_ERROR,
      "Madara:JNI_OnLoad: "
      "ERROR: No class loader found.\n");
  }

  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MINOR,
    "Madara:JNI_OnLoad: "
    "Clearing any exceptions\n");

  /**
   * We could send exceptions to stderr, but that's a waste of time
   * on Android, and there is no way to redirect to the logger.
   **/
  if (env->ExceptionCheck ())
  {
    env->ExceptionClear ();
  }

  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MINOR,
    "Madara:JNI_OnLoad: "
    "Creating handle to findClass\n");

  jmethodID find_class = env->GetMethodID (cl_class, "findClass",
    "(Ljava/lang/String;)Ljava/lang/Class;");

  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MINOR,
    "Madara:JNI_OnLoad: "
    "Attempting to call class loader\n");

  jclass kr_class = (jclass) env->CallObjectMethod (
    madara_class_loader,
    find_class,
    env->NewStringUTF ("com.madara.KnowledgeRecord"));

  if (!kr_class || env->ExceptionCheck ())
  {
    env->ExceptionClear ();
    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_ERROR,
      "Madara:JNI_OnLoad: "
      "Class loader call failed for com.madara.KnowledgeRecord\n");
  }
  else
  {
    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Madara:JNI_OnLoad: "
      "Class loader call succeeded\n");
  }

  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "Madara:JNI_OnLoad: "
    "Leaving OnLoad\n");

  return env->GetVersion();
}

void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
    "Madara:JNI_OnUnload: "
    "Entering Unload\n");

  JNIEnv* env;
  vm->GetEnv ((void**)&env, JNI_VERSION_1_6);

  if (env)
  {
    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MINOR,
      "Madara:JNI_OnUnload: "
      "Deleting global ref to class loader\n");

    env->DeleteGlobalRef (madara_class_loader);
  }

  madara_logger_ptr_log (logger::global_logger.get (),
    logger::LOG_MAJOR,
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
    std::string dot_name = name;

    std::replace (dot_name.begin (), dot_name.end (), '/', '.');

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MINOR,
      "Madara::Utility::Java::find_class: "
      "Retrieving class loader and loadClass method\n", dot_name.c_str ());

    jclass java_lang_class_loader = env->FindClass ("java/lang/ClassLoader");
    assert (java_lang_class_loader != NULL);
    jmethodID loadClass =
      env->GetMethodID (java_lang_class_loader,
      "loadClass",
      "(Ljava/lang/String;)Ljava/lang/Class;");

    madara_logger_ptr_log (logger::global_logger.get (),
      logger::LOG_MAJOR,
      "Madara::Utility::Java::find_class: "
      "Attempting to find class %s via ClassLoader\n", dot_name.c_str ());

    result = (jclass)env->NewWeakGlobalRef (env->CallObjectMethod (
      madara_class_loader,
      loadClass,
      env->NewStringUTF (dot_name.c_str ())));

    if (env->ExceptionCheck ())
    {
      env->ExceptionClear ();

      madara_logger_ptr_log (logger::global_logger.get (),
        logger::LOG_WARNING,
        "Madara::Utility::Java::find_class: "
        "Exception in Class Loader. Attempting FindClass on %s.\n", name);

      result = (jclass)env->NewWeakGlobalRef (env->FindClass (name));

      if (env->ExceptionCheck ())
      {
        env->ExceptionClear ();

        result = 0;
      }
    }

    if (result == 0)
    {
      madara_logger_ptr_log (logger::global_logger.get (),
        logger::LOG_ERROR,
        "Madara::Utility::Java::find_class: "
        "Class %s was not found. Returning zero. Expect exception.\n", name);
    }
    else
    {
      madara_logger_ptr_log (logger::global_logger.get (),
        logger::LOG_MINOR,
        "Madara::Utility::Java::find_class: "
        "Class was found. Returning.\n", name);
    }
  }

  return result;
}
