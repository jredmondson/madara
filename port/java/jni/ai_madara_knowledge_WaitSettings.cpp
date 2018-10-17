/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "ai_madara_knowledge_WaitSettings.h"
#include "madara/knowledge/WaitSettings.h"
#include "madara_jni.h"

// define useful shorthands
namespace knowledge = madara::knowledge;
typedef knowledge::WaitSettings WaitSettings;

/*
 * Class:     ai_madara_knowledge_WaitSettings
 * Method:    jni_current
 * Signature: ()J
 */
jlong JNICALL Java_ai_madara_knowledge_WaitSettings_jni_1waitSettings__(
    JNIEnv*, jclass)
{
  return (jlong) new WaitSettings();
}

/*
 * Class:     ai_madara_knowledge_WaitSettings
 * Method:    jni_current
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_knowledge_WaitSettings_jni_1waitSettings__J(
    JNIEnv* env, jclass, jlong original)
{
  jlong result(0);
  WaitSettings* source = (WaitSettings*)original;

  if(source)
  {
    result = (jlong) new WaitSettings(*source);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "WaitSettings::copyConstructor: "
        "WaitSettings objects are released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_WaitSettings
 * Method:    jni_setPollFrequency
 * Signature: (JD)V
 */
void JNICALL Java_ai_madara_knowledge_WaitSettings_jni_1setPollFrequency(
    JNIEnv* env, jclass, jlong cptr, jdouble pollFreq)
{
  WaitSettings* current = (WaitSettings*)cptr;

  if(current)
  {
    current->poll_frequency = pollFreq;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "WaitSettings::setPollFrequency: "
        "WaitSettings objects are released already");
  }
}

/*
 * Class:     ai_madara_knowledge_WaitSettings
 * Method:    jni_getPollFrequency
 * Signature: (J)D
 */
jdouble JNICALL Java_ai_madara_knowledge_WaitSettings_jni_1getPollFrequency(
    JNIEnv* env, jclass, jlong cptr)
{
  jdouble result(0);
  WaitSettings* current = (WaitSettings*)cptr;

  if(current)
  {
    result = (jdouble)current->poll_frequency;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "WaitSettings::getPollFrequency: "
        "WaitSettings objects are released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_WaitSettings
 * Method:    jni_setMaxWaitTime
 * Signature: (JD)V
 */
void JNICALL Java_ai_madara_knowledge_WaitSettings_jni_1setMaxWaitTime(
    JNIEnv* env, jclass, jlong cptr, jdouble maxWaitTime)
{
  WaitSettings* current = (WaitSettings*)cptr;

  if(current)
  {
    current->max_wait_time = maxWaitTime;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "WaitSettings::setMaxWaitTime: "
        "WaitSettings objects are released already");
  }
}

/*
 * Class:     ai_madara_knowledge_WaitSettings
 * Method:    jni_getMaxWaitTime
 * Signature: (J)D
 */
jdouble JNICALL Java_ai_madara_knowledge_WaitSettings_jni_1getMaxWaitTime(
    JNIEnv* env, jclass, jlong cptr)
{
  jdouble result(0);
  WaitSettings* current = (WaitSettings*)cptr;

  if(current)
  {
    result = (jdouble)current->max_wait_time;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "WaitSettings::getMaxWaitTime: "
        "WaitSettings objects are released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_WaitSettings
 * Method:    jni_freeWaitSettings
 * Signature: (J)V
 */
void JNICALL Java_ai_madara_knowledge_WaitSettings_jni_1freeWaitSettings(
    JNIEnv*, jclass, jlong cptr)
{
  WaitSettings* current = (WaitSettings*)cptr;

  delete current;
}
