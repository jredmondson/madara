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
#include "ai_madara_knowledge_containers_Vector.h"
#include "madara/knowledge/containers/Vector.h"
#include "madara_jni.h"
#include <iostream>

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef containers::Vector    Vector;
typedef madara::knowledge::KnowledgeRecord::Integer    Integer;

/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_Vector
 * Signature: ()J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1Vector__
  (JNIEnv *, jobject)
{
  return (jlong) new Vector ();
}

/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_Vector
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1Vector__J
  (JNIEnv *, jobject, jlong cptr)
{
  Vector * result (0);
  Vector * source = (Vector *) cptr;

  if (source)
  {
    result = new Vector (*source);
  }

  return (jlong) result;
}

/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_freeVector
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1freeVector
  (JNIEnv *, jclass, jlong cptr)
{
  delete (Vector *) cptr;
}

/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_set
 * Signature: (JILjava/lang/String;)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1setString__JILjava_lang_String_2
  (JNIEnv * env, jobject, jlong cptr, jint index, jstring value)
{
  Vector * current = (Vector *) cptr;

  if (current)
  {
    const char * str_value = env->GetStringUTFChars (value, 0);

    current->set (index, str_value);

    env->ReleaseStringUTFChars (value, str_value);
  }
}

/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_set
 * Signature: (JID)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1setDouble__JID
  (JNIEnv *, jobject, jlong cptr, jint index, jdouble value)
{
  Vector * current = (Vector *) cptr;

  if (current)
  {
    current->set (index, value);
  }
}

/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_set
 * Signature: (JIJJ)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1set__JIJJ
  (JNIEnv *, jobject, jlong cptr, jint index, jlong type, jlong value)
{
  Vector * current = (Vector *) cptr;

  if (current)
  {
    // set integer
    if (type == 0)
    {
      current->set (index, (madara::knowledge::KnowledgeRecord::Integer)value);
    }
    // set knowledge record
    else
    {
      madara::knowledge::KnowledgeRecord * record = (madara::knowledge::KnowledgeRecord *) value;
      
      if (record)
      {
        if (record->type () == madara::knowledge::KnowledgeRecord::DOUBLE)
        {
          current->set (index, record->to_double ());
        }
        else if (record->type () == madara::knowledge::KnowledgeRecord::DOUBLE_ARRAY)
        {
          current->set (index, record->to_doubles ());
        }
        else if (record->type () == madara::knowledge::KnowledgeRecord::INTEGER)
        {
          current->set (index, record->to_integer ());
        }
        else if (record->type () == madara::knowledge::KnowledgeRecord::INTEGER_ARRAY)
        {
          current->set (index, record->to_integers ());
        }
        else if (record->is_binary_file_type ())
        {
          size_t size;
          unsigned char * buffer = record->to_unmanaged_buffer (size);
          current->set_file (index, buffer, size);
          delete [] buffer;
        }
        else if (record->type () == madara::knowledge::KnowledgeRecord::STRING)
        {
          current->set (index, record->to_string ());
        }
        else if (record->is_string_type ())
        {
          current->set (index, record->to_string ());
        }
      }
    }
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1pushbackRecord
(JNIEnv *, jobject, jlong cptr, jlong value_ptr)
{
  Vector * current = (Vector *)cptr;

  if (current)
  {
    madara::knowledge::KnowledgeRecord * record = (madara::knowledge::KnowledgeRecord *) value_ptr;

    current->push_back (*record);
  }
}


void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1pushbackLong
(JNIEnv *, jobject, jlong cptr, jlong value)
{
  Vector * current = (Vector *)cptr;

  if (current)
  {
    current->push_back (madara::knowledge::KnowledgeRecord (value));
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1pushbackDouble
(JNIEnv *, jobject, jlong cptr, jdouble value)
{
  Vector * current = (Vector *)cptr;

  if (current)
  {
    current->push_back (madara::knowledge::KnowledgeRecord ((double)value));
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1pushbackDoubleArray
(JNIEnv * env, jobject, jlong cptr, jdoubleArray data)
{
  Vector * current = (Vector *)cptr;

  if (current)
  {
    jsize len = env->GetArrayLength (data);
    jboolean isCopy;
    std::vector<double> dblVector (len);

    jdouble* dblArray = env->GetDoubleArrayElements (data, &isCopy);

    // copy elements to STL vector
    for (int x = 0; x < len; x++)
      dblVector[x] = dblArray[x];

    current->push_back (madara::knowledge::KnowledgeRecord (dblVector));

    if (isCopy)
      env->ReleaseDoubleArrayElements (data, dblArray, JNI_ABORT);
  }
}

/*
* Class:     ai_madara_knowledge_containers_Vector
* Method:    jni_pushbackLongArray
* Signature: (J[J)V
*/
void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1pushbackLongArray
(JNIEnv * env, jobject, jlong cptr, jlongArray data)
{
  Vector * current = (Vector *)cptr;

  if (current)
  {
    jsize len = env->GetArrayLength (data);
    jboolean isCopy;
    std::vector<double> dblVector (len);

    jlong * longArray = env->GetLongArrayElements (data, &isCopy);

    // copy elements to STL vector
    for (int x = 0; x < len; x++)
      dblVector[x] = longArray[x];

    current->push_back (madara::knowledge::KnowledgeRecord (dblVector));

    if (isCopy)
      env->ReleaseLongArrayElements (data, longArray, JNI_ABORT);
  }
}

/*
* Class:     ai_madara_knowledge_containers_Vector
* Method:    jni_pushbackString
* Signature: (JLjava/lang/String;)V
*/
void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1pushbackString
(JNIEnv * env, jobject, jlong cptr, jstring data)
{
  Vector * current = (Vector *)cptr;

  if (current)
  {
    const char *nativeData = env->GetStringUTFChars (data, 0);
    current->push_back (madara::knowledge::KnowledgeRecord (nativeData));
    env->ReleaseStringUTFChars (data, nativeData);
  }
}


/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  Vector * current = (Vector *) cptr;

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

/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  Vector * current = (Vector *) cptr;

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

/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_get
 * Signature: (JI)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1get
  (JNIEnv *, jobject, jlong cptr, jint index)
{
  madara::knowledge::KnowledgeRecord * result (0);
  Vector * current = (Vector *) cptr;

  if (current)
  {
    result = new madara::knowledge::KnowledgeRecord (current->to_record (index));
  }

  return (jlong) result;
}

/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_resize
 * Signature: (JJ)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1resize
  (JNIEnv *, jobject, jlong cptr, jlong length)
{
  Vector * current = (Vector *) cptr;

  if (current)
  {
    current->resize (length, true);
  }
}

/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_size
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1size
  (JNIEnv *, jobject, jlong cptr)
{
  jlong result (0);
  Vector * current = (Vector *) cptr;

  if (current)
  {
    result = (jlong) current->size ();
  }

  return (jlong) result;
}

/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_toRecord
 * Signature: (JI)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1toRecord
  (JNIEnv *, jobject, jlong cptr, jint index)
{
  madara::knowledge::KnowledgeRecord * result (0);
  Vector * current = (Vector *) cptr;

  if (current)
  {
    result = new madara::knowledge::KnowledgeRecord (current->to_record (index));
  }

  return (jlong) result;
}

/*
 * Class:     ai_madara_knowledge_containers_Vector
 * Method:    jni_toArray
 * Signature: (JI)[Ljava/lang/Object;
 */
jobjectArray JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1toArray
  (JNIEnv * env, jobject, jlong cptr)
{
  jclass kr_class = madara::utility::java::find_class (
    env, "com/madara/KnowledgeRecord");
  jobjectArray list = 0;

  if (kr_class && cptr != 0)
  {
    jmethodID method = env->GetStaticMethodID (kr_class,
      "fromPointer", "(J)Lcom/madara/KnowledgeRecord;");
    madara::knowledge::KnowledgeVector records;
    Vector * current =
      (Vector *)cptr;
    current->copy_to (records);
    jsize size = (jsize)records.size ();

    list = env->NewObjectArray ((jsize)records.size (), kr_class, 0);

    if (method)
    {
      for (jsize i = 0; i < size; ++i)
      {
        std::cout << "record[" << i << "] = " << records[i] << "\n";
        jobject result = env->CallStaticObjectMethod (
          kr_class, method, (jlong)records[i].clone ());
        env->SetObjectArrayElement (list, i, result);

        env->DeleteLocalRef (result);
      }
    }
  }

  env->DeleteWeakGlobalRef (kr_class);

  return list;
}


/*
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    modify
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  Vector * current = (Vector *) cptr;

  if (current)
  {
    current->modify ();
  }
}

/*
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    modifyIndex
 * Signature: (JI)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1modifyIndex
  (JNIEnv *, jobject, jlong cptr, jint index)
{
  Vector * current = (Vector *) cptr;

  if (current)
  {
    current->modify ( (size_t)index);
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1setSettings
(JNIEnv *, jobject, jlong cptr, jlong settings_ptr)
{
  containers::Vector * current = (containers::Vector *)cptr;
  knowledge::KnowledgeUpdateSettings * settings =
    (knowledge::KnowledgeUpdateSettings *)settings_ptr;

  if (current && settings)
  {
    current->set_settings (*settings);
  }
}

jboolean JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1isTrue
(JNIEnv *, jobject, jlong cptr)
{
  containers::Vector * current = (containers::Vector *)cptr;
  bool result (true);

  if (current)
  {
    result = current->is_true ();
  }

  return result;
}

jboolean JNICALL
Java_ai_madara_knowledge_containers_Vector_jni_1isFalse
(JNIEnv *, jobject, jlong cptr)
{
  containers::Vector * current = (containers::Vector *)cptr;
  bool result (true);

  if (current)
  {
    result = current->is_false ();
  }

  return result;
}