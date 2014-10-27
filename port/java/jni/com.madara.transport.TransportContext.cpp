/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "com.madara.transport.TransportContext.h"
#include "madara/transport/Transport_Context.h"

#include <string>

/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_addRecord
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportContext_jni_1addRecord
  (JNIEnv * env, jclass cls, jlong cptr, jstring name, jlong record)
{
  Madara::Transport::Transport_Context* context = (Madara::Transport::Transport_Context*)cptr;
  Madara::Knowledge_Record* rec = (Madara::Knowledge_Record*)record;
  const char *nativeName = env->GetStringUTFChars(name, 0);

  context->add_record(std::string(nativeName), *rec);

  env->ReleaseStringUTFChars(name, nativeName);
}

/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_clearRecords
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportContext_jni_1clearRecords
  (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Transport::Transport_Context* context = (Madara::Transport::Transport_Context*)cptr;
  context->clear_records();
}

/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getCurrentTime
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getCurrentTime
  (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Transport::Transport_Context* context = (Madara::Transport::Transport_Context*)cptr;
  return (jlong)context->get_current_time();
}

/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getDomain
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_transport_TransportContext_jni_1getDomain
  (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Transport::Transport_Context* context = (Madara::Transport::Transport_Context*)cptr;
  return env->NewStringUTF(context->get_domain().c_str());
}

/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getMessageTime
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getMessageTime
  (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Transport::Transport_Context* context = (Madara::Transport::Transport_Context*)cptr;
  return context->get_message_time();
}

/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getOperation
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getOperation
  (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Transport::Transport_Context* context = (Madara::Transport::Transport_Context*)cptr;
  return context->get_operation();
}

/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getOriginator
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_transport_TransportContext_jni_1getOriginator
  (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Transport::Transport_Context* context = (Madara::Transport::Transport_Context*)cptr;
  return env->NewStringUTF(context->get_originator().c_str());
}

/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getReceiveBandwidth
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getReceiveBandwidth
  (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Transport::Transport_Context* context = (Madara::Transport::Transport_Context*)cptr;
  return context->get_receive_bandwidth();
}

/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getRecords
 * Signature: (JLcom/madara/transport/TransportContext/MapReturn;)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportContext_jni_1getRecords
  (JNIEnv * env, jclass cls, jlong cptr, jobject jniRet)
{
  //Stub for future use
}

/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getSendBandwidth
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getSendBandwidth
  (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Transport::Transport_Context* context = (Madara::Transport::Transport_Context*)cptr;
  return context->get_send_bandwidth();
}