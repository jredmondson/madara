/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "com.madara.transport.QoSTransportSettings.h"
#include "madara/transport/QoS_Transport_Settings.h"

#include <iostream>

/*
 * Class:   com_madara_transport_QoSTransportSettings
 * Method:  jni_QoSTransportSettings
 * Signature: ()J
 */
jlong JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1QoSTransportSettings__
  (JNIEnv *, jobject)
{
  return (jlong) new Madara::Transport::QoS_Transport_Settings();
}

/*
 * Class:   com_madara_transport_QoSTransportSettings
 * Method:  jni_QoSTransportSettings
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1QoSTransportSettings__J
  (JNIEnv *, jobject, jlong old)
{
  return (jlong) new Madara::Transport::QoS_Transport_Settings(
    *(Madara::Transport::QoS_Transport_Settings*)old);
}

/*
 * Class:   com_madara_transport_QoSTransportSettings
 * Method:  jni_freeQoSTransportSettings
 * Signature: (J)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1freeQoSTransportSettings
  (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Transport::QoS_Transport_Settings* settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  if (settings)
    delete settings;
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addRebroadcastFilter
 * Signature: (JILcom/madara/transport/filters/RecordFilter;)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addRebroadcastFilter__JILcom_madara_transport_filters_RecordFilter_2
  (JNIEnv *, jobject, jlong cptr, jint type, jobject filter)
{
  Madara::Transport::QoS_Transport_Settings* settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;

  settings->add_rebroadcast_filter ((uint32_t)type, filter);
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addRebroadcastFilter
 * Signature: (JLcom/madara/transport/filters/AggregateFilter;)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addRebroadcastFilter__JLcom_madara_transport_filters_AggregateFilter_2
  (JNIEnv *, jobject, jlong cptr, jobject filter)
{
  Madara::Transport::QoS_Transport_Settings* settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;

  settings->add_rebroadcast_filter (filter);
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addSendFilter
 * Signature: (JILcom/madara/transport/filters/RecordFilter;)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addSendFilter__JILcom_madara_transport_filters_RecordFilter_2
  (JNIEnv *, jobject, jlong cptr, jint type, jobject filter)
{
  Madara::Transport::QoS_Transport_Settings* settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;

  settings->add_send_filter ((uint32_t)type, filter);
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addSendFilter
 * Signature: (JLcom/madara/transport/filters/AggregateFilter;)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addSendFilter__JLcom_madara_transport_filters_AggregateFilter_2
  (JNIEnv *, jobject, jlong cptr, jobject filter)
{
  Madara::Transport::QoS_Transport_Settings* settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;

  settings->add_send_filter (filter);
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addReceiveFilter
 * Signature: (JILcom/madara/transport/filters/RecordFilter;)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addReceiveFilter__JILcom_madara_transport_filters_RecordFilter_2
  (JNIEnv *, jobject, jlong cptr, jint type, jobject filter)
{
  Madara::Transport::QoS_Transport_Settings* settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;

  settings->add_receive_filter ((uint32_t)type, filter);
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addReceiveFilter
 * Signature: (JLcom/madara/transport/filters/AggregateFilter;)V
 */
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addReceiveFilter__JLcom_madara_transport_filters_AggregateFilter_2
  (JNIEnv *, jobject, jlong cptr, jobject filter)
{
  Madara::Transport::QoS_Transport_Settings* settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;

  settings->add_receive_filter (filter);
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_setRebroadcastTtl
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1setRebroadcastTtl
  (JNIEnv * env, jobject obj, jlong cptr, jint ttl)
{
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  settings->set_rebroadcast_ttl ((unsigned char)ttl);
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_getRebroadcastTtl
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1getRebroadcastTtl
  (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  return (jint) settings->get_rebroadcast_ttl ();
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_enableParticipantTtl
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1enableParticipantTtl
  (JNIEnv * env, jobject obj, jlong cptr, jint ttl)
{
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  settings->enable_participant_ttl ((unsigned char)ttl);
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_getParticpantTtl
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1gettParticpantTtl (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  return (jint) settings->get_participant_ttl ();
}


/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_setSendBandwidthLimit
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1setSendBandwidthLimit (JNIEnv * env, jobject obj, jlong cptr, jint limit)
{
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  settings->set_send_bandwidth_limit (limit);
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_getSendBandwidthLimit
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1getSendBandwidthLimit (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  return (jint) settings->get_send_bandwidth_limit ();
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_setTotalBandwidthLimit
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1setTotalBandwidthLimit
  (JNIEnv * env, jobject obj, jlong cptr, jint limit)
{
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  settings->set_total_bandwidth_limit (limit);
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_getTotalBandwidthLimit
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1getTotalBandwidthLimit
  (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  return (jint) settings->get_total_bandwidth_limit ();
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_setDeadline
* Signature: (JI)V
*/
void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1setDeadline
  (JNIEnv * env, jobject obj, jlong cptr, jint limit)
{
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  settings->set_deadline (limit);
}

/*
* Class:   com_madara_transport_QoSTransportSettings
* Method:  jni_getDeadline
* Signature: (J)I
*/
jint JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1getDeadline
  (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  return (jint) settings->get_deadline ();
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addTrustedPeer
 * Signature: (JLjava/lang/String;)V
 */
 void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addTrustedPeer
  (JNIEnv * env, jobject, jlong cptr, jstring jhost)
{
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  const char *host = env->GetStringUTFChars(jhost, 0);

  settings->add_trusted_peer(host);

  env->ReleaseStringUTFChars(jhost, host);
}

/*
 * Class:     com_madara_transport_QoSTransportSettings
 * Method:    jni_addBannedPeer
 * Signature: (JLjava/lang/String;)V
 */
 void JNICALL Java_com_madara_transport_QoSTransportSettings_jni_1addBannedPeer
  (JNIEnv * env, jobject, jlong cptr, jstring jhost)
{
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;
  const char *host = env->GetStringUTFChars(jhost, 0);

  settings->add_banned_peer(host);

  env->ReleaseStringUTFChars(jhost, host);
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
  Madara::Transport::QoS_Transport_Settings * settings =
    (Madara::Transport::QoS_Transport_Settings*)cptr;

  settings->update_drop_rate(drop_rate, (int)drop_type, (uint64_t)burstamount);
}
