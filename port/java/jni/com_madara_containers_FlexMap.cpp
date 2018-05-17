#include "com_madara_containers_FlexMap.h"
#include "madara/knowledge/containers/FlexMap.h"
#include "madara_jni.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef knowledge::KnowledgeRecord          KnowledgeRecord;
typedef KnowledgeRecord::Integer            Integer;

jlong JNICALL
Java_com_madara_containers_FlexMap_jni_1FlexMap__
  (JNIEnv *, jobject)
{
  return (jlong) new containers::FlexMap ();
}


jlong JNICALL
Java_com_madara_containers_FlexMap_jni_1FlexMap__J
  (JNIEnv *, jobject, jlong cptr)
{
  containers::FlexMap * result (0);
  containers::FlexMap * source = (containers::FlexMap *) cptr;

  if (source)
  {
    result = new containers::FlexMap (*source);
  }

  return (jlong)result;
}


void JNICALL
Java_com_madara_containers_FlexMap_jni_1freeFlexMap
  (JNIEnv *, jclass, jlong cptr)
{
  delete (containers::FlexMap *) cptr;
}


void JNICALL
Java_com_madara_containers_FlexMap_jni_1clear
  (JNIEnv *, jobject, jlong cptr)
{
  containers::FlexMap * current = (containers::FlexMap *) cptr;

  if (current)
  {
    current->clear ();
  }
}


void JNICALL
Java_com_madara_containers_FlexMap_jni_1erase
  (JNIEnv * env, jobject, jlong cptr, jstring key)
{
  containers::FlexMap * current = (containers::FlexMap *) cptr;

  if (current)
  {
    const char * str_key = env->GetStringUTFChars (key, 0);

    current->erase (str_key);

    env->ReleaseStringUTFChars (key, str_key);
  }
}

void JNICALL
Java_com_madara_containers_FlexMap_jni_1setString
  (JNIEnv * env, jobject, jlong cptr, jstring value)
{
  containers::FlexMap * current = (containers::FlexMap *) cptr;
  if (current)
  {
    const char * str_value = env->GetStringUTFChars (value, 0);

    current->set (str_value);

    env->ReleaseStringUTFChars (value, str_value);
  }
}

void JNICALL
Java_com_madara_containers_FlexMap_jni_1setDouble
  (JNIEnv *, jobject, jlong cptr, jdouble value)
{
  containers::FlexMap * current = (containers::FlexMap *) cptr;

  if (current)
  {
    current->set ((double)value);
  }
}


void JNICALL
Java_com_madara_containers_FlexMap_jni_1set
  (JNIEnv *, jobject, jlong cptr, jlong type, jlong value)
{
  containers::FlexMap * current = (containers::FlexMap *) cptr;

  if (current)
  {
    // integer set
    if (type == 0)
    {
      current->set ((KnowledgeRecord::Integer)value);
    }
    // knowledge record set
    else
    {
      KnowledgeRecord * record = (KnowledgeRecord *)value;

      if (record)
      {
        // check the type and set accordingly
        if (record->type () == KnowledgeRecord::DOUBLE)
        {
          current->set (record->to_double ());
        }
        else if (record->type () == KnowledgeRecord::DOUBLE_ARRAY)
        {
          current->set (record->to_doubles ());
        }
        else if (record->type () == KnowledgeRecord::INTEGER)
        {
          current->set (record->to_integer ());
        }
        else if (record->type () == KnowledgeRecord::INTEGER_ARRAY)
        {
          current->set (record->to_integers ());
        }
        else if (record->is_binary_file_type ())
        {
          size_t size;
          unsigned char * buffer = record->to_unmanaged_buffer (size);
          current->set_file (buffer, size);
          delete[] buffer;
        }
        else if (record->type () == KnowledgeRecord::STRING)
        {
          current->set (record->to_string ());
        }
        else if (record->is_string_type ())
        {
          current->set (record->to_string ());
        }
      }
    }
  }
}


jstring JNICALL
Java_com_madara_containers_FlexMap_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  containers::FlexMap * current = (containers::FlexMap *) cptr;

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

void JNICALL
Java_com_madara_containers_FlexMap_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  containers::FlexMap * current = (containers::FlexMap *) cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars (name, 0);

    if (type == 0)
    {
      knowledge::KnowledgeBase * kb = (knowledge::KnowledgeBase *) context;
      current->set_name (str_name, *kb);
    }
    else if (type == 1)
    {
      knowledge::Variables * vars = (knowledge::Variables *) context;
      current->set_name (str_name, *vars);
    }

    env->ReleaseStringUTFChars (name, str_name);
  }
}


jlong JNICALL
Java_com_madara_containers_FlexMap_jni_1get
  (JNIEnv * env, jobject, jlong cptr, jstring key)
{
  containers::FlexMap * result (0);
  containers::FlexMap * current = (containers::FlexMap *) cptr;

  if (current)
  {
    const char * str_key = env->GetStringUTFChars (key, 0);

    result = new containers::FlexMap ((*current)[str_key]);

    env->ReleaseStringUTFChars (key, str_key);
  }

  return (jlong)result;
}

jlong JNICALL
Java_com_madara_containers_FlexMap_jni_1getIndex
(JNIEnv *, jobject, jlong cptr, jint index)
{
  containers::FlexMap * result (0);
  containers::FlexMap * current = (containers::FlexMap *) cptr;

  if (current)
  {

    result = new containers::FlexMap ((*current)[index]);

  }

  return (jlong)result;
}

jlong JNICALL
Java_com_madara_containers_FlexMap_jni_1toRecord
  (JNIEnv *, jobject, jlong cptr)
{
  KnowledgeRecord * result (0);
  containers::FlexMap * current = (containers::FlexMap *) cptr;

  if (current)
  {
    result = new KnowledgeRecord (current->to_record ());
  }

  return (jlong)result;
}


void JNICALL
Java_com_madara_containers_FlexMap_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  containers::FlexMap * current = (containers::FlexMap *) cptr;

  if (current)
  {
    current->modify ();
  }
}

jlong JNICALL
Java_com_madara_containers_FlexMap_jni_1toMapContainer
  (JNIEnv *, jobject, jlong cptr)
{
  containers::FlexMap * current = (containers::FlexMap *) cptr;
  containers::Map * result = new containers::Map ();

  if (current)
  {
    current->to_container (*result);
  }

  return (jlong) result;
}

jstring JNICALL
Java_com_madara_containers_FlexMap_jni_1getDelimiter
(JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  containers::FlexMap * current = (containers::FlexMap *) cptr;

  if (current)
  {
    result = env->NewStringUTF (current->get_delimiter ().c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

void JNICALL
Java_com_madara_containers_FlexMap_jni_1setDelimiter
(JNIEnv * env, jobject, jlong cptr, jstring delimiter)
{
  containers::FlexMap * current = (containers::FlexMap *) cptr;

  if (current)
  {
    const char * str_delimiter = env->GetStringUTFChars (delimiter, 0);

    current->set_delimiter (str_delimiter);

    env->ReleaseStringUTFChars (delimiter, str_delimiter);
  }
}


jobjectArray JNICALL
Java_com_madara_containers_FlexMap_jni_1keys
(JNIEnv * env, jobject, jlong cptr, jboolean first_level)
{
  jobjectArray result (0);
  containers::FlexMap * current = (containers::FlexMap *)cptr;

  if (current)
  {
    std::vector<std::string> keys;
    current->keys (keys, first_level);

    jclass string_class = madara::utility::java::find_class (
      env, "java/lang/String");
    jstring empty_string = env->NewStringUTF ("");

    result = env->NewObjectArray (
      (jsize)keys.size (),
      string_class,
      empty_string);

    for (unsigned int i = 0; i < keys.size (); i++)
    {
      jstring temp_string = env->NewStringUTF (keys[i].c_str ());

      env->SetObjectArrayElement (
        result, i, temp_string);

      env->DeleteLocalRef (temp_string);
    }

    env->DeleteLocalRef (empty_string);
    env->DeleteWeakGlobalRef (string_class);
  }

  return result;
}

void JNICALL
Java_com_madara_containers_FlexMap_jni_1setSettings
(JNIEnv *, jobject, jlong cptr, jlong settings_ptr)
{
  containers::FlexMap * current = (containers::FlexMap *)cptr;
  knowledge::KnowledgeUpdateSettings * settings =
    (knowledge::KnowledgeUpdateSettings *)settings_ptr;

  if (current && settings)
  {
    current->set_settings (*settings);
  }
}

jboolean JNICALL
Java_com_madara_containers_FlexMap_jni_1isTrue
(JNIEnv *, jobject, jlong cptr)
{
  containers::FlexMap * current = (containers::FlexMap *)cptr;
  bool result (true);

  if (current)
  {
    result = current->is_true ();
  }

  return result;
}


jboolean JNICALL
Java_com_madara_containers_FlexMap_jni_1isFalse
(JNIEnv *, jobject, jlong cptr)
{
  containers::FlexMap * current = (containers::FlexMap *)cptr;
  bool result (true);

  if (current)
  {
    result = current->is_false ();
  }

  return result;
}
