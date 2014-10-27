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

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_Settings
* Signature: ()J
*/
jlong JNICALL Java_com_madara_transport_TransportSettings_jni_1Settings__(JNIEnv *env, jobject obj)
{
  return (jlong) new Madara::Transport::Settings();
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_Settings
* Signature: (J)J
*/
jlong JNICALL Java_com_madara_transport_TransportSettings_jni_1Settings__J (JNIEnv *env, jobject obj, jlong cptr)
{
  return (jlong) new Madara::Transport::Settings(*(Madara::Transport::Settings*)cptr);
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setDomains
* Signature: (JLjava/lang/String;)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setDomains (JNIEnv *env, jobject obj, jlong cptr, jstring domains)
{
  const char *nativeDomains = env->GetStringUTFChars(domains, 0);

  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  settings->domains = std::string(nativeDomains);

  env->ReleaseStringUTFChars(domains, nativeDomains);
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getDomains
* Signature: (J)Ljava/lang/String;
*/
jstring JNICALL Java_com_madara_transport_TransportSettings_jni_1getDomains (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  return (jstring) env->NewStringUTF(settings->domains.c_str());
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setQueueLength
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setQueueLength (JNIEnv * env, jobject obj, jlong cptr, jint queueLength)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  settings->queue_length = queueLength;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getQueueLength
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getQueueLength (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  return (jint) settings->queue_length;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setDeadline
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setDeadline (JNIEnv * env, jobject obj, jlong cptr, jint deadline)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  settings->deadline = deadline;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getDeadline
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getDeadline (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  return (jint) settings->deadline;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setType
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setType (JNIEnv * env, jobject obj, jlong cptr, jint type)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  settings->type = type;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getType
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getType (JNIEnv *env, jobject obj, jlong cptr)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  return (jint) settings->type;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setReliability
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setReliability (JNIEnv * env, jobject obj, jlong cptr, jint reliability)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  settings->reliability = reliability;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getReliability
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getReliability (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  return (jint) settings->reliability;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setId
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setId (JNIEnv *env, jobject obj, jlong cptr, jint id)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  settings->id = id;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getId
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getId (JNIEnv *env, jobject obj, jlong cptr)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  return (jint) settings->id;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setProcesses
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setProcesses (JNIEnv *env, jobject obj, jlong cptr, jint processes)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  settings->processes = processes;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getProcesses
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getProcesses (JNIEnv *env, jobject obj, jlong cptr)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  return (jint) settings->processes;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setOnDataReceivedLogic
* Signature: (JLjava/lang/String;)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setOnDataReceivedLogic (JNIEnv *env, jobject obj, jlong cptr, jstring onDataReceivedLogic)
{
  const char *nativeOnDataReceivedLogic = env->GetStringUTFChars(onDataReceivedLogic, 0);

  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  settings->on_data_received_logic = std::string(nativeOnDataReceivedLogic);

  env->ReleaseStringUTFChars(onDataReceivedLogic, nativeOnDataReceivedLogic);
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getOnDataReceivedLogic
* Signature: (J)Ljava/lang/String;
*/
jstring JNICALL Java_com_madara_transport_TransportSettings_jni_1getOnDataReceivedLogic (JNIEnv *env, jobject obj, jlong cptr)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  return (jstring) env->NewStringUTF(settings->on_data_received_logic.c_str());
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_setHosts
* Signature: (J[Ljava/lang/String;)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1setHosts (JNIEnv *env, jobject obj, jlong cptr, jobjectArray hosts)
{
  int hostsLen = env->GetArrayLength(hosts);

  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;

  settings->hosts.resize(hostsLen);

  for (int x = 0; x < hostsLen; x++)
  {
    jstring jhost = (jstring) env->GetObjectArrayElement(hosts, x);
    const char* curHost = env->GetStringUTFChars(jhost, 0);
    settings->hosts[x] = std::string(curHost);
    env->ReleaseStringUTFChars(jhost, curHost);
  }
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_getHosts
* Signature: (J)[Ljava/lang/String;
*/
jobjectArray JNICALL Java_com_madara_transport_TransportSettings_jni_1getHosts (JNIEnv *env, jobject obj, jlong cptr)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;

  jobjectArray ret = (jobjectArray) env->NewObjectArray(
    (jsize)settings->hosts.size(), env->FindClass("java/lang/String"),
    env->NewStringUTF(""));

  for (unsigned int x = 0; x < settings->hosts.size(); x++)
  {
    env->SetObjectArrayElement(ret, x, env->NewStringUTF(settings->hosts[x].c_str()));
  }

  return ret;
}

/*
* Class:     com_madara_transport_TransportSettings
* Method:    jni_freeSettings
* Signature: (J)V
*/
void JNICALL Java_com_madara_transport_TransportSettings_jni_1freeSettings (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Transport::Settings* settings = (Madara::Transport::Settings*)cptr;
  if (settings)
    delete settings;
}


