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
#include "ai_madara_knowledge_containers_String.h"
#include "madara/knowledge/containers/String.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef containers::String    String;

/*
 * Class:     ai_madara_knowledge_containers_String
 * Method:    jni_String
 * Signature: ()J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_String_jni_1String__
  (JNIEnv *, jobject)
{
  return (jlong) new String ();
}

/*
 * Class:     ai_madara_knowledge_containers_String
 * Method:    jni_String
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_String_jni_1String__J
  (JNIEnv * env, jobject, jlong cptr)
{
  String * result (0);
  String * source = (String *) cptr;

  if (source)
  {
    result = new String (*source);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "String::copyConstructor: "
      "String object is released already");
  }

  return (jlong) result;
}

/*
 * Class:     ai_madara_knowledge_containers_String
 * Method:    jni_freeString
 * Signature: (J)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_String_jni_1freeString
  (JNIEnv *, jclass, jlong cptr)
{
  delete (String *) cptr;
}

/*
 * Class:     ai_madara_knowledge_containers_String
 * Method:    jni_set
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_String_jni_1set
  (JNIEnv * env, jobject, jlong cptr, jstring value)
{
  String * current = (String *) cptr;
  if (current)
  {
    const char * str_value = env->GetStringUTFChars (value, 0);

    *current = str_value;

    env->ReleaseStringUTFChars (value, str_value);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "String::set: "
      "String object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_String
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL
Java_ai_madara_knowledge_containers_String_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result = 0;
  String * current = (String *) cptr;

  if (current)
  {
    result = env->NewStringUTF (current->get_name ().c_str ());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "String::getName: "
      "String object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_String
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
void JNICALL
Java_ai_madara_knowledge_containers_String_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  String * current = (String *) cptr;

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
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "String::setName: "
      "String object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_String
 * Method:    jni_toString
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL
Java_ai_madara_knowledge_containers_String_jni_1toString
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result = 0;
  String * current = (String *) cptr;

  if (current)
  {
    result = env->NewStringUTF (current->to_string ().c_str ());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "String::toString: "
      "String object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_String
 * Method:    jni_toDouble
 * Signature: (J)D
 */
jdouble JNICALL
Java_ai_madara_knowledge_containers_String_jni_1toDouble
  (JNIEnv * env, jobject, jlong cptr)
{
  jdouble result (0.0);
  String * current = (String *) cptr;

  if (current)
  {
    result = current->to_double ();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "String::toDouble: "
      "String object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_String
 * Method:    jni_toLong
 * Signature: (J)J
 */
jlong JNICALL
Java_ai_madara_knowledge_containers_String_jni_1toLong
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);
  String * current = (String *) cptr;

  if (current)
  {
    result = current->to_integer ();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "String::toLong: "
      "String object is released already");
  }

  return result;
}

void JNICALL
Java_ai_madara_knowledge_containers_String_jni_1modify
  (JNIEnv * env, jobject, jlong cptr)
{
  String * current = (String *) cptr;

  if (current)
  {
    current->modify ();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "String::modify: "
      "String object is released already");
  }
}

void JNICALL
Java_ai_madara_knowledge_containers_String_jni_1setSettings
(JNIEnv * env, jobject, jlong cptr, jlong settings_ptr)
{
  containers::String * current = (containers::String *)cptr;
  knowledge::KnowledgeUpdateSettings * settings =
    (knowledge::KnowledgeUpdateSettings *)settings_ptr;

  if (current && settings)
  {
    current->set_settings (*settings);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "String::setSettings: "
      "String or settings objects are released already");
  }
}

jboolean JNICALL
Java_ai_madara_knowledge_containers_String_jni_1isTrue
(JNIEnv * env, jobject, jlong cptr)
{
  containers::String * current = (containers::String *)cptr;
  bool result (true);

  if (current)
  {
    result = current->is_true ();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "String::isTrue: "
      "String object is released already");
  }

  return result;
}


jboolean JNICALL
Java_ai_madara_knowledge_containers_String_jni_1isFalse
(JNIEnv * env, jobject, jlong cptr)
{
  containers::String * current = (containers::String *)cptr;
  bool result (true);

  if (current)
  {
    result = current->is_false ();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw
  
    madara::utility::java::throw_dead_obj_exception(env,
      "String::isFalse: "
      "String object is released already");
  }

  return result;
}
