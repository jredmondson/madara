
#include "com_madara_containers_Double.h"
#include "madara/knowledge_engine/containers/Double.h"

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
typedef containers::Double    Double;

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_Double
 * Signature: ()J
 */
jlong JNICALL Java_com_madara_containers_Double_jni_1Double__
  (JNIEnv * env, jobject)
{
  return (jlong) new Double ();
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_Double
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_Double_jni_1Double__J
  (JNIEnv * env, jobject, jlong cptr)
{
  Double * result (0);
  Double * source = (Double *) cptr;

  if (source)
  {
    result = new Double (*source);
  }

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_freeDouble
 * Signature: (J)V
 */
void JNICALL Java_com_madara_containers_Double_jni_1freeDouble
  (JNIEnv * env, jclass, jlong cptr)
{
  delete (Double *) cptr;
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_set
 * Signature: (JD)V
 */
void JNICALL Java_com_madara_containers_Double_jni_1set
  (JNIEnv * env, jobject, jlong cptr, jdouble value)
{
  Double * current = (Double *) cptr;

  if (current)
  {
    *current = value;
  }
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_getName
 * Signature: (J)Lcom/madara/containers/String;
 */
jstring JNICALL Java_com_madara_containers_Double_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  Double * current = (Double *) cptr;

  if (current)
  {
    result = env->NewStringUTF (current->get_name ().c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_setName
 * Signature: (JJJLcom/madara/containers/String;)V
 */
void JNICALL Java_com_madara_containers_Double_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  Double * current = (Double *) cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars (name, 0);

    if (type == 0)
    {
      engine::Knowledge_Base * kb = (engine::Knowledge_Base *) context;
      current->set_name (str_name, *kb);
    }
    else if (type == 1)
    {
      engine::Variables * vars = (engine::Variables *) context;
      current->set_name (str_name, *vars);
    }

    env->ReleaseStringUTFChars (name, str_name);
  }
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_toString
 * Signature: (J)Lcom/madara/containers/String;
 */
jstring JNICALL Java_com_madara_containers_Double_jni_1toString
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  Double * current = (Double *) cptr;

  if (current)
  {
    result = env->NewStringUTF (current->to_string ().c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_toDouble
 * Signature: (J)D
 */
jdouble JNICALL Java_com_madara_containers_Double_jni_1toDouble
  (JNIEnv * env, jobject, jlong cptr)
{
  jdouble result (0.0);
  Double * current = (Double *) cptr;

  if (current)
  {
    result = current->to_double ();
  }

  return result;
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_toLong
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_Double_jni_1toLong
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);
  Double * current = (Double *) cptr;

  if (current)
  {
    result = current->to_integer ();
  }

  return result;
}

void JNICALL Java_com_madara_containers_Double_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  Double * current = (Double *) cptr;

  if (current)
  {
    current->modify ();
  }
}
