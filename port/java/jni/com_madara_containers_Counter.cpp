#include "com_madara_containers_Counter.h"
#include "madara/knowledge_engine/containers/Counter.h"

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;

/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_Counter
 * Signature: ()J
 */
jlong JNICALL Java_com_madara_containers_Counter_jni_1Counter__
  (JNIEnv *, jobject)
{
  return (jlong) new containers::Counter ();
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_Counter
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_Counter_jni_1Counter__J
  (JNIEnv *, jobject, jlong cptr)
{
  return (jlong) new containers::Counter (*(containers::Counter *)cptr);
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_freeCounter
 * Signature: (J)V
 */
void JNICALL Java_com_madara_containers_Counter_jni_1freeCounter
  (JNIEnv *, jclass, jlong cptr)
{
  delete (containers::Counter *) cptr;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_set
 * Signature: (JJ)V
 */
void JNICALL Java_com_madara_containers_Counter_jni_1set
  (JNIEnv *, jobject, jlong cptr, jlong value)
{
  containers::Counter * current = (containers::Counter *) cptr;
  if (current)
    *current = value;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_com_madara_containers_Counter_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;

  containers::Counter * current = (containers::Counter *) cptr;
  if (current)
    result = env->NewStringUTF(current->get_name ().c_str ());

  return result;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
void JNICALL Java_com_madara_containers_Counter_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  containers::Counter * current = (containers::Counter *) cptr;

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
 * Class:     com_madara_containers_Counter
 * Method:    jni_toString
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_com_madara_containers_Counter_jni_1toString
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;

  containers::Counter * current = (containers::Counter *) cptr;
  if (current)
    result = env->NewStringUTF(current->to_string ().c_str ());

  return result;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_toDouble
 * Signature: (J)D
 */
jdouble JNICALL Java_com_madara_containers_Counter_jni_1toDouble
  (JNIEnv * env, jobject, jlong cptr)
{
  jdouble result (0.0);

  containers::Counter * current = (containers::Counter *) cptr;
  if (current)
    result = current->to_double ();

  return result;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_toLong
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_Counter_jni_1toLong
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);

  containers::Counter * current = (containers::Counter *) cptr;
  if (current)
    result = current->to_integer ();

  return result;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_inc
 * Signature: (J)J
 */
void JNICALL Java_com_madara_containers_Counter_jni_1inc
  (JNIEnv *, jobject, jlong cptr)
{
  containers::Counter * current = (containers::Counter *) cptr;
  ++(*current);
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_dec
 * Signature: (J)J
 */
void JNICALL Java_com_madara_containers_Counter_jni_1dec
  (JNIEnv *, jobject, jlong cptr)
{
  containers::Counter * current = (containers::Counter *) cptr;
  --(*current);
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_incValue
 * Signature: (JJ)J
 */
void JNICALL Java_com_madara_containers_Counter_jni_1incValue
  (JNIEnv *, jobject, jlong cptr, jlong value)
{
  containers::Counter * current = (containers::Counter *) cptr;
  *current += value;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_decValue
 * Signature: (JJ)J
 */
void JNICALL Java_com_madara_containers_Counter_jni_1decValue
  (JNIEnv *, jobject, jlong cptr, jlong value)
{
  containers::Counter * current = (containers::Counter *) cptr;
  *current -= value;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_modify
 * Signature: (J)V
 */
void JNICALL Java_com_madara_containers_Counter_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  containers::Counter * current = (containers::Counter *) cptr;
  current->modify ();
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_resize
 * Signature: (JII)V
 */
void JNICALL Java_com_madara_containers_Counter_jni_1resize
  (JNIEnv *, jobject, jlong cptr, jint id, jint counters)
{
  containers::Counter * current = (containers::Counter *) cptr;
  current->resize (id, counters);
}
