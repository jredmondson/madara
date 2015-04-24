/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "com.madara.Variables.h"
#include "madara/knowledge_engine/Knowledge_Base.h"

// define useful shorthands
namespace engine = Madara::Knowledge_Engine;
typedef engine::Variables Variables;
typedef engine::Compiled_Expression   Compiled_Expression;
typedef engine::Eval_Settings         Eval_Settings;
typedef Madara::Knowledge_Record      Knowledge_Record;

/*
* Class:     com_madara_Variables
* Method:    jni_evaluate
* Signature: (JJJ)J
*/
MADARA_Export jlong JNICALL Java_com_madara_Variables_jni_1evaluate__JJJ (JNIEnv * env, jobject obj, jlong cptr, jlong expression, jlong evalSettings)
{
  Knowledge_Record * result (0);
  Variables * vars = (Variables *) cptr;
  Compiled_Expression * compiled = (Compiled_Expression*) expression;
  Eval_Settings * settings = (Eval_Settings*) evalSettings;

  if (vars && compiled && settings)
  {
    result = new Knowledge_Record (vars->evaluate (*compiled, *settings));
  }

  return (jlong) result;
}

/*
* Class:     com_madara_Variables
* Method:    jni_compile
* Signature: (JLjava/lang/String;)J
*/
MADARA_Export jlong JNICALL Java_com_madara_Variables_jni_1compile (JNIEnv *env, jobject obj, jlong cptr, jstring expression)
{
  const char *nativeExpression = env->GetStringUTFChars (expression, 0);

  Variables * vars = (Variables *) cptr;
  Compiled_Expression * result (0);

  if (vars)
  {
    result = new Compiled_Expression (vars->compile (nativeExpression));

    env->ReleaseStringUTFChars (expression, nativeExpression);
  }

  return (jlong) result;
}

/*
* Class:     com_madara_Variables
* Method:    jni_get
* Signature: (JLjava/lang/String;)J
*/
MADARA_Export jlong JNICALL Java_com_madara_Variables_jni_1get (JNIEnv * env, jobject obj, jlong cptr, jstring var)
{
  const char * nativeVar = env->GetStringUTFChars (var, 0);

  Variables * vars = (Variables *) cptr;
  Knowledge_Record * result (0);

  if (vars)
  {
    result = new Knowledge_Record (vars->get (nativeVar));
  }

  env->ReleaseStringUTFChars (var, nativeVar);
  return (jlong) result;
}

/*
* Class:     com_madara_Variables
* Method:    jni_set
* Signature: (JLjava/lang/String;J)V
*/
MADARA_Export void JNICALL Java_com_madara_Variables_jni_1set (JNIEnv *env, jobject obj, jlong cptr, jstring var, jlong recordPtr)
{
  const char *nativeVar = env->GetStringUTFChars (var, 0);

  Variables * vars = (Variables *) cptr;
  Knowledge_Record * record = (Knowledge_Record*) recordPtr;

  if (vars)
  {
    vars->set (nativeVar, *record);
  }

  env->ReleaseStringUTFChars (var, nativeVar);
}



