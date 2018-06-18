/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "ai_madara_knowledge_WaitSettings.h"
#include "madara/knowledge/WaitSettings.h"

// define useful shorthands
namespace knowledge = madara::knowledge;
typedef knowledge::WaitSettings         WaitSettings;

/*
* Class:     ai_madara_knowledge_WaitSettings
* Method:    jni_current
* Signature: ()J
*/
jlong JNICALL
Java_ai_madara_knowledge_WaitSettings_jni_1waitSettings__(JNIEnv *, jclass)
{
  return (jlong) new WaitSettings();
}

/*
* Class:     ai_madara_knowledge_WaitSettings
* Method:    jni_current
* Signature: (J)J
*/
jlong JNICALL
Java_ai_madara_knowledge_WaitSettings_jni_1waitSettings__J (JNIEnv *, jclass, jlong original)
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
* Class:     ai_madara_knowledge_WaitSettings
* Method:    jni_setPollFrequency
* Signature: (JD)V
*/
void JNICALL
Java_ai_madara_knowledge_WaitSettings_jni_1setPollFrequency (JNIEnv *, jclass, jlong cptr, jdouble pollFreq)
{
  WaitSettings * current = (WaitSettings *) cptr;

  if (current)
  {
    current->poll_frequency = pollFreq;
  }
}

/*
* Class:     ai_madara_knowledge_WaitSettings
* Method:    jni_getPollFrequency
* Signature: (J)D
*/
jdouble JNICALL
Java_ai_madara_knowledge_WaitSettings_jni_1getPollFrequency (JNIEnv *, jclass, jlong cptr)
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
* Class:     ai_madara_knowledge_WaitSettings
* Method:    jni_setMaxWaitTime
* Signature: (JD)V
*/
void JNICALL
Java_ai_madara_knowledge_WaitSettings_jni_1setMaxWaitTime (JNIEnv *, jclass, jlong cptr, jdouble maxWaitTime)
{
  WaitSettings * current = (WaitSettings *) cptr;

  if (current)
  {
    current->max_wait_time = maxWaitTime;
  }
}

/*
* Class:     ai_madara_knowledge_WaitSettings
* Method:    jni_getMaxWaitTime
* Signature: (J)D
*/
jdouble JNICALL
Java_ai_madara_knowledge_WaitSettings_jni_1getMaxWaitTime (JNIEnv *, jclass, jlong cptr)
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
* Class:     ai_madara_knowledge_WaitSettings
* Method:    jni_freeWaitSettings
* Signature: (J)V
*/
void JNICALL
Java_ai_madara_knowledge_WaitSettings_jni_1freeWaitSettings (JNIEnv *, jclass, jlong cptr)
{
  WaitSettings * current = (WaitSettings *) cptr;

  delete current;
}
