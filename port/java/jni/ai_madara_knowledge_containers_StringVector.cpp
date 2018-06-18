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
#include "ai_madara_knowledge_containers_StringVector.h"
#include "madara/knowledge/containers/StringVector.h"
#include "madara_jni.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef containers::StringVector    StringVector;

/*
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    jni_StringVector
 * Signature: ()J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1StringVector__
  (JNIEnv *, jobject)
{
  return (jlong) new StringVector ();
}

/*
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    jni_StringVector
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1StringVector__J
  (JNIEnv *, jobject, jlong cptr)
{
  StringVector * result (0);
  StringVector * source = (StringVector *) cptr;

  if (source)
  {
    result = new StringVector (*source);
  }

  return (jlong) result;
}

/*
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    jni_freeStringVector
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1freeStringVector
  (JNIEnv *, jclass, jlong cptr)
{
  delete (StringVector *) cptr;
}

/*
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    jni_set
 * Signature: (JILjava/lang/String;)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1set
  (JNIEnv * env, jobject, jlong cptr, jint index, jstring value)
{
  StringVector * current = (StringVector *) cptr;

  if (current)
  {
    const char * str_value = env->GetStringUTFChars (value, 0);

    current->set (index, str_value);

    env->ReleaseStringUTFChars (value, str_value);
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1pushback
(JNIEnv * env, jobject, jlong cptr, jstring value)
{
  StringVector * current = (StringVector *)cptr;

  if (current)
  {
    const char * str_value = env->GetStringUTFChars (value, 0);

    current->push_back (str_value);

    env->ReleaseStringUTFChars (value, str_value);
  }
}

/*
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  StringVector * current = (StringVector *) cptr;

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
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  StringVector * current = (StringVector *) cptr;

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
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    jni_get
 * Signature: (JI)Ljava/lang/String;
 */
jstring JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1get
  (JNIEnv * env, jobject, jlong cptr, jint index)
{
  jstring result;
  StringVector * current = (StringVector *) cptr;

  if (current)
  {
    result = env->NewStringUTF ( (*current)[index].c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    jni_toRecord
 * Signature: (JI)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1toRecord__JI
  (JNIEnv *, jobject, jlong cptr, jint index)
{
  madara::knowledge::KnowledgeRecord * result (0);
  StringVector * current = (StringVector *) cptr;

  if (current)
  {
    result = new madara::knowledge::KnowledgeRecord (current->to_record (index));
  }

  return (jlong) result;
}


/*
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    jni_toArray
 * Signature: (J)[Ljava/lang/Object;
 */
jobjectArray JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1toArray
  (JNIEnv * env, jobject, jlong cptr)
{
  jclass kr_class = madara::utility::java::find_class (
    env, "com/madara/KnowledgeRecord");
  jobjectArray list = 0;

  if (kr_class && cptr != 0)
  {
    jmethodID method = env->GetStaticMethodID (kr_class,
      "fromPointer", " (J)Lcom/madara/KnowledgeRecord;");
    madara::knowledge::KnowledgeVector records;
    StringVector * current =
      (StringVector *)cptr;
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
 * Method:    jni_size
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1size
  (JNIEnv *, jobject, jlong cptr)
{
  jlong result (0);
  StringVector * current = (StringVector *) cptr;

  if (current)
  {
    result = (jlong) current->size ();
  }

  return (jlong) result;
}

/*
 * Class:     ai_madara_knowledge_containers_StringVector
 * Method:    jni_resize
 * Signature: (JJ)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1resize
  (JNIEnv *, jobject, jlong cptr, jlong length)
{
  StringVector * current = (StringVector *) cptr;

  if (current)
  {
    current->resize (length);
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  StringVector * current = (StringVector *) cptr;

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
Java_ai_madara_knowledge_containers_StringVector_jni_1modifyIndex
  (JNIEnv *, jobject, jlong cptr, jint index)
{
  StringVector * current = (StringVector *) cptr;

  if (current)
  {
    current->modify ( (size_t)index);
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1setSettings
(JNIEnv *, jobject, jlong cptr, jlong settings_ptr)
{
  containers::StringVector * current =
    (containers::StringVector *)cptr;
  knowledge::KnowledgeUpdateSettings * settings =
    (knowledge::KnowledgeUpdateSettings *)settings_ptr;

  if (current && settings)
  {
    current->set_settings (*settings);
  }
}

jboolean JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1isTrue
(JNIEnv *, jobject, jlong cptr)
{
  containers::StringVector * current = (containers::StringVector *)cptr;
  bool result (true);

  if (current)
  {
    result = current->is_true ();
  }

  return result;
}


jboolean JNICALL
Java_ai_madara_knowledge_containers_StringVector_jni_1isFalse
(JNIEnv *, jobject, jlong cptr)
{
  containers::StringVector * current = (containers::StringVector *)cptr;
  bool result (true);

  if (current)
  {
    result = current->is_false ();
  }

  return result;
}
