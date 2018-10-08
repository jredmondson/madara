#include "ai_madara_filters_CounterFilter.h"
#include "madara/filters/CounterFilter.h"
#include "madara/transport/QoSTransportSettings.h"

namespace transport = madara::transport;
namespace knowledge = madara::knowledge;
namespace filters = madara::filters;
typedef madara::knowledge::KnowledgeRecord::Integer Integer;
typedef transport::QoSTransportSettings QoSTransportSettings;
typedef filters::CounterFilter CounterFilter;

jlong JNICALL Java_ai_madara_filters_CounterFilter_jni_1CounterFilter__(
    JNIEnv*, jobject)
{
  return (jlong) new CounterFilter();
}

jdouble JNICALL Java_ai_madara_filters_CounterFilter_jni_1getThroughput(
    JNIEnv* env, jobject, jlong cptr)
{
  CounterFilter* current = (CounterFilter*)cptr;
  jdouble result(0);

  if (current)
  {
    result = current->get_throughput();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CounterFilter::getThroughput: "
        "CounterFilter object is released already");
  }

  return result;
}

void JNICALL Java_ai_madara_filters_CounterFilter_jni_1addReceiveFilterTo(
    JNIEnv* env, jobject, jlong cptr, jlong qosCptr)
{
  CounterFilter* current = (CounterFilter*)cptr;
  QoSTransportSettings* settings = (QoSTransportSettings*)qosCptr;

  if (current && settings)
  {
    settings->add_receive_filter(current);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CounterFilter::addReceiveFilterTo: "
        "CounterFilter or settings objects are released already");
  }
}

void JNICALL Java_ai_madara_filters_CounterFilter_jni_1addSendFilterTo(
    JNIEnv* env, jobject, jlong cptr, jlong qosCptr)
{
  CounterFilter* current = (CounterFilter*)cptr;
  QoSTransportSettings* settings = (QoSTransportSettings*)qosCptr;

  if (current && settings)
  {
    settings->add_send_filter(current);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CounterFilter::addSendFilterTo: "
        "CounterFilter or settings objects are released already");
  }
}

void JNICALL Java_ai_madara_filters_CounterFilter_jni_1addRebroadcastFilterTo(
    JNIEnv* env, jobject, jlong cptr, jlong qosCptr)
{
  CounterFilter* current = (CounterFilter*)cptr;
  QoSTransportSettings* settings = (QoSTransportSettings*)qosCptr;

  if (current && settings)
  {
    settings->add_rebroadcast_filter(current);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CounterFilter::addRebroadcastFilterTo: "
        "CounterFilter or settings objects are released already");
  }
}

jlong JNICALL Java_ai_madara_filters_CounterFilter_jni_1getCount(
    JNIEnv* env, jobject, jlong cptr)
{
  CounterFilter* current = (CounterFilter*)cptr;
  Integer result(0);

  if (current)
  {
    result = current->get_count();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CounterFilter::getCount: "
        "CounterFilter object is released already");
  }

  return result;
}

jlong JNICALL Java_ai_madara_filters_CounterFilter_jni_1getElapsed(
    JNIEnv* env, jobject, jlong cptr)
{
  CounterFilter* current = (CounterFilter*)cptr;
  Integer result(0);

  if (current)
  {
    result = current->get_elapsed();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CounterFilter::getElapsed: "
        "CounterFilter object is released already");
  }

  return result;
}

void JNICALL Java_ai_madara_filters_CounterFilter_jni_1freeCounterFilter(
    JNIEnv*, jclass, jlong cptr)
{
  delete (CounterFilter*)cptr;
}
