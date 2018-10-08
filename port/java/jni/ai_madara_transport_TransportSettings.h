/*********************************************************************
 * Copyright (c) 2013-2015 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following acknowledgments and disclaimers.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The names "Carnegie Mellon University," "SEI" and/or
 * "Software Engineering Institute" shall not be used to endorse or promote
 * products derived from this software without prior written permission. For
 * written permission, please contact permission@sei.cmu.edu.
 *
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 * appear in their names without prior written permission of
 * permission@sei.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 * acknowledgment:
 *
 * This material is based upon work funded and supported by the Department of
 * Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon University
 * for the operation of the Software Engineering Institute, a federally funded
 * research and development center. Any opinions, findings and conclusions or
 * recommendations expressed in this material are those of the author(s) and
 * do not necessarily reflect the views of the United States Department of
 * Defense.
 *
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
 * AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR
 * PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
 * MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
 * WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 *
 * This material has been approved for public release and unlimited
 * distribution.
 *
 * @author James Edmondson <jedmondson@gmail.com>
 * @author James Root <james.root@gmail.com>
 *********************************************************************/

#ifndef _Included_ai_madara_transport_TransportSettings
#define _Included_ai_madara_transport_TransportSettings

#include <jni.h>
#include "madara/MadaraExport.h"

#ifdef __cplusplus
extern "C" {
#endif

//===================================================================================
// Settings
//===================================================================================

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_Settings
 * Signature: ()J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_transport_TransportSettings_jni_1Settings__(JNIEnv*, jobject);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_Settings
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_transport_TransportSettings_jni_1Settings__J(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_save
 * Signature: (JLjava/lang/String;)J
 */
MADARA_EXPORT void JNICALL Java_ai_madara_transport_TransportSettings_jni_1save(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_load
 * Signature: (JLjava/lang/String;)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_transport_TransportSettings_jni_1load(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_setDomains
 * Signature: (JLjava/lang/String;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportSettings_jni_1setWriteDomain(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_getDomains
 * Signature: (J)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_transport_TransportSettings_jni_1getWriteDomain(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_setQueueLength
 * Signature: (JI)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportSettings_jni_1setQueueLength(
    JNIEnv*, jobject, jlong, jint);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_getQueueLength
 * Signature: (J)I
 */
MADARA_EXPORT jint JNICALL
Java_ai_madara_transport_TransportSettings_jni_1getQueueLength(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_setResendAttempts
 * Signature: (JI)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportSettings_jni_1setResendAttempts(
    JNIEnv*, jobject, jlong, jint);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_getResendAttempts
 * Signature: (J)I
 */
MADARA_EXPORT jint JNICALL
Java_ai_madara_transport_TransportSettings_jni_1getResendAttempts(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_setType
 * Signature: (JI)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportSettings_jni_1setType(
    JNIEnv*, jobject, jlong, jint);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_getType
 * Signature: (J)I
 */
MADARA_EXPORT jint JNICALL
Java_ai_madara_transport_TransportSettings_jni_1getType(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_setReliability
 * Signature: (JI)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportSettings_jni_1setReliability(
    JNIEnv*, jobject, jlong, jint);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_getReliability
 * Signature: (J)I
 */
MADARA_EXPORT jint JNICALL
Java_ai_madara_transport_TransportSettings_jni_1getReliability(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_setId
 * Signature: (JI)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportSettings_jni_1setId(
    JNIEnv*, jobject, jlong, jint);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_getId
 * Signature: (J)I
 */
MADARA_EXPORT jint JNICALL
Java_ai_madara_transport_TransportSettings_jni_1getId(JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_setProcesses
 * Signature: (JI)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportSettings_jni_1setProcesses(
    JNIEnv*, jobject, jlong, jint);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_getProcesses
 * Signature: (J)I
 */
MADARA_EXPORT jint JNICALL
Java_ai_madara_transport_TransportSettings_jni_1getProcesses(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_setOnDataReceivedLogic
 * Signature: (JLjava/lang/String;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportSettings_jni_1setOnDataReceivedLogic(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_getOnDataReceivedLogic
 * Signature: (J)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_transport_TransportSettings_jni_1getOnDataReceivedLogic(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_setHosts
 * Signature: (J[Ljava/lang/String;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportSettings_jni_1setHosts(
    JNIEnv*, jobject, jlong, jobjectArray);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_getHosts
 * Signature: (J)[Ljava/lang/String;
 */
MADARA_EXPORT jobjectArray JNICALL
Java_ai_madara_transport_TransportSettings_jni_1getHosts(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_getHosts
 * Signature: (J)[Ljava/lang/String;
 */
MADARA_EXPORT jobjectArray JNICALL
Java_ai_madara_transport_TransportSettings_jni_1getHosts(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_transport_TransportSettings
 * Method:    jni_freeSettings
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_transport_TransportSettings_jni_1freeSettings(
    JNIEnv*, jclass, jlong);

#ifdef __cplusplus
}
#endif
#endif
