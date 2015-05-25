/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "com.madara.transport.TransportSettings.h"
#include "madara/transport/Transport.h"

#include <string>
#include <stdio.h>

// define useful shorthands
namespace engine = Madara::Knowledge_Engine;
namespace transport = Madara::Transport;
typedef Madara::Knowledge_Record  Knowledge_Record;
typedef Knowledge_Record::Integer Integer;
typedef transport::Settings Settings;

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_Settings
* Signature: ()J
*/
jlong JNICALL Java_com_madara_transport_TransportSettings_jni_1Settings__ (JNIEnv *env, jobject obj)
{
  return (jlong) new Settings;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_Settings
* Signature: (J)J
*/
jlong JNICALL Java_com_madara_transport_TransportSettings_jni_1Settings__J (JNIEnv *env, jobject obj, jlong cptr)
{
  jlong result (0);
  Settings * source = (Settings *) cptr;

  if (source)
  {
    result = (jlong) new Settings (*source);
  }

  return result;
}


void JNICALL Java_com_madara_transport_TransportSettings_jni_1save
(JNIEnv * env, jobject, jlong cptr, jstring filename)
{
  if (cptr && filename)
  {
    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    Settings * settings = (Settings *)cptr;

    settings->save (nativeFilename);

    env->ReleaseStringUTFChars (filename, nativeFilename);
  }
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_load
* Signature: (JLjava/lang/String;)J
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1load
(JNIEnv * env, jobject, jlong cptr, jstring filename)
{
  if (cptr && filename)
  {
    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    Settings * settings = (Settings *)cptr;

    settings->load (nativeFilename);

    env->ReleaseStringUTFChars (filename, nativeFilename);
  }
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setDomains
* Signature: (JLjava/lang/String;)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setDomains (JNIEnv *env, jobject obj, jlong cptr, jstring domains)
{
  const char * nativeDomains = env->GetStringUTFChars (domains, 0);

  Settings * settings = (Settings *) cptr;

  if (settings)
  {
    settings->domains = nativeDomains;
  }

  env->ReleaseStringUTFChars (domains, nativeDomains);
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getDomains
* Signature: (J)Ljava/lang/String;
*/
jstring JNICALL Java_com_madara_transport_TransportSesttings_jni_1getDomains (JNIEnv * env, jobject obj, jlong cptr)
{
  jstring result;
  Settings * settings = (Settings *) cptr;

  if (settings)
  {
    result = env->NewStringUTF (settings->domains.c_str ());
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
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setQueueLength (JNIEnv * env, jobject obj, jlong cptr, jint queueLength)
{
  Settings * settings = (Settings *) cptr;

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
jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getQueueLength (JNIEnv * env, jobject obj, jlong cptr)
{
  jint result (0);
  Settings * settings = (Settings *) cptr;

  if (settings)
  {
    result = (jint) settings->queue_length;
  }

  return result;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setDeadline
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setDeadline (JNIEnv * env, jobject obj, jlong cptr, jint deadline)
{
  Settings * settings = (Settings *) cptr;

  if (settings)
  {
    settings->deadline = deadline;
  }
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getDeadline
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getDeadline (JNIEnv * env, jobject obj, jlong cptr)
{
  jint result (0);
  Settings * settings = (Settings *) cptr;

  if (settings)
  {
    result = (jint) settings->deadline;
  }

  return result;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setType
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setType (JNIEnv * env, jobject obj, jlong cptr, jint type)
{
  Settings * settings = (Settings *) cptr;

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
jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getType (JNIEnv *env, jobject obj, jlong cptr)
{
  jint result (0);
  Settings * settings = (Settings *) cptr;

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
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setReliability (JNIEnv * env, jobject obj, jlong cptr, jint reliability)
{
  Settings * settings = (Settings *) cptr;

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
jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getReliability (JNIEnv * env, jobject obj, jlong cptr)
{
  jint result (0);
  Settings * settings = (Settings *) cptr;

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
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setId (JNIEnv *env, jobject obj, jlong cptr, jint id)
{
  Settings * settings = (Settings *) cptr;

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
jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getId (JNIEnv *env, jobject obj, jlong cptr)
{
  jint result (0);
  Settings * settings = (Settings *) cptr;

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
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setProcesses (JNIEnv *env, jobject obj, jlong cptr, jint processes)
{
  Settings * settings = (Settings *) cptr;

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
jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getProcesses (JNIEnv *env, jobject obj, jlong cptr)
{
  jint result (0);
  Settings * settings = (Settings *) cptr;

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
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setOnDataReceivedLogic (JNIEnv *env, jobject obj, jlong cptr, jstring onDataReceivedLogic)
{
  const char * nativeOnDataReceivedLogic = env->GetStringUTFChars (
    onDataReceivedLogic, 0);
  Settings * settings = (Settings *) cptr;

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
jstring JNICALL Java_com_madara_transport_TransportSettings_jni_1getOnDataReceivedLogic (JNIEnv *env, jobject obj, jlong cptr)
{
  jstring result;
  Settings * settings = (Settings *) cptr;

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
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setHosts (JNIEnv *env, jobject obj, jlong cptr, jobjectArray hosts)
{
  int hostsLen = env->GetArrayLength (hosts);
  Settings * settings = (Settings *) cptr;

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
jobjectArray JNICALL Java_com_madara_transport_TransportSettings_jni_1getHosts (JNIEnv *env, jobject obj, jlong cptr)
{
  jobjectArray result (0);
  Settings * settings = (Settings *) cptr;

  if (settings)
  {
    result = env->NewObjectArray (
      (jsize)settings->hosts.size (), env->FindClass ("java/lang/String"),
      env->NewStringUTF (""));

    for (unsigned int x = 0; x < settings->hosts.size (); x++)
    {
      env->SetObjectArrayElement (
        result, x, env->NewStringUTF (settings->hosts[x].c_str ()));
    }
  }
  return result;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_freeSettings
* Signature: (J)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1freeSettings (JNIEnv * env, jclass cls, jlong cptr)
{
  Settings * settings = (Settings *) cptr;

  delete settings;
}


