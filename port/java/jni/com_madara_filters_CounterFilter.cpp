#include "com_madara_filters_CounterFilter.h"
#include "madara/filters/CounterFilter.h"
#include "madara/transport/QoSTransportSettings.h"

namespace transport = madara::transport;
namespace knowledge = madara::knowledge;
namespace filters = madara::filters;
typedef madara::KnowledgeRecord::Integer    Integer;
typedef transport::QoSTransportSettings QoSTransportSettings;
typedef filters::CounterFilter CounterFilter;

jlong JNICALL Java_com_madara_filters_CounterFilter_jni_1CounterFilter__
  (JNIEnv *, jobject)
{
  return (jlong) new CounterFilter ();
}


jdouble JNICALL Java_com_madara_filters_CounterFilter_jni_1getThroughput
  (JNIEnv *, jobject, jlong cptr)
{
  CounterFilter * current = (CounterFilter *) cptr;
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
  CounterFilter * current = (CounterFilter *) cptr;
  QoSTransportSettings * settings = (QoSTransportSettings *) qosCptr;

  if (current && settings)
  {
    settings->add_receive_filter (current);
  }
}

void JNICALL Java_com_madara_filters_CounterFilter_jni_1addSendFilterTo
  (JNIEnv *, jobject, jlong cptr, jlong qosCptr)
{
  CounterFilter * current = (CounterFilter *)cptr;
  QoSTransportSettings * settings = (QoSTransportSettings *)qosCptr;

  if (current && settings)
  {
    settings->add_send_filter (current);
  }
}


void JNICALL Java_com_madara_filters_CounterFilter_jni_1addRebroadcastFilterTo
(JNIEnv *, jobject, jlong cptr, jlong qosCptr)
{
  CounterFilter * current = (CounterFilter *)cptr;
  QoSTransportSettings * settings = (QoSTransportSettings *)qosCptr;

  if (current && settings)
  {
    settings->add_rebroadcast_filter (current);
  }
}


jlong JNICALL Java_com_madara_filters_CounterFilter_jni_1getCount
  (JNIEnv *, jobject, jlong cptr)
{
  CounterFilter * current = (CounterFilter *)cptr;
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
  CounterFilter * current = (CounterFilter *)cptr;
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
  delete (CounterFilter *) cptr;
}
