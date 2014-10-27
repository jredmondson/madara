
#include "com_madara_containers_Integer.h"
#include "madara/knowledge_engine/containers/Integer.h"

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;

/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_Integer
 * Signature: ()J
 */
jlong JNICALL Java_com_madara_containers_Integer_jni_1Integer__
  (JNIEnv * env, jobject)
{
  return (jlong) new containers::Integer ();
}

/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_Integer
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_Integer_jni_1Integer__J
  (JNIEnv * env, jobject, jlong cptr)
{
  return (jlong) new containers::Integer (*(containers::Integer *)cptr);
}

/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_freeInteger
 * Signature: (J)V
 */
void JNICALL Java_com_madara_containers_Integer_jni_1freeInteger
  (JNIEnv * env, jclass, jlong cptr)
{
  delete (containers::Integer *) cptr;
}

/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_set
 * Signature: (JJ)V
 */
void JNICALL Java_com_madara_containers_Integer_jni_1set
  (JNIEnv * env, jobject, jlong cptr, jlong value)
{
  containers::Integer * current = (containers::Integer *) cptr;
  if (current)
    *current = value;
}

/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_inc
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_Integer_jni_1inc
  (JNIEnv *, jobject, jlong cptr)
{
  jlong result (0);
  containers::Integer * current = (containers::Integer *) cptr;

  if (current)
    result = ++(*current);

  return result;
}

/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_dec
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_Integer_jni_1dec
  (JNIEnv *, jobject, jlong cptr)
{
  jlong result (0);
  containers::Integer * current = (containers::Integer *) cptr;

  if (current)
    result = --(*current);

  return result;
}

/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_incValue
 * Signature: (JJ)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_Integer_jni_1incValue
  (JNIEnv *, jobject, jlong cptr, jlong value)
{
  jlong result (0);
  containers::Integer * current = (containers::Integer *) cptr;

  if (current)
    result = ((*current) += value);

  return result;
}

/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_decValue
 * Signature: (JJ)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_Integer_jni_1decValue
  (JNIEnv *, jobject, jlong cptr, jlong value)
{
  jlong result (0);
  containers::Integer * current = (containers::Integer *) cptr;

  if (current)
    result = ((*current) -= value);

  return result;
}

/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_com_madara_containers_Integer_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;

  containers::Integer * current = (containers::Integer *) cptr;
  if (current)
    result = env->NewStringUTF(current->get_name ().c_str ());

  return result;
}


/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
void JNICALL Java_com_madara_containers_Integer_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  containers::Integer * current = (containers::Integer *) cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars(name, 0);

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

    env->ReleaseStringUTFChars(name, str_name);
  }
}


/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_toString
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_com_madara_containers_Integer_jni_1toString
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;

  containers::Integer * current = (containers::Integer *) cptr;
  if (current)
    result = env->NewStringUTF(current->to_string ().c_str ());

  return result;
}


/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_toDouble
 * Signature: (J)D
 */
jdouble JNICALL Java_com_madara_containers_Integer_jni_1toDouble
  (JNIEnv * env, jobject, jlong cptr)
{
  jdouble result (0.0);

  containers::Integer * current = (containers::Integer *) cptr;
  if (current)
    result = current->to_double ();

  return result;
}


/*
 * Class:     com_madara_containers_Integer
 * Method:    jni_toLong
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_Integer_jni_1toLong
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);

  containers::Integer * current = (containers::Integer *) cptr;
  if (current)
    result = current->to_integer ();

  return result;
}

void JNICALL Java_com_madara_containers_Integer_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  containers::Integer * current = (containers::Integer *) cptr;
  if (current)
    current->modify ();
}
