/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _Included_com_madara_transport_TransportSettings
#define _Included_com_madara_transport_TransportSettings

#include <jni.h>
#include "madara/MADARA_export.h"

#ifdef __cplusplus
extern "C" {
#endif

//===================================================================================
//Settings
//===================================================================================

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_Settings
 * Signature: ()J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportSettings_jni_1Settings__
  (JNIEnv *, jobject);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_Settings
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_transport_TransportSettings_jni_1Settings__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_setDomains
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportSettings_jni_1setDomains
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_getDomains
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_transport_TransportSettings_jni_1getDomains
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_setQueueLength
 * Signature: (JI)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportSettings_jni_1setQueueLength
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_getQueueLength
 * Signature: (J)I
 */
MADARA_Export jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getQueueLength
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_setDeadline
 * Signature: (JI)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportSettings_jni_1setDeadline
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_getDeadline
 * Signature: (J)I
 */
MADARA_Export jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getDeadline
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_setType
 * Signature: (JI)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportSettings_jni_1setType
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_getType
 * Signature: (J)I
 */
MADARA_Export jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getType
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_setReliability
 * Signature: (JI)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportSettings_jni_1setReliability
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_getReliability
 * Signature: (J)I
 */
MADARA_Export jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getReliability
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_setId
 * Signature: (JI)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportSettings_jni_1setId
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_getId
 * Signature: (J)I
 */
MADARA_Export jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getId
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_setProcesses
 * Signature: (JI)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportSettings_jni_1setProcesses
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_getProcesses
 * Signature: (J)I
 */
MADARA_Export jint JNICALL Java_com_madara_transport_TransportSettings_jni_1getProcesses
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_setOnDataReceivedLogic
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportSettings_jni_1setOnDataReceivedLogic
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_getOnDataReceivedLogic
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_transport_TransportSettings_jni_1getOnDataReceivedLogic
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_setHosts
 * Signature: (J[Ljava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportSettings_jni_1setHosts
  (JNIEnv *, jobject, jlong, jobjectArray);

/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_getHosts
 * Signature: (J)[Ljava/lang/String;
 */
MADARA_Export jobjectArray JNICALL Java_com_madara_transport_TransportSettings_jni_1getHosts
  (JNIEnv *, jobject, jlong);
    
/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_getHosts
 * Signature: (J)[Ljava/lang/String;
 */
MADARA_Export jobjectArray JNICALL Java_com_madara_transport_TransportSettings_jni_1getHosts
  (JNIEnv *, jobject, jlong);
    
/*
 * Class:     com_madara_transport_TransportSettings
 * Method:    jni_freeSettings
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_transport_TransportSettings_jni_1freeSettings
  (JNIEnv *, jclass, jlong);
    
#ifdef __cplusplus
}
#endif
#endif
