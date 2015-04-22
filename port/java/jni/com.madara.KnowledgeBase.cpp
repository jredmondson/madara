/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "com.madara.KnowledgeBase.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/transport/QoS_Transport_Settings.h"

#include <string>
#include <string.h>
#include <map>

namespace engine = Madara::Knowledge_Engine;
namespace transport = Madara::Transport;
typedef Madara::Knowledge_Record::Integer Integer;

static jclass knowledgeBaseClass = 0;
static jmethodID callbackMethod = 0;
static std::map<std::string, jlong> definedFunctionsMap;

static JavaVM *jvm;


//===================================================================================
//KnowledgeBase
//===================================================================================
jlong JNICALL Java_com_madara_KnowledgeBase_jni_1KnowledgeBase__
  (JNIEnv * env, jobject obj)
{
  return (jlong) (new engine::Knowledge_Base ());
}

jlong JNICALL Java_com_madara_KnowledgeBase_jni_1KnowledgeBase__Ljava_lang_String_2ILjava_lang_String_2
  (JNIEnv * env, jobject obj, jstring host, jint transport, jstring domain)
{

  const char * nativeHost = env->GetStringUTFChars (host, 0);
  const char * nativeDomain = env->GetStringUTFChars (domain, 0);

  engine::Knowledge_Base * knowledge =
   new engine::Knowledge_Base (
     std::string (nativeHost), transport, std::string (nativeDomain));

  env->ReleaseStringUTFChars (host, nativeHost);
  env->ReleaseStringUTFChars (domain, nativeDomain);

  return (jlong) knowledge;
}

jlong JNICALL Java_com_madara_KnowledgeBase_jni_1KnowledgeBase__Ljava_lang_String_2J
  (JNIEnv *env, jobject obj, jstring host, jlong csettings)
{
  const char * nativeHost = env->GetStringUTFChars (host, 0);
  transport::Settings * settings = (Madara::Transport::Settings *) csettings;
  engine::Knowledge_Base * knowledge (0);

  if (settings)
  {
    knowledge = new engine::Knowledge_Base (
      std::string (nativeHost), *settings);
  }

  env->ReleaseStringUTFChars (host, nativeHost);

  return (jlong) knowledge;
}

jlong JNICALL Java_com_madara_KnowledgeBase_jni_1KnowledgeBase__J
  (JNIEnv * env, jobject obj, jlong original)
{
  return (jlong) new engine::Knowledge_Base (
  * (engine::Knowledge_Base *) original);
}


/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_evaluate
* Signature: (JLjava/lang/String;J)J
*/
jlong JNICALL Java_com_madara_KnowledgeBase_jni_1evaluate__JLjava_lang_String_2J
  (JNIEnv *env, jobject obj, jlong cptr, jstring expression, jlong evalSettings)
{
  const char *nativeExpression = env->GetStringUTFChars (expression, 0);
  engine::Eval_Settings * settings = (engine::Eval_Settings *)evalSettings;
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  Madara::Knowledge_Record * result (0);

  if (knowledge && settings)
  {
    result = knowledge->evaluate (
      std::string (nativeExpression), *settings).clone ();
  }

  env->ReleaseStringUTFChars (expression, nativeExpression);

  return (jlong) result;
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_evaluate
* Signature: (JJJ)J
*/
jlong JNICALL Java_com_madara_KnowledgeBase_jni_1evaluate__JJJ
  (JNIEnv *env, jobject obj, jlong cptr, jlong expression, jlong evalSettings)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  engine::Compiled_Expression * compiled =
    (engine::Compiled_Expression *) expression;
  engine::Eval_Settings * settings = (engine::Eval_Settings*) evalSettings;

  jlong result (0);

  if (knowledge && compiled && settings)
  {
    result = (jlong) knowledge->evaluate (*compiled, *settings).clone ();
  }

  return result;
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_evaluateNoReturn
* Signature: (JLjava/lang/String;J)V
*/
void JNICALL Java_com_madara_KnowledgeBase_jni_1evaluateNoReturn__JLjava_lang_String_2J
  (JNIEnv * env, jobject obj, jlong cptr, jstring expression, jlong evalSettings)
{
  const char *nativeExpression = env->GetStringUTFChars (expression, 0);
  
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  engine::Eval_Settings * settings = (engine::Eval_Settings*) evalSettings;

  if (knowledge && settings)
  {
    knowledge->evaluate (std::string (nativeExpression), *settings);
  }

  env->ReleaseStringUTFChars (expression, nativeExpression);
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_evaluateNoReturn
* Signature: (JJJ)V
*/
void JNICALL Java_com_madara_KnowledgeBase_jni_1evaluateNoReturn__JJJ
  (JNIEnv *env, jobject obj, jlong cptr, jlong expression, jlong evalSettings)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  engine::Compiled_Expression * compiled =
    (engine::Compiled_Expression *) expression;
  engine::Eval_Settings * settings = (engine::Eval_Settings*) evalSettings;

  if (knowledge && compiled && settings)
  {
    knowledge->evaluate (*compiled, *settings);
  }
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_compile
* Signature: (JLjava/lang/String;)J
*/
jlong JNICALL Java_com_madara_KnowledgeBase_jni_1compile
  (JNIEnv *env, jobject obj, jlong cptr, jstring expression)
{
  const char *nativeExpression = env->GetStringUTFChars (expression, 0);
  
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;

  engine::Compiled_Expression* result (0);
  
  if (knowledge)
  {
    result = new engine::Compiled_Expression (
      knowledge->compile (std::string (nativeExpression)));
  }

  env->ReleaseStringUTFChars (expression, nativeExpression);

  return (jlong) result;
}

Madara::Knowledge_Record default_madara_return_function
  (const char * name, engine::Function_Arguments & args,
   engine::Variables & variables)
{
  JNIEnv *env;

#ifndef _USING_OPENJDK_
  jvm->AttachCurrentThread ( (void **)&env, NULL);
#else
  jvm->AttachCurrentThread (&env, NULL);
#endif

  jlong ret;
  jlong * argsArrayNative = new jlong [args.size ()];
  for (unsigned int x = 0; x < args.size (); x++)
  {
    argsArrayNative[x] = (jlong)& (args[x]);
  }

  jlongArray argsArray = env->NewLongArray ( (jsize)args.size ());
  env->SetLongArrayRegion (argsArray, 0, (jsize)args.size (), argsArrayNative);

  ret = env->CallStaticLongMethod (knowledgeBaseClass, callbackMethod,
  env->NewStringUTF (name), definedFunctionsMap[std::string (name)],
  argsArray, &variables);

  delete [] argsArrayNative;

  if (ret == 0)
  {
    return Madara::Knowledge_Record::Integer (0);
  }
  else
  {
    Madara::Knowledge_Record returnValue (* (Madara::Knowledge_Record*)ret);
    delete (Madara::Knowledge_Record*)ret;
    return returnValue;
  }
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_defineFunction
* Signature: (JLjava/lang/String;)V
*/
void JNICALL Java_com_madara_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2
  (JNIEnv *env, jobject obj, jlong cptr, jstring name)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;

  if (knowledge)
  {
    if (!knowledgeBaseClass)
    {
      knowledgeBaseClass = (jclass)env->NewGlobalRef (env->GetObjectClass (obj));
      callbackMethod = env->GetStaticMethodID (
        knowledgeBaseClass, "callBack", " (Ljava/lang/String;J[JJ)J" );
    }

    if (!jvm)
    {
      env->GetJavaVM (&jvm);
    }

    const char *nativeName = env->GetStringUTFChars (name, 0);
    definedFunctionsMap[std::string (nativeName)] = cptr;

    knowledge->define_function (
      std::string (nativeName), default_madara_return_function);

    env->ReleaseStringUTFChars (name, nativeName);
  }
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_defineFunction
* Signature: (JLjava/lang/String;Ljava/lang/String;)V
*/

void JNICALL Java_com_madara_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jlong cptr, jstring name, jstring expression)
{
  const char *nativeExpression = env->GetStringUTFChars (expression, 0);
  const char *nativeName = env->GetStringUTFChars (name, 0);
  
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;

  knowledge->define_function (std::string (nativeName),
    std::string (nativeExpression));

  env->ReleaseStringUTFChars (name, nativeName);
  env->ReleaseStringUTFChars (expression, nativeExpression);
}

/*
 * Class:   com_madara_KnowledgeBase
 * Method:  jni_defineFunction
 * Signature: (JLjava/lang/String;Lcom/madara/MadaraFunction;)V
 */
void JNICALL Java_com_madara_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2Lcom_madara_MadaraFunction_2
  (JNIEnv * env, jobject obj, jlong cptr, jstring name, jobject func)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;

  const char * nativeName = env->GetStringUTFChars (name, 0);

  if (knowledge)
  {
    knowledge->define_function (std::string (nativeName), func);
  }

  env->ReleaseStringUTFChars (name, nativeName);
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_clear
* Signature: (J)V
*/
void JNICALL Java_com_madara_KnowledgeBase_jni_1clear (JNIEnv *env, jobject obj, jlong cptr)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;

  if (knowledge)
    knowledge->clear ();
}

jboolean JNICALL Java_com_madara_KnowledgeBase_jni_1exists
  (JNIEnv * env, jobject, jlong cptr, jstring name)
{
  jboolean result (false);
  
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;

  const char * nativeName = env->GetStringUTFChars (name, 0);

  if (knowledge)
  {
    result = (jboolean) knowledge->exists (nativeName);
  }

  env->ReleaseStringUTFChars (name, nativeName);

  return result;
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_get
* Signature: (JLjava/lang/String;)J
*/
jlong JNICALL Java_com_madara_KnowledgeBase_jni_1get
  (JNIEnv *env, jobject obj, jlong cptr, jstring var)
{
  const char *nativeVar = env->GetStringUTFChars (var, 0);
  
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;

  Madara::Knowledge_Record * result (0);
  if (result)
  {
    result = knowledge->get (nativeVar).clone ();
  }

  env->ReleaseStringUTFChars (var, nativeVar);

  return (jlong) result;
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_setInteger
* Signature: (JLjava/lang/String;J)V
*/
void JNICALL Java_com_madara_KnowledgeBase_jni_1setInteger
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jlong value)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);

  if (knowledge)
  {
    knowledge->set (nativeVar, Integer (value));
  }

  env->ReleaseStringUTFChars (var, nativeVar);
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_setDouble
* Signature: (JLjava/lang/String;D)V
*/
void JNICALL Java_com_madara_KnowledgeBase_jni_1setDouble
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jdouble value)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);

  if (knowledge)
  {
    knowledge->set (nativeVar, (double) value);
  }

  env->ReleaseStringUTFChars (var, nativeVar);
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_setString
* Signature: (JLjava/lang/String;Ljava/lang/String;)V
*/
void JNICALL Java_com_madara_KnowledgeBase_jni_1setString
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jstring value)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  const char * nativeValue = env->GetStringUTFChars (value, 0);

  if (knowledge)
  {
    knowledge->set (nativeVar, nativeValue);
  }

  env->ReleaseStringUTFChars (var, nativeVar);
  env->ReleaseStringUTFChars (value, nativeValue);
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_setIntegerArray
* Signature: (JLjava/lang/String;[J)V
*/
void JNICALL Java_com_madara_KnowledgeBase_jni_1setIntegerArray
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jlongArray value)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  jsize len = env->GetArrayLength (value);
  jboolean isCopy;

  jlong * intArray = env->GetLongArrayElements (value, &isCopy);
  std::vector<Madara::Knowledge_Record::Integer> intVector (len);

  // copy elements to the STL vector
  for (int x = 0; x < len; x++)
    intVector[x] = intArray[x];

  if (knowledge)
  {
    knowledge->set (nativeVar, intVector);
  }

  if (isCopy)
    env->ReleaseLongArrayElements (value, intArray, JNI_ABORT);
  env->ReleaseStringUTFChars (var, nativeVar);
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_setDoubleArray
* Signature: (JLjava/lang/String;[D)V
*/
void JNICALL Java_com_madara_KnowledgeBase_jni_1setDoubleArray
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jdoubleArray value)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  jsize len = env->GetArrayLength (value);
  jboolean isCopy;

  jdouble* dblArray = env->GetDoubleArrayElements (value, &isCopy);
  std::vector<double> dblVector (len);

  // copy elements to the STL vector
  for (int x = 0; x < len; x++)
    dblVector[x] = dblArray[x];

  if (knowledge)
  {
    knowledge->set (std::string (nativeVar), dblVector);
  }

  if (isCopy)
    env->ReleaseDoubleArrayElements (value, dblArray, JNI_ABORT);

  env->ReleaseStringUTFChars (var, nativeVar);
}

void JNICALL Java_com_madara_KnowledgeBase_jni_1setFile
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jbyteArray value)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  jsize len = env->GetArrayLength (value);
  jboolean isCopy;

  jbyte * source = env->GetByteArrayElements (value, &isCopy);
  unsigned char * dest = new unsigned char [len];

  // copy file contents
  memcpy (dest, source, (size_t)len);

  if (knowledge)
  {
    knowledge->set_file (std::string (nativeVar), dest, (size_t)len);
  }

  delete [] dest;

  if (isCopy)
    env->ReleaseByteArrayElements (value, source, JNI_ABORT);

  env->ReleaseStringUTFChars (var, nativeVar);
}

void JNICALL Java_com_madara_KnowledgeBase_jni_1setImage
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jbyteArray value)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);

  jsize len = env->GetArrayLength (value);
  jboolean isCopy;

  jbyte * source = env->GetByteArrayElements (value, &isCopy);
  unsigned char * dest = new unsigned char [len];

  // copy image contents
  memcpy (dest, source, (size_t)len);

  if (knowledge)
  {
    knowledge->set_jpeg (std::string (nativeVar), dest, (size_t)len);
  }

  delete [] dest;

  if (isCopy)
    env->ReleaseByteArrayElements (value, source, JNI_ABORT);

  env->ReleaseStringUTFChars (var, nativeVar);
}


void JNICALL Java_com_madara_KnowledgeBase_jni_1setIntegerSettings
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jlong value, jlong settings_ptr)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  engine::Eval_Settings * settings = (engine::Eval_Settings *)settings_ptr;
  
  if (knowledge && settings)
  {
    knowledge->set (nativeVar, Integer (value), *settings);
  }

  env->ReleaseStringUTFChars (var, nativeVar);
}

/*
 * Class:   com_madara_KnowledgeBase
 * Method:  jni_setDouble
 * Signature: (JLjava/lang/String;DJ)V
 */
void JNICALL Java_com_madara_KnowledgeBase_jni_1setDoubleSettings
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jdouble value, jlong settings_ptr)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  engine::Eval_Settings * settings = (engine::Eval_Settings *)settings_ptr;
  
  if (knowledge && settings)
  {
    knowledge->set (nativeVar, value, *settings);
  }

  env->ReleaseStringUTFChars (var, nativeVar);
}

/*
 * Class:   com_madara_KnowledgeBase
 * Method:  jni_setString
 * Signature: (JLjava/lang/String;Ljava/lang/String;J)V
 */
void JNICALL Java_com_madara_KnowledgeBase_jni_1setStringSettings
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jstring value, jlong settings_ptr)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  const char * nativeValue = env->GetStringUTFChars (value, 0);
  engine::Eval_Settings * settings = (engine::Eval_Settings *)settings_ptr;
  
  if (knowledge && settings)
  {
    knowledge->set (nativeVar, nativeValue, *settings);
  }

  env->ReleaseStringUTFChars (var, nativeVar);
  env->ReleaseStringUTFChars (value, nativeValue);
}

/*
 * Class:   com_madara_KnowledgeBase
 * Method:  jni_setIntegerArray
 * Signature: (JLjava/lang/String;[JJ)V
 */
void JNICALL Java_com_madara_KnowledgeBase_jni_1setIntegerArraySettings
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jlongArray value, jlong settings_ptr)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  engine::Eval_Settings * settings = (engine::Eval_Settings *)settings_ptr;
  jsize len = env->GetArrayLength (value);
  jboolean isCopy;

  jlong* intArray = env->GetLongArrayElements (value, &isCopy);
  std::vector<Madara::Knowledge_Record::Integer> intVector (len);

  //copy elements to STL vector
  for (int x = 0; x < len; x++)
    intVector[x] = intArray[x];

  if (knowledge && settings)
  {
    knowledge->set (std::string (nativeVar), intVector, *settings);
  }

  if (isCopy)
    env->ReleaseLongArrayElements (value, intArray, JNI_ABORT);
  env->ReleaseStringUTFChars (var, nativeVar);
}

/*
 * Class:   com_madara_KnowledgeBase
 * Method:  jni_setDoubleArray
 * Signature: (JLjava/lang/String;[DJ)V
 */
void JNICALL Java_com_madara_KnowledgeBase_jni_1setDoubleArraySettings
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jdoubleArray value, jlong settings_ptr)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  engine::Eval_Settings * settings = (engine::Eval_Settings *)settings_ptr;
  jsize len = env->GetArrayLength (value);
  jboolean isCopy;

  jdouble * dblArray = env->GetDoubleArrayElements (value, &isCopy);
  std::vector<double> dblVector (len);

  // copy elements to STL vector
  for (int x = 0; x < len; x++)
    dblVector[x] = dblArray[x];

  if (knowledge && settings)
  {
    knowledge->set (std::string (nativeVar), dblVector, *settings);
  }

  if (isCopy)
    env->ReleaseDoubleArrayElements (value, dblArray, JNI_ABORT);
  env->ReleaseStringUTFChars (var, nativeVar);
}


void JNICALL Java_com_madara_KnowledgeBase_jni_1setFileSettings
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jbyteArray value, jlong settings_ptr)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  engine::Eval_Settings * settings = (engine::Eval_Settings *)settings_ptr;
  jsize len = env->GetArrayLength (value);
  jboolean isCopy;

  jbyte * source = env->GetByteArrayElements (value, &isCopy);
  unsigned char * dest = new unsigned char [len];
  memcpy (dest, source, (size_t)len);

  if (knowledge && settings)
  {
    knowledge->set_file (nativeVar, dest, (size_t)len, *settings);
  }

  delete [] dest;

  if (isCopy)
    env->ReleaseByteArrayElements (value, source, JNI_ABORT);

  env->ReleaseStringUTFChars (var, nativeVar);
}

void JNICALL Java_com_madara_KnowledgeBase_jni_1setImageSettings
  (JNIEnv * env, jclass cls, jlong cptr, jstring var, jbyteArray value, jlong settings_ptr)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  engine::Eval_Settings * settings = (engine::Eval_Settings *)settings_ptr;
  jsize len = env->GetArrayLength (value);
  jboolean isCopy;

  jbyte * source = env->GetByteArrayElements (value, &isCopy);
  unsigned char * dest = new unsigned char [len];

  // copy image to buffer
  memcpy (dest, source, (size_t)len);

  if (knowledge && settings)
  {
    knowledge->set_jpeg (std::string (nativeVar), dest, (size_t)len, *settings);
  }

  delete [] dest;

  if (isCopy)
    env->ReleaseByteArrayElements (value, source, JNI_ABORT);

  env->ReleaseStringUTFChars (var, nativeVar);
}


/*
 * Class:   com_madara_KnowledgeBase
 * Method:  jni_sendModifieds
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1sendModifieds__J
  (JNIEnv *, jobject, jlong cptr)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;

  if (knowledge)
  {
    knowledge->send_modifieds ();
  }
}

/*
 * Class:   com_madara_KnowledgeBase
 * Method:  jni_sendModifieds
 * Signature: (JJ)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1sendModifieds__JJ
  (JNIEnv *, jobject, jlong cptr, jlong evalSettings)
{
  engine::Knowledge_Base * knowledge =
  (engine::Knowledge_Base *) cptr;
  engine::Eval_Settings * settings = (engine::Eval_Settings*) evalSettings;

  if (knowledge && settings)
  {
    knowledge->send_modifieds ("Knowledge_Base::send_modifieds", evalSettings);
  }
}

/*
 * Class:   com_madara_KnowledgeBase
 * Method:  jni_print
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1print__JLjava_lang_String_2
  (JNIEnv * env, jobject, jlong cptr, jstring statement)
{
  const char * printable = env->GetStringUTFChars (statement, 0);

  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;

  if (knowledge)
  {
    knowledge->print (printable);
  }

  env->ReleaseStringUTFChars (statement, printable);
}


/*
 * Class:   com_madara_KnowledgeBase
 * Method:  jni_print
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1print__J
  (JNIEnv *, jobject, jlong cptr)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;

  if (knowledge)
  {
    knowledge->print ();
  }
}


/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_wait
* Signature: (JLjava/lang/String;J)J
*/
jlong JNICALL Java_com_madara_KnowledgeBase_jni_1wait__JLjava_lang_String_2J
  (JNIEnv * env, jobject obj, jlong cptr, jstring expression, jlong waitSettings)
{
  const char *nativeExpression = env->GetStringUTFChars (expression, 0);

  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  engine::Wait_Settings * settings = (engine::Wait_Settings *) waitSettings;

  Madara::Knowledge_Record * result (0);

  if (knowledge && settings)
  {
    result = knowledge->wait (nativeExpression, *settings).clone ();
  }

  env->ReleaseStringUTFChars (expression, nativeExpression);

  return (jlong) result;
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_wait
* Signature: (JJJ)J
*/
jlong JNICALL Java_com_madara_KnowledgeBase_jni_1wait__JJJ
  (JNIEnv * env, jobject obj, jlong cptr, jlong cexpression, jlong waitSettings)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  engine::Compiled_Expression * compiled = (engine::Compiled_Expression *) cexpression;
  engine::Wait_Settings * settings = (engine::Wait_Settings *) waitSettings;
  Madara::Knowledge_Record * result (0);
  
  if (knowledge && compiled && settings)
  {
    result = knowledge->wait (*compiled, *settings).clone ();
  }

  return (jlong) result;
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_waitNoReturn
* Signature: (JLjava/lang/String;J)V
*/
void JNICALL Java_com_madara_KnowledgeBase_jni_1waitNoReturn__JLjava_lang_String_2J
  (JNIEnv * env, jobject obj, jlong cptr, jstring expression, jlong waitSettings)
{
  const char *nativeExpression = env->GetStringUTFChars (expression, 0);

  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  engine::Wait_Settings * settings = (engine::Wait_Settings *) waitSettings;

  if (knowledge)
  {
    knowledge->wait (nativeExpression, *settings);
  }

  env->ReleaseStringUTFChars (expression, nativeExpression);
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_waitNoReturn
* Signature: (JJJ)V
*/
void JNICALL Java_com_madara_KnowledgeBase_jni_1waitNoReturn__JJJ
  (JNIEnv * env, jobject obj, jlong cptr, jlong expression, jlong waitSettings)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  engine::Compiled_Expression * compiled = (engine::Compiled_Expression *) expression;
  engine::Wait_Settings * settings = (engine::Wait_Settings*) waitSettings;

  if (knowledge && compiled && settings)
  {
    knowledge->wait (*compiled, *settings);
  }
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_freeKnowledgeBase
* Signature: (J)V
*/
void JNICALL Java_com_madara_KnowledgeBase_jni_1freeKnowledgeBase
  (JNIEnv * env, jobject obj, jlong cptr)
{
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;

  // if check is unnecessary as standard allows deletion of 0 (no-op)
  delete knowledge;
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_toKnowledgeList
* Signature: (JLjava/lang/String;II)[J
*/

jlongArray JNICALL Java_com_madara_KnowledgeBase_jni_1toKnowledgeList
  (JNIEnv * env, jobject obj, jlong cptr, jstring subject, jint start, jint end)
{
  const char *nativeSubject = env->GetStringUTFChars (subject, 0);
  
  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
  jlongArray ret (0);

  if (knowledge)
  {
    std::vector<Madara::Knowledge_Record> returnVector;

    knowledge->to_vector (nativeSubject, start, end, returnVector);

    jlongArray ret = env->NewLongArray ( (jsize)returnVector.size ());
    jlong * tmp = new jlong [ (jsize)returnVector.size ()];

    for (unsigned int x = 0; x < returnVector.size (); x++)
    {
      tmp[x] = (jlong) new Madara::Knowledge_Record (returnVector[x]);
    }

    env->SetLongArrayRegion (ret, 0, (jsize)returnVector.size (), tmp);

    delete [] tmp;
  }

  return ret;
}


/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_toKnowledgeMap
* Signature: (JLjava/lang/String;Lcom/madara/KnowledgeBase/MapReturn;)V
*/

void JNICALL Java_com_madara_KnowledgeBase_jni_1toKnowledgeMap
  (JNIEnv * env, jobject obj, jlong cptr, jstring expression, jobject jniRet)
{
  jclass jniRetClass = env->GetObjectClass (jniRet);
  jclass classStrArray = env->FindClass ("java/lang/String");
  jfieldID valsID = env->GetFieldID (jniRetClass, "vals", "[J");
  jfieldID keysID = env->GetFieldID (jniRetClass,
  "keys", "[Ljava/lang/String;");

  const char * nativeExpression = env->GetStringUTFChars (expression, 0);

  engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;

  if (knowledge)
  {
    std::map<std::string, Madara::Knowledge_Record> recordsMap;

    knowledge->to_map (std::string (nativeExpression), recordsMap);

    env->ReleaseStringUTFChars (expression, nativeExpression);

    // break the resulting map into keys and values
    jlongArray recordsArray = env->NewLongArray ( (jsize)recordsMap.size ());
    jlong * records = new jlong [ (jsize)recordsMap.size ()];

    // create the java keys array
    jobjectArray keysArray = env->NewObjectArray ( (jsize)recordsMap.size (),
      classStrArray, NULL);

    std::map<std::string, Madara::Knowledge_Record>::iterator iter;
    int counter = 0;

    // populate the Java objects
    for (iter = recordsMap.begin (); iter != recordsMap.end (); ++iter)
    {
      env->SetObjectArrayElement (keysArray, counter,
        env->NewStringUTF (iter->first.c_str ()));
      records[counter++] = (jlong) new Madara::Knowledge_Record (iter->second);
    }

    env->SetLongArrayRegion (recordsArray, 0, (jsize)recordsMap.size (), records);

    delete [] records;

    env->SetObjectField (jniRet, valsID, recordsArray);
    env->SetObjectField (jniRet, keysID, keysArray);
  }
}


//===================================================================================

//===================================================================================
//CompiledExpression
//===================================================================================
/*
* Class:   com_madara_KnowledgeBase_CompiledExpression
* Method:  jni_freeCompiledExpression
* Signature: (J)V
*/
void JNICALL Java_com_madara_KnowledgeBase_00024CompiledExpression_jni_1freeCompiledExpression
  (JNIEnv * env, jobject obj, jlong cptr)
{
  engine::Compiled_Expression * expression = (engine::Compiled_Expression*)cptr;

  // deleting 0 is allowed by standard
  delete expression;
}  
//===================================================================================


/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_saveContext
 * Signature: (JLjava/lang/String;)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1saveContext
  (JNIEnv * env, jobject, jlong cptr, jstring filename)
{
  int64_t result (0);

  if (cptr && filename)
  {
    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
   
    result = knowledge->save_context (nativeFilename);

    env->ReleaseStringUTFChars (filename, nativeFilename);
  }

  return result;
}

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_saveCheckpoint
 * Signature: (JLjava/lang/String;Z)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1saveCheckpoint
  (JNIEnv * env, jobject, jlong cptr, jstring filename, jboolean resetModifieds)
{
  int64_t result (0);

  if (cptr && filename)
  {
    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
   
    bool flag = resetModifieds != 0;

    result = knowledge->save_checkpoint (nativeFilename, flag);

    env->ReleaseStringUTFChars (filename, nativeFilename);
  }

  return result;
}

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_loadContext
 * Signature: (JLjava/lang/String;ZJ)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1loadContext
  (JNIEnv * env, jobject, jlong cptr, jstring filename, jboolean useId, jlong settings_ptr)
{
  int64_t result (0);

  if (cptr && filename)
  {
    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    engine::Knowledge_Base * knowledge = (engine::Knowledge_Base *) cptr;
    engine::Knowledge_Update_Settings * settings =
      (engine::Knowledge_Update_Settings*) settings_ptr;
   
    bool flag = useId != 0;

    result = knowledge->load_context (nativeFilename, flag, *settings);

    env->ReleaseStringUTFChars (filename, nativeFilename);
  }

  return result;
}
