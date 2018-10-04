#include "ai_madara_knowledge_containers_Collection.h"
#include "madara/knowledge/containers/Collection.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef containers::Collection Collection;

jlong JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1Collection__(
    JNIEnv*, jobject)
{
  return (jlong) new Collection();
}

jlong JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1Collection__J(
    JNIEnv* env, jobject, jlong cptr)
{
  Collection* result(0);
  Collection* source = (Collection*)cptr;

  if (source)
  {
    result = new Collection(*source);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::constConstructor: "
        "Collection object is released already");
  }

  return (jlong)result;
}

void JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1freeCollection(
    JNIEnv*, jclass, jlong cptr)
{
  delete (Collection*)cptr;
}

jstring JNICALL
Java_ai_madara_knowledge_containers_Collection_jni_1getDebugInfo(
    JNIEnv* env, jobject, jlong cptr)
{
  jstring result = 0;
  Collection* current = (Collection*)cptr;

  if (current)
  {
    result = env->NewStringUTF(current->get_debug_info().c_str());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::getDebugInfo: "
        "Collection object is released already");
  }

  return result;
}

void JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1modify(
    JNIEnv* env, jobject, jlong cptr)
{
  Collection* current = (Collection*)cptr;

  if (current)
  {
    current->modify();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::modify: "
        "Collection object is released already");
  }
}

void JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1setSettings(
    JNIEnv* env, jobject, jlong cptr, jlong settings_ptr)
{
  Collection* current = (Collection*)cptr;
  knowledge::KnowledgeUpdateSettings* settings =
      (knowledge::KnowledgeUpdateSettings*)settings_ptr;

  if (current && settings)
  {
    current->set_settings(*settings);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::setSettings: "
        "Collection or settings objects are released already");
  }
}

void JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1addBarrier(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::Barrier* container = (containers::Barrier*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addBarrier: "
        "Collection or added objects are released already");
  }
}

void JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1addCounter(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::Counter* container = (containers::Counter*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addCounter: "
        "Collection or added objects are released already");
  }
}

void JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1addDouble(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::Double* container = (containers::Double*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addDouble: "
        "Collection or added objects are released already");
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_Collection_jni_1addDoubleVector(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::DoubleVector* container =
      (containers::DoubleVector*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addDoubleVector: "
        "Collection or added objects are released already");
  }
}

void JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1addFlexMap(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::FlexMap* container = (containers::FlexMap*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addFlexMap: "
        "Collection or added objects are released already");
  }
}

void JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1addInteger(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::Integer* container = (containers::Integer*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addInteger: "
        "Collection or added objects are released already");
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_Collection_jni_1addIntegerVector(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::IntegerVector* container =
      (containers::IntegerVector*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addIntegerVector: "
        "Collection or added objects are released already");
  }
}

void JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1addMap(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::Map* container = (containers::Map*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addMap: "
        "Collection or added objects are released already");
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_Collection_jni_1addNativeDoubleVector(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::NativeDoubleVector* container =
      (containers::NativeDoubleVector*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addNativeDoubleVector: "
        "Collection or added objects are released already");
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_Collection_jni_1addNativeIntegerVector(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::NativeIntegerVector* container =
      (containers::NativeIntegerVector*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addNativeIntegerVector: "
        "Collection or added objects are released already");
  }
}

void JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1addString(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::String* container = (containers::String*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addString: "
        "Collection or added objects are released already");
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_Collection_jni_1addStringVector(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::StringVector* container =
      (containers::StringVector*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addStringVector: "
        "Collection or added objects are released already");
  }
}

void JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1addVector(
    JNIEnv* env, jobject, jlong cptr, jlong container_ptr)
{
  Collection* current = (Collection*)cptr;
  containers::Vector* container = (containers::Vector*)container_ptr;

  if (current && container)
  {
    current->add(*container);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::addVector: "
        "Collection or added objects are released already");
  }
}

jboolean JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1isTrue(
    JNIEnv* env, jobject, jlong cptr)
{
  Collection* current = (Collection*)cptr;
  bool result(true);

  if (current)
  {
    result = current->is_true();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::isTrue: "
        "Collection object is released already");
  }

  return result;
}

jboolean JNICALL Java_ai_madara_knowledge_containers_Collection_jni_1isFalse(
    JNIEnv* env, jobject, jlong cptr)
{
  Collection* current = (Collection*)cptr;
  bool result(true);

  if (current)
  {
    result = current->is_false();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Collection::isFalse: "
        "Collection object is released already");
  }

  return result;
}
