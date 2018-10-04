#include "madara/knowledge/containers/CircularBuffer.h"
#include "ai_madara_knowledge_containers_CircularBuffer.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef knowledge::KnowledgeRecord KnowledgeRecord;
typedef KnowledgeRecord::Integer Integer;
typedef containers::CircularBuffer CircularBuffer;

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_CircularBuffer
 * Signature: ()J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_CircularBuffer_jni_1CircularBuffer__(
    JNIEnv*, jobject)
{
  return (jlong) new containers::CircularBuffer();
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_CircularBuffer
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_CircularBuffer_jni_1CircularBuffer__J(
    JNIEnv* env, jobject, jlong cptr)
{
  containers::CircularBuffer* result(0);
  containers::CircularBuffer* source = (containers::CircularBuffer*)cptr;

  if (source)
  {
    result = new containers::CircularBuffer(*source);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::copyConstructor: "
        "CircularBuffer object is released already");
  }

  return (jlong)result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_freeCircularBuffer
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_CircularBuffer_jni_1freeCircularBuffer(
    JNIEnv*, jclass, jlong cptr)
{
  delete (containers::CircularBuffer*)cptr;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_addRecord
 * Signature: (JJ)V
 */
void JNICALL Java_ai_madara_knowledge_containers_CircularBuffer_jni_1addRecord(
    JNIEnv* env, jobject, jlong cptr, jlong record_ptr)
{
  containers::CircularBuffer* current = (containers::CircularBuffer*)cptr;

  if (current)
  {
    KnowledgeRecord* record = (KnowledgeRecord*)record_ptr;

    if (record)
    {
      current->add(*record);
    }
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::add: "
        "CircularBuffer object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_addRecordVector
 * Signature: (J[J)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_CircularBuffer_jni_1addRecordVector(
    JNIEnv* env, jobject, jlong cptr, jlongArray records_array)
{
  containers::CircularBuffer* current = (containers::CircularBuffer*)cptr;

  if (current)
  {
    jsize len = env->GetArrayLength(records_array);
    jboolean isCopy;
    std::vector<KnowledgeRecord> record_vector(len);
    jlong* intArray = env->GetLongArrayElements(records_array, &isCopy);

    // convert from the ptr array into knowledge records
    for (int i = 0; i < len; i++)
      record_vector[i] = *(KnowledgeRecord*)intArray[i];

    if (len > 0)
    {
      current->add(record_vector);
    }
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::add: "
        "CircularBuffer object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_getRecord
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_CircularBuffer_jni_1getRecord(
    JNIEnv* env, jobject, jlong cptr)
{
  jlong result(0);
  containers::CircularBuffer* current = (containers::CircularBuffer*)cptr;

  if (current)
  {
    result = (jlong) new KnowledgeRecord(current->get());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::get: "
        "CircularBuffer object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_getEarliestRecordVector
 * Signature: (JI)[Ljava/lang/Object
{
}
 */
jobjectArray JNICALL
Java_ai_madara_knowledge_containers_CircularBuffer_jni_1getEarliestRecordVector(
    JNIEnv* env, jobject, jlong cptr, jint count)
{
  jclass kr_class = madara::utility::java::find_class(
      env, "ai/madara/knowledge/KnowledgeRecord");
  jobjectArray list = 0;

  if (kr_class && cptr != 0)
  {
    jmethodID method = env->GetStaticMethodID(
        kr_class, "fromPointer", "(J)Lai/madara/knowledge/KnowledgeRecord;");

    CircularBuffer* current = (CircularBuffer*)cptr;
    std::vector<KnowledgeRecord> records = current->get_earliest(count);
    jsize size = (jsize)records.size();

    list = env->NewObjectArray((jsize)records.size(), kr_class, 0);

    if (method)
    {
      for (jsize i = 0; i < size; ++i)
      {
        jobject result = env->CallStaticObjectMethod(
            kr_class, method, (jlong)records[i].clone());

        env->SetObjectArrayElement(list, i, result);

        env->DeleteLocalRef(result);
      }
    }
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
    env->DeleteWeakGlobalRef(kr_class);

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::get_earliest: "
        "CircularBuffer object is released already");
  }

  env->DeleteWeakGlobalRef(kr_class);

  return list;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_getLatestRecordVector
 * Signature: (JI)[Ljava/lang/Object
{
}
 */
jobjectArray JNICALL
Java_ai_madara_knowledge_containers_CircularBuffer_jni_1getLatestRecordVector(
    JNIEnv* env, jobject, jlong cptr, jint count)
{
  jclass kr_class = madara::utility::java::find_class(
      env, "ai/madara/knowledge/KnowledgeRecord");
  jobjectArray list = 0;

  if (kr_class && cptr != 0)
  {
    jmethodID method = env->GetStaticMethodID(
        kr_class, "fromPointer", "(J)Lai/madara/knowledge/KnowledgeRecord;");

    CircularBuffer* current = (CircularBuffer*)cptr;
    std::vector<KnowledgeRecord> records = current->get_latest(count);
    jsize size = (jsize)records.size();

    list = env->NewObjectArray((jsize)records.size(), kr_class, 0);

    if (method)
    {
      for (jsize i = 0; i < size; ++i)
      {
        jobject result = env->CallStaticObjectMethod(
            kr_class, method, (jlong)records[i].clone());

        env->SetObjectArrayElement(list, i, result);

        env->DeleteLocalRef(result);
      }
    }
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
    env->DeleteWeakGlobalRef(kr_class);

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::get_latest: "
        "CircularBuffer object is released already");
  }

  env->DeleteWeakGlobalRef(kr_class);

  return list;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_inspectRecord
 * Signature: (JI)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_CircularBuffer_jni_1inspectRecord(
    JNIEnv* env, jobject, jlong cptr, jint position)
{
  jlong result(0);
  containers::CircularBuffer* current = (containers::CircularBuffer*)cptr;

  if (current)
  {
    result = (jlong) new KnowledgeRecord(current->inspect(position));
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::inspect: "
        "CircularBuffer object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_inspectRecordVector
 * Signature: (JII)[Ljava/lang/Object
{
}
 */
jobjectArray JNICALL
Java_ai_madara_knowledge_containers_CircularBuffer_jni_1inspectRecordVector(
    JNIEnv* env, jobject, long cptr, int position, int count)
{
  jclass kr_class = madara::utility::java::find_class(
      env, "ai/madara/knowledge/KnowledgeRecord");
  jobjectArray list = 0;

  if (kr_class && cptr != 0)
  {
    jmethodID method = env->GetStaticMethodID(
        kr_class, "fromPointer", "(J)Lai/madara/knowledge/KnowledgeRecord;");

    CircularBuffer* current = (CircularBuffer*)cptr;
    std::vector<KnowledgeRecord> records =
        current->inspect(position, (size_t)count);
    jsize size = (jsize)records.size();

    list = env->NewObjectArray((jsize)records.size(), kr_class, 0);

    if (method)
    {
      for (jsize i = 0; i < size; ++i)
      {
        jobject result = env->CallStaticObjectMethod(
            kr_class, method, (jlong)records[i].clone());

        env->SetObjectArrayElement(list, i, result);

        env->DeleteLocalRef(result);
      }
    }
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
    env->DeleteWeakGlobalRef(kr_class);

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::inspect: "
        "CircularBuffer object is released already");
  }

  env->DeleteWeakGlobalRef(kr_class);

  return list;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String
{
}
 */
jstring JNICALL Java_ai_madara_knowledge_containers_CircularBuffer_jni_1getName(
    JNIEnv* env, jobject, jlong cptr)
{
  jstring result = 0;
  containers::CircularBuffer* current = (containers::CircularBuffer*)cptr;

  if (current)
  {
    result = env->NewStringUTF(current->get_name().c_str());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::getName: "
        "CircularBuffer object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String
{
})V
 */
void JNICALL Java_ai_madara_knowledge_containers_CircularBuffer_jni_1setName(
    JNIEnv* env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  containers::CircularBuffer* current = (containers::CircularBuffer*)cptr;

  if (current)
  {
    const char* str_name = env->GetStringUTFChars(name, 0);

    if (type == 0)
    {
      knowledge::KnowledgeBase* kb = (knowledge::KnowledgeBase*)context;

      current->set_name(str_name, *kb);
    }
    else if (type == 1)
    {
      knowledge::Variables* vars = (knowledge::Variables*)context;

      current->set_name(str_name, *vars);
    }

    env->ReleaseStringUTFChars(name, str_name);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::setName: "
        "CircularBuffer object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_size
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_CircularBuffer_jni_1size(
    JNIEnv* env, jobject, jlong cptr)
{
  jlong result(0);
  containers::CircularBuffer* current = (containers::CircularBuffer*)cptr;

  if (current)
  {
    result = (jlong)current->size();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::size: "
        "CircularBuffer object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_clear
 * Signature: ()V
 */
void JNICALL Java_ai_madara_knowledge_containers_CircularBuffer_jni_1clear(
    JNIEnv* env, jobject, jlong cptr)
{
  containers::CircularBuffer* current = (containers::CircularBuffer*)cptr;

  if (current)
  {
    current->clear();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::clear: "
        "CircularBuffer object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_count
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_CircularBuffer_jni_1count(
    JNIEnv* env, jobject, jlong cptr)
{
  jlong result(0);
  containers::CircularBuffer* current = (containers::CircularBuffer*)cptr;

  if (current)
  {
    result = (jlong)current->count();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::count: "
        "CircularBuffer object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_resize
 * Signature: (JJ)V
 */
void JNICALL Java_ai_madara_knowledge_containers_CircularBuffer_jni_1resize(
    JNIEnv* env, jobject, jlong cptr, jlong new_size)
{
  containers::CircularBuffer* current = (containers::CircularBuffer*)cptr;

  if (current)
  {
    current->resize(new_size);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBuffer::resize: "
        "CircularBuffer object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBuffer
 * Method:    jni_setSettings
 * Signature: (JJ)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_CircularBuffer_jni_1setSettings(
    JNIEnv* env, jobject, jlong cptr, jlong settings_ptr)
{
  containers::CircularBuffer* current = (containers::CircularBuffer*)cptr;
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
        "CircularBuffer::setSettings: "
        "CircularBuffer or settings objects are released already");
  }
}
