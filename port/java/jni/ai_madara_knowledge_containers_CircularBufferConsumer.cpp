#include "madara/knowledge/containers/CircularBufferConsumer.h"
#include "ai_madara_knowledge_containers_CircularBufferConsumer.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef knowledge::KnowledgeRecord KnowledgeRecord;
typedef KnowledgeRecord::Integer Integer;
typedef containers::CircularBufferConsumer CircularBufferConsumer;

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_CircularBufferConsumer
 * Signature: ()J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1CircularBufferConsumer__(
    JNIEnv*, jobject)
{
  return (jlong) new CircularBufferConsumer();
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_CircularBufferConsumer
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1CircularBufferConsumer__J(
    JNIEnv* env, jobject, jlong cptr)
{
  CircularBufferConsumer* result(0);
  CircularBufferConsumer* source = (CircularBufferConsumer*)cptr;

  if(source)
  {
    result = new CircularBufferConsumer(*source);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBufferConsumer::copyConstructor: "
        "CircularBufferConsumer object is released already");
  }

  return (jlong)result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_freeCircularBufferConsumer
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1freeCircularBufferConsumer(
    JNIEnv*, jclass, jlong cptr)
{
  delete (CircularBufferConsumer*)cptr;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_consumeRecord
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1consumeRecord(
    JNIEnv* env, jobject, jlong cptr)
{
  jlong result(0);
  CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;

  if(current)
  {
    result = (jlong) new KnowledgeRecord(current->consume());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBufferConsumer::consume: "
        "CircularBufferConsumer object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_consumeEarliestRecordVector
 * Signature: (JI)[Ljava/lang/Object;
 */
jobjectArray JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1consumeEarliestRecordVector(
    JNIEnv* env, jobject, jlong cptr, jint count)
{
  jclass kr_class = madara::utility::java::find_class(
      env, "ai/madara/knowledge/KnowledgeRecord");
  jobjectArray list = 0;

  if(kr_class && cptr != 0)
  {
    jmethodID method = env->GetStaticMethodID(
        kr_class, "fromPointer", "(J)Lai/madara/knowledge/KnowledgeRecord;");

    CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;
    std::vector<KnowledgeRecord> records = current->consume_earliest(count);
    jsize size = (jsize)records.size();

    list = env->NewObjectArray((jsize)records.size(), kr_class, 0);

    if(method)
    {
      for(jsize i = 0; i < size; ++i)
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
        "CircularBufferConsumer::consume_earliest: "
        "CircularBufferConsumer object is released already");
  }

  env->DeleteWeakGlobalRef(kr_class);

  return list;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_consumeLatestRecordVector
 * Signature: (JI)[Ljava/lang/Object;
 */
jobjectArray JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1consumeLatestRecordVector(
    JNIEnv* env, jobject, jlong cptr, jint count)
{
  jclass kr_class = madara::utility::java::find_class(
      env, "ai/madara/knowledge/KnowledgeRecord");
  jobjectArray list = 0;

  if(kr_class && cptr != 0)
  {
    jmethodID method = env->GetStaticMethodID(
        kr_class, "fromPointer", "(J)Lai/madara/knowledge/KnowledgeRecord;");

    CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;
    std::vector<KnowledgeRecord> records = current->consume_latest(count);
    jsize size = (jsize)records.size();

    list = env->NewObjectArray((jsize)records.size(), kr_class, 0);

    if(method)
    {
      for(jsize i = 0; i < size; ++i)
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
        "CircularBufferConsumer::consume_latest: "
        "CircularBufferConsumer object is released already");
  }

  env->DeleteWeakGlobalRef(kr_class);

  return list;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_inspectRecord
 * Signature: (JI)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1inspectRecord(
    JNIEnv* env, jobject, jlong cptr, jint position)
{
  jlong result(0);
  CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;

  if(current)
  {
    result = (jlong) new KnowledgeRecord(current->inspect(position));
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBufferConsumer::inspect: "
        "CircularBufferConsumer object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_inspectRecordVector
 * Signature: (JII)[Ljava/lang/Object;
 */
jobjectArray JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1inspectRecordVector(
    JNIEnv* env, jobject, long cptr, int position, int count)
{
  jclass kr_class = madara::utility::java::find_class(
      env, "ai/madara/knowledge/KnowledgeRecord");
  jobjectArray list = 0;

  if(kr_class && cptr != 0)
  {
    jmethodID method = env->GetStaticMethodID(
        kr_class, "fromPointer", "(J)Lai/madara/knowledge/KnowledgeRecord;");

    CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;
    std::vector<KnowledgeRecord> records =
        current->inspect(position, (size_t)count);
    jsize size = (jsize)records.size();

    list = env->NewObjectArray((jsize)records.size(), kr_class, 0);

    if(method)
    {
      for(jsize i = 0; i < size; ++i)
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
        "CircularBufferConsumer::inspect: "
        "CircularBufferConsumer object is released already");
  }

  env->DeleteWeakGlobalRef(kr_class);

  return list;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_peekRecord
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1peekRecord(
    JNIEnv* env, jobject, jlong cptr)
{
  jlong result(0);
  CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;

  if(current)
  {
    result = (jlong) new KnowledgeRecord(current->peek_latest());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBufferConsumer::peek: "
        "CircularBufferConsumer object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_peekRecordVector
 * Signature: (JI)[Ljava/lang/Object;
 */
jobjectArray JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1peekRecordVector(
    JNIEnv* env, jobject, jlong cptr, jint count)
{
  jclass kr_class = madara::utility::java::find_class(
      env, "ai/madara/knowledge/KnowledgeRecord");
  jobjectArray list = 0;

  if(kr_class && cptr != 0)
  {
    jmethodID method = env->GetStaticMethodID(
        kr_class, "fromPointer", "(J)Lai/madara/knowledge/KnowledgeRecord;");

    CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;
    std::vector<KnowledgeRecord> records = current->peek_latest((size_t)count);
    jsize size = (jsize)records.size();

    list = env->NewObjectArray((jsize)records.size(), kr_class, 0);

    if(method)
    {
      for(jsize i = 0; i < size; ++i)
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
        "CircularBufferConsumer::peek_latest: "
        "CircularBufferConsumer object is released already");
  }

  env->DeleteWeakGlobalRef(kr_class);

  return list;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1getName(
    JNIEnv* env, jobject, jlong cptr)
{
  jstring result = 0;
  CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;

  if(current)
  {
    result = env->NewStringUTF(current->get_name().c_str());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBufferConsumer::getName: "
        "CircularBufferConsumer object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1setName(
    JNIEnv* env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;

  if(current)
  {
    const char* str_name = env->GetStringUTFChars(name, 0);

    if(type == 0)
    {
      knowledge::KnowledgeBase* kb = (knowledge::KnowledgeBase*)context;

      current->set_name(str_name, *kb);
    }
    else if(type == 1)
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
        "CircularBufferConsumer::setName: "
        "CircularBufferConsumer object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_remaining
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1remaining(
    JNIEnv* env, jobject, jlong cptr)
{
  jlong result(0);
  CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;

  if(current)
  {
    result = (jlong)current->remaining();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBufferConsumer::remaining: "
        "CircularBufferConsumer object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_resync
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1resync(
    JNIEnv* env, jobject, jlong cptr)
{
  CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;

  if(current)
  {
    current->resync();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBufferConsumer::resync: "
        "CircularBufferConsumer object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_resync
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1setIndex(
    JNIEnv* env, jobject, jlong cptr, jlong index)
{
  CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;

  if(current)
  {
    current->set_index(index);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBufferConsumer::setIndex: "
        "CircularBufferConsumer object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_size
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1size(
    JNIEnv* env, jobject, jlong cptr)
{
  jlong result(0);
  CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;

  if(current)
  {
    result = (jlong)current->size();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBufferConsumer::size: "
        "CircularBufferConsumer object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_count
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1count(
    JNIEnv* env, jobject, jlong cptr)
{
  jlong result(0);
  CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;

  if(current)
  {
    result = (jlong)current->count();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBufferConsumer::count: "
        "CircularBufferConsumer object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_CircularBufferConsumer
 * Method:    jni_resize
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_CircularBufferConsumer_jni_1resize(
    JNIEnv* env, jobject, jlong cptr)
{
  CircularBufferConsumer* current = (CircularBufferConsumer*)cptr;

  if(current)
  {
    current->resize();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "CircularBufferConsumer::resize: "
        "CircularBufferConsumer object is released already");
  }
}
