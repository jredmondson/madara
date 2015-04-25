/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "com.madara.EvalSettings.h"
#include "madara/knowledge_engine/Eval_Settings.h"

#include <stdio.h>
#include <string>

// define useful shorthands
namespace engine = Madara::Knowledge_Engine;
typedef engine::Eval_Settings Eval_Settings;

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_getDefaultEvalSettings
 * Signature: (I)J
 */
jlong JNICALL Java_com_madara_EvalSettings_jni_1getDefaultEvalSettings (JNIEnv *env, jclass clazz, jint which)
{
  return (jlong) (new Eval_Settings ());
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_current
 * Signature: ()J
 */
jlong JNICALL Java_com_madara_EvalSettings_jni_1current__ (JNIEnv *env, jclass clazz)
{
  return (jlong) (new Eval_Settings ());
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_current
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_EvalSettings_jni_1current__J
  (JNIEnv *env, jclass clazz, jlong original)
{
  jlong result (0);
  Eval_Settings * source = (Eval_Settings *) original;

  if (source)
  {
    result = (jlong) new Eval_Settings (*source);
  }

  return result;
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_setDelaySendingModifieds
 * Signature: (JZ)V
 */
void JNICALL Java_com_madara_EvalSettings_jni_1setDelaySendingModifieds
  (JNIEnv * env, jclass clazz, jlong cptr, jboolean delaySendingModifieds)
{
  Eval_Settings * current = (Eval_Settings *) cptr;

  if (current)
  {
    current->delay_sending_modifieds = delaySendingModifieds;
  }
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_getDelaySendingModifieds
 * Signature: (J)Z
 */
jboolean JNICALL Java_com_madara_EvalSettings_jni_1getDelaySendingModifieds
  (JNIEnv * env, jclass clazz, jlong cptr)
{
  jboolean result (0);
  Eval_Settings * current = (Eval_Settings *) cptr;

  if (current)
  {
    result = (jboolean) current->delay_sending_modifieds;
  }

  return result;
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_setPrePrintStatement
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_com_madara_EvalSettings_jni_1setPrePrintStatement
  (JNIEnv * env, jclass clazz, jlong cptr, jstring prePrintStatement)
{
  const char *nativePrePrint = env->GetStringUTFChars (prePrintStatement, 0);
  
  Eval_Settings * current = (Eval_Settings *) cptr;

  if (current)
  {
    current->pre_print_statement = nativePrePrint;
  }

  env->ReleaseStringUTFChars (prePrintStatement, nativePrePrint);
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_getPrePrintStatement
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_com_madara_EvalSettings_jni_1getPrePrintStatement
  (JNIEnv * env, jclass clazz, jlong cptr)
{
  jstring result;
  Eval_Settings * current = (Eval_Settings *) cptr;

  if (current)
  {
    result = env->NewStringUTF (current->pre_print_statement.c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_setPostPrintStatement
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_com_madara_EvalSettings_jni_1setPostPrintStatement
  (JNIEnv *env, jclass clazz, jlong cptr, jstring postPrintStatement)
{
  const char *nativePostPrint = env->GetStringUTFChars (postPrintStatement, 0);
  
  Eval_Settings * current = (Eval_Settings *) cptr;

  if (current)
  {
    current->post_print_statement = std::string (nativePostPrint);
  }

  env->ReleaseStringUTFChars (postPrintStatement, nativePostPrint);
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_getPostPrintStatement
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_com_madara_EvalSettings_jni_1getPostPrintStatement
  (JNIEnv * env, jclass clazz, jlong cptr)
{
  jstring result;
  Eval_Settings * current = (Eval_Settings *) cptr;

  if (current)
  {
    result = env->NewStringUTF (current->post_print_statement.c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_setAlwaysOverwrite
 * Signature: (JZ)V
 */
void JNICALL Java_com_madara_EvalSettings_jni_1setAlwaysOverwrite
  (JNIEnv * env, jclass clazz, jlong cptr, jboolean alwaysOverwrite)
{
  Eval_Settings * current = (Eval_Settings *) cptr;

  if (current)
  {
    current->always_overwrite = alwaysOverwrite;
  }
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_getAlwaysOverwrite
 * Signature: (J)Z
 */
jboolean JNICALL Java_com_madara_EvalSettings_jni_1getAlwaysOverwrite
  (JNIEnv * env, jclass clazz, jlong cptr)
{
  jboolean result (0);
  Eval_Settings * current = (Eval_Settings *) cptr;

  if (current)
  {
    result = (jboolean) current->always_overwrite;
  }

  return result;
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_setTreatGlobalsAsLocals
 * Signature: (JZ)V
 */
void JNICALL Java_com_madara_EvalSettings_jni_1setTreatGlobalsAsLocals
  (JNIEnv *env, jclass clazz, jlong cptr, jboolean treatGlobalsAsLocals)
{
  Eval_Settings * current = (Eval_Settings *) cptr;

  if (current)
  {
    current->treat_globals_as_locals = treatGlobalsAsLocals;
  }
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_getTreatGlobalsAsLocals
 * Signature: (J)Z
 */
jboolean JNICALL Java_com_madara_EvalSettings_jni_1getTreatGlobalsAsLocals
  (JNIEnv * env, jclass clazz, jlong cptr)
{
  jboolean result (0);
  Eval_Settings * current = (Eval_Settings *) cptr;

  if (current)
  {
    result = (jboolean) current->treat_globals_as_locals;
  }

  return result;
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_setDefaultClockIncrement
 * Signature: (JJ)V
 */
void JNICALL Java_com_madara_EvalSettings_jni_1setClockIncrement
  (JNIEnv *env, jclass clazz, jlong cptr, jlong defaultClockIncrement)
{
  Eval_Settings * current = (Eval_Settings *) cptr;

  if (current)
  {
    current->clock_increment = defaultClockIncrement;
  }
}

/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_getDefaultClockIncrement
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_EvalSettings_jni_1getClockIncrement
  (JNIEnv *env, jclass clazz, jlong cptr)
{
  jlong result (0);
  Eval_Settings * current = (Eval_Settings *) cptr;

  if (current)
  {
   result = (jlong) current->clock_increment;
  }

  return result;
}
  
/*
 * Class:   com_madara_EvalSettings
 * Method:  jni_freeEvalSettings
 * Signature: (J)V
 */
void JNICALL Java_com_madara_EvalSettings_jni_1freeEvalSettings
  (JNIEnv * env, jclass cls, jlong cptr)
{
  Eval_Settings * current = (Eval_Settings *) cptr;

  delete current;
}
