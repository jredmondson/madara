/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "ai_madara_transport_TransportContext.h"
#include "madara/transport/TransportContext.h"
#include "madara_jni.h"

#include <string>

// define useful shorthands
namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
typedef madara::knowledge::KnowledgeRecord KnowledgeRecord;
typedef KnowledgeRecord::Integer Integer;
typedef transport::TransportContext TransportContext;

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_addRecord
 * Signature: (JLjava/lang/String;J)V
 */
void JNICALL Java_ai_madara_transport_TransportContext_jni_1addRecord(
    JNIEnv* env, jclass, jlong cptr, jstring name, jlong record)
{
  TransportContext* context = (TransportContext*)cptr;
  KnowledgeRecord* rec = (KnowledgeRecord*)record;
  const char* nativeName = env->GetStringUTFChars(name, 0);

  if(context && rec)
  {
    context->add_record(nativeName, *rec);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "TransportContext::addRecord: "
        "TransportContext or record objects are released already");
  }

  env->ReleaseStringUTFChars(name, nativeName);
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_clearRecords
 * Signature: (J)V
 */
void JNICALL Java_ai_madara_transport_TransportContext_jni_1clearRecords(
    JNIEnv* env, jclass, jlong cptr)
{
  TransportContext* context = (TransportContext*)cptr;

  if(context)
    context->clear_records();
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "TransportContext::clearRecords: "
        "TransportContext object is released already");
  }
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getCurrentTime
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_transport_TransportContext_jni_1getCurrentTime(
    JNIEnv* env, jclass, jlong cptr)
{
  jlong result(0);
  TransportContext* context = (TransportContext*)cptr;

  if(context)
    result = context->get_current_time();
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "TransportContext::getCurrentTime: "
        "TransportContext object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getDomain
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_ai_madara_transport_TransportContext_jni_1getDomain(
    JNIEnv* env, jclass, jlong cptr)
{
  jstring result = 0;
  TransportContext* context = (TransportContext*)cptr;

  if(context)
  {
    result = env->NewStringUTF(context->get_domain().c_str());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "TransportContext::getDomain: "
        "TransportContext object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getMessageTime
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_transport_TransportContext_jni_1getMessageTime(
    JNIEnv* env, jclass, jlong cptr)
{
  jlong result(0);
  TransportContext* context = (TransportContext*)cptr;

  if(context)
  {
    result = context->get_message_time();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "TransportContext::getMessageTime: "
        "TransportContext object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getOperation
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_transport_TransportContext_jni_1getOperation(
    JNIEnv* env, jclass, jlong cptr)
{
  jlong result(0);
  TransportContext* context = (TransportContext*)cptr;

  if(context)
  {
    result = context->get_operation();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "TransportContext::getOperation: "
        "TransportContext object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getOriginator
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_ai_madara_transport_TransportContext_jni_1getOriginator(
    JNIEnv* env, jclass, jlong cptr)
{
  jstring result = 0;
  TransportContext* context = (TransportContext*)cptr;

  if(context)
  {
    result = env->NewStringUTF(context->get_originator().c_str());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "TransportContext::getOriginator: "
        "TransportContext object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getReceiveBandwidth
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_transport_TransportContext_jni_1getReceiveBandwidth(
    JNIEnv* env, jclass, jlong cptr)
{
  jlong result(0);
  TransportContext* context = (TransportContext*)cptr;

  if(context)
  {
    result = context->get_receive_bandwidth();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "TransportContext::getReceiveBandwidth: "
        "TransportContext object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getRecords
 * Signature: (JLai/madara/transport/TransportContext/MapReturn;)V
 */
void JNICALL Java_ai_madara_transport_TransportContext_jni_1getRecords(
    JNIEnv*, jclass, jlong, jobject)
{
  // Stub for future use
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getSendBandwidth
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_transport_TransportContext_jni_1getSendBandwidth(
    JNIEnv* env, jclass, jlong cptr)
{
  jlong result(0);
  TransportContext* context = (TransportContext*)cptr;

  if(context)
  {
    result = context->get_send_bandwidth();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "TransportContext::getSendBandwidth: "
        "TransportContext object is released already");
  }

  return result;
}
