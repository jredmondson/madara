
#include "madara_jni.h"

#include "madara/logger/GlobalLogger.h"
#include <algorithm>
#include <string>
#include <assert.h>
#include <iostream>

namespace logger = madara::logger;

static JavaVM* madara_JVM = NULL;

static jobject madara_class_loader;

jint JNICALL JNI_OnLoad(JavaVM* vm, void*)
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "madara:JNI_OnLoad: "
      "Entering OnLoad\n");

  JNIEnv* env;
  if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK)
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

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
      "madara:JNI_OnLoad: "
      "Retrieving current thread context\n");

  jclass thread_class = env->FindClass("java/lang/Thread");
  jclass cl_class = env->FindClass("java/lang/ClassLoader");
  jmethodID current_thread = env->GetStaticMethodID(
      thread_class, "currentThread", "()Ljava/lang/Thread;");

  jmethodID get_class_loader = env->GetMethodID(
      thread_class, "getContextClassLoader", "()Ljava/lang/ClassLoader;");

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
      "madara:JNI_OnLoad: "
      "Retrieving class loader for current thread\n");

  jobject thread = env->CallStaticObjectMethod(thread_class, current_thread);
  jobject class_loader = env->CallObjectMethod(thread, get_class_loader);

  if (class_loader != NULL)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "madara:JNI_OnLoad: "
        "SUCCESS: Class loader found. Storing reference.\n");

    madara_class_loader = env->NewGlobalRef(class_loader);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
        "madara:JNI_OnLoad: "
        "ERROR: No class loader found.\n");
  }

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
      "madara:JNI_OnLoad: "
      "Clearing any exceptions\n");

  /**
   * We could send exceptions to stderr, but that's a waste of time
   * on Android, and there is no way to redirect to the logger.
   **/
  if (env->ExceptionCheck())
  {
    env->ExceptionClear();
  }

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
      "madara:JNI_OnLoad: "
      "Creating handle to findClass\n");

  jmethodID find_class = env->GetMethodID(
      cl_class, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
      "madara:JNI_OnLoad: "
      "Attempting to call class loader\n");

  jclass kr_class = (jclass)env->CallObjectMethod(madara_class_loader,
      find_class, env->NewStringUTF("ai.madara.knowledge.KnowledgeRecord"));

  if (!kr_class || env->ExceptionCheck())
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
        "madara:JNI_OnLoad: "
        "Class loader call failed for ai.madara.knowledge.KnowledgeRecord\n");
    env->ExceptionDescribe();
    env->ExceptionClear();
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "madara:JNI_OnLoad: "
        "Class loader call succeeded\n");
  }

  env->DeleteLocalRef(kr_class);
  env->DeleteLocalRef(class_loader);
  env->DeleteLocalRef(thread);
  env->DeleteLocalRef(cl_class);
  env->DeleteLocalRef(thread_class);

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "madara:JNI_OnLoad: "
      "Leaving OnLoad\n");

  return env->GetVersion();
}

void JNICALL JNI_OnUnload(JavaVM* vm, void*)
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "madara:JNI_OnUnload: "
      "Entering Unload\n");

  JNIEnv* env;
  vm->GetEnv((void**)&env, JNI_VERSION_1_6);

  if (env)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
        "madara:JNI_OnUnload: "
        "Deleting global ref to class loader\n");

    env->DeleteGlobalRef(madara_class_loader);
  }

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "madara:JNI_OnUnload: "
      "Leaving OnUnload\n");

  madara_JVM = NULL;
}

bool madara_jni_is_attached()
{
  JNIEnv* env(0);

  if (madara_JVM)
  {
    madara_JVM->GetEnv((void**)&env, JNI_VERSION_1_6);
  }

  return env != 0;
}

JNIEnv* madara_jni_get_env()
{
  JNIEnv* env(0);
  if (madara_JVM)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
        "madara_jni_get_env:"
        " calling GetEnv\n");

    madara_JVM->GetEnv((void**)&env, JNI_VERSION_1_6);

    if (env == 0)
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
          "madara_jni_get_env:"
          " calling AttachCurrentThread\n");
      madara_JVM->AttachCurrentThread((void**)&env, NULL);
    }
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
        "madara_jni_get_env:"
        " No VM exists\n");
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
    madara_JVM->DetachCurrentThread();
  }
}

jclass madara::utility::java::find_class(JNIEnv* env, const char* name)
{
  jclass result(0);

  if (env != 0)
  {
    std::string dot_name = name;

    std::replace(dot_name.begin(), dot_name.end(), '/', '.');

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
        "madara::utility::java::find_class: "
        "Retrieving class loader and loadClass method\n",
        dot_name.c_str());

    jclass class_loader = env->FindClass("java/lang/ClassLoader");

    jmethodID load_class = env->GetMethodID(
        class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "madara::utility::java::find_class: "
        "Attempting to find class %s via ClassLoader\n",
        dot_name.c_str());

    jstring j_name = env->NewStringUTF(dot_name.c_str());

    jclass loaded_class =
        (jclass)env->CallObjectMethod(madara_class_loader, load_class, j_name);

    result = (jclass)env->NewWeakGlobalRef(loaded_class);

    env->DeleteLocalRef(loaded_class);
    env->DeleteLocalRef(j_name);
    env->DeleteLocalRef(class_loader);

    if (env->ExceptionCheck())
    {
      env->ExceptionClear();

      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_WARNING,
          "madara::utility::java::find_class: "
          "Exception in Class Loader. Attempting FindClass on %s.\n",
          name);

      jobject local_class = env->FindClass(name);

      result = (jclass)env->NewWeakGlobalRef(local_class);

      env->DeleteLocalRef(local_class);

      if (env->ExceptionCheck())
      {
        env->ExceptionClear();

        result = 0;
      }
    }

    if (result == 0)
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ERROR,
          "madara::utility::java::find_class: "
          "Class %s was not found. Returning zero. Expect exception.\n",
          name);
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MINOR,
          "madara::utility::java::find_class: "
          "Class was found. Returning.\n",
          name);
    }
  }

  return result;
}

void madara::utility::java::throw_dead_obj_exception(
    JNIEnv* env, const char* message)
{
  jclass excp_cls = madara::utility::java::find_class(
      env, "ai/madara/exceptions/MadaraDeadObjectException");
  env->ThrowNew(excp_cls, message);
}

void madara::utility::java::throw_feature_not_impl_exception(
    JNIEnv* env, const char* message)
{
  jclass excp_cls = madara::utility::java::find_class(
      env, "ai/madara/exceptions/FeatureNotImplementedException");
  env->ThrowNew(excp_cls, message);
}
