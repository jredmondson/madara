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

#include "com_madara_containers_Counter.h"
#include "madara/knowledge/containers/Counter.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef containers::Counter    Counter;

/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_Counter
 * Signature: ()J
 */
jlong JNICALL
Java_com_madara_containers_Counter_jni_1Counter__
  (JNIEnv *, jobject)
{
  return (jlong) new Counter ();
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_Counter
 * Signature: (J)J
 */
jlong JNICALL
Java_com_madara_containers_Counter_jni_1Counter__J
  (JNIEnv *, jobject, jlong cptr)
{
  Counter * result (0);
  Counter * source = (Counter *) cptr;

  if (source)
  {
    result = new Counter (*source);
  }

  return (jlong) result;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_freeCounter
 * Signature: (J)V
 */
void JNICALL
Java_com_madara_containers_Counter_jni_1freeCounter
  (JNIEnv *, jclass, jlong cptr)
{
  delete (Counter *) cptr;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_set
 * Signature: (JJ)V
 */
void JNICALL
Java_com_madara_containers_Counter_jni_1set
  (JNIEnv *, jobject, jlong cptr, jlong value)
{
  Counter * current = (Counter *) cptr;

  if (current)
  {
    *current = value;
  }
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL
Java_com_madara_containers_Counter_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  Counter * current = (Counter *) cptr;

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
 * Class:     com_madara_containers_Counter
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
void JNICALL
Java_com_madara_containers_Counter_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  Counter * current = (Counter *) cptr;

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
 * Class:     com_madara_containers_Counter
 * Method:    jni_toString
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL
Java_com_madara_containers_Counter_jni_1toString
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  Counter * current = (Counter *) cptr;

  if (current)
  {
    result = env->NewStringUTF (current->to_string ().c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_toDouble
 * Signature: (J)D
 */
jdouble JNICALL
Java_com_madara_containers_Counter_jni_1toDouble
  (JNIEnv *, jobject, jlong cptr)
{
  jdouble result (0.0);
  Counter * current = (Counter *) cptr;

  if (current)
  {
    result = current->to_double ();
  }

  return result;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_toLong
 * Signature: (J)J
 */
jlong JNICALL
Java_com_madara_containers_Counter_jni_1toLong
  (JNIEnv *, jobject, jlong cptr)
{
  jlong result (0);
  Counter * current = (Counter *) cptr;

  if (current)
  {
    result = current->to_integer ();
  }

  return result;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_inc
 * Signature: (J)J
 */
void JNICALL
Java_com_madara_containers_Counter_jni_1inc
  (JNIEnv *, jobject, jlong cptr)
{
  Counter * current = (Counter *) cptr;
  ++ (*current);
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_dec
 * Signature: (J)J
 */
void JNICALL
Java_com_madara_containers_Counter_jni_1dec
  (JNIEnv *, jobject, jlong cptr)
{
  Counter * current = (Counter *) cptr;
  -- (*current);
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_incValue
 * Signature: (JJ)J
 */
void JNICALL
Java_com_madara_containers_Counter_jni_1incValue
  (JNIEnv *, jobject, jlong cptr, jlong value)
{
  Counter * current = (Counter *) cptr;
  *current += value;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_decValue
 * Signature: (JJ)J
 */
void JNICALL
Java_com_madara_containers_Counter_jni_1decValue
  (JNIEnv *, jobject, jlong cptr, jlong value)
{
  Counter * current = (Counter *) cptr;
  *current -= value;
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_modify
 * Signature: (J)V
 */
void JNICALL
Java_com_madara_containers_Counter_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  Counter * current = (Counter *) cptr;
  current->modify ();
}


/*
 * Class:     com_madara_containers_Counter
 * Method:    jni_resize
 * Signature: (JII)V
 */
void JNICALL
Java_com_madara_containers_Counter_jni_1resize
  (JNIEnv *, jobject, jlong cptr, jint id, jint counters)
{
  Counter * current = (Counter *) cptr;
  current->resize (id, counters);
}

void JNICALL
Java_com_madara_containers_Counter_jni_1setSettings
(JNIEnv *, jobject, jlong cptr, jlong settings_ptr)
{
  Counter * current = (Counter *)cptr;
  knowledge::KnowledgeUpdateSettings * settings =
    (knowledge::KnowledgeUpdateSettings *)settings_ptr;

  if (current && settings)
  {
    current->set_settings (*settings);
  }
}

jboolean JNICALL
Java_com_madara_containers_Counter_jni_1isTrue
(JNIEnv *, jobject, jlong cptr)
{
  Counter * current = (Counter *)cptr;
  bool result (true);

  if (current)
  {
    result = current->is_true ();
  }

  return result;
}


jboolean JNICALL
Java_com_madara_containers_Counter_jni_1isFalse
(JNIEnv *, jobject, jlong cptr)
{
  Counter * current = (Counter *)cptr;
  bool result (true);

  if (current)
  {
    result = current->is_false ();
  }

  return result;
}
