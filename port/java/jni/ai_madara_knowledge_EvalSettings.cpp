/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "ai_madara_knowledge_EvalSettings.h"
#include "madara/knowledge/EvalSettings.h"
#include "madara_jni.h"

#include <stdio.h>
#include <string>

// define useful shorthands
namespace knowledge = madara::knowledge;
typedef knowledge::EvalSettings EvalSettings;

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_getDefaultEvalSettings
 * Signature: (I)J
 */
jlong JNICALL Java_ai_madara_knowledge_EvalSettings_jni_1getDefaultEvalSettings(
    JNIEnv*, jclass, jint)
{
  return (jlong)(new EvalSettings());
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_current
 * Signature: ()J
 */
jlong JNICALL Java_ai_madara_knowledge_EvalSettings_jni_1evalSettings__(
    JNIEnv*, jclass)
{
  return (jlong)(new EvalSettings());
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_current
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_knowledge_EvalSettings_jni_1evalSettings__J(
    JNIEnv* env, jclass, jlong original)
{
  jlong result(0);
  EvalSettings* source = (EvalSettings*)original;

  if (source)
  {
    result = (jlong) new EvalSettings(*source);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::copyConstructor: "
        "EvalSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_setDelaySendingModifieds
 * Signature: (JZ)V
 */
void JNICALL
Java_ai_madara_knowledge_EvalSettings_jni_1setDelaySendingModifieds(
    JNIEnv* env, jclass, jlong cptr, jboolean delaySendingModifieds)
{
  EvalSettings* current = (EvalSettings*)cptr;

  if (current)
  {
    current->delay_sending_modifieds = delaySendingModifieds;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::setDelaySendingModifieds: "
        "EvalSettings object is released already");
  }
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_getDelaySendingModifieds
 * Signature: (J)Z
 */
jboolean JNICALL
Java_ai_madara_knowledge_EvalSettings_jni_1getDelaySendingModifieds(
    JNIEnv* env, jclass, jlong cptr)
{
  jboolean result(0);
  EvalSettings* current = (EvalSettings*)cptr;

  if (current)
  {
    result = (jboolean)current->delay_sending_modifieds;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::getDelaySendingModifieds: "
        "EvalSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_setPrePrintStatement
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_ai_madara_knowledge_EvalSettings_jni_1setPrePrintStatement(
    JNIEnv* env, jclass, jlong cptr, jstring prePrintStatement)
{
  const char* nativePrePrint = env->GetStringUTFChars(prePrintStatement, 0);

  EvalSettings* current = (EvalSettings*)cptr;

  if (current)
  {
    current->pre_print_statement = nativePrePrint;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::setPrePrintStatement: "
        "EvalSettings object is released already");
  }

  env->ReleaseStringUTFChars(prePrintStatement, nativePrePrint);
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_getPrePrintStatement
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_ai_madara_knowledge_EvalSettings_jni_1getPrePrintStatement(
    JNIEnv* env, jclass, jlong cptr)
{
  jstring result = 0;
  EvalSettings* current = (EvalSettings*)cptr;

  if (current)
  {
    result = env->NewStringUTF(current->pre_print_statement.c_str());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::getPrePrintStatement: "
        "EvalSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_setPostPrintStatement
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_ai_madara_knowledge_EvalSettings_jni_1setPostPrintStatement(
    JNIEnv* env, jclass, jlong cptr, jstring postPrintStatement)
{
  const char* nativePostPrint = env->GetStringUTFChars(postPrintStatement, 0);

  EvalSettings* current = (EvalSettings*)cptr;

  if (current)
  {
    current->post_print_statement = std::string(nativePostPrint);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
    env->ReleaseStringUTFChars(postPrintStatement, nativePostPrint);

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::setPostPrintStatement: "
        "EvalSettings object is released already");
  }

  env->ReleaseStringUTFChars(postPrintStatement, nativePostPrint);
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_getPostPrintStatement
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL
Java_ai_madara_knowledge_EvalSettings_jni_1getPostPrintStatement(
    JNIEnv* env, jclass, jlong cptr)
{
  jstring result = 0;
  EvalSettings* current = (EvalSettings*)cptr;

  if (current)
  {
    result = env->NewStringUTF(current->post_print_statement.c_str());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::getPostPrintStatement: "
        "EvalSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_setAlwaysOverwrite
 * Signature: (JZ)V
 */
void JNICALL Java_ai_madara_knowledge_EvalSettings_jni_1setAlwaysOverwrite(
    JNIEnv* env, jclass, jlong cptr, jboolean alwaysOverwrite)
{
  EvalSettings* current = (EvalSettings*)cptr;

  if (current)
  {
    current->always_overwrite = alwaysOverwrite;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::setAlwaysOverwrite: "
        "EvalSettings object is released already");
  }
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_getAlwaysOverwrite
 * Signature: (J)Z
 */
jboolean JNICALL Java_ai_madara_knowledge_EvalSettings_jni_1getAlwaysOverwrite(
    JNIEnv* env, jclass, jlong cptr)
{
  jboolean result(0);
  EvalSettings* current = (EvalSettings*)cptr;

  if (current)
  {
    result = (jboolean)current->always_overwrite;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::getAlwaysOverwrite: "
        "EvalSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_setTreatGlobalsAsLocals
 * Signature: (JZ)V
 */
void JNICALL Java_ai_madara_knowledge_EvalSettings_jni_1setTreatGlobalsAsLocals(
    JNIEnv* env, jclass, jlong cptr, jboolean treatGlobalsAsLocals)
{
  EvalSettings* current = (EvalSettings*)cptr;

  if (current)
  {
    current->treat_globals_as_locals = treatGlobalsAsLocals;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::setTreatGlobalsAsLocals: "
        "EvalSettings object is released already");
  }
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_getTreatGlobalsAsLocals
 * Signature: (J)Z
 */
jboolean JNICALL
Java_ai_madara_knowledge_EvalSettings_jni_1getTreatGlobalsAsLocals(
    JNIEnv* env, jclass, jlong cptr)
{
  jboolean result(0);
  EvalSettings* current = (EvalSettings*)cptr;

  if (current)
  {
    result = (jboolean)current->treat_globals_as_locals;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::getTreatGlobalsAsLocals: "
        "EvalSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_setDefaultClockIncrement
 * Signature: (JJ)V
 */
void JNICALL Java_ai_madara_knowledge_EvalSettings_jni_1setClockIncrement(
    JNIEnv* env, jclass, jlong cptr, jlong defaultClockIncrement)
{
  EvalSettings* current = (EvalSettings*)cptr;

  if (current)
  {
    current->clock_increment = defaultClockIncrement;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::setClockIncrement: "
        "EvalSettings object is released already");
  }
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_getDefaultClockIncrement
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_knowledge_EvalSettings_jni_1getClockIncrement(
    JNIEnv* env, jclass, jlong cptr)
{
  jlong result(0);
  EvalSettings* current = (EvalSettings*)cptr;

  if (current)
  {
    result = (jlong)current->clock_increment;
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "EvalSettings::getClockIncrement: "
        "EvalSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_knowledge_EvalSettings
 * Method:  jni_freeEvalSettings
 * Signature: (J)V
 */
void JNICALL Java_ai_madara_knowledge_EvalSettings_jni_1freeEvalSettings(
    JNIEnv*, jclass, jlong cptr)
{
  EvalSettings* current = (EvalSettings*)cptr;

  delete current;
}
