
#include "ai_madara_knowledge_UpdateSettings.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "madara_jni.h"

#include <stdio.h>
#include <string>

// define useful shorthands
namespace knowledge = madara::knowledge;
typedef knowledge::KnowledgeUpdateSettings UpdateSettings;

/*
 * Class:   ai_madara_knowledge_UpdateSettings
 * Method:  jni_current
 * Signature: ()J
 */
jlong JNICALL Java_ai_madara_knowledge_UpdateSettings_jni_1updateSettings__(
    JNIEnv*, jclass)
{
  return (jlong)(new UpdateSettings());
}

/*
 * Class:   ai_madara_knowledge_UpdateSettings
 * Method:  jni_current
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_knowledge_UpdateSettings_jni_1updateSettings__J(
    JNIEnv* env, jclass, jlong original)
{
  jlong result(0);
  UpdateSettings* source = (UpdateSettings*)original;

  if (source)
  {
    result = (jlong) new UpdateSettings(*source);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "UpdateSettings::copyConstructor: "
        "UpdateSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_knowledge_UpdateSettings
 * Method:  jni_setAlwaysOverwrite
 * Signature: (JZ)V
 */
void JNICALL Java_ai_madara_knowledge_UpdateSettings_jni_1setAlwaysOverwrite(
    JNIEnv* env, jclass, jlong cptr, jboolean alwaysOverwrite)
{
  UpdateSettings* current = (UpdateSettings*)cptr;

  if (current)
  {
    current->always_overwrite = alwaysOverwrite;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "UpdateSettings::setAlwaysOverwrite: "
        "UpdateSettings object is released already");
  }
}

/*
 * Class:   ai_madara_knowledge_UpdateSettings
 * Method:  jni_getAlwaysOverwrite
 * Signature: (J)Z
 */
jboolean JNICALL
Java_ai_madara_knowledge_UpdateSettings_jni_1getAlwaysOverwrite(
    JNIEnv* env, jclass, jlong cptr)
{
  jboolean result(0);
  UpdateSettings* current = (UpdateSettings*)cptr;

  if (current)
  {
    result = (jboolean)current->always_overwrite;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "UpdateSettings::getAlwaysOverwrite: "
        "UpdateSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_knowledge_UpdateSettings
 * Method:  jni_setTreatGlobalsAsLocals
 * Signature: (JZ)V
 */
void JNICALL
Java_ai_madara_knowledge_UpdateSettings_jni_1setTreatGlobalsAsLocals(
    JNIEnv* env, jclass, jlong cptr, jboolean treatGlobalsAsLocals)
{
  UpdateSettings* current = (UpdateSettings*)cptr;

  if (current)
  {
    current->treat_globals_as_locals = treatGlobalsAsLocals;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "UpdateSettings::setTreatGlobalsAsLocals: "
        "UpdateSettings object is released already");
  }
}

/*
 * Class:   ai_madara_knowledge_UpdateSettings
 * Method:  jni_getTreatGlobalsAsLocals
 * Signature: (J)Z
 */
jboolean JNICALL
Java_ai_madara_knowledge_UpdateSettings_jni_1getTreatGlobalsAsLocals(
    JNIEnv* env, jclass, jlong cptr)
{
  jboolean result(0);
  UpdateSettings* current = (UpdateSettings*)cptr;

  if (current)
  {
    result = (jboolean)current->treat_globals_as_locals;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "UpdateSettings::getTreatGlobalsAsLocals: "
        "UpdateSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_knowledge_UpdateSettings
 * Method:  jni_setDefaultClockIncrement
 * Signature: (JJ)V
 */
void JNICALL Java_ai_madara_knowledge_UpdateSettings_jni_1setClockIncrement(
    JNIEnv* env, jclass, jlong cptr, jlong defaultClockIncrement)
{
  UpdateSettings* current = (UpdateSettings*)cptr;

  if (current)
  {
    current->clock_increment = defaultClockIncrement;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "UpdateSettings::setClockIncrement: "
        "UpdateSettings object is released already");
  }
}

/*
 * Class:   ai_madara_knowledge_UpdateSettings
 * Method:  jni_getDefaultClockIncrement
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_knowledge_UpdateSettings_jni_1getClockIncrement(
    JNIEnv* env, jclass, jlong cptr)
{
  jlong result(0);
  UpdateSettings* current = (UpdateSettings*)cptr;

  if (current)
  {
    result = (jlong)current->clock_increment;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "UpdateSettings::getClockIncrement: "
        "UpdateSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_knowledge_UpdateSettings
 * Method:  jni_freeUpdateSettings
 * Signature: (J)V
 */
void JNICALL Java_ai_madara_knowledge_UpdateSettings_jni_1freeUpdateSettings(
    JNIEnv*, jclass, jlong cptr)
{
  UpdateSettings* current = (UpdateSettings*)cptr;

  delete current;
}
