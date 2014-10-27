/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include "madara/MADARA_export.h"
/* Header for class com_madara_transport_filters_Packet */

#ifndef _Included_com_madara_transport_filters_Packet
#define _Included_com_madara_transport_filters_Packet
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_get
 * Signature: (JLjava/lang/String;)J
 */
MADARA_Export  jlong JNICALL Java_com_madara_transport_filters_Packet_jni_1get
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_set
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_Export  void JNICALL Java_com_madara_transport_filters_Packet_jni_1set
  (JNIEnv *, jobject, jlong, jstring, jlong);

/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_get_keys
 * Signature: (J)[Ljava/lang/String;
 */
MADARA_Export  jobjectArray JNICALL Java_com_madara_transport_filters_Packet_jni_1get_1keys
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_exists
 * Signature: (JLjava/lang/String;)Z
 */
MADARA_Export  jboolean JNICALL Java_com_madara_transport_filters_Packet_jni_1exists
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_clear
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_filters_Packet_jni_1clear
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_erase
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_filters_Packet_jni_1erase
  (JNIEnv *, jobject, jlong, jstring);

#ifdef __cplusplus
}
#endif
#endif
