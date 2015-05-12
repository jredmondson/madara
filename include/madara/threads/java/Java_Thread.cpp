#include "Java_Thread.h"
#include "madara/utility/Log_Macros.h"
#include "madara/utility/java/Acquire_VM.h"

namespace threads = Madara::Threads;
namespace engine = Madara::Knowledge_Engine;

threads::Java_Thread::Java_Thread ()
  : obj_ (0), class_ (0),
  run_method_ (0), init_method_ (0), cleanup_method_ (0)
{
}

threads::Java_Thread::~Java_Thread ()
{
  // manage VM attachment
  Madara::Utility::Java::Acquire_VM jvm;

  if (jvm.env != 0)
  {
    MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "threads::Java_Thread::destructor:" \
      " Deleting global references.\n"));

    jvm.env->DeleteGlobalRef (obj_);
    jvm.env->DeleteGlobalRef (class_);
  }
}

void
threads::Java_Thread::operator= (const Java_Thread & rhs)
{
  MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "threads::Java_Thread::assignment:" \
    " Checking for source not being same as dest.\n"));

  if (this != &rhs && obj_ != rhs.obj_)
  {
    JNIEnv * env = ::madara_jni_get_env ();

    // perform the assignment
    if (env)
    {
      MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "threads::Java_Thread::assignment:" \
        " Deleting global references from left hand side.\n"));

      env->DeleteGlobalRef (obj_);
      env->DeleteGlobalRef (class_);

      obj_ = env->NewGlobalRef (rhs.obj_);
      class_ = (jclass) env->NewGlobalRef (rhs.class_);
      cleanup_method_ = rhs.cleanup_method_;
      init_method_ = rhs.init_method_;
      run_method_ = rhs.run_method_;
    }
  }
}
 
void
threads::Java_Thread::run (void)
{
  JNIEnv * env = ::madara_jni_get_env ();
  
  MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "threads::Java_Thread::run:" \
    " Calling user-defined run method.\n"));

  env->CallVoidMethod (obj_, run_method_);
}

void
threads::Java_Thread::cleanup (void)
{
  JNIEnv * env = ::madara_jni_get_env ();
  
  MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "threads::Java_Thread::cleanup:" \
    " Calling user-defined cleanup method.\n"));

  env->CallVoidMethod (obj_, cleanup_method_);
}

void
threads::Java_Thread::init (engine::Knowledge_Base & context)
{
  JNIEnv * env = ::madara_jni_get_env ();
  
  MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "threads::Java_Thread::init:" \
    " Creating Java KnowledgeBase from data plane.\n"));

  jclass kb_class = env->FindClass ("com/madara/KnowledgeBase");
    
  jmethodID fromPointerCall = env->GetStaticMethodID (kb_class,
    "fromPointer", "(JZ)Lcom/madara/KnowledgeBase;");
    
  jboolean manage (false);

  jobject jknowledge = env->CallStaticObjectMethod (kb_class,
    fromPointerCall, (jlong) &context, manage);

  MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "threads::Java_Thread::init:" \
    " Calling user-defined init method.\n"));

  env->CallVoidMethod (obj_, init_method_, jknowledge);
}


threads::Java_Thread *
threads::Java_Thread::create (jobject obj)
{
  Java_Thread * result = new Java_Thread ();
  
  if (!result->check_compliance (obj))
  {
    delete result;
    result = 0;
  }

  return result;
}

bool
threads::Java_Thread::check_compliance (jobject obj)
{  
  JNIEnv * env = ::madara_jni_get_env ();

  bool result (true);
  
  if (env)
  {
    MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "threads::Java_Thread::check_compliance:" \
      " allocating global reference for object.\n"));
  
    obj_ = (jobject) env->NewGlobalRef (obj);

    if (obj_)
    {
      MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "threads::Java_Thread::check_compliance:" \
        " allocating global reference for object's class.\n"));
      class_ = (jclass) env->NewGlobalRef (env->GetObjectClass (obj_));
      if (class_)
      {
        MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "threads::Java_Thread::check_compliance:" \
          " class and object obtained successfully.\n"));

        // stash the method calls to minimize call time
        run_method_ = env->GetMethodID(class_,
          "run", "()V" );
        init_method_ = env->GetMethodID(class_,
          "init", "(Lcom/madara/KnowledgeBase;)V" );
        cleanup_method_ = env->GetMethodID(class_,
          "cleanup", "()V" );

        if (!run_method_)
        {
          MADARA_DEBUG (Madara::Utility::LOG_EMERGENCY, (LM_DEBUG, 
            DLINFO "threads::Java_Thread::check_compliance:" \
            " ERROR: run method must be defined.\n"));
        
          result = false;
        }

        if (!init_method_)
        {
          MADARA_DEBUG (Madara::Utility::LOG_EMERGENCY, (LM_DEBUG, 
            DLINFO "threads::Java_Thread::check_compliance:" \
            " ERROR: init method must be defined.\n"));
        
          result = false;
        }

        if (!cleanup_method_)
        {
          MADARA_DEBUG (Madara::Utility::LOG_EMERGENCY, (LM_DEBUG, 
            DLINFO "threads::Java_Thread::check_compliance:" \
            " ERROR: cleanup method must be defined.\n"));
        
          result = false;
        }
      }
      else
      {
        MADARA_DEBUG (Madara::Utility::LOG_EMERGENCY, (LM_DEBUG, 
          DLINFO "threads::Java_Thread::check_compliance:" \
          " ERROR: class object inaccessible.\n"));
        
        result = false;
      }
    }
    else
    {
      MADARA_DEBUG (Madara::Utility::LOG_EMERGENCY, (LM_DEBUG, 
        DLINFO "threads::Java_Thread::check_compliance:" \
        " ERROR: object is invalid.\n"));

      result = false;
    }
  }
  else
  {
    MADARA_DEBUG (Madara::Utility::LOG_EMERGENCY, (LM_DEBUG, 
      DLINFO "threads::Java_Thread::check_compliance:" \
      " ERROR: unable to acquire JAVA environment.\n"));

    result = false;
  }

  return result;
}

void
threads::Java_Thread::init_control_vars (engine::Knowledge_Base & control)
{
  // Initialize the underlying control variables
  threads::Base_Thread::init_control_vars (control);
  
  // setup the Java variables
  JNIEnv * env = ::madara_jni_get_env ();
  
  MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "threads::Java_Thread::init_control_vars:" \
    " Populating user control plane variables.\n"));

  // obtain fromPointer method for com.madara.containers.Integer
  jclass i_class = env->FindClass ("com/madara/containers/Integer");
  jmethodID fromPointerCall = env->GetStaticMethodID (i_class,
    "fromPointer", "(JZ)Lcom/madara/containers/Integer;");

  // the user thread should not manage C++ memory. We'll do that.
  jboolean manage (false);

  // create java objects for the underlying containers
  jobject jterminate = env->CallStaticObjectMethod (i_class,
    fromPointerCall, (jlong) &terminated, manage);
  jobject jpaused = env->CallStaticObjectMethod (i_class,
    fromPointerCall, (jlong) &paused, manage);

  // set the user-defined threads's containers to these new containers
  jfieldID fieldId = env->GetFieldID(class_,
    "terminated","Lcom/madara/containers/Integer;");
  env->SetObjectField (obj_, fieldId, jterminate);
  
  fieldId = env->GetFieldID(class_,
    "paused","Lcom/madara/containers/Integer;");
  env->SetObjectField (obj_, fieldId, jpaused);
}
