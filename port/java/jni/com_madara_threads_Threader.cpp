#include "com_madara_threads_Threader.h"
#include "madara/threads/Threader.h"

namespace engine = Madara::Knowledge_Engine;
namespace threads = Madara::Threads;

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_Threader
 * Signature: ()J
 */
jlong JNICALL Java_com_madara_threads_Threader_jni_1Threader__
  (JNIEnv *, jobject)
{
  return (jlong) new threads::Threader ();
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_Threader
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_threads_Threader_jni_1Threader__J
  (JNIEnv *, jobject, jlong kb)
{
  return (jlong) new threads::Threader (*(engine::Knowledge_Base *)kb);
}

/*
 * Class:     com_madara_threads_Threader
 * Method:    jni_freeThreader
 * Signature: (J)V
 */
void JNICALL Java_com_madara_threads_Threader_jni_1freeThreader
  (JNIEnv *, jobject, jlong cptr)
{
  delete (threads::Threader *)cptr;
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
  threads::Threader * current = (threads::Threader *)cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars(name, 0);

    current->run (str_name, obj, paused);

    env->ReleaseStringUTFChars(name, str_name);
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
  threads::Threader * current = (threads::Threader *)cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars(name, 0);

    current->run (hertz, str_name, obj, paused);

    env->ReleaseStringUTFChars(name, str_name);
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
  threads::Threader * current = (threads::Threader *)cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars(name, 0);

    current->pause (str_name);

    env->ReleaseStringUTFChars(name, str_name);
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
  threads::Threader * current = (threads::Threader *)cptr;

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
  threads::Threader * current = (threads::Threader *)cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars(name, 0);

    current->wait (str_name);

    env->ReleaseStringUTFChars(name, str_name);
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
  threads::Threader * current = (threads::Threader *)cptr;

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
  threads::Threader * current = (threads::Threader *)cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars(name, 0);

    current->terminate (str_name);

    env->ReleaseStringUTFChars(name, str_name);
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
  threads::Threader * current = (threads::Threader *)cptr;

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
  threads::Threader * current = (threads::Threader *)cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars(name, 0);

    current->resume (str_name);

    env->ReleaseStringUTFChars(name, str_name);
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
  threads::Threader * current = (threads::Threader *)cptr;

  if (current)
  {
    current->resume ();
  }
}
