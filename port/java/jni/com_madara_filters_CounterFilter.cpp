#include "com_madara_filters_CounterFilter.h"
#include "madara/filters/Counter_Filter.h"
#include "madara/transport/QoS_Transport_Settings.h"

namespace transport = Madara::Transport;
namespace engine = Madara::Knowledge_Engine;
namespace filters = Madara::Filters;
typedef Madara::Knowledge_Record::Integer    Integer;
typedef transport::QoS_Transport_Settings QoS_Transport_Settings;
typedef filters::Counter_Filter Counter_Filter;

jlong JNICALL Java_com_madara_filters_CounterFilter_jni_1CounterFilter__
  (JNIEnv *, jobject)
{
  return (jlong) new Counter_Filter ();
}


jdouble JNICALL Java_com_madara_filters_CounterFilter_jni_1getThroughput
  (JNIEnv *, jobject, jlong cptr)
{
  Counter_Filter * current = (Counter_Filter *) cptr;
  jdouble result (0);

  if (current)
  {
    result = current->get_throughput ();
  }

  return result;
}


void JNICALL Java_com_madara_filters_CounterFilter_jni_1addReceiveFilterTo
  (JNIEnv *, jobject, jlong cptr, jlong qosCptr)
{
  Counter_Filter * current = (Counter_Filter *) cptr;
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *) qosCptr;

  if (current && settings)
  {
    settings->add_receive_filter (current);
  }
}

void JNICALL Java_com_madara_filters_CounterFilter_jni_1addSendFilterTo
  (JNIEnv *, jobject, jlong cptr, jlong qosCptr)
{
  Counter_Filter * current = (Counter_Filter *)cptr;
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)qosCptr;

  if (current && settings)
  {
    settings->add_send_filter (current);
  }
}


void JNICALL Java_com_madara_filters_CounterFilter_jni_1addRebroadcastFilterTo
(JNIEnv *, jobject, jlong cptr, jlong qosCptr)
{
  Counter_Filter * current = (Counter_Filter *)cptr;
  QoS_Transport_Settings * settings = (QoS_Transport_Settings *)qosCptr;

  if (current && settings)
  {
    settings->add_rebroadcast_filter (current);
  }
}


jlong JNICALL Java_com_madara_filters_CounterFilter_jni_1getCount
  (JNIEnv *, jobject, jlong cptr)
{
  Counter_Filter * current = (Counter_Filter *)cptr;
  Integer result (0);

  if (current)
  {
    result = current->get_count ();
  }

  return result;
}


jlong JNICALL Java_com_madara_filters_CounterFilter_jni_1getElapsed
  (JNIEnv *, jobject, jlong cptr)
{
  Counter_Filter * current = (Counter_Filter *)cptr;
  Integer result (0);

  if (current)
  {
    result = current->get_elapsed ();
  }

  return result;
}


void JNICALL Java_com_madara_filters_CounterFilter_jni_1freeCounterFilter
  (JNIEnv *, jclass, jlong cptr)
{
  delete (Counter_Filter *) cptr;
}
