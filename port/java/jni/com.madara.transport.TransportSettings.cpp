/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "com.madara.transport.TransportSettings.h"
#include "madara/transport/Transport.h"
#include "madara_jni.h"

#include <string>
#include <stdio.h>

// define useful shorthands
namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
typedef madara::knowledge::KnowledgeRecord  KnowledgeRecord;
typedef knowledge::KnowledgeRecord::Integer Integer;
typedef transport::TransportSettings TransportSettings;

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_Settings
* Signature: ()J
*/
jlong JNICALL
Java_com_madara_transport_TransportSettings_jni_1Settings__ (JNIEnv *env, jobject obj)
{
  return (jlong) new TransportSettings;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_Settings
* Signature: (J)J
*/
jlong JNICALL
Java_com_madara_transport_TransportSettings_jni_1Settings__J (JNIEnv *env, jobject obj, jlong cptr)
{
  jlong result (0);
  TransportSettings * source = (TransportSettings *) cptr;

  if (source)
  {
    result = (jlong) new TransportSettings (*source);
  }

  return result;
}


void JNICALL
Java_com_madara_transport_TransportSettings_jni_1save
(JNIEnv * env, jobject, jlong cptr, jstring filename)
{
  if (cptr && filename)
  {
    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    TransportSettings * settings = (TransportSettings *)cptr;

    settings->save (nativeFilename);

    env->ReleaseStringUTFChars (filename, nativeFilename);
  }
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_load
* Signature: (JLjava/lang/String;)J
*/
void JNICALL
Java_com_madara_transport_TransportSettings_jni_1load
(JNIEnv * env, jobject, jlong cptr, jstring filename)
{
  if (cptr && filename)
  {
    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    TransportSettings * settings = (TransportSettings *)cptr;

    settings->load (nativeFilename);

    env->ReleaseStringUTFChars (filename, nativeFilename);
  }
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setDomains
* Signature: (JLjava/lang/String;)V
*/
void JNICALL
Java_com_madara_transport_TransportSettings_jni_1setWriteDomain (
  JNIEnv *env, jobject obj, jlong cptr, jstring domains)
{
  const char * nativeDomains = env->GetStringUTFChars (domains, 0);

  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    settings->write_domain = nativeDomains;
  }

  env->ReleaseStringUTFChars (domains, nativeDomains);
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getDomains
* Signature: (J)Ljava/lang/String;
*/
jstring JNICALL
Java_com_madara_transport_TransportSesttings_jni_1getWriteDomain (
  JNIEnv * env, jobject obj, jlong cptr)
{
  jstring result;
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    result = env->NewStringUTF (settings->write_domain.c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setQueueLength
* Signature: (JI)V
*/
void JNICALL
Java_com_madara_transport_TransportSettings_jni_1setQueueLength (JNIEnv * env, jobject obj, jlong cptr, jint queueLength)
{
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    settings->queue_length = queueLength;
  }
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getQueueLength
* Signature: (J)I
*/
jint JNICALL
Java_com_madara_transport_TransportSettings_jni_1getQueueLength (JNIEnv * env, jobject obj, jlong cptr)
{
  jint result (0);
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    result = (jint) settings->queue_length;
  }

  return result;
}

void JNICALL
Java_com_madara_transport_TransportSettings_jni_1setResendAttempts
(JNIEnv *, jobject, jlong cptr, jint resends)
{
  TransportSettings * settings = (TransportSettings *)cptr;

  if (settings)
  {
    settings->resend_attempts = resends;
  }
}


jint JNICALL
Java_com_madara_transport_TransportSettings_jni_1getResendAttempts
(JNIEnv *, jobject, jlong cptr)
{
  jint result (0);
  TransportSettings * settings = (TransportSettings *)cptr;

  if (settings)
  {
    result = (jint)settings->resend_attempts;
  }

  return result;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setType
* Signature: (JI)V
*/
void JNICALL
Java_com_madara_transport_TransportSettings_jni_1setType (JNIEnv * env, jobject obj, jlong cptr, jint type)
{
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    settings->type = type;
  }
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getType
* Signature: (J)I
*/
jint JNICALL
Java_com_madara_transport_TransportSettings_jni_1getType (JNIEnv *env, jobject obj, jlong cptr)
{
  jint result (0);
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    result = (jint) settings->type;
  }

  return result;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setReliability
* Signature: (JI)V
*/
void JNICALL
Java_com_madara_transport_TransportSettings_jni_1setReliability (JNIEnv * env, jobject obj, jlong cptr, jint reliability)
{
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    settings->reliability = reliability;
  }
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getReliability
* Signature: (J)I
*/
jint JNICALL
Java_com_madara_transport_TransportSettings_jni_1getReliability (JNIEnv * env, jobject obj, jlong cptr)
{
  jint result (0);
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    result = (jint) settings->reliability;
  }

  return result;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setId
* Signature: (JI)V
*/
void JNICALL
Java_com_madara_transport_TransportSettings_jni_1setId (JNIEnv *env, jobject obj, jlong cptr, jint id)
{
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    settings->id = id;
  }
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getId
* Signature: (J)I
*/
jint JNICALL
Java_com_madara_transport_TransportSettings_jni_1getId (JNIEnv *env, jobject obj, jlong cptr)
{
  jint result (0);
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    result = (jint) settings->id;
  }

  return result;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setProcesses
* Signature: (JI)V
*/
void JNICALL
Java_com_madara_transport_TransportSettings_jni_1setProcesses (JNIEnv *env, jobject obj, jlong cptr, jint processes)
{
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    settings->processes = processes;
  }
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getProcesses
* Signature: (J)I
*/
jint JNICALL
Java_com_madara_transport_TransportSettings_jni_1getProcesses (JNIEnv *env, jobject obj, jlong cptr)
{
  jint result (0);
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    result = (jint) settings->processes;
  }

  return result;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setOnDataReceivedLogic
* Signature: (JLjava/lang/String;)V
*/
void JNICALL
Java_com_madara_transport_TransportSettings_jni_1setOnDataReceivedLogic (JNIEnv *env, jobject obj, jlong cptr, jstring onDataReceivedLogic)
{
  const char * nativeOnDataReceivedLogic = env->GetStringUTFChars (
    onDataReceivedLogic, 0);
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    settings->on_data_received_logic = nativeOnDataReceivedLogic;
  }

  env->ReleaseStringUTFChars (onDataReceivedLogic, nativeOnDataReceivedLogic);
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getOnDataReceivedLogic
* Signature: (J)Ljava/lang/String;
*/
jstring JNICALL
Java_com_madara_transport_TransportSettings_jni_1getOnDataReceivedLogic (JNIEnv *env, jobject obj, jlong cptr)
{
  jstring result;
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    result = env->NewStringUTF (settings->on_data_received_logic.c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setHosts
* Signature: (J[Ljava/lang/String;)V
*/
void JNICALL
Java_com_madara_transport_TransportSettings_jni_1setHosts (JNIEnv *env, jobject obj, jlong cptr, jobjectArray hosts)
{
  int hostsLen = env->GetArrayLength (hosts);
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    settings->hosts.resize (hostsLen);

    for (int x = 0; x < hostsLen; x++)
    {
      jstring jhost = (jstring) env->GetObjectArrayElement (hosts, x);
      const char * curHost = env->GetStringUTFChars (jhost, 0);
      settings->hosts[x] = std::string (curHost);
      env->ReleaseStringUTFChars (jhost, curHost);
    }
  }
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getHosts
* Signature: (J)[Ljava/lang/String;
*/
jobjectArray JNICALL
Java_com_madara_transport_TransportSettings_jni_1getHosts (JNIEnv *env, jobject obj, jlong cptr)
{
  jobjectArray result (0);
  TransportSettings * settings = (TransportSettings *) cptr;

  if (settings)
  {
    jclass string_class = madara::utility::java::find_class (
      env, "java/lang/String");
    jstring empty_string = env->NewStringUTF ("");

    result = env->NewObjectArray (
      (jsize)settings->hosts.size (),
      string_class,
      empty_string);

    for (unsigned int x = 0; x < settings->hosts.size (); x++)
    {
      jstring temp_string = env->NewStringUTF (settings->hosts[x].c_str ());

      env->SetObjectArrayElement (
        result, x, temp_string);

      env->DeleteLocalRef (temp_string);
    }

    env->DeleteWeakGlobalRef (string_class);
    env->DeleteLocalRef (empty_string);
  }
  return result;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_freeSettings
* Signature: (J)V
*/
void JNICALL
Java_com_madara_transport_TransportSettings_jni_1freeSettings (JNIEnv * env, jclass cls, jlong cptr)
{
  TransportSettings * settings = (TransportSettings *) cptr;

  delete settings;
}


