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
*********************************************************************/

#include "com.madara.transport.QoSTransportSettings.h"
#include "madara/transport/QoS_Transport_Settings.h"
#include "madara/filters/java/Java_Buffer_Filter.h"
#include "madara/logger/Global_Logger.h"

#include <iostream>

// define useful shorthands
namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
namespace filters = madara::filters;
namespace logger = madara::logger;

typedef madara::Knowledge_Record  Knowledge_Record;
typedef Knowledge_Record::Integer Integer;
typedef transport::QoS_Transport_Settings QoS_Transport_Settings;
typedef filters::Java_Buffer_Filter  Java_Buffer_Filter;
typedef filters::Buffer_Filter Buffer_Filter;

/*
 * Class:   com_madara_transport_QoSTransportSettings
 * Method:  jni_QoSTransportSettings
 * Signature: ()J
 */
jlong JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1QoSTransportSettings__
  (JNIEnv *, jobject)
{
  return (jlong) new QoS_Transport_Settings ();
}

/*
 * Class:   com_madara_transport_QoSTransportSettings
 * Method:  jni_QoSTransportSettings
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1QoSTransportSettings__J
  (JNIEnv *, jobject, jlong old)
{
  jlong result (0);
  QoS_Transport_Settings * source = (QoS_Transport_Settings *) old;

  if (source)
  {
    result = (jlong) new QoS_Transport_Settings (*source);
  }

  return result;
}

/*
 * Class:   com_madara_transport_QoSTransportSettings
 * Method:  jni_freeQoSTransportSettings
 * Signature: (J)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1freeQoSTransportSettings
  (JNIEnv * env, jclass cls, jlong cptr)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  delete settings;
}


/*
* Class:     com_madara_transport_QoSTransportSettings
* Method:    jni_addBufferFilter
* Signature: (JJ)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addBufferFilter
(JNIEnv *, jobject, jlong cptr, jlong filter)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;
  Buffer_Filter * buffer_filter = (Buffer_Filter *)filter;

  if (settings)
  {
    settings->add_filter (buffer_filter);
  }
}

/*
* Class:     com_madara_transport_QoSTransportSettings
* Method:    jni_addBufferFilterObj
* Signature: (JLcom/madara/filters/BufferFilter;)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addBufferFilterObj
(JNIEnv *, jobject, jlong cptr, jobject filter)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;
  Java_Buffer_Filter * buffer_filter = new Java_Buffer_Filter (
    *madara::logger::global_logger.get (), filter);

  if (settings)
  {
    settings->add_filter (buffer_filter);
  }
}

/*
* Class:     com_madara_transport_QoSTransportSettings
* Method:    jni_clearBufferFilters
* Signature: (J)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1clearBufferFilters
(JNIEnv *, jobject, jlong cptr)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::clear_buffer_filters: "
    "Clearing buffer filters\n");

  if (settings)
  {
    settings->clear_buffer_filters ();
  }
}

/*
* Class:     com_madara_transport_QoSTransportSettings
* Method:    jni_getNumberOfBufferFilters
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1getNumberOfBufferFilters
(JNIEnv *, jobject, jlong cptr)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;
  jint result (0);

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::get_number_of_buffer_filters: "
    "Retreiving number of buffer filters\n");

  if (settings)
  {
    result = (jint)settings->get_number_of_buffer_filters ();
  }

  return result;
}


void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1saveQoS
(JNIEnv * env, jobject, jlong cptr, jstring filename)
{
  if (cptr && filename)
  {
    madara_logger_ptr_log (
      logger::global_logger.get (),
      logger::LOG_MAJOR,
      "JNI:QoS_Transport_Settings::add: "
      "Saving settings to a file\n");

    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

    settings->save (nativeFilename);

    env->ReleaseStringUTFChars (filename, nativeFilename);
  }
}

void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1loadQoS
(JNIEnv * env, jobject, jlong cptr, jstring filename)
{
  if (cptr && filename)
  {
    madara_logger_ptr_log (
      logger::global_logger.get (),
      logger::LOG_MAJOR,
      "JNI:QoS_Transport_Settings::load: "
      "Loading settings from a file\n");

    const char * nativeFilename = env->GetStringUTFChars (filename, 0);
    QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

    settings->load (nativeFilename);

    env->ReleaseStringUTFChars (filename, nativeFilename);
  }
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addRebroadcastFilter
 * Signature: (JILcom/madara/transport/filters/RecordFilter;)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addRebroadcastRecordFilter__JILcom_madara_transport_filters_RecordFilter_2
  (JNIEnv *, jobject, jlong cptr, jint type, jobject filter)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::add: "
    "Adding Java record filter to rebroadcast queue\n");

  if (settings)
  {
    settings->add_rebroadcast_filter ( (uint32_t)type, filter);
  }
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addRebroadcastFilter
 * Signature: (JLcom/madara/transport/filters/AggregateFilter;)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addRebroadcastAggregateFilter__JLcom_madara_transport_filters_AggregateFilter_2
  (JNIEnv *, jobject, jlong cptr, jobject filter)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::add: "
    "Adding Java aggregate filter to rebroadcast queue\n");

  if (settings)
  {
    settings->add_rebroadcast_filter (filter);
  }
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addSendFilter
 * Signature: (JILcom/madara/transport/filters/RecordFilter;)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addSendRecordFilter__JILcom_madara_transport_filters_RecordFilter_2
  (JNIEnv *, jobject, jlong cptr, jint type, jobject filter)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::add: "
    "Adding Java record filter to send queue\n");

  if (settings)
  {
    settings->add_send_filter ( (uint32_t)type, filter);
  }
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addSendFilter
 * Signature: (JLcom/madara/transport/filters/AggregateFilter;)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addSendAggregateFilter__JLcom_madara_transport_filters_AggregateFilter_2
  (JNIEnv *, jobject, jlong cptr, jobject filter)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::add: "
    "Adding Java aggregate filter to send queue\n");

  if (settings)
  {
    settings->add_send_filter (filter);
  }
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addReceiveFilter
 * Signature: (JILcom/madara/transport/filters/RecordFilter;)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addReceiveRecordFilter__JILcom_madara_transport_filters_RecordFilter_2
  (JNIEnv *, jobject, jlong cptr, jint type, jobject filter)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::add: "
    "Adding Java record filter to receive queue\n");

  if (settings)
  {
    settings->add_receive_filter ( (uint32_t)type, filter);
  }
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addReceiveFilter
 * Signature: (JLcom/madara/transport/filters/AggregateFilter;)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addReceiveAggregateFilter__JLcom_madara_transport_filters_AggregateFilter_2
  (JNIEnv *, jobject, jlong cptr, jobject filter)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::add: "
    "Adding Java aggregate filter to receive queue\n");

  if (settings)
  {
    settings->add_receive_filter (filter);
  }
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_setRebroadcastTtl
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1setRebroadcastTtl
  (JNIEnv * env, jobject obj, jlong cptr, jint ttl)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::set_rebroadcast_ttl: "
    "Setting the rebroadcast time-to-live\n");

  if (settings)
  {
    settings->set_rebroadcast_ttl ( (unsigned char)ttl);
  }
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_getRebroadcastTtl
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1getRebroadcastTtl
  (JNIEnv * env, jobject obj, jlong cptr)
{
  jint result (0);
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::get_rebroadcast_ttl: "
    "Getting the rebroadcast time-to-live\n");

  if (settings)
  {
    result = (jint) settings->get_rebroadcast_ttl ();
  }

  return result;
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_enableParticipantTtl
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1enableParticipantTtl
  (JNIEnv * env, jobject obj, jlong cptr, jint ttl)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::enable_participant_ttl: "
    "Modifying participation in rebroadcast operations\n");

  if (settings)
  {
    settings->enable_participant_ttl ( (unsigned char)ttl);
  }
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_getParticpantTtl
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1gettParticpantTtl (JNIEnv * env, jobject obj, jlong cptr)
{
  jint result (0);
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::get_participant_ttl: "
    "Retrieving participant time-to-live in other's rebroadcasts\n");

  if (settings)
  {
    result = (jint) settings->get_participant_ttl ();
  }

  return result;
}


/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_setSendBandwidthLimit
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1setSendBandwidthLimit (JNIEnv * env, jobject obj, jlong cptr, jint limit)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::set_send_bandwidth: "
    "Modifying send bandwidth limits\n");

  if (settings)
  {
    settings->set_send_bandwidth_limit (limit);
  }
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_getSendBandwidthLimit
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1getSendBandwidthLimit (JNIEnv * env, jobject obj, jlong cptr)
{
  jint result (0);
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  madara_logger_ptr_log (
    logger::global_logger.get (),
    logger::LOG_MAJOR,
    "JNI:QoS_Transport_Settings::get_send_bandwidth: "
    "Retrieving send bandwidth limits\n");

  if (settings)
  {
    result = (jint) settings->get_send_bandwidth_limit ();
  }

  return result;
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_setTotalBandwidthLimit
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1setTotalBandwidthLimit
  (JNIEnv * env, jobject obj, jlong cptr, jint limit)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  if (settings)
  {
    settings->set_total_bandwidth_limit (limit);
  }
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_getTotalBandwidthLimit
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1getTotalBandwidthLimit
  (JNIEnv * env, jobject obj, jlong cptr)
{
  jint result (0);
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  if (settings)
  {
    result = (jint) settings->get_total_bandwidth_limit ();
  }

  return result;
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_setDeadline
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1setDeadline
  (JNIEnv * env, jobject obj, jlong cptr, jint limit)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  if (settings)
  {
    settings->set_deadline (limit);
  }
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_getDeadline
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1getDeadline
  (JNIEnv * env, jobject obj, jlong cptr)
{
  jint result (0);
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  if (settings)
  {
    result = (jint) settings->get_deadline ();
  }

  return result;
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addTrustedPeer
 * Signature: (JLjava/lang/String;)V
 */
 void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addTrustedPeer
  (JNIEnv * env, jobject, jlong cptr, jstring jhost)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;
  const char * host = env->GetStringUTFChars (jhost, 0);

  if (settings)
  {
    settings->add_trusted_peer (host);
  }

  env->ReleaseStringUTFChars (jhost, host);
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addBannedPeer
 * Signature: (JLjava/lang/String;)V
 */
 void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addBannedPeer
  (JNIEnv * env, jobject, jlong cptr, jstring jhost)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;
  const char *host = env->GetStringUTFChars (jhost, 0);

  if (settings)
  {
    settings->add_banned_peer (host);
  }

  env->ReleaseStringUTFChars (jhost, host);
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_updateDropRate
 * Signature: (JDII)V
 */
 void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1updateDropRate
  (JNIEnv *, jobject, jlong cptr,
   jdouble drop_rate, jint drop_type, jint burstamount)
{
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)cptr;

  if (settings)
  {
    settings->update_drop_rate (drop_rate, (int)drop_type,
      (uint64_t)burstamount);
  }
}
