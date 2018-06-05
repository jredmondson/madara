/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "com.madara.KnowledgeBase.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/transport/QoSTransportSettings.h"

#include <string>
#include <string.h>
#include <map>

namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
namespace logger = madara::logger;
typedef madara::knowledge::KnowledgeRecord::Integer Integer;
typedef knowledge::KnowledgeBase         KnowledgeBase;
typedef knowledge::CompiledExpression    CompiledExpression;
typedef logger::Logger                 Logger;

static jclass knowledgeBaseClass = 0;
static jmethodID callbackMethod = 0;
static std::map<std::string, jlong> definedFunctionsMap;

static JavaVM *jvm;


//===================================================================================
//KnowledgeBase
//===================================================================================
jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1KnowledgeBase__
  (JNIEnv *, jobject)
{
  return (jlong) (new KnowledgeBase ());
}

jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1KnowledgeBase__Ljava_lang_String_2ILjava_lang_String_2
  (JNIEnv * env, jobject, jstring host, jint transport, jstring domain)
{

  const char * nativeHost = env->GetStringUTFChars (host, 0);
  const char * nativeDomain = env->GetStringUTFChars (domain, 0);
  KnowledgeBase * knowledge (0);

  if (nativeHost && nativeDomain)
  {
    knowledge = new KnowledgeBase (
      nativeHost, transport, nativeDomain);
  }

  if (nativeHost)
  {
    env->ReleaseStringUTFChars (host, nativeHost);
  }

  if (nativeDomain)
  {
    env->ReleaseStringUTFChars (domain, nativeDomain);
  }

  return (jlong) knowledge;
}

jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1KnowledgeBase__Ljava_lang_String_2J
  (JNIEnv *env, jobject, jstring host, jlong csettings)
{
  const char * nativeHost = env->GetStringUTFChars (host, 0);
  transport::TransportSettings * settings = (madara::transport::TransportSettings *) csettings;
  KnowledgeBase * knowledge (0);

  if (settings)
  {
    if (nativeHost)
    {
      knowledge = new KnowledgeBase (nativeHost, *settings);
      env->ReleaseStringUTFChars (host, nativeHost);
    }
    else
    {
      knowledge = new KnowledgeBase ("", *settings);
    }
  }


  return (jlong) knowledge;
}

jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1KnowledgeBase__J
  (JNIEnv *, jobject, jlong original)
{
  KnowledgeBase * result (0);
  KnowledgeBase * source = (KnowledgeBase *) original;

  if (source)
  {
    result = new KnowledgeBase (*source);
  }

  return (jlong) result;
}

jstring JNICALL
Java_com_madara_KnowledgeBase_jni_1getID
  (JNIEnv * env, jobject, jlong cptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *)cptr;
  jstring result;

  if (knowledge)
  {
    std::string id = knowledge->get_id ();
    result = env->NewStringUTF (id.c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

void JNICALL
Java_com_madara_KnowledgeBase_jni_1attachLogger
(JNIEnv *, jobject, jlong cptr, jlong logger_ptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *)cptr;
  Logger * logger = (Logger *)logger_ptr;

  if (knowledge && logger)
  {
    knowledge->attach_logger (*logger);
  }
}

void JNICALL
Java_com_madara_KnowledgeBase_jni_1attachTransport
  (JNIEnv * env, jobject, jlong cptr, jstring id, jlong settings_ptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  const char * native_id = env->GetStringUTFChars (id, 0);
  transport::TransportSettings * settings = (transport::TransportSettings *) settings_ptr;

  if (knowledge && native_id && settings)
  {
    knowledge->attach_transport (native_id, *settings);
  }
}

jstring JNICALL
Java_com_madara_KnowledgeBase_jni_1debugModifieds
(JNIEnv * env, jobject, jlong cptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *)cptr;
  jstring result;

  if (knowledge)
  {
    result = env->NewStringUTF (knowledge->debug_modifieds ().c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}


void JNICALL
Java_com_madara_KnowledgeBase_jni_1closeTransports
(JNIEnv *, jobject, jlong cptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *)cptr;

  if (knowledge)
  {
    knowledge->close_transport ();
  }
}

jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1getLogger
(JNIEnv *, jobject, jlong cptr)
{
  Logger * result (0);
  KnowledgeBase * knowledge = (KnowledgeBase *)cptr;

  if (knowledge)
  {
    result = &(knowledge->get_logger ());
  }

  return (jlong) result;
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_evaluate
* Signature: (JLjava/lang/String;J)J
*/
jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1evaluate__JLjava_lang_String_2J
  (JNIEnv *env, jobject, jlong cptr, jstring expression, jlong evalSettings)
{
  madara::knowledge::KnowledgeRecord * result (0);
  const char * nativeExpression = env->GetStringUTFChars (expression, 0);
  knowledge::EvalSettings * settings = (knowledge::EvalSettings *)evalSettings;
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

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
jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1evaluate__JJJ
  (JNIEnv *, jobject, jlong cptr, jlong expression, jlong evalSettings)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  CompiledExpression * compiled =
    (CompiledExpression *) expression;
  knowledge::EvalSettings * settings = (knowledge::EvalSettings*) evalSettings;

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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1evaluateNoReturn__JLjava_lang_String_2J
  (JNIEnv * env, jobject, jlong cptr, jstring expression, jlong evalSettings)
{
  const char * nativeExpression = env->GetStringUTFChars (expression, 0);
  
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  knowledge::EvalSettings * settings = (knowledge::EvalSettings*) evalSettings;

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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1evaluateNoReturn__JJJ
  (JNIEnv *, jobject, jlong cptr, jlong expression, jlong evalSettings)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  CompiledExpression * compiled =
    (CompiledExpression *) expression;
  knowledge::EvalSettings * settings = (knowledge::EvalSettings*) evalSettings;

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
jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1compile
  (JNIEnv *env, jobject, jlong cptr, jstring expression)
{
  const char * nativeExpression = env->GetStringUTFChars (expression, 0);
  
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

  CompiledExpression* result (0);
  
  if (knowledge)
  {
    result = new CompiledExpression (
      knowledge->compile (std::string (nativeExpression)));
  }

  env->ReleaseStringUTFChars (expression, nativeExpression);

  return (jlong) result;
}

madara::knowledge::KnowledgeRecord default_madara_return_function
  (const char * name, knowledge::FunctionArguments & args,
   knowledge::Variables & variables)
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
    return madara::knowledge::KnowledgeRecord (0);
  }
  else
  {
    madara::knowledge::KnowledgeRecord returnValue (* (madara::knowledge::KnowledgeRecord*)ret);
    delete (madara::knowledge::KnowledgeRecord*)ret;
    return returnValue;
  }
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_defineFunction
* Signature: (JLjava/lang/String;)V
*/
void JNICALL
Java_com_madara_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2
  (JNIEnv *env, jobject obj, jlong cptr, jstring name)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

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

    const char * nativeName = env->GetStringUTFChars (name, 0);
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

void JNICALL
Java_com_madara_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2Ljava_lang_String_2
  (JNIEnv *env, jobject, jlong cptr, jstring name, jstring expression)
{
  const char * nativeExpression = env->GetStringUTFChars (expression, 0);
  const char * nativeName = env->GetStringUTFChars (name, 0);
  
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2Lcom_madara_MadaraFunction_2
  (JNIEnv * env, jobject, jlong cptr, jstring name, jobject func)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1clear (
  JNIEnv *, jobject, jlong cptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

  if (knowledge)
  {
    knowledge->clear ();
  }
}

jboolean JNICALL
Java_com_madara_KnowledgeBase_jni_1exists
  (JNIEnv * env, jobject, jlong cptr, jstring name)
{
  jboolean result (false);
  
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

  const char * nativeName = env->GetStringUTFChars (name, 0);

  if (knowledge)
  {
    result = (jboolean) knowledge->exists (nativeName);
  }

  env->ReleaseStringUTFChars (name, nativeName);

  return result;
}

jstring JNICALL
Java_com_madara_KnowledgeBase_jni_1toString
(JNIEnv * env, jobject, jlong cptr, jstring arrayDelimiter,
  jstring recordDelimiter, jstring keyvalDelimiter)
{
  jstring result;

  KnowledgeBase * knowledge = (KnowledgeBase *)cptr;

  const char * nativeArrayDelimiter = env->GetStringUTFChars (arrayDelimiter, 0);
  const char * nativeRecordDelimiter = env->GetStringUTFChars (recordDelimiter, 0);
  const char * nativeKeyvalDelimiter = env->GetStringUTFChars (keyvalDelimiter, 0);

  if (knowledge &&
    nativeArrayDelimiter && nativeRecordDelimiter && nativeKeyvalDelimiter)
  {
    std::string stringkb;
    knowledge->to_string (stringkb, nativeArrayDelimiter,
      nativeRecordDelimiter, nativeKeyvalDelimiter);
    result = env->NewStringUTF (stringkb.c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  env->ReleaseStringUTFChars (arrayDelimiter, nativeArrayDelimiter);
  env->ReleaseStringUTFChars (recordDelimiter, nativeRecordDelimiter);
  env->ReleaseStringUTFChars (keyvalDelimiter, nativeKeyvalDelimiter);

  return result;
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_get
* Signature: (JLjava/lang/String;)J
*/
jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1get
  (JNIEnv *env, jobject, jlong cptr, jstring var)
{
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

  madara::knowledge::KnowledgeRecord * result (0);
  if (knowledge)
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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1setInteger
  (JNIEnv * env, jclass, jlong cptr, jstring var, jlong value)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1setDouble
  (JNIEnv * env, jclass, jlong cptr, jstring var, jdouble value)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1setString
  (JNIEnv * env, jclass, jlong cptr, jstring var, jstring value)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1setIntegerArray
  (JNIEnv * env, jclass, jlong cptr, jstring var, jlongArray value)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  jsize len = env->GetArrayLength (value);
  jboolean isCopy;

  jlong * intArray = env->GetLongArrayElements (value, &isCopy);
  std::vector<madara::knowledge::KnowledgeRecord::Integer> intVector (len);

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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1setDoubleArray
  (JNIEnv * env, jclass, jlong cptr, jstring var, jdoubleArray value)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
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

void JNICALL
Java_com_madara_KnowledgeBase_jni_1setFile
  (JNIEnv * env, jclass, jlong cptr, jstring var, jbyteArray value)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
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

void JNICALL
Java_com_madara_KnowledgeBase_jni_1setImage
  (JNIEnv * env, jclass, jlong cptr, jstring var, jbyteArray value)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
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


void JNICALL
Java_com_madara_KnowledgeBase_jni_1setIntegerSettings
  (JNIEnv * env, jclass, jlong cptr, jstring var, jlong value, jlong settings_ptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  knowledge::EvalSettings * settings = (knowledge::EvalSettings *)settings_ptr;
  
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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1setDoubleSettings
  (JNIEnv * env, jclass, jlong cptr, jstring var, jdouble value, jlong settings_ptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  knowledge::EvalSettings * settings = (knowledge::EvalSettings *)settings_ptr;
  
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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1setStringSettings
  (JNIEnv * env, jclass, jlong cptr, jstring var, jstring value, jlong settings_ptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  const char * nativeValue = env->GetStringUTFChars (value, 0);
  knowledge::EvalSettings * settings = (knowledge::EvalSettings *)settings_ptr;
  
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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1setIntegerArraySettings
  (JNIEnv * env, jclass, jlong cptr, jstring var, jlongArray value, jlong settings_ptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  knowledge::EvalSettings * settings = (knowledge::EvalSettings *)settings_ptr;
  jsize len = env->GetArrayLength (value);
  jboolean isCopy;

  jlong* intArray = env->GetLongArrayElements (value, &isCopy);
  std::vector<madara::knowledge::KnowledgeRecord::Integer> intVector (len);

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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1setDoubleArraySettings
  (JNIEnv * env, jclass, jlong cptr, jstring var, jdoubleArray value, jlong settings_ptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  knowledge::EvalSettings * settings = (knowledge::EvalSettings *)settings_ptr;
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


void JNICALL
Java_com_madara_KnowledgeBase_jni_1setFileSettings
  (JNIEnv * env, jclass, jlong cptr, jstring var, jbyteArray value, jlong settings_ptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  knowledge::EvalSettings * settings = (knowledge::EvalSettings *)settings_ptr;
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

void JNICALL
Java_com_madara_KnowledgeBase_jni_1setImageSettings
  (JNIEnv * env, jclass, jlong cptr, jstring var, jbyteArray value, jlong settings_ptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  const char * nativeVar = env->GetStringUTFChars (var, 0);
  knowledge::EvalSettings * settings = (knowledge::EvalSettings *)settings_ptr;
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
MADARA_EXPORT void JNICALL
Java_com_madara_KnowledgeBase_jni_1sendModifieds__J
  (JNIEnv *, jobject, jlong cptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

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
MADARA_EXPORT void JNICALL
Java_com_madara_KnowledgeBase_jni_1sendModifieds__JJ
  (JNIEnv *, jobject, jlong cptr, jlong evalSettings)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  knowledge::EvalSettings * settings = (knowledge::EvalSettings*) evalSettings;

  if (knowledge && settings)
  {
    knowledge->send_modifieds ("KnowledgeBase::send_modifieds", *settings);
  }
}

MADARA_EXPORT void JNICALL
Java_com_madara_KnowledgeBase_jni_1clearModifieds
(JNIEnv *, jobject, jlong cptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *)cptr;

  if (knowledge)
  {
    knowledge->clear_modifieds ();
  }
}

/*
 * Class:   com_madara_KnowledgeBase
 * Method:  jni_print
 * Signature: (JLjava/lang/String;)V
 */
MADARA_EXPORT void JNICALL
Java_com_madara_KnowledgeBase_jni_1print__JLjava_lang_String_2
  (JNIEnv * env, jobject, jlong cptr, jstring statement)
{
  const char * printable = env->GetStringUTFChars (statement, 0);
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

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
MADARA_EXPORT void JNICALL
Java_com_madara_KnowledgeBase_jni_1print__J
  (JNIEnv *, jobject, jlong cptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

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
jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1wait__JLjava_lang_String_2J
  (JNIEnv * env, jobject, jlong cptr, jstring expression, jlong waitSettings)
{
  const char * nativeExpression = env->GetStringUTFChars (expression, 0);
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  knowledge::WaitSettings * settings = (knowledge::WaitSettings *) waitSettings;
  madara::knowledge::KnowledgeRecord * result (0);

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
jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1wait__JJJ
  (JNIEnv *, jobject, jlong cptr, jlong cexpression, jlong waitSettings)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  CompiledExpression * compiled = (CompiledExpression *) cexpression;
  knowledge::WaitSettings * settings = (knowledge::WaitSettings *) waitSettings;
  madara::knowledge::KnowledgeRecord * result (0);
  
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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1waitNoReturn__JLjava_lang_String_2J
  (JNIEnv * env, jobject, jlong cptr, jstring expression, jlong waitSettings)
{
  const char * nativeExpression = env->GetStringUTFChars (expression, 0);

  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  knowledge::WaitSettings * settings = (knowledge::WaitSettings *) waitSettings;

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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1waitNoReturn__JJJ
  (JNIEnv *, jobject, jlong cptr, jlong expression, jlong waitSettings)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  CompiledExpression * compiled = (CompiledExpression *) expression;
  knowledge::WaitSettings * settings = (knowledge::WaitSettings*) waitSettings;

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
void JNICALL
Java_com_madara_KnowledgeBase_jni_1freeKnowledgeBase
  (JNIEnv *, jobject, jlong cptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

  // if check is unnecessary as standard allows deletion of 0 (no-op)
  delete knowledge;
}

/*
* Class:   com_madara_KnowledgeBase
* Method:  jni_toKnowledgeList
* Signature: (JLjava/lang/String;II)[J
*/

jlongArray JNICALL
Java_com_madara_KnowledgeBase_jni_1toKnowledgeList
  (JNIEnv * env, jobject, jlong cptr, jstring subject, jint start, jint end)
{
  const char * nativeSubject = env->GetStringUTFChars (subject, 0);
  
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  jlongArray ret (0);

  if (knowledge)
  {
    std::vector<madara::knowledge::KnowledgeRecord> returnVector;

    knowledge->to_vector (nativeSubject, start, end, returnVector);

    jlongArray ret = env->NewLongArray ( (jsize)returnVector.size ());
    jlong * tmp = new jlong [ (jsize)returnVector.size ()];

    for (unsigned int x = 0; x < returnVector.size (); x++)
    {
      tmp[x] = (jlong) new madara::knowledge::KnowledgeRecord (returnVector[x]);
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

void JNICALL
Java_com_madara_KnowledgeBase_jni_1toKnowledgeMap
  (JNIEnv * env, jobject, jlong cptr, jstring expression, jobject jniRet)
{
  jclass jniRetClass = env->GetObjectClass (jniRet);
  jclass classStrArray = madara::utility::java::find_class (env,
    "java/lang/String");
  jfieldID valsID = env->GetFieldID (jniRetClass, "vals", "[J");
  jfieldID keysID = env->GetFieldID (jniRetClass,
  "keys", "[Ljava/lang/String;");

  const char * nativeExpression = env->GetStringUTFChars (expression, 0);

  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;

  if (knowledge)
  {
    std::map<std::string, madara::knowledge::KnowledgeRecord> recordsMap;

    knowledge->to_map (std::string (nativeExpression), recordsMap);

    env->ReleaseStringUTFChars (expression, nativeExpression);

    // break the resulting map into keys and values
    jlongArray recordsArray = env->NewLongArray ( (jsize)recordsMap.size ());
    jlong * records = new jlong [ (jsize)recordsMap.size ()];

    // create the java keys array
    jobjectArray keysArray = env->NewObjectArray ( (jsize)recordsMap.size (),
      classStrArray, NULL);

    std::map<std::string, madara::knowledge::KnowledgeRecord>::iterator iter;
    int counter = 0;

    // populate the Java objects
    for (iter = recordsMap.begin (); iter != recordsMap.end (); ++iter)
    {
      env->SetObjectArrayElement (keysArray, counter,
        env->NewStringUTF (iter->first.c_str ()));
      records[counter++] = (jlong) new madara::knowledge::KnowledgeRecord (iter->second);
    }

    env->SetLongArrayRegion (recordsArray, 0, (jsize)recordsMap.size (), records);

    delete [] records;

    env->SetObjectField (jniRet, valsID, recordsArray);
    env->SetObjectField (jniRet, keysID, keysArray);
  }

  env->DeleteLocalRef (jniRetClass);
  env->DeleteWeakGlobalRef (classStrArray);
}

MADARA_EXPORT void JNICALL
Java_com_madara_KnowledgeBase_jni_1toMap
(JNIEnv * env, jobject, jlong cptr, jstring prefix, jstring suffix, jobject jniRet)
{
  jclass jniRetClass = env->GetObjectClass (jniRet);
  jclass classStrArray = madara::utility::java::find_class (env,
    "java/lang/String");
  jfieldID valsID = env->GetFieldID (jniRetClass, "vals", "[J");
  jfieldID keysID = env->GetFieldID (jniRetClass,
    "keys", "[Ljava/lang/String;");

  const char * nativePrefix = env->GetStringUTFChars (prefix, 0);
  const char * nativeSuffix = env->GetStringUTFChars (suffix, 0);

  KnowledgeBase * knowledge = (KnowledgeBase *)cptr;

  if (knowledge)
  {
    std::map<std::string, madara::knowledge::KnowledgeRecord> recordsMap;
    std::vector<std::string> nextKeys;

    knowledge->to_map (nativePrefix, "", nativeSuffix, nextKeys, recordsMap);

    env->ReleaseStringUTFChars (prefix, nativePrefix);
    env->ReleaseStringUTFChars (suffix, nativeSuffix);

    // break the resulting map into keys and values
    jlongArray recordsArray = env->NewLongArray ((jsize)recordsMap.size ());
    jlong * records = new jlong[(jsize)recordsMap.size ()];

    // create the java keys array
    jobjectArray keysArray = env->NewObjectArray ((jsize)recordsMap.size (),
      classStrArray, NULL);

    std::map<std::string, madara::knowledge::KnowledgeRecord>::iterator iter;
    int counter = 0;

    // populate the Java objects
    for (iter = recordsMap.begin (); iter != recordsMap.end (); ++iter)
    {
      env->SetObjectArrayElement (keysArray, counter,
        env->NewStringUTF (iter->first.c_str ()));
      records[counter++] = (jlong) new madara::knowledge::KnowledgeRecord (iter->second);
    }

    env->SetLongArrayRegion (recordsArray, 0, (jsize)recordsMap.size (), records);

    delete[] records;

    env->SetObjectField (jniRet, valsID, recordsArray);
    env->SetObjectField (jniRet, keysID, keysArray);
  }

  env->DeleteLocalRef (jniRetClass);
  env->DeleteWeakGlobalRef (classStrArray);
}

//===================================================================================

//===================================================================================
//CompiledExpression
//===================================================================================
/*
* Class:   com_madara_KnowledgeBaseCompiledExpression
* Method:  jni_freeCompiledExpression
* Signature: (J)V
*/
void JNICALL
Java_com_madara_KnowledgeBase_00024CompiledExpression_jni_1freeCompiledExpression
  (JNIEnv *, jobject, jlong cptr)
{
  CompiledExpression * expression = (CompiledExpression*)cptr;

  // deleting 0 is allowed by standard
  delete expression;
}  
//===================================================================================

jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1saveAsKarl
(JNIEnv * env, jobject, jlong cptr, jstring filename)
{
  int64_t result (0);

  if (cptr && filename)
  {
    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    KnowledgeBase * knowledge = (KnowledgeBase *)cptr;

    result = knowledge->save_as_karl (nativeFilename);

    env->ReleaseStringUTFChars (filename, nativeFilename);
  }

  return result;
}

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_saveContext
 * Signature: (JLjava/lang/String;)J
 */
MADARA_EXPORT jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1saveContext
  (JNIEnv * env, jobject, jlong cptr, jstring filename)
{
  int64_t result (0);

  if (cptr && filename)
  {
    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
   
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
MADARA_EXPORT jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1saveCheckpoint
  (JNIEnv * env, jobject, jlong cptr, jstring filename, jboolean resetModifieds)
{
  int64_t result (0);

  if (cptr && filename)
  {
    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
   
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
MADARA_EXPORT jlong JNICALL
Java_com_madara_KnowledgeBase_jni_1loadContext
  (JNIEnv * env, jobject, jlong cptr, jstring filename, jboolean useId, jlong settings_ptr)
{
  int64_t result (0);

  if (cptr && filename)
  {
    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
    knowledge::KnowledgeUpdateSettings * settings =
      (knowledge::KnowledgeUpdateSettings*) settings_ptr;
   
    bool flag = useId != 0;

    result = knowledge->load_context (nativeFilename, flag, *settings);

    env->ReleaseStringUTFChars (filename, nativeFilename);
  }

  return result;
}
