
#include "com_madara_containers_Map.h"
#include "madara/knowledge_engine/containers/Map.h"

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;

/*
 * Class:     com_madara_containers_Map
 * Method:    jni_Map
 * Signature: ()J
 */
jlong JNICALL Java_com_madara_containers_Map_jni_1Map__
  (JNIEnv * env, jobject)
{
  return (jlong) new containers::Map ();
}

/*
 * Class:     com_madara_containers_Map
 * Method:    jni_Map
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_Map_jni_1Map__J
  (JNIEnv * env, jobject, jlong cptr)
{
  return (jlong) new containers::Map (*(containers::Map *)cptr);
}

/*
 * Class:     com_madara_containers_Map
 * Method:    jni_freeMap
 * Signature: (J)V
 */
void JNICALL Java_com_madara_containers_Map_jni_1freeMap
  (JNIEnv * env, jclass, jlong cptr)
{
  delete (containers::Map *) cptr;
}

/*
 * Class:     com_madara_containers_Map
 * Method:    jni_set
 * Signature: (JLjava/lang/String;Ljava/lang/String;)V
 */
void JNICALL Java_com_madara_containers_Map_jni_1set__JLjava_lang_String_2Ljava_lang_String_2
  (JNIEnv * env, jobject, jlong cptr, jstring key, jstring value)
{
  containers::Map * current = (containers::Map *) cptr;
  if (current)
  {
    const char * str_key = env->GetStringUTFChars(key, 0);
    const char * str_value = env->GetStringUTFChars(value, 0);

    current->set (str_key, str_value);

    env->ReleaseStringUTFChars(key, str_key);
    env->ReleaseStringUTFChars(value, str_value);
  }
}

/*
 * Class:     com_madara_containers_Map
 * Method:    jni_set
 * Signature: (JLjava/lang/String;D)V
 */
void JNICALL Java_com_madara_containers_Map_jni_1set__JLjava_lang_String_2D
  (JNIEnv * env, jobject, jlong cptr, jstring key, jdouble value)
{
  containers::Map * current = (containers::Map *) cptr;
  if (current)
  {
    const char * str_key = env->GetStringUTFChars(key, 0);

    current->set (str_key, value);

    env->ReleaseStringUTFChars(key, str_key);
  }
}

/*
 * Class:     com_madara_containers_Map
 * Method:    jni_set
 * Signature: (JLjava/lang/String;JJ)V
 */
void JNICALL Java_com_madara_containers_Map_jni_1set__JLjava_lang_String_2JJ
  (JNIEnv * env, jobject, jlong cptr, jstring key, jlong type, jlong value)
{
  containers::Map * current = (containers::Map *) cptr;
  if (current)
  {
    const char * str_key = env->GetStringUTFChars(key, 0);
    
    if (type == 0)
    {
      current->set (str_key, (Madara::Knowledge_Record::Integer)value);
    }
    else
    {
      Madara::Knowledge_Record * record = (Madara::Knowledge_Record *) cptr;
      
      if (record->type () == Madara::Knowledge_Record::DOUBLE)
      {
        current->set (str_key, record->to_double ());
      }
      else if (record->type () == Madara::Knowledge_Record::DOUBLE_ARRAY)
      {
        current->set (str_key, record->to_doubles ());
      }
      else if (record->type () == Madara::Knowledge_Record::INTEGER)
      {
        current->set (str_key, record->to_integer ());
      }
      else if (record->type () == Madara::Knowledge_Record::INTEGER_ARRAY)
      {
        current->set (str_key, record->to_integers ());
      }
      else if (record->is_binary_file_type ())
      {
        size_t size;
        unsigned char * buffer = record->to_unmanaged_buffer (size);
        current->set_file (str_key, buffer, size);
        delete [] buffer;
      }
      else if (record->type () == Madara::Knowledge_Record::STRING)
      {
        current->set (str_key, record->to_string ());
      }
      else if (record->is_string_type ())
      {
        current->set (str_key, record->to_string ());
      }
    }

    env->ReleaseStringUTFChars(key, str_key);
  }
}

/*
 * Class:     com_madara_containers_Map
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_com_madara_containers_Map_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;

  containers::Map * current = (containers::Map *) cptr;
  if (current)
    result = env->NewStringUTF(current->get_name ().c_str ());

  return result;
}

/*
 * Class:     com_madara_containers_Map
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
void JNICALL Java_com_madara_containers_Map_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  containers::Map * current = (containers::Map *) cptr;

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
 * Class:     com_madara_containers_Map
 * Method:    jni_get
 * Signature: (JI)J
 */
jlong JNICALL Java_com_madara_containers_Map_jni_1get
  (JNIEnv * env, jobject, jlong cptr, jstring key)
{  
  Madara::Knowledge_Record * result (0);

  containers::Map * current = (containers::Map *) cptr;
  if (current)
  {
    const char * str_key = env->GetStringUTFChars(key, 0);

    result = new Madara::Knowledge_Record ((*current) [str_key]);

    env->ReleaseStringUTFChars(key, str_key);
  }

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_Map
 * Method:    jni_toRecord
 * Signature: (JI)J
 */
jlong JNICALL Java_com_madara_containers_Map_jni_1toRecord
  (JNIEnv * env, jobject, jlong cptr, jstring key)
{
  Madara::Knowledge_Record * result (0);

  containers::Map * current = (containers::Map *) cptr;
  if (current)
  {
    const char * str_key = env->GetStringUTFChars(key, 0);

    result = new Madara::Knowledge_Record ((*current) [str_key]);

    env->ReleaseStringUTFChars(key, str_key);
  }

  return (jlong) result;
}
