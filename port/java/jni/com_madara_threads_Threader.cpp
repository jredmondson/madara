#include "com_madara_threads_Threader.h"
#include "madara/threads/Threader.h"

namespace engine = Madara::Knowledge_Engine;
namespace threads = Madara::Threads;
typedef threads::Threader       Threader;
typedef engine::Knowledge_Base  Knowledge_Base;

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_Threader
 * Signature: ()J
 */
jlong JNICALL Java_com_madara_threads_Threader_jni_1Threader__
  (JNIEnv *, jobject)
{
  return (jlong) new Threader ();
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_Threader
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_threads_Threader_jni_1Threader__J
  (JNIEnv *, jobject, jlong kb)
{
  Threader * result (0);
  Knowledge_Base * knowledge = (Knowledge_Base *) kb;

  if (knowledge)
  {
    result = new Threader (*knowledge);
  }

  return (jlong) result;
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_freeThreader
 * Signature: (J)V
 */
void JNICALL Java_com_madara_threads_Threader_jni_1freeThreader
  (JNIEnv *, jobject, jlong cptr)
{
  delete (Threader *)cptr;
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_run
 * Signature: (JLjava/lang/String;Ljava/lang/Object;Z)V
 */
void JNICALL Java_com_madara_threads_Threader_jni_1run
  (JNIEnv * env, jobject, jlong cptr,
   jstring name, jobject obj, jboolean paused)
{
  Threader * current = (Threader *)cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars (name, 0);

    current->run (str_name, obj, paused);

    env->ReleaseStringUTFChars (name, str_name);
  }
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_runhz
 * Signature: (JDLjava/lang/String;Ljava/lang/Object;Z)V
 */
void JNICALL Java_com_madara_threads_Threader_jni_1runhz
  (JNIEnv * env, jobject, jlong cptr,
   jdouble hertz, jstring name, jobject obj, jboolean paused)
{
  Threader * current = (Threader *)cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars (name, 0);

    current->run (hertz, str_name, obj, paused);

    env->ReleaseStringUTFChars (name, str_name);
  }
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_pauseThread
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_com_madara_threads_Threader_jni_1pauseThread
  (JNIEnv * env, jobject, jlong cptr, jstring name)
{
  Threader * current = (Threader *)cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars (name, 0);

    current->pause (str_name);

    env->ReleaseStringUTFChars (name, str_name);
  }
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_pause
 * Signature: (J)V
 */
void JNICALL Java_com_madara_threads_Threader_jni_1pause
  (JNIEnv *, jobject, jlong cptr)
{
  Threader * current = (Threader *)cptr;

  if (current)
  {
    current->pause ();
  }
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_waitThread
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_com_madara_threads_Threader_jni_1waitThread
  (JNIEnv * env, jobject, jlong cptr, jstring name)
{
  Threader * current = (Threader *)cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars (name, 0);

    current->wait (str_name);

    env->ReleaseStringUTFChars (name, str_name);
  }
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_wait
 * Signature: (J)V
 */
void JNICALL Java_com_madara_threads_Threader_jni_1wait
  (JNIEnv *, jobject, jlong cptr)
{
  Threader * current = (Threader *)cptr;

  if (current)
  {
    current->wait ();
  }
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_terminateThread
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_com_madara_threads_Threader_jni_1terminateThread
  (JNIEnv * env, jobject, jlong cptr, jstring name)
{
  Threader * current = (Threader *)cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars (name, 0);

    current->terminate (str_name);

    env->ReleaseStringUTFChars (name, str_name);
  }
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_terminate
 * Signature: (J)V
 */
void JNICALL Java_com_madara_threads_Threader_jni_1terminate
  (JNIEnv *, jobject, jlong cptr)
{
  Threader * current = (Threader *)cptr;

  if (current)
  {
    current->terminate ();
  }
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_resumeThread
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_com_madara_threads_Threader_jni_1resumeThread
  (JNIEnv * env, jobject, jlong cptr, jstring name)
{
  Threader * current = (Threader *)cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars (name, 0);

    current->resume (str_name);

    env->ReleaseStringUTFChars (name, str_name);
  }
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_resume
 * Signature: (J)V
 */
void JNICALL Java_com_madara_threads_Threader_jni_1resume
  (JNIEnv *, jobject, jlong cptr)
{
  Threader * current = (Threader *)cptr;

  if (current)
  {
    current->resume ();
  }
}
