#include "com_madara_containers_FlexMap.h"
#include "madara/knowledge_engine/containers/Flex_Map.h"
#include "madara_jni.h"

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
typedef Madara::Knowledge_Record  Knowledge_Record;
typedef Knowledge_Record::Integer Integer;

jlong JNICALL Java_com_madara_containers_FlexMap_jni_1FlexMap__
  (JNIEnv *, jobject)
{
  return (jlong) new containers::Flex_Map ();
}


jlong JNICALL Java_com_madara_containers_FlexMap_jni_1FlexMap__J
  (JNIEnv *, jobject, jlong cptr)
{
  containers::Flex_Map * result (0);
  containers::Flex_Map * source = (containers::Flex_Map *) cptr;

  if (source)
  {
    result = new containers::Flex_Map (*source);
  }

  return (jlong)result;
}


void JNICALL Java_com_madara_containers_FlexMap_jni_1freeFlexMap
  (JNIEnv *, jclass, jlong cptr)
{
  delete (containers::Flex_Map *) cptr;
}


void JNICALL Java_com_madara_containers_FlexMap_jni_1clear
  (JNIEnv *, jobject, jlong cptr)
{
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;

  if (current)
  {
    current->clear ();
  }
}


void JNICALL Java_com_madara_containers_FlexMap_jni_1erase
  (JNIEnv * env, jobject, jlong cptr, jstring key)
{
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;

  if (current)
  {
    const char * str_key = env->GetStringUTFChars (key, 0);

    current->erase (str_key);

    env->ReleaseStringUTFChars (key, str_key);
  }
}

void JNICALL Java_com_madara_containers_FlexMap_jni_1setString
  (JNIEnv * env, jobject, jlong cptr, jstring value)
{
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;
  if (current)
  {
    const char * str_value = env->GetStringUTFChars (value, 0);

    current->set (str_value);

    env->ReleaseStringUTFChars (value, str_value);
  }
}

void JNICALL Java_com_madara_containers_FlexMap_jni_1setDouble
  (JNIEnv *, jobject, jlong cptr, jdouble value)
{
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;

  if (current)
  {
    current->set ((double)value);
  }
}


void JNICALL Java_com_madara_containers_FlexMap_jni_1set
  (JNIEnv *, jobject, jlong cptr, jlong type, jlong value)
{
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;

  if (current)
  {
    // integer set
    if (type == 0)
    {
      current->set ((Knowledge_Record::Integer)value);
    }
    // knowledge record set
    else
    {
      Knowledge_Record * record = (Knowledge_Record *)value;

      if (record)
      {
        // check the type and set accordingly
        if (record->type () == Knowledge_Record::DOUBLE)
        {
          current->set (record->to_double ());
        }
        else if (record->type () == Knowledge_Record::DOUBLE_ARRAY)
        {
          current->set (record->to_doubles ());
        }
        else if (record->type () == Knowledge_Record::INTEGER)
        {
          current->set (record->to_integer ());
        }
        else if (record->type () == Knowledge_Record::INTEGER_ARRAY)
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
        else if (record->type () == Knowledge_Record::STRING)
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


jstring JNICALL Java_com_madara_containers_FlexMap_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;

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

void JNICALL Java_com_madara_containers_FlexMap_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;

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


jlong JNICALL Java_com_madara_containers_FlexMap_jni_1get
  (JNIEnv * env, jobject, jlong cptr, jstring key)
{
  containers::Flex_Map * result (0);
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;

  if (current)
  {
    const char * str_key = env->GetStringUTFChars (key, 0);

    result = new containers::Flex_Map ((*current)[str_key]);

    env->ReleaseStringUTFChars (key, str_key);
  }

  return (jlong)result;
}

jlong JNICALL Java_com_madara_containers_FlexMap_jni_1getIndex
(JNIEnv *, jobject, jlong cptr, jint index)
{
  containers::Flex_Map * result (0);
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;

  if (current)
  {

    result = new containers::Flex_Map ((*current)[index]);

  }

  return (jlong)result;
}

jlong JNICALL Java_com_madara_containers_FlexMap_jni_1toRecord
  (JNIEnv * env, jobject, jlong cptr)
{
  Knowledge_Record * result (0);
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;

  if (current)
  {
    result = new Knowledge_Record (current->to_record ());
  }

  return (jlong)result;
}


void JNICALL Java_com_madara_containers_FlexMap_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;

  if (current)
  {
    current->modify ();
  }
}

jlong JNICALL Java_com_madara_containers_FlexMap_jni_1toMapContainer
  (JNIEnv *, jobject, jlong cptr)
{
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;
  containers::Map * result = new containers::Map ();

  if (current)
  {
    current->to_container (*result);
  }

  return (jlong) result;
}

jstring JNICALL Java_com_madara_containers_FlexMap_jni_1getDelimiter
(JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;

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

void JNICALL Java_com_madara_containers_FlexMap_jni_1setDelimiter
(JNIEnv * env, jobject, jlong cptr, jstring delimiter)
{
  containers::Flex_Map * current = (containers::Flex_Map *) cptr;

  if (current)
  {
    const char * str_delimiter = env->GetStringUTFChars (delimiter, 0);

    current->set_delimiter (str_delimiter);

    env->ReleaseStringUTFChars (delimiter, str_delimiter);
  }
}


jobjectArray JNICALL Java_com_madara_containers_FlexMap_jni_1keys
(JNIEnv * env, jobject, jlong cptr, jboolean first_level)
{
  jobjectArray result (0);
  containers::Flex_Map * current = (containers::Flex_Map *)cptr;

  if (current)
  {
    std::vector<std::string> keys;
    current->keys (keys, first_level);

    result = env->NewObjectArray (
      (jsize)keys.size (),
      Madara::Utility::Java::find_class (env, "java/lang/String"),
      env->NewStringUTF (""));

    for (unsigned int i = 0; i < keys.size (); i++)
    {
      env->SetObjectArrayElement (
        result, i, env->NewStringUTF (keys[i].c_str ()));
    }
  }
  return result;
}

void JNICALL Java_com_madara_containers_FlexMap_jni_1setSettings
(JNIEnv *, jobject, jlong cptr, jlong settings_ptr)
{
  containers::Flex_Map * current = (containers::Flex_Map *)cptr;
  engine::Knowledge_Update_Settings * settings =
    (engine::Knowledge_Update_Settings *)settings_ptr;

  if (current && settings)
  {
    current->set_settings (*settings);
  }
}
