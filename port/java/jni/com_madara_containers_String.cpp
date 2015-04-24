
#include "com_madara_containers_String.h"
#include "madara/knowledge_engine/containers/String.h"

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
typedef containers::String    String;

/*
 * Class:     com_madara_containers_String
 * Method:    jni_String
 * Signature: ()J
 */
jlong JNICALL Java_com_madara_containers_String_jni_1String__
  (JNIEnv * env, jobject)
{
  return (jlong) new String ();
}

/*
 * Class:     com_madara_containers_String
 * Method:    jni_String
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_String_jni_1String__J
  (JNIEnv * env, jobject, jlong cptr)
{
  String * result (0);
  String * source = (String *) cptr;

  if (source)
  {
    result = new String (*source);
  }

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_String
 * Method:    jni_freeString
 * Signature: (J)V
 */
void JNICALL Java_com_madara_containers_String_jni_1freeString
  (JNIEnv * env, jclass, jlong cptr)
{
  delete (String *) cptr;
}

/*
 * Class:     com_madara_containers_String
 * Method:    jni_set
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_com_madara_containers_String_jni_1set
  (JNIEnv * env, jobject, jlong cptr, jstring value)
{
  String * current = (String *) cptr;
  if (current)
  {
    const char * str_value = env->GetStringUTFChars (value, 0);

    *current = str_value;

    env->ReleaseStringUTFChars (value, str_value);
  }
}

/*
 * Class:     com_madara_containers_String
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_com_madara_containers_String_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  String * current = (String *) cptr;

  if (current)
  {
    result = env->NewStringUTF (current->get_name ().c_str ());
  }

  return result;
}

/*
 * Class:     com_madara_containers_String
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
void JNICALL Java_com_madara_containers_String_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  String * current = (String *) cptr;

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
 * Class:     com_madara_containers_String
 * Method:    jni_toString
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_com_madara_containers_String_jni_1toString
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  String * current = (String *) cptr;

  if (current)
  {
    result = env->NewStringUTF (current->to_string ().c_str ());
  }

  return result;
}

/*
 * Class:     com_madara_containers_String
 * Method:    jni_toDouble
 * Signature: (J)D
 */
jdouble JNICALL Java_com_madara_containers_String_jni_1toDouble
  (JNIEnv * env, jobject, jlong cptr)
{
  jdouble result (0.0);
  String * current = (String *) cptr;

  if (current)
  {
    result = current->to_double ();
  }

  return result;
}

/*
 * Class:     com_madara_containers_String
 * Method:    jni_toLong
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_String_jni_1toLong
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);
  String * current = (String *) cptr;

  if (current)
  {
    result = current->to_integer ();
  }

  return result;
}

void JNICALL Java_com_madara_containers_String_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  String * current = (String *) cptr;

  if (current)
  {
    current->modify ();
  }
}
