/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "ai_madara_transport_TransportContext.h"
#include "madara/transport/TransportContext.h"

#include <string>


// define useful shorthands
namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
typedef madara::knowledge::KnowledgeRecord  KnowledgeRecord;
typedef KnowledgeRecord::Integer Integer;
typedef transport::TransportContext TransportContext;


/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_addRecord
 * Signature: (JLjava/lang/String;J)V
 */
void JNICALL
Java_ai_madara_transport_TransportContext_jni_1addRecord
  (JNIEnv * env, jclass, jlong cptr, jstring name, jlong record)
{
  TransportContext * context = (TransportContext *) cptr;
  KnowledgeRecord * rec = (KnowledgeRecord *) record;
  const char * nativeName = env->GetStringUTFChars (name, 0);

  if (context && rec)
  {
    context->add_record (nativeName, *rec);
  }

  env->ReleaseStringUTFChars (name, nativeName);
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_clearRecords
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_transport_TransportContext_jni_1clearRecords
  (JNIEnv *, jclass, jlong cptr)
{
  TransportContext * context = (TransportContext *) cptr;

  if (context)
    context->clear_records ();
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getCurrentTime
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_transport_TransportContext_jni_1getCurrentTime
  (JNIEnv *, jclass, jlong cptr)
{
  jlong result (0);
  TransportContext * context = (TransportContext *) cptr;

  if (context)
    result = context->get_current_time ();

  return result;
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getDomain
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL
Java_ai_madara_transport_TransportContext_jni_1getDomain
  (JNIEnv * env, jclass, jlong cptr)
{
  jstring result;
  TransportContext * context = (TransportContext *) cptr;

  if (context)
  {
    result = env->NewStringUTF (context->get_domain ().c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getMessageTime
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_transport_TransportContext_jni_1getMessageTime
  (JNIEnv *, jclass, jlong cptr)
{
  jlong result (0);
  TransportContext * context = (TransportContext *) cptr;

  if (context)
  {
    result = context->get_message_time ();
  }

  return result;
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getOperation
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_transport_TransportContext_jni_1getOperation
  (JNIEnv *, jclass, jlong cptr)
{
  jlong result (0);
  TransportContext * context = (TransportContext *) cptr;

  if (context)
  {
    result = context->get_operation ();
  }

  return result;
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getOriginator
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL
Java_ai_madara_transport_TransportContext_jni_1getOriginator
  (JNIEnv * env, jclass, jlong cptr)
{
  jstring result;
  TransportContext * context = (TransportContext *) cptr;

  if (context)
  {
    result = env->NewStringUTF (context->get_originator ().c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getReceiveBandwidth
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_transport_TransportContext_jni_1getReceiveBandwidth
  (JNIEnv *, jclass, jlong cptr)
{
  jlong result (0);
  TransportContext * context = (TransportContext *) cptr;

  if (context)
  {
    result = context->get_receive_bandwidth ();
  }

  return result;
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getRecords
 * Signature: (JLcom/madara/transport/TransportContext/MapReturn;)V
 */
void JNICALL
Java_ai_madara_transport_TransportContext_jni_1getRecords
  (JNIEnv *, jclass, jlong, jobject)
{
  //Stub for future use
}

/*
 * Class:   ai_madara_transport_TransportContext
 * Method:  jni_getSendBandwidth
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_transport_TransportContext_jni_1getSendBandwidth
  (JNIEnv *, jclass, jlong cptr)
{
  jlong result (0);
  TransportContext * context = (TransportContext *) cptr;

  if (context)
  {
    result = context->get_send_bandwidth ();
  }

  return result;
}
