#include "com_madara_containers_Collection.h"
#include "madara/knowledge/containers/Collection.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef containers::Collection    Collection;


jlong JNICALL
Java_com_madara_containers_Collection_jni_1Collection__
(JNIEnv *, jobject)
{
  return (jlong) new Collection ();
}


jlong JNICALL
Java_com_madara_containers_Collection_jni_1Collection__J
  (JNIEnv *, jobject, jlong cptr)
{
  Collection * result (0);
  Collection * source = (Collection *)cptr;

  if (source)
  {
    result = new Collection (*source);
  }

  return (jlong)result;
}


void JNICALL
Java_com_madara_containers_Collection_jni_1freeCollection
  (JNIEnv *, jclass, jlong cptr)
{
  delete (Collection *)cptr;
}


jstring JNICALL
Java_com_madara_containers_Collection_jni_1getDebugInfo
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  Collection * current = (Collection *)cptr;

  if (current)
  {
    result = env->NewStringUTF (current->get_debug_info ().c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}


void JNICALL
Java_com_madara_containers_Collection_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  Collection * current = (Collection *)cptr;

  if (current)
  {
    current->modify ();
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1setSettings
  (JNIEnv *, jobject, jlong cptr, jlong settings_ptr)
{
  Collection * current = (Collection *)cptr;
  knowledge::KnowledgeUpdateSettings * settings =
    (knowledge::KnowledgeUpdateSettings *)settings_ptr;

  if (current && settings)
  {
    current->set_settings (*settings);
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1addBarrier
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::Barrier * container = (containers::Barrier *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1addCounter
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::Counter * container = (containers::Counter *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1addDouble
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::Double * container = (containers::Double *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1addDoubleVector
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::DoubleVector * container =
    (containers::DoubleVector *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1addFlexMap
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::FlexMap * container =
    (containers::FlexMap *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1addInteger
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::Integer * container =
    (containers::Integer *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}

void JNICALL
Java_com_madara_containers_Collection_jni_1addIntegerVector
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::IntegerVector * container =
    (containers::IntegerVector *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1addMap
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::Map * container =
    (containers::Map *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1addNativeDoubleVector
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::NativeDoubleVector * container =
    (containers::NativeDoubleVector *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1addNativeIntegerVector
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::NativeIntegerVector * container =
    (containers::NativeIntegerVector *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1addString
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::String * container =
    (containers::String *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1addStringVector
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::StringVector * container =
    (containers::StringVector *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}


void JNICALL
Java_com_madara_containers_Collection_jni_1addVector
  (JNIEnv *, jobject, jlong cptr, jlong container_ptr)
{
  Collection * current = (Collection *)cptr;
  containers::Vector * container =
    (containers::Vector *)container_ptr;

  if (current && container)
  {
    current->add (*container);
  }
}

jboolean JNICALL
Java_com_madara_containers_Collection_jni_1isTrue
(JNIEnv *, jobject, jlong cptr)
{
  Collection * current = (Collection *)cptr;
  bool result (true);

  if (current)
  {
    result = current->is_true ();
  }

  return result;
}


jboolean JNICALL
Java_com_madara_containers_Collection_jni_1isFalse
(JNIEnv *, jobject, jlong cptr)
{
  Collection * current = (Collection *)cptr;
  bool result (true);

  if (current)
  {
    result = current->is_false ();
  }

  return result;
}
