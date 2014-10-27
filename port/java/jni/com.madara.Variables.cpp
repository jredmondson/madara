/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "com.madara.Variables.h"
#include "madara/knowledge_engine/Knowledge_Base.h"

/*
* Class:     com_madara_Variables
* Method:    jni_evaluate
* Signature: (JJJ)J
*/
MADARA_Export jlong JNICALL Java_com_madara_Variables_jni_1evaluate__JJJ (JNIEnv * env, jobject obj, jlong cptr, jlong expression, jlong evalSettings)
{
  Madara::Knowledge_Engine::Variables vars = *(Madara::Knowledge_Engine::Variables*)cptr;
  Madara::Knowledge_Engine::Compiled_Expression compiled_expression = *(Madara::Knowledge_Engine::Compiled_Expression*)expression;
  Madara::Knowledge_Engine::Eval_Settings settings = *(Madara::Knowledge_Engine::Eval_Settings*)evalSettings;
  return (jlong) (new Madara::Knowledge_Record(vars.evaluate(compiled_expression, settings)));
}

/*
* Class:     com_madara_Variables
* Method:    jni_compile
* Signature: (JLjava/lang/String;)J
*/
MADARA_Export jlong JNICALL Java_com_madara_Variables_jni_1compile (JNIEnv *env, jobject obj, jlong cptr, jstring expression)
{
  const char *nativeExpression = env->GetStringUTFChars(expression, 0);

  Madara::Knowledge_Engine::Variables vars = *(Madara::Knowledge_Engine::Variables*)cptr;
  Madara::Knowledge_Engine::Compiled_Expression* ret = new Madara::Knowledge_Engine::Compiled_Expression(vars.compile(nativeExpression));

  env->ReleaseStringUTFChars(expression, nativeExpression);

  return (jlong) ret;
}

/*
* Class:     com_madara_Variables
* Method:    jni_get
* Signature: (JLjava/lang/String;)J
*/
MADARA_Export jlong JNICALL Java_com_madara_Variables_jni_1get (JNIEnv * env, jobject obj, jlong cptr, jstring var)
{
  const char *nativeVar = env->GetStringUTFChars(var, 0);

  Madara::Knowledge_Engine::Variables vars = *(Madara::Knowledge_Engine::Variables*)cptr;
  Madara::Knowledge_Record* ret = new Madara::Knowledge_Record(vars.get(nativeVar));

  env->ReleaseStringUTFChars(var, nativeVar);
  return (jlong) ret;
}

/*
* Class:     com_madara_Variables
* Method:    jni_set
* Signature: (JLjava/lang/String;J)V
*/
MADARA_Export void JNICALL Java_com_madara_Variables_jni_1set (JNIEnv *env, jobject obj, jlong cptr, jstring var, jlong recordPtr)
{
  const char *nativeVar = env->GetStringUTFChars(var, 0);

  Madara::Knowledge_Engine::Variables vars = *(Madara::Knowledge_Engine::Variables*)cptr;
  Madara::Knowledge_Record record = *(Madara::Knowledge_Record*)recordPtr;
  vars.set(nativeVar, Madara::Knowledge_Record(record));

  env->ReleaseStringUTFChars(var, nativeVar);
}



