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

#include "ai_madara_transport_QoSTransportSettings.h"
#include "madara/transport/QoSTransportSettings.h"
#include "madara/filters/java/JavaBufferFilter.h"
#include "madara/logger/GlobalLogger.h"

#include <iostream>

// define useful shorthands
namespace knowledge = madara::knowledge;
namespace transport = madara::transport;
namespace filters = madara::filters;
namespace logger = madara::logger;

typedef madara::knowledge::KnowledgeRecord KnowledgeRecord;
typedef knowledge::KnowledgeRecord::Integer Integer;
typedef transport::QoSTransportSettings QoSTransportSettings;
typedef filters::JavaBufferFilter JavaBufferFilter;
typedef filters::BufferFilter BufferFilter;

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_QoSTransportSettings
 * Signature: ()J
 */
jlong JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1QoSTransportSettings__(
    JNIEnv*, jobject)
{
  return (jlong) new QoSTransportSettings();
}

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_QoSTransportSettings
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1QoSTransportSettings__J(
    JNIEnv* env, jobject, jlong old)
{
  jlong result(0);
  QoSTransportSettings* source = (QoSTransportSettings*)old;

  if (source)
  {
    result = (jlong) new QoSTransportSettings(*source);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::copyConstructor: "
        "QoSTransportSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_freeQoSTransportSettings
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1freeQoSTransportSettings(
    JNIEnv*, jclass, jlong cptr)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  delete settings;
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_addBufferFilter
 * Signature: (JJ)V
 */
void JNICALL Java_ai_madara_transport_QoSTransportSettings_jni_1addBufferFilter(
    JNIEnv* env, jobject, jlong cptr, jlong filter)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;
  BufferFilter* buffer_filter = (BufferFilter*)filter;

  if (settings && buffer_filter)
  {
    settings->add_filter(buffer_filter);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::addBufferFilter: "
        "QoSTransportSettings or BufferFilter objects are released already");
  }
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_addBufferFilterObj
 * Signature: (JLai/madara/filters/BufferFilter;)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1addBufferFilterObj(
    JNIEnv* env, jobject, jlong cptr, jobject filter)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;
  JavaBufferFilter* buffer_filter =
      new JavaBufferFilter(*madara::logger::global_logger.get(), filter);

  if (settings && buffer_filter)
  {
    settings->add_filter(buffer_filter);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::addBufferFilter: "
        "QoSTransportSettings or BufferFilter objects are released already");
  }
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_clearBufferFilters
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1clearBufferFilters(
    JNIEnv* env, jobject, jlong cptr)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::clear_buffer_filters: "
      "Clearing buffer filters\n");

  if (settings)
  {
    settings->clear_buffer_filters();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::clearBufferFilters: "
        "QoSTransportSettings object is released already");
  }
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_getNumberOfBufferFilters
 * Signature: (J)I
 */
jint JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1getNumberOfBufferFilters(
    JNIEnv* env, jobject, jlong cptr)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;
  jint result(0);

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::get_number_of_buffer_filters: "
      "Retreiving number of buffer filters\n");

  if (settings)
  {
    result = (jint)settings->get_number_of_buffer_filters();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::getNumberOfBufferFilters: "
        "QoSTransportSettings object is released already");
  }

  return result;
}

void JNICALL Java_ai_madara_transport_QoSTransportSettings_jni_1saveQoS(
    JNIEnv* env, jobject, jlong cptr, jstring filename)
{
  if (cptr && filename)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "JNI:QoSTransportSettings::add: "
        "Saving settings to a file\n");

    const char* nativeFilename = env->GetStringUTFChars(filename, 0);
    QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

    settings->save(nativeFilename);

    env->ReleaseStringUTFChars(filename, nativeFilename);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::saveQoS: "
        "QoSTransportSettings or filename object is released already");
  }
}

void JNICALL Java_ai_madara_transport_QoSTransportSettings_jni_1loadQoS(
    JNIEnv* env, jobject, jlong cptr, jstring filename)
{
  if (cptr && filename)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
        "JNI:QoSTransportSettings::load: "
        "Loading settings from a file\n");

    const char* nativeFilename = env->GetStringUTFChars(filename, 0);
    QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

    settings->load(nativeFilename);

    env->ReleaseStringUTFChars(filename, nativeFilename);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::loadQoS: "
        "QoSTransportSettings object is released already");
  }
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_addRebroadcastFilter
 * Signature: (JILai/madara/transport/filters/RecordFilter;)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1addRebroadcastRecordFilter__JILai_madara_transport_filters_RecordFilter_2(
    JNIEnv* env, jobject, jlong cptr, jint type, jobject filter)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::add: "
      "Adding Java record filter to rebroadcast queue\n");

  if (settings && filter)
  {
    settings->add_rebroadcast_filter((uint32_t)type, filter);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::addRebroadcastFilter: "
        "QoSTransportSettings or filter objects are released already");
  }
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_addRebroadcastFilter
 * Signature: (JLai/madara/transport/filters/AggregateFilter;)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1addRebroadcastAggregateFilter__JLai_madara_transport_filters_AggregateFilter_2(
    JNIEnv* env, jobject, jlong cptr, jobject filter)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::add: "
      "Adding Java aggregate filter to rebroadcast queue\n");

  if (settings && filter)
  {
    settings->add_rebroadcast_filter(filter);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::addRebroadcastAggregateFilter: "
        "QoSTransportSettings or filter objects are released already");
  }
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_addSendFilter
 * Signature: (JILai/madara/transport/filters/RecordFilter;)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1addSendRecordFilter__JILai_madara_transport_filters_RecordFilter_2(
    JNIEnv* env, jobject, jlong cptr, jint type, jobject filter)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::add: "
      "Adding Java record filter to send queue\n");

  if (settings && filter)
  {
    settings->add_send_filter((uint32_t)type, filter);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::addSendRecordFilter: "
        "QoSTransportSettings or filter objects are released already");
  }
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_addSendFilter
 * Signature: (JLai/madara/transport/filters/AggregateFilter;)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1addSendAggregateFilter__JLai_madara_transport_filters_AggregateFilter_2(
    JNIEnv* env, jobject, jlong cptr, jobject filter)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::add: "
      "Adding Java aggregate filter to send queue\n");

  if (settings && filter)
  {
    settings->add_send_filter(filter);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::addSendAggregateRecordFilter: "
        "QoSTransportSettings or filter objects are released already");
  }
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_addReceiveFilter
 * Signature: (JILai/madara/transport/filters/RecordFilter;)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1addReceiveRecordFilter__JILai_madara_transport_filters_RecordFilter_2(
    JNIEnv* env, jobject, jlong cptr, jint type, jobject filter)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::add: "
      "Adding Java record filter to receive queue\n");

  if (settings && filter)
  {
    settings->add_receive_filter((uint32_t)type, filter);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::addReceiveRecordFilter: "
        "QoSTransportSettings or filter objects are released already");
  }
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_addReceiveFilter
 * Signature: (JLai/madara/transport/filters/AggregateFilter;)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1addReceiveAggregateFilter__JLai_madara_transport_filters_AggregateFilter_2(
    JNIEnv* env, jobject, jlong cptr, jobject filter)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::add: "
      "Adding Java aggregate filter to receive queue\n");

  if (settings && filter)
  {
    settings->add_receive_filter(filter);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::addReceiveAggregateFilter: "
        "QoSTransportSettings or filter objects are released already");
  }
}

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_setRebroadcastTtl
 * Signature: (JI)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1setRebroadcastTtl(
    JNIEnv* env, jobject, jlong cptr, jint ttl)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::set_rebroadcast_ttl: "
      "Setting the rebroadcast time-to-live\n");

  if (settings)
  {
    settings->set_rebroadcast_ttl((unsigned char)ttl);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::addReceiveAggregateFilter: "
        "QoSTransportSettings object is released already");
  }
}

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_getRebroadcastTtl
 * Signature: (J)I
 */
jint JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1getRebroadcastTtl(
    JNIEnv* env, jobject, jlong cptr)
{
  jint result(0);
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::get_rebroadcast_ttl: "
      "Getting the rebroadcast time-to-live\n");

  if (settings)
  {
    result = (jint)settings->get_rebroadcast_ttl();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::getRebroadcastTtl: "
        "QoSTransportSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_enableParticipantTtl
 * Signature: (JI)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1enableParticipantTtl(
    JNIEnv* env, jobject, jlong cptr, jint ttl)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::enable_participant_ttl: "
      "Modifying participation in rebroadcast operations\n");

  if (settings)
  {
    settings->enable_participant_ttl((unsigned char)ttl);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::enableParticipantTtl: "
        "QoSTransportSettings object is released already");
  }
}

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_getParticpantTtl
 * Signature: (J)I
 */
jint JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1gettParticpantTtl(
    JNIEnv* env, jobject, jlong cptr)
{
  jint result(0);
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::get_participant_ttl: "
      "Retrieving participant time-to-live in other's rebroadcasts\n");

  if (settings)
  {
    result = (jint)settings->get_participant_ttl();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::getParticipantTtl: "
        "QoSTransportSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_setSendBandwidthLimit
 * Signature: (JI)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1setSendBandwidthLimit(
    JNIEnv* env, jobject, jlong cptr, jint limit)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::set_send_bandwidth: "
      "Modifying send bandwidth limits\n");

  if (settings)
  {
    settings->set_send_bandwidth_limit(limit);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::setSendBandwidthLimit: "
        "QoSTransportSettings object is released already");
  }
}

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_getSendBandwidthLimit
 * Signature: (J)I
 */
jint JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1getSendBandwidthLimit(
    JNIEnv* env, jobject, jlong cptr)
{
  jint result(0);
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_MAJOR,
      "JNI:QoSTransportSettings::get_send_bandwidth: "
      "Retrieving send bandwidth limits\n");

  if (settings)
  {
    result = (jint)settings->get_send_bandwidth_limit();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::getSendBandwidthLimit: "
        "QoSTransportSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_setTotalBandwidthLimit
 * Signature: (JI)V
 */
void JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1setTotalBandwidthLimit(
    JNIEnv* env, jobject, jlong cptr, jint limit)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  if (settings)
  {
    settings->set_total_bandwidth_limit(limit);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::setTotalBandwidthLimit: "
        "QoSTransportSettings object is released already");
  }
}

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_getTotalBandwidthLimit
 * Signature: (J)I
 */
jint JNICALL
Java_ai_madara_transport_QoSTransportSettings_jni_1getTotalBandwidthLimit(
    JNIEnv* env, jobject, jlong cptr)
{
  jint result(0);
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  if (settings)
  {
    result = (jint)settings->get_total_bandwidth_limit();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::getTotalBandwidthLimit: "
        "QoSTransportSettings object is released already");
  }

  return result;
}

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_setDeadline
 * Signature: (JI)V
 */
void JNICALL Java_ai_madara_transport_QoSTransportSettings_jni_1setDeadline(
    JNIEnv* env, jobject, jlong cptr, jint limit)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  if (settings)
  {
    settings->set_deadline(limit);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::setDeadline: "
        "QoSTransportSettings object is released already");
  }
}

/*
 * Class:   ai_madara_transport_QoSTransportSettings
 * Method:  jni_getDeadline
 * Signature: (J)I
 */
jint JNICALL Java_ai_madara_transport_QoSTransportSettings_jni_1getDeadline(
    JNIEnv* env, jobject, jlong cptr)
{
  jint result(0);
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  if (settings)
  {
    result = (jint)settings->get_deadline();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::getDeadline: "
        "QoSTransportSettings object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_addTrustedPeer
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_ai_madara_transport_QoSTransportSettings_jni_1addTrustedPeer(
    JNIEnv* env, jobject, jlong cptr, jstring jhost)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;
  const char* host = env->GetStringUTFChars(jhost, 0);

  if (settings)
  {
    settings->add_trusted_peer(host);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::addTrustedPeer: "
        "QoSTransportSettings object is released already");
  }

  env->ReleaseStringUTFChars(jhost, host);
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_addBannedPeer
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_ai_madara_transport_QoSTransportSettings_jni_1addBannedPeer(
    JNIEnv* env, jobject, jlong cptr, jstring jhost)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;
  const char* host = env->GetStringUTFChars(jhost, 0);

  if (settings)
  {
    settings->add_banned_peer(host);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::addBannedPeer: "
        "QoSTransportSettings object is released already");
  }

  env->ReleaseStringUTFChars(jhost, host);
}

/*
 * Class:     ai_madara_transport_QoSTransportSettings
 * Method:    jni_updateDropRate
 * Signature: (JDII)V
 */
void JNICALL Java_ai_madara_transport_QoSTransportSettings_jni_1updateDropRate(
    JNIEnv* env, jobject, jlong cptr, jdouble drop_rate, jint drop_type,
    jint burstamount)
{
  QoSTransportSettings* settings = (QoSTransportSettings*)cptr;

  if (settings)
  {
    settings->update_drop_rate(
        drop_rate, (int)drop_type, (uint64_t)burstamount);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "QoSTransportSettings::updateDropRate: "
        "QoSTransportSettings object is released already");
  }
}
