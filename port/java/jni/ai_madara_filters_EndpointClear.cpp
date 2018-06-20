#include "ai_madara_filters_EndpointClear.h"
#include "madara/filters/EndpointClear.h"

namespace transport = madara::transport;
namespace knowledge = madara::knowledge;
namespace filters = madara::filters;
typedef madara::knowledge::KnowledgeRecord::Integer    Integer;
typedef transport::QoSTransportSettings QoSTransportSettings;
typedef filters::EndpointClear EndpointClear;


jlong JNICALL
Java_ai_madara_filters_EndpointClear_jni_1EndpointClear
(JNIEnv *, jobject)
{
  return (jlong) new EndpointClear ();
}

void JNICALL
Java_ai_madara_filters_EndpointClear_jni_1addReceiveFilterTo
(JNIEnv * env, jobject, jlong cptr, jlong qosCptr)
{
  EndpointClear * current = (EndpointClear *)cptr;
  QoSTransportSettings * settings = (QoSTransportSettings *)qosCptr;

  if (current && settings)
  {
    settings->add_receive_filter (current);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "EndpointClear::addReceiveFilterTo: "
      "EndpointClear or settings objects are released already");
  }
}

void JNICALL
Java_ai_madara_filters_EndpointClear_jni_1addSendFilterTo
(JNIEnv * env, jobject, jlong cptr, jlong qosCptr)
{
  EndpointClear * current = (EndpointClear *)cptr;
  QoSTransportSettings * settings = (QoSTransportSettings *)qosCptr;

  if (current && settings)
  {
    settings->add_send_filter (current);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "EndpointClear::addSendFilterto: "
      "EndpointClear or settings objects are released already");
  }
}

void JNICALL
Java_ai_madara_filters_EndpointClear_jni_1addRebroadcastFilterTo
(JNIEnv * env, jobject, jlong cptr, jlong qosCptr)
{
  EndpointClear * current = (EndpointClear *)cptr;
  QoSTransportSettings * settings = (QoSTransportSettings *)qosCptr;

  if (current && settings)
  {
    settings->add_rebroadcast_filter (current);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "EndpointClear::addRebroadcastFilter: "
      "EndpointClear or settings objects are released already");
  }
}

/*
 * Class:     ai_madara_filters_EndpointClear
 * Method:    jni_setPrefix
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL
Java_ai_madara_filters_EndpointClear_jni_1setPrefix
(JNIEnv * env, jobject, jlong cptr, jstring prefix)
{
  EndpointClear * current = (EndpointClear *)cptr;

  if (current)
  {
    const char * str_prefix = env->GetStringUTFChars (prefix, 0);

    current->set_prefix (str_prefix);

    env->ReleaseStringUTFChars (prefix, str_prefix);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "EndpointClear::setPrefix: "
      "EndpointClear object is released already");
  }
}

/*
 * Class:     ai_madara_filters_EndpointClear
 * Method:    jni_freeEndpointClear
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_filters_EndpointClear_jni_1freeEndpointClear
(JNIEnv *, jclass, jlong cptr)
{
  delete (EndpointClear *)cptr;
}
