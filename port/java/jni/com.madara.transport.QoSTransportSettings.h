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

#ifndef _Included_com_madara_transport_QoSTransportSettings
#define _Included_com_madara_transport_QoSTransportSettings

#include <jni.h>
#include "madara/MADARA_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_QoSTransportSettings
 * Signature: ()J
 */
MADARA_Export jlong JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1QoSTransportSettings__
  (JNIEnv *, jobject);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_QoSTransportSettings
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1QoSTransportSettings__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_freeQoSTransportSettings
 * Signature: (J)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1freeQoSTransportSettings
  (JNIEnv *, jclass, jlong);

/*
* Class:     com_madara_transport_QoSTransportSettings
* Method:    jni_addBufferFilter
* Signature: (JJ)V
*/
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1addBufferFilter
(JNIEnv *, jobject, jlong, jlong);

/*
* Class:     com_madara_transport_QoSTransportSettings
* Method:    jni_addBufferFilterObj
* Signature: (JLcom/madara/filters/BufferFilter;)V
*/
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1addBufferFilterObj
(JNIEnv *, jobject, jlong, jobject);

/*
* Class:     com_madara_transport_QoSTransportSettings
* Method:    jni_clearBufferFilters
* Signature: (J)V
*/
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1clearBufferFilters
(JNIEnv *, jobject, jlong);

/*
* Class:     com_madara_transport_QoSTransportSettings
* Method:    jni_getNumberOfBufferFilters
* Signature: (J)I
*/
MADARA_Export jint JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1getNumberOfBufferFilters
(JNIEnv *, jobject, jlong);

/*
* Class:     com_madara_transport_QoSTransportSettings
* Method:    jni_saveQoS
* Signature: (JLjava/lang/String;)V
*/
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1saveQoS
(JNIEnv *, jobject, jlong, jstring);

/*
* Class:     com_madara_transport_QoSTransportSettings
* Method:    jni_loadQoS
* Signature: (JLjava/lang/String;)V
*/
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1loadQoS
(JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addRebroadcastFilter
 * Signature: (JILcom/madara/transport/filters/RecordFilter;)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1addRebroadcastRecordFilter__JILcom_madara_transport_filters_RecordFilter_2
  (JNIEnv *, jobject, jlong, jint, jobject);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addRebroadcastFilter
 * Signature: (JLcom/madara/transport/filters/AggregateFilter;)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1addRebroadcastAggregateFilter__JLcom_madara_transport_filters_AggregateFilter_2
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addSendFilter
 * Signature: (JILcom/madara/transport/filters/RecordFilter;)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1addSendRecordFilter__JILcom_madara_transport_filters_RecordFilter_2
  (JNIEnv *, jobject, jlong, jint, jobject);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addSendFilter
 * Signature: (JLcom/madara/transport/filters/AggregateFilter;)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1addSendAggregateFilter__JLcom_madara_transport_filters_AggregateFilter_2
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addReceiveFilter
 * Signature: (JILcom/madara/transport/filters/RecordFilter;)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1addReceiveRecordFilter__JILcom_madara_transport_filters_RecordFilter_2
  (JNIEnv *, jobject, jlong, jint, jobject);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addReceiveFilter
 * Signature: (JLcom/madara/transport/filters/AggregateFilter;)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1addReceiveAggregateFilter__JLcom_madara_transport_filters_AggregateFilter_2
  (JNIEnv *, jobject, jlong, jobject);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_setRebroadcastTtl
 * Signature: (JI)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1setRebroadcastTtl
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_getRebroadcastTtl
 * Signature: (J)I
 */
MADARA_Export jint JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1getRebroadcastTtl
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_enableParticipantTtl
 * Signature: (JI)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1enableParticipantTtl
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_getParticipantTtl
 * Signature: (J)I
 */
MADARA_Export jint JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1getParticipantTtl
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_setSendBandwidthLimit
 * Signature: (JI)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1setSendBandwidthLimit
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_getSendBandwidthLimit
 * Signature: (J)I
 */
MADARA_Export jint JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1getSendBandwidthLimit
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_setTotalBandwidthLimit
 * Signature: (JI)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1setTotalBandwidthLimit
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_getTotalBandwidthLimit
 * Signature: (J)I
 */
MADARA_Export jint JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1getTotalBandwidthLimit
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_setDeadline
 * Signature: (JI)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1setDeadline
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_getDeadline
 * Signature: (J)I
 */
MADARA_Export jint JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1getDeadline
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addTrustedPeer
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1addTrustedPeer
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addBannedPeer
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1addBannedPeer
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_updateDropRate
 * Signature: (JDII)V
 */
MADARA_Export void JNICALL
Java_com_madara_transport_QoSTransportSettings_jni_1updateDropRate
  (JNIEnv *, jobject, jlong, jdouble, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
