#include "JavaThread.h"

#include "madara/utility/java/Acquire_VM.h"

#include "madara/logger/GlobalLogger.h"

namespace threads = madara::threads;
namespace knowledge = madara::knowledge;

threads::JavaThread::JavaThread()
  : obj_(0), class_(0), run_method_(0), init_method_(0), cleanup_method_(0)
{
}

threads::JavaThread::~JavaThread()
{
  // manage VM attachment
  madara::utility::java::Acquire_VM jvm;

  if (jvm.env != 0)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "threads::JavaThread::destructor:"
        " Deleting global references.\n");

    jvm.env->DeleteGlobalRef(obj_);
    jvm.env->DeleteGlobalRef(class_);
  }
}

void threads::JavaThread::operator=(const JavaThread& rhs)
{
  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "threads::JavaThread::assignment:"
      " Checking for source not being same as dest\n");

  if (this != &rhs && obj_ != rhs.obj_)
  {
    JNIEnv* env = ::madara_jni_get_env();

    // perform the assignment
    if (env)
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
          "threads::JavaThread::assignment:"
          " Deleting global references from left hand side\n");

      env->DeleteGlobalRef(obj_);
      env->DeleteGlobalRef(class_);

      obj_ = env->NewGlobalRef(rhs.obj_);
      class_ = (jclass)env->NewGlobalRef(rhs.class_);
      cleanup_method_ = rhs.cleanup_method_;
      init_method_ = rhs.init_method_;
      run_method_ = rhs.run_method_;
    }
  }
}

void threads::JavaThread::run(void)
{
  madara::utility::java::Acquire_VM jvm;
  JNIEnv* env = jvm.env;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "threads::JavaThread(%s)::run:"
      " Calling user-defined run method\n",
      name.c_str());

  env->CallVoidMethod(obj_, run_method_);
}

void threads::JavaThread::cleanup(void)
{
  madara::utility::java::Acquire_VM jvm;
  JNIEnv* env = jvm.env;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "threads::JavaThread(%s)::cleanup:"
      " Calling user-defined cleanup method\n",
      name.c_str());

  env->CallVoidMethod(obj_, cleanup_method_);
}

void threads::JavaThread::init(knowledge::KnowledgeBase& context)
{
  madara::utility::java::Acquire_VM jvm;
  JNIEnv* env = jvm.env;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "threads::JavaThread(%s)::init:"
      " Creating Java KnowledgeBase from data plane\n",
      name.c_str());

  jclass kb_class = madara::utility::java::find_class(
      env, "ai/madara/knowledge/KnowledgeBase");

  jmethodID fromPointerCall = env->GetStaticMethodID(
      kb_class, "fromPointer", "(JZ)Lai/madara/knowledge/KnowledgeBase;");

  jboolean manage(false);

  jobject jknowledge = env->CallStaticObjectMethod(
      kb_class, fromPointerCall, (jlong)&context, manage);

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "threads::JavaThread(%s)::init:"
      " Calling user-defined init method\n",
      name.c_str());

  env->CallVoidMethod(obj_, init_method_, jknowledge);

  env->DeleteLocalRef(jknowledge);
  env->DeleteWeakGlobalRef(kb_class);
}

threads::JavaThread* threads::JavaThread::create(jobject obj)
{
  JavaThread* result = new JavaThread();

  if (!result->check_compliance(obj))
  {
    delete result;
    result = 0;
  }

  return result;
}

bool threads::JavaThread::check_compliance(jobject obj)
{
  madara::utility::java::Acquire_VM jvm;
  JNIEnv* env = jvm.env;

  bool result(true);

  if (env)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "threads::JavaThread(%s)::check_compliance:"
        " allocating global reference for object\n",
        name.c_str());

    obj_ = (jobject)env->NewGlobalRef(obj);

    if (obj_)
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
          "threads::JavaThread(%s)::check_compliance:"
          " allocating global reference for object's class\n",
          name.c_str());

      class_ = (jclass)env->NewGlobalRef(env->GetObjectClass(obj_));
      if (class_)
      {
        madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
            "threads::JavaThread(%s)::check_compliance:"
            " class and object obtained successfully\n",
            name.c_str());

        // stash the method calls to minimize call time
        run_method_ = env->GetMethodID(class_, "run", "()V");
        init_method_ = env->GetMethodID(
            class_, "init", "(Lai/madara/knowledge/KnowledgeBase;)V");
        cleanup_method_ = env->GetMethodID(class_, "cleanup", "()V");

        if (!run_method_)
        {
          madara_logger_ptr_log(logger::global_logger.get(),
              logger::LOG_EMERGENCY,
              "threads::JavaThread(%s)::check_compliance:"
              " ERROR: run method must be defined\n",
              name.c_str());

          result = false;
        }

        if (!init_method_)
        {
          madara_logger_ptr_log(logger::global_logger.get(),
              logger::LOG_EMERGENCY,
              "threads::JavaThread(%s)::check_compliance:"
              " ERROR: init method must be defined\n",
              name.c_str());

          result = false;
        }

        if (!cleanup_method_)
        {
          madara_logger_ptr_log(logger::global_logger.get(),
              logger::LOG_EMERGENCY,
              "threads::JavaThread(%s)::check_compliance:"
              " ERROR: cleanup method must be defined\n",
              name.c_str());

          result = false;
        }
      }
      else
      {
        madara_logger_ptr_log(logger::global_logger.get(),
            logger::LOG_EMERGENCY,
            "threads::JavaThread(%s)::check_compliance:"
            " ERROR: class object inaccessible\n",
            name.c_str());

        result = false;
      }
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
          "threads::JavaThread(%s)::check_compliance:"
          " ERROR: object is invalid\n",
          name.c_str());

      result = false;
    }
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "threads::JavaThread(%s)::check_compliance:"
        " ERROR: unable to acquire JAVA environment\n",
        name.c_str());

    result = false;
  }

  return result;
}

void threads::JavaThread::init_control_vars(knowledge::KnowledgeBase& control)
{
  // Initialize the underlying control variables
  threads::BaseThread::init_control_vars(control);

  // setup the Java variables
  madara::utility::java::Acquire_VM jvm;
  JNIEnv* env = ::madara_jni_get_env();

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "threads::JavaThread(%s)::init_control_vars:"
      " Populating user control plane variables\n",
      name.c_str());

  // obtain fromPointer method for ai.madara.knowledge.containers.Integer
  jclass i_class = madara::utility::java::find_class(
      env, "ai/madara/knowledge/containers/Integer");
  jmethodID fromPointerCall = env->GetStaticMethodID(
      i_class, "fromPointer", "(JZ)Lai/madara/knowledge/containers/Integer;");

  // the user thread should not manage C++ memory. We'll do that.
  jboolean manage(false);

  // create java objects for the underlying containers
  jobject jterminate = env->CallStaticObjectMethod(
      i_class, fromPointerCall, (jlong)&terminated, manage);
  jobject jpaused = env->CallStaticObjectMethod(
      i_class, fromPointerCall, (jlong)&paused, manage);

  // set the user-defined threads's containers to these new containers
  jfieldID fieldId = env->GetFieldID(
      class_, "terminated", "Lai/madara/knowledge/containers/Integer;");
  env->SetObjectField(obj_, fieldId, jterminate);

  fieldId = env->GetFieldID(
      class_, "paused", "Lai/madara/knowledge/containers/Integer;");
  env->SetObjectField(obj_, fieldId, jpaused);

  jvm.env->DeleteWeakGlobalRef(i_class);
  jvm.env->DeleteLocalRef(jterminate);
  jvm.env->DeleteLocalRef(jpaused);
}
