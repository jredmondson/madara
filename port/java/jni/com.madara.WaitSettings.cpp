/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "com.madara.WaitSettings.h"
#include "madara/knowledge/WaitSettings.h"

// define useful shorthands
namespace knowledge = madara::knowledge;
typedef knowledge::WaitSettings         WaitSettings;

/*
* Class:     com_madara_WaitSettings
* Method:    jni_current
* Signature: ()J
*/
jlong JNICALL
Java_com_madara_WaitSettings_jni_1waitSettings__(JNIEnv * env, jclass clazz)
{
  return (jlong) new WaitSettings();
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_current
* Signature: (J)J
*/
jlong JNICALL
Java_com_madara_WaitSettings_jni_1waitSettings__J (JNIEnv * env, jclass clazz, jlong original)
{
  jlong result (0);
  WaitSettings * source = (WaitSettings *) original;

  if (source)
  {
    result = (jlong) new WaitSettings(*source);
  }

  return result;
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_setPollFrequency
* Signature: (JD)V
*/
void JNICALL
Java_com_madara_WaitSettings_jni_1setPollFrequency (JNIEnv *env, jclass clazz, jlong cptr, jdouble pollFreq)
{
  WaitSettings * current = (WaitSettings *) cptr;

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
jdouble JNICALL
Java_com_madara_WaitSettings_jni_1getPollFrequency (JNIEnv *env, jclass clazz, jlong cptr)
{
  jdouble result (0);
  WaitSettings * current = (WaitSettings *) cptr;

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
void JNICALL
Java_com_madara_WaitSettings_jni_1setMaxWaitTime (JNIEnv *env, jclass clazz, jlong cptr, jdouble maxWaitTime)
{
  WaitSettings * current = (WaitSettings *) cptr;

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
jdouble JNICALL
Java_com_madara_WaitSettings_jni_1getMaxWaitTime (JNIEnv *env, jclass clazz, jlong cptr)
{
  jdouble result (0);
  WaitSettings * current = (WaitSettings *) cptr;

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
void JNICALL
Java_com_madara_WaitSettings_jni_1freeWaitSettings (JNIEnv * env, jclass cls, jlong cptr)
{
  WaitSettings * current = (WaitSettings *) cptr;

  delete current;
}
