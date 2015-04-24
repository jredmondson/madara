/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "com.madara.WaitSettings.h"
#include "madara/knowledge_engine/Wait_Settings.h"

// define useful shorthands
namespace engine = Madara::Knowledge_Engine;
typedef engine::Wait_Settings         Wait_Settings;

/*
* Class:     com_madara_WaitSettings
* Method:    jni_current
* Signature: ()J
*/
MADARA_Export jlong JNICALL Java_com_madara_WaitSettings_jni_1current__(JNIEnv * env, jclass clazz)
{
  return (jlong) new Wait_Settings();
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_current
* Signature: (J)J
*/
MADARA_Export jlong JNICALL Java_com_madara_WaitSettings_jni_1current__J (JNIEnv * env, jclass clazz, jlong original)
{
  jlong result (0);
  Wait_Settings * source = (Wait_Settings *) original;

  if (source)
  {
    result = (jlong) new Wait_Settings(*source);
  }

  return result;
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_setPollFrequency
* Signature: (JD)V
*/
MADARA_Export void JNICALL Java_com_madara_WaitSettings_jni_1setPollFrequency (JNIEnv *env, jclass clazz, jlong cptr, jdouble pollFreq)
{
  Wait_Settings * current = (Wait_Settings *) cptr;

  if (current)
  {
    current->poll_frequency = pollFreq;
  }
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_getPollFrequency
* Signature: (J)D
*/
MADARA_Export jdouble JNICALL Java_com_madara_WaitSettings_jni_1getPollFrequency (JNIEnv *env, jclass clazz, jlong cptr)
{
  jdouble result (0);
  Wait_Settings * current = (Wait_Settings *) cptr;

  if (current)
  {
    result = (jdouble) current->poll_frequency;
  }

  return result;
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_setMaxWaitTime
* Signature: (JD)V
*/
MADARA_Export void JNICALL Java_com_madara_WaitSettings_jni_1setMaxWaitTime (JNIEnv *env, jclass clazz, jlong cptr, jdouble maxWaitTime)
{
  Wait_Settings * current = (Wait_Settings *) cptr;

  if (current)
  {
    current->max_wait_time = maxWaitTime;
  }
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_getMaxWaitTime
* Signature: (J)D
*/
MADARA_Export jdouble JNICALL Java_com_madara_WaitSettings_jni_1getMaxWaitTime (JNIEnv *env, jclass clazz, jlong cptr)
{
  jdouble result (0);
  Wait_Settings * current = (Wait_Settings *) cptr;

  if (current)
  {
    result = (jdouble) current->max_wait_time;
  }

  return result;
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_freeWaitSettings
* Signature: (J)V
*/
MADARA_Export void JNICALL Java_com_madara_WaitSettings_jni_1freeWaitSettings (JNIEnv * env, jclass cls, jlong cptr)
{
  Wait_Settings * current = (Wait_Settings *) cptr;

  delete current;
}
