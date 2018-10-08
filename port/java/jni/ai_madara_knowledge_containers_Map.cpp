/*********************************************************************
 * Copyright (c) 2013-2015 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following acknowledgments and disclaimers.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The names "Carnegie Mellon University," "SEI" and/or
 * "Software Engineering Institute" shall not be used to endorse or promote
 * products derived from this software without prior written permission. For
 * written permission, please contact permission@sei.cmu.edu.
 *
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 * appear in their names without prior written permission of
 * permission@sei.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 * acknowledgment:
 *
 * This material is based upon work funded and supported by the Department of
 * Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon University
 * for the operation of the Software Engineering Institute, a federally funded
 * research and development center. Any opinions, findings and conclusions or
 * recommendations expressed in this material are those of the author(s) and
 * do not necessarily reflect the views of the United States Department of
 * Defense.
 *
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
 * AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR
 * PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
 * MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
 * WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 *
 * This material has been approved for public release and unlimited
 * distribution.
 *
 * @author James Edmondson <jedmondson@gmail.com>
 *********************************************************************/
#include "ai_madara_knowledge_containers_Map.h"
#include "madara/knowledge/containers/Map.h"
#include "madara_jni.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef madara::knowledge::KnowledgeRecord KnowledgeRecord;
typedef KnowledgeRecord::Integer Integer;

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_Map
 * Signature: ()J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_Map_jni_1Map__(
    JNIEnv*, jobject)
{
  return (jlong) new containers::Map();
}

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_Map
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_Map_jni_1Map__J(
    JNIEnv* env, jobject, jlong cptr)
{
  containers::Map* result(0);
  containers::Map* source = (containers::Map*)cptr;

  if (source)
  {
    result = new containers::Map(*source);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::copyConstructor: "
        "Map object is released already");
  }

  return (jlong)result;
}

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_freeMap
 * Signature: (J)V
 */
void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1freeMap(
    JNIEnv*, jclass, jlong cptr)
{
  delete (containers::Map*)cptr;
}

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_clear
 * Signature: (JZ)V
 */
void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1clear(
    JNIEnv* env, jobject, jlong cptr, jboolean clear_variables)
{
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    current->clear(clear_variables);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::clear: "
        "Map object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_erase
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1erase(
    JNIEnv* env, jobject, jlong cptr, jstring key)
{
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    const char* str_key = env->GetStringUTFChars(key, 0);

    current->erase(str_key);

    env->ReleaseStringUTFChars(key, str_key);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::erase: "
        "Map object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_set
 * Signature: (JLjava/lang/String;Ljava/lang/String;)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1setString__JLjava_lang_String_2Ljava_lang_String_2(
    JNIEnv* env, jobject, jlong cptr, jstring key, jstring value)
{
  containers::Map* current = (containers::Map*)cptr;
  if (current)
  {
    const char* str_key = env->GetStringUTFChars(key, 0);
    const char* str_value = env->GetStringUTFChars(value, 0);

    current->set(str_key, str_value);

    env->ReleaseStringUTFChars(key, str_key);
    env->ReleaseStringUTFChars(value, str_value);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::setString: "
        "Map object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_set
 * Signature: (JLjava/lang/String;D)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1setDouble__JLjava_lang_String_2D(
    JNIEnv* env, jobject, jlong cptr, jstring key, jdouble value)
{
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    const char* str_key = env->GetStringUTFChars(key, 0);

    current->set(str_key, (double)value);

    env->ReleaseStringUTFChars(key, str_key);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::setDouble: "
        "Map object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_set
 * Signature: (JLjava/lang/String;JJ)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1set__JLjava_lang_String_2JJ(
    JNIEnv* env, jobject, jlong cptr, jstring key, jlong type, jlong value)
{
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    const char* str_key = env->GetStringUTFChars(key, 0);

    // integer set
    if (type == 0)
    {
      current->set(str_key, (KnowledgeRecord::Integer)value);
    }
    // knowledge record set
    else
    {
      KnowledgeRecord* record = (KnowledgeRecord*)value;

      if (record)
      {
        // check the type and set accordingly
        if (record->type() == knowledge::KnowledgeRecord::DOUBLE)
        {
          current->set(str_key, record->to_double());
        }
        else if (record->type() == knowledge::KnowledgeRecord::DOUBLE_ARRAY)
        {
          current->set(str_key, record->to_doubles());
        }
        else if (record->type() == knowledge::KnowledgeRecord::INTEGER)
        {
          current->set(str_key, record->to_integer());
        }
        else if (record->type() == knowledge::KnowledgeRecord::INTEGER_ARRAY)
        {
          current->set(str_key, record->to_integers());
        }
        else if (record->is_binary_file_type())
        {
          size_t size;
          unsigned char* buffer = record->to_unmanaged_buffer(size);
          current->set_file(str_key, buffer, size);
          delete[] buffer;
        }
        else if (record->type() == knowledge::KnowledgeRecord::STRING)
        {
          current->set(str_key, record->to_string());
        }
        else if (record->is_string_type())
        {
          current->set(str_key, record->to_string());
        }
      }
    }

    env->ReleaseStringUTFChars(key, str_key);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::set: "
        "Map object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_ai_madara_knowledge_containers_Map_jni_1getName(
    JNIEnv* env, jobject, jlong cptr)
{
  jstring result = 0;
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    result = env->NewStringUTF(current->get_name().c_str());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::getName: "
        "Map object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1setName(
    JNIEnv* env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  containers::Map* current = (containers::Map*)cptr;

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
        "Map::setName: "
        "Map object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_get
 * Signature: (JI)J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_Map_jni_1get(
    JNIEnv* env, jobject, jlong cptr, jstring key)
{
  knowledge::KnowledgeRecord* result(0);
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    const char* str_key = env->GetStringUTFChars(key, 0);

    result = new knowledge::KnowledgeRecord((*current)[str_key]);

    env->ReleaseStringUTFChars(key, str_key);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::get: "
        "Map object is released already");
  }

  return (jlong)result;
}

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_toRecord
 * Signature: (JI)J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_Map_jni_1toRecord(
    JNIEnv* env, jobject, jlong cptr, jstring key)
{
  knowledge::KnowledgeRecord* result(0);
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    const char* str_key = env->GetStringUTFChars(key, 0);

    result = new knowledge::KnowledgeRecord((*current)[str_key]);

    env->ReleaseStringUTFChars(key, str_key);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::toRecord: "
        "Map object is released already");
  }

  return (jlong)result;
}

void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1modify(
    JNIEnv* env, jobject, jlong cptr)
{
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    current->modify();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::modify: "
        "Map object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    modifyIndex
 * Signature: (JI)V
 */
void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1modifyIndex(
    JNIEnv* env, jobject, jlong cptr, jstring key)
{
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    const char* str_key = env->GetStringUTFChars(key, 0);

    current->modify(str_key);

    env->ReleaseStringUTFChars(key, str_key);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::modifyIndex: "
        "Map object is released already");
  }
}

jstring JNICALL Java_ai_madara_knowledge_containers_Map_jni_1getDelimiter(
    JNIEnv* env, jobject, jlong cptr)
{
  jstring result = 0;
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    result = env->NewStringUTF(current->get_delimiter().c_str());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::getDelimiter: "
        "Map object is released already");
  }

  return result;
}

void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1setDelimiter(
    JNIEnv* env, jobject, jlong cptr, jstring delimiter)
{
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    const char* str_delimiter = env->GetStringUTFChars(delimiter, 0);

    current->set_delimiter(str_delimiter);

    env->ReleaseStringUTFChars(delimiter, str_delimiter);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::setDelimiter: "
        "Map object is released already");
  }
}

jobjectArray JNICALL Java_ai_madara_knowledge_containers_Map_jni_1keys(
    JNIEnv* env, jobject, jlong cptr)
{
  jobjectArray result(0);
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    std::vector<std::string> keys;
    current->keys(keys);

    jclass string_class =
        madara::utility::java::find_class(env, "java/lang/String");
    jstring empty_string = env->NewStringUTF("");

    result =
        env->NewObjectArray((jsize)keys.size(), string_class, empty_string);

    for (unsigned int i = 0; i < keys.size(); i++)
    {
      jstring temp_string = env->NewStringUTF(keys[i].c_str());

      env->SetObjectArrayElement(result, i, temp_string);

      env->DeleteLocalRef(temp_string);
    }

    env->DeleteLocalRef(empty_string);
    env->DeleteWeakGlobalRef(string_class);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::keys: "
        "Map object is released already");
  }

  return result;
}

void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1sync(
    JNIEnv* env, jobject, jlong cptr)
{
  containers::Map* current = (containers::Map*)cptr;

  if (current)
  {
    current->sync_keys();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::sync: "
        "Map object is released already");
  }
}

void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1setSettings(
    JNIEnv* env, jobject, jlong cptr, jlong settings_ptr)
{
  containers::Map* current = (containers::Map*)cptr;
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
        "Map::setSettings: "
        "Map or settings objects are released already");
  }
}

jboolean JNICALL Java_ai_madara_knowledge_containers_Map_jni_1isTrue(
    JNIEnv* env, jobject, jlong cptr)
{
  containers::Map* current = (containers::Map*)cptr;
  bool result(true);

  if (current)
  {
    result = current->is_true();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::isTrue: "
        "Map object is released already");
  }

  return result;
}

jboolean JNICALL Java_ai_madara_knowledge_containers_Map_jni_1isFalse(
    JNIEnv* env, jobject, jlong cptr)
{
  containers::Map* current = (containers::Map*)cptr;
  bool result(true);

  if (current)
  {
    result = current->is_false();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Map::isFalse: "
        "Map object is released already");
  }

  return result;
}
