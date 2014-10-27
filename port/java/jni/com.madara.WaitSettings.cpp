/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "com.madara.WaitSettings.h"
#include "madara/knowledge_engine/Wait_Settings.h"

/*
* Class:     com_madara_WaitSettings
* Method:    jni_waitSettings
* Signature: ()J
*/
MADARA_Export jlong JNICALL Java_com_madara_WaitSettings_jni_1waitSettings__(JNIEnv * env, jclass clazz)
{
  return (jlong) new Madara::Knowledge_Engine::Wait_Settings();
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_waitSettings
* Signature: (J)J
*/
MADARA_Export jlong JNICALL Java_com_madara_WaitSettings_jni_1waitSettings__J (JNIEnv * env, jclass clazz, jlong original)
{
  return (jlong) new Madara::Knowledge_Engine::Wait_Settings(*(Madara::Knowledge_Engine::Wait_Settings*)original);
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_setPollFrequency
* Signature: (JD)V
*/
MADARA_Export void JNICALL Java_com_madara_WaitSettings_jni_1setPollFrequency (JNIEnv *env, jclass clazz, jlong cptr, jdouble pollFreq)
{
  Madara::Knowledge_Engine::Wait_Settings* waitSettings = (Madara::Knowledge_Engine::Wait_Settings*) cptr;
  waitSettings->poll_frequency = pollFreq;
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_getPollFrequency
* Signature: (J)D
*/
MADARA_Export jdouble JNICALL Java_com_madara_WaitSettings_jni_1getPollFrequency (JNIEnv *env, jclass clazz, jlong cptr)
{
  Madara::Knowledge_Engine::Wait_Settings* waitSettings = (Madara::Knowledge_Engine::Wait_Settings*) cptr;
  return (jdouble) waitSettings->poll_frequency;
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_setMaxWaitTime
* Signature: (JD)V
*/
MADARA_Export void JNICALL Java_com_madara_WaitSettings_jni_1setMaxWaitTime (JNIEnv *env, jclass clazz, jlong cptr, jdouble maxWaitTime)
{
  Madara::Knowledge_Engine::Wait_Settings* waitSettings = (Madara::Knowledge_Engine::Wait_Settings*) cptr;
  waitSettings->max_wait_time = maxWaitTime;
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_getMaxWaitTime
* Signature: (J)D
*/
MADARA_Export jdouble JNICALL Java_com_madara_WaitSettings_jni_1getMaxWaitTime (JNIEnv *env, jclass clazz, jlong cptr)
{
  Madara::Knowledge_Engine::Wait_Settings* waitSettings = (Madara::Knowledge_Engine::Wait_Settings*) cptr;
  return (jdouble) waitSettings->max_wait_time;
}

/*
* Class:     com_madara_WaitSettings
* Method:    jni_freeWaitSettings
* Signature: (J)V
*/
MADARA_Export void JNICALL Java_com_madara_WaitSettings_jni_1freeWaitSettings (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Knowledge_Engine::Wait_Settings* waitSettings = (Madara::Knowledge_Engine::Wait_Settings*) cptr;
  if (waitSettings)
    delete waitSettings;
}