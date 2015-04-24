#include "com_madara_containers_Queue.h"
#include "madara/knowledge_engine/containers/Queue.h"

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
typedef Madara::Knowledge_Record    Knowledge_Record;
typedef Knowledge_Record::Integer   Integer;

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_Queue
 * Signature: ()J
 */
jlong JNICALL Java_com_madara_containers_Queue_jni_1Queue__
  (JNIEnv *, jobject)
{
  return (jlong) new containers::Queue ();
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_Queue
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_Queue_jni_1Queue__J
  (JNIEnv *, jobject, jlong cptr)
{
  containers::Queue * result (0);
  containers::Queue * source = (containers::Queue *) cptr;

  if (source)
  {
    result = new containers::Queue (*source);
  }

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_freeQueue
 * Signature: (J)V
 */
void JNICALL Java_com_madara_containers_Queue_jni_1freeQueue
  (JNIEnv *, jclass, jlong cptr)
{
  delete (containers::Queue *) cptr;
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_enqueue
 * Signature: (JJ)Z
 */
jboolean JNICALL Java_com_madara_containers_Queue_jni_1enqueue
  (JNIEnv *, jobject, jlong cptr, jlong record_ptr)
{
  jboolean result (false);
  containers::Queue * current = (containers::Queue *)cptr;

  if (current)
  {
    Knowledge_Record * record =
      (Knowledge_Record *)record_ptr;

    if (record)
    {
      result = current->enqueue (*record);
    }
  }

  return result;
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_enqueueDouble
 * Signature: (JD)Z
 */
jboolean JNICALL Java_com_madara_containers_Queue_jni_1enqueueDouble
  (JNIEnv *, jobject, jlong cptr, jdouble value)
{
  jboolean result (false);
  containers::Queue * current = (containers::Queue *)cptr;

  if (current)
  {
    result = current->enqueue (Knowledge_Record (value));
  }

  return result;
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_enqueueLong
 * Signature: (JJ)Z
 */
jboolean JNICALL Java_com_madara_containers_Queue_jni_1enqueueLong
  (JNIEnv *, jobject, jlong cptr, jlong value)
{
  jboolean result (false);
  containers::Queue * current = (containers::Queue *)cptr;

  if (current)
  {
    result = current->enqueue (Knowledge_Record (Integer (value)));
  }

  return result;
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_enqueueString
 * Signature: (JLcom/madara/containers/String;)Z
 */
jboolean JNICALL Java_com_madara_containers_Queue_jni_1enqueueString
  (JNIEnv * env, jobject, jlong cptr, jstring value)
{
  jboolean result (false);
  containers::Queue * current = (containers::Queue *)cptr;

  if (current)
  {
    const char * str_value = env->GetStringUTFChars (value, 0);

    result = current->enqueue (Knowledge_Record (str_value));

    env->ReleaseStringUTFChars (value, str_value);
  }

  return result;
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_dequeue
 * Signature: (JZ)J
 */
jlong JNICALL Java_com_madara_containers_Queue_jni_1dequeue
  (JNIEnv *, jobject, jlong cptr, jboolean wait)
{
  jlong result (0);
  containers::Queue * current = (containers::Queue *)cptr;

  if (current)
  {
    result = (jlong) new Knowledge_Record (current->dequeue (wait));
  }

  return result;
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_inspect
 * Signature: (JI)J
 */
jlong JNICALL Java_com_madara_containers_Queue_jni_1inspect
  (JNIEnv *, jobject, jlong cptr, jint position)
{
  jlong result (0);
  containers::Queue * current = (containers::Queue *)cptr;

  if (current)
  {
    result = (jlong) new Knowledge_Record (current->inspect (position));
  }

  return result;
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String
{
}
 */
jstring JNICALL Java_com_madara_containers_Queue_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  containers::Queue * current = (containers::Queue *) cptr;

  if (current)
  {
    result = env->NewStringUTF (current->get_name ().c_str ());
  }

  return result;
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String
{
})V
 */
void JNICALL Java_com_madara_containers_Queue_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  containers::Queue * current = (containers::Queue *) cptr;

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
 * Class:     com_madara_containers_Queue
 * Method:    jni_size
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_Queue_jni_1size
  (JNIEnv *, jobject, jlong cptr)
{
  jlong result (0);
  containers::Queue * current = (containers::Queue *)cptr;

  if (current)
  {
    result = (jlong) current->size ();
  }

  return result;
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_count
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_Queue_jni_1count
  (JNIEnv *, jobject, jlong cptr)
{
  jlong result (0);
  containers::Queue * current = (containers::Queue *)cptr;

  if (current)
  {
    result = (jlong) current->count ();
  }

  return result;
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_resize
 * Signature: (JJ)V
 */
void JNICALL Java_com_madara_containers_Queue_jni_1resize
  (JNIEnv *, jobject, jlong cptr, jlong new_size)
{
  containers::Queue * current = (containers::Queue *)cptr;

  if (current)
  {
    current->resize (new_size);
  }
}

/*
 * Class:     com_madara_containers_Queue
 * Method:    jni_clear
 * Signature: (J)V
 */
void JNICALL Java_com_madara_containers_Queue_jni_1clear
  (JNIEnv *, jobject, jlong cptr)
{
  containers::Queue * current = (containers::Queue *)cptr;

  if (current)
  {
    current->clear ();
  }
}
