/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "com.madara.transport.TransportContext.h"
#include "madara/transport/TransportContext.h"

#include <string>


// define useful shorthands
namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
typedef madara::KnowledgeRecord  KnowledgeRecord;
typedef KnowledgeRecord::Integer Integer;
typedef transport::TransportContext TransportContext;


/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_addRecord
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportContext_jni_1addRecord
  (JNIEnv * env, jclass cls, jlong cptr, jstring name, jlong record)
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
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_clearRecords
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportContext_jni_1clearRecords
  (JNIEnv * env, jclass cls, jlong cptr)
{
  TransportContext * context = (TransportContext *) cptr;

  if (context)
    context->clear_records ();
}

/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getCurrentTime
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getCurrentTime
  (JNIEnv * env, jclass cls, jlong cptr)
{
  jlong result (0);
  TransportContext * context = (TransportContext *) cptr;

  if (context)
    result = context->get_current_time ();

  return result;
}

/*
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getDomain
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_transport_TransportContext_jni_1getDomain
  (JNIEnv * env, jclass cls, jlong cptr)
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
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getMessageTime
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getMessageTime
  (JNIEnv * env, jclass cls, jlong cptr)
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
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getOperation
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getOperation
  (JNIEnv * env, jclass cls, jlong cptr)
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
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getOriginator
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_transport_TransportContext_jni_1getOriginator
  (JNIEnv * env, jclass cls, jlong cptr)
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
 * Class:   com_madara_transport_TransportContext
 * Method:  jni_getReceiveBandwidth
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getReceiveBandwidth
  (JNIEnv * env, jclass cls, jlong cptr)
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
  jlong result (0);
  TransportContext * context = (TransportContext *) cptr;

  if (context)
  {
    result = context->get_send_bandwidth ();
  }

  return result;
}
