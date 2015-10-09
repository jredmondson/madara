
#include "com_madara_UpdateSettings.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"

#include <stdio.h>
#include <string>

// define useful shorthands
namespace knowledge = madara::knowledge;
typedef knowledge::KnowledgeUpdateSettings UpdateSettings;

/*
* Class:   com_madara_UpdateSettings
* Method:  jni_current
* Signature: ()J
*/
jlong Java_com_madara_UpdateSettings_jni_1updateSettings__ (JNIEnv *env, jclass clazz)
{
  return (jlong)(new UpdateSettings ());
}

/*
* Class:   com_madara_UpdateSettings
* Method:  jni_current
* Signature: (J)J
*/
jlong Java_com_madara_UpdateSettings_jni_1updateSettings__J
(JNIEnv *env, jclass clazz, jlong original)
{
  jlong result (0);
  UpdateSettings * source = (UpdateSettings *)original;

  if (source)
  {
    result = (jlong) new UpdateSettings (*source);
  }

  return result;
}


/*
* Class:   com_madara_UpdateSettings
* Method:  jni_setAlwaysOverwrite
* Signature: (JZ)V
*/
void Java_com_madara_UpdateSettings_jni_1setAlwaysOverwrite
(JNIEnv * env, jclass clazz, jlong cptr, jboolean alwaysOverwrite)
{
  UpdateSettings * current = (UpdateSettings *)cptr;

  if (current)
  {
    current->always_overwrite = alwaysOverwrite;
  }
}

/*
* Class:   com_madara_UpdateSettings
* Method:  jni_getAlwaysOverwrite
* Signature: (J)Z
*/
jboolean Java_com_madara_UpdateSettings_jni_1getAlwaysOverwrite
(JNIEnv * env, jclass clazz, jlong cptr)
{
  jboolean result (0);
  UpdateSettings * current = (UpdateSettings *)cptr;

  if (current)
  {
    result = (jboolean)current->always_overwrite;
  }

  return result;
}

/*
* Class:   com_madara_UpdateSettings
* Method:  jni_setTreatGlobalsAsLocals
* Signature: (JZ)V
*/
void Java_com_madara_UpdateSettings_jni_1setTreatGlobalsAsLocals
(JNIEnv *env, jclass clazz, jlong cptr, jboolean treatGlobalsAsLocals)
{
  UpdateSettings * current = (UpdateSettings *)cptr;

  if (current)
  {
    current->treat_globals_as_locals = treatGlobalsAsLocals;
  }
}

/*
* Class:   com_madara_UpdateSettings
* Method:  jni_getTreatGlobalsAsLocals
* Signature: (J)Z
*/
jboolean Java_com_madara_UpdateSettings_jni_1getTreatGlobalsAsLocals
(JNIEnv * env, jclass clazz, jlong cptr)
{
  jboolean result (0);
  UpdateSettings * current = (UpdateSettings *)cptr;

  if (current)
  {
    result = (jboolean)current->treat_globals_as_locals;
  }

  return result;
}

/*
* Class:   com_madara_UpdateSettings
* Method:  jni_setDefaultClockIncrement
* Signature: (JJ)V
*/
void Java_com_madara_UpdateSettings_jni_1setClockIncrement
(JNIEnv *env, jclass clazz, jlong cptr, jlong defaultClockIncrement)
{
  UpdateSettings * current = (UpdateSettings *)cptr;

  if (current)
  {
    current->clock_increment = defaultClockIncrement;
  }
}

/*
* Class:   com_madara_UpdateSettings
* Method:  jni_getDefaultClockIncrement
* Signature: (J)J
*/
jlong Java_com_madara_UpdateSettings_jni_1getClockIncrement
(JNIEnv *env, jclass clazz, jlong cptr)
{
  jlong result (0);
  UpdateSettings * current = (UpdateSettings *)cptr;

  if (current)
  {
    result = (jlong)current->clock_increment;
  }

  return result;
}

/*
* Class:   com_madara_UpdateSettings
* Method:  jni_freeUpdateSettings
* Signature: (J)V
*/
void Java_com_madara_UpdateSettings_jni_1freeUpdateSettings
(JNIEnv * env, jclass cls, jlong cptr)
{
  UpdateSettings * current = (UpdateSettings *)cptr;

  delete current;
}
