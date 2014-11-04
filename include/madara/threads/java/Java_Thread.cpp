#include "Java_Thread.h"
#include "madara/utility/Log_Macros.h"
#include "madara/utility/java/Acquire_VM.h"

namespace threads = Madara::Threads;

Madara::Threads::Java_Thread::Java_Thread (
  jobject obj)
{
  Madara::Utility::Java::Acquire_VM jvm;

  if (jvm.env)
  {
    MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Madara::Threads::Java_Thread::constructor:" \
      " allocating global reference for object.\n"));
  
    obj_ = (jobject) jvm.env->NewGlobalRef (obj);

    if (obj_)
    {
      MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Madara::Threads::Java_Thread::constructor:" \
        " allocating global reference for object's class.\n"));
      class_ = (jclass) jvm.env->NewGlobalRef (jvm.env->GetObjectClass (obj_));
      if (class_)
      {
        MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "Madara::Threads::Java_Thread::constructor:" \
          " class and object obtained successfully.\n"));

        // stash the run_method_ call because we will be running it frequently
        run_method_ = jvm.env->GetMethodID(class_, "run", "()V" );
      }
      else
      {
        MADARA_DEBUG (Madara::Utility::LOG_EMERGENCY, (LM_DEBUG, 
          DLINFO "Madara::Threads::Java_Thread::constructor:" \
          " ERROR: class object inaccessible.\n"));
      }
    }
    else
    {
      MADARA_DEBUG (Madara::Utility::LOG_EMERGENCY, (LM_DEBUG, 
        DLINFO "Madara::Threads::Java_Thread::constructor:" \
        " ERROR: object is invalid.\n"));
    }
  }
  else
  {
    MADARA_DEBUG (Madara::Utility::LOG_EMERGENCY, (LM_DEBUG, 
      DLINFO "Madara::Threads::Java_Thread::constructor:" \
      " ERROR: unable to acquire JAVA environment.\n"));
  }
}

Madara::Threads::Java_Thread::~Java_Thread ()
{
  Madara::Utility::Java::Acquire_VM jvm;
  if (jvm.env)
  {
    MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Madara::Threads::Java_Thread::destructor:" \
      " Deleting global references.\n"));

    jvm.env->DeleteGlobalRef (obj_);
    jvm.env->DeleteGlobalRef (class_);
  }
}

void
Madara::Threads::Java_Thread::operator= (const Java_Thread & rhs)
{
  MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "Madara::Threads::Java_Thread::assignment:" \
    " Checking for source not being same as dest.\n"));

  if (this != &rhs && obj_ != rhs.obj_)
  {
    Madara::Utility::Java::Acquire_VM jvm;
    threads::Java_Thread * dest = dynamic_cast <threads::Java_Thread *> (this);
    const threads::Java_Thread * source =
      dynamic_cast <const threads::Java_Thread *> (&rhs);
    
    MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Madara::Threads::Java_Thread::assignment:" \
      " Copying source to dest.\n"));

    *dest = *source;

    if (jvm.env)
    {
      MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Madara::Threads::Java_Thread::assignment:" \
        " Deleting global references.\n"));

      jvm.env->DeleteGlobalRef (obj_);
      jvm.env->DeleteGlobalRef (class_);

      obj_ = jvm.env->NewGlobalRef (rhs.obj_);
      class_ = (jclass) jvm.env->NewGlobalRef (rhs.class_);
    }
  }
}
 
void
Madara::Threads::Java_Thread::run (void)
{
  Madara::Utility::Java::Acquire_VM jvm;
  
  MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "Madara::Threads::Java_Thread::run:" \
    " Obtaining user-defined run method.\n"));

  if (run_method_)
  {
    MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Madara::Threads::Java_Thread::run:" \
      " Calling user-defined run method.\n"));
    jvm.env->CallVoidMethod (obj_, run_method_);
  }
  else
  {
    MADARA_DEBUG (Madara::Utility::LOG_EMERGENCY, (LM_DEBUG, 
      DLINFO "Madara::Threads::Java_Thread::run:" \
      " ERROR: Unable to find user-defined run method.\n"));
  }
}

void
Madara::Threads::Java_Thread::cleanup (void)
{
  Madara::Utility::Java::Acquire_VM jvm;
  
  MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "Madara::Threads::Java_Thread::cleanup:" \
    " Obtaining user-defined cleanup method.\n"));

  jmethodID call = jvm.env->GetMethodID(class_, "cleanup", "()V" );

  if (call)
  {
    MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Madara::Threads::Java_Thread::cleanup:" \
      " Calling user-defined cleanup method.\n"));

    jvm.env->CallVoidMethod (obj_, call);
  }
  else
  {
    MADARA_DEBUG (Madara::Utility::LOG_EMERGENCY, (LM_DEBUG, 
      DLINFO "Madara::Threads::Java_Thread::cleanup:" \
      " ERROR: Unable to find user-defined cleanup method.\n"));
  }
}

void
Madara::Threads::Java_Thread::init (Knowledge_Engine::Knowledge_Base & context)
{
  Madara::Utility::Java::Acquire_VM jvm;
  
  MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "Madara::Threads::Java_Thread::init:" \
    " Obtaining user-defined init method.\n"));

  jmethodID call = jvm.env->GetMethodID(class_, "init", "(Lcom.madara.KnowledgeBase;)V" );

  if (call)
  {
    jclass kb_class = jvm.env->FindClass ("com/madara/KnowledgeBase");
    
    jmethodID fromPointerCall = jvm.env->GetStaticMethodID (kb_class,
      "fromPointer", "(JZ)Lcom/madara/KnowledgeBase;");
    
    jboolean manage (false);

    jobject jknowledge = jvm.env->CallStaticObjectMethod (kb_class,
      fromPointerCall, (jlong) &context, manage);

    MADARA_DEBUG (Madara::Utility::LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Madara::Threads::Java_Thread::init:" \
      " Calling user-defined init method.\n"));

    jvm.env->CallVoidMethod (obj_, call, jknowledge);
  }
  else
  {
    MADARA_DEBUG (Madara::Utility::LOG_EMERGENCY, (LM_DEBUG, 
      DLINFO "Madara::Threads::Java_Thread::init:" \
      " ERROR: Unable to find user-defined init method.\n"));
  }
}

