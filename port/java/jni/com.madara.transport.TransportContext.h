/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _Included_com_madara_transport_TransportContext
#define _Included_com_madara_transport_TransportContext

#include "madara/MADARA_export.h"
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_madara_transport_TransportContext
 * Method:    jni_addRecord
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportContext_jni_1addRecord
  (JNIEnv *, jclass, jlong, jstring, jlong);

/*
 * Class:     com_madara_transport_TransportContext
 * Method:    jni_clearRecords
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportContext_jni_1clearRecords
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_transport_TransportContext
 * Method:    jni_getCurrentTime
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getCurrentTime
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_transport_TransportContext
 * Method:    jni_getDomain
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_transport_TransportContext_jni_1getDomain
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_transport_TransportContext
 * Method:    jni_getMessageTime
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getMessageTime
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_transport_TransportContext
 * Method:    jni_getOperation
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getOperation
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_transport_TransportContext
 * Method:    jni_getOriginator
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_transport_TransportContext_jni_1getOriginator
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_transport_TransportContext
 * Method:    jni_getReceiveBandwidth
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getReceiveBandwidth
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_transport_TransportContext
 * Method:    jni_getRecords
 * Signature: (JLcom/madara/transport/TransportContext/MapReturn;)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportContext_jni_1getRecords
  (JNIEnv *, jclass, jlong, jobject);

/*
 * Class:     com_madara_transport_TransportContext
 * Method:    jni_getSendBandwidth
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportContext_jni_1getSendBandwidth
  (JNIEnv *, jclass, jlong);

#ifdef __cplusplus
}
#endif
#endif
