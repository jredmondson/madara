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
#include "com_madara_containers_NativeIntegerVector.h"
#include "madara/knowledge_engine/containers/Native_Integer_Vector.h"

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
typedef containers::Native_Integer_Vector    Native_Integer_Vector;

/*
 * Class:     com_madara_containers_NativeIntegerVector
 * Method:    jni_NativeIntegerVector
 * Signature: ()J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1NativeIntegerVector__
  (JNIEnv * env, jobject)
{
  return (jlong) new Native_Integer_Vector ();
}

/*
 * Class:     com_madara_containers_NativeIntegerVector
 * Method:    jni_NativeIntegerVector
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1NativeIntegerVector__J
  (JNIEnv * env, jobject, jlong cptr)
{
  Native_Integer_Vector * result (0);
  Native_Integer_Vector * source = (Native_Integer_Vector *) cptr;

  if (source)
  {
    result = new Native_Integer_Vector (*source);
  }

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_NativeIntegerVector
 * Method:    jni_freeNativeIntegerVector
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1freeNativeIntegerVector
  (JNIEnv * env, jclass, jlong cptr)
{
  delete (Native_Integer_Vector *) cptr;
}

/*
 * Class:     com_madara_containers_NativeIntegerVector
 * Method:    jni_set
 * Signature: (JIJ)V
 */
MADARA_Export void JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1set
  (JNIEnv * env, jobject, jlong cptr, jint index, jlong value)
{
  Native_Integer_Vector * current = (Native_Integer_Vector *) cptr;

  if (current)
  {
    current->set (index, value);
  }
}

void JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1pushback
(JNIEnv *, jobject, jlong cptr, jlong value)
{
  Native_Integer_Vector * current = (Native_Integer_Vector *)cptr;

  if (current)
  {
    current->push_back (value);
  }
}

/*
 * Class:     com_madara_containers_NativeIntegerVector
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  Native_Integer_Vector * current = (Native_Integer_Vector *) cptr;

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
 * Class:     com_madara_containers_NativeIntegerVector
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  Native_Integer_Vector * current = (Native_Integer_Vector *) cptr;

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
 * Class:     com_madara_containers_NativeIntegerVector
 * Method:    jni_get
 * Signature: (JI)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1get
  (JNIEnv * env, jobject, jlong cptr, jint index)
{
  jlong result (0);
  Native_Integer_Vector * current = (Native_Integer_Vector *) cptr;

  if (current)
  {
    result = (*current) [index];
  }

  return result;
}

/*
 * Class:     com_madara_containers_NativeIntegerVector
 * Method:    jni_toRecord
 * Signature: (JI)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1toRecord__JI
  (JNIEnv * env, jobject, jlong cptr, jint index)
{
  Madara::Knowledge_Record * result (0);
  Native_Integer_Vector * current = (Native_Integer_Vector *) cptr;

  if (current)
  {
    result = new Madara::Knowledge_Record (current->to_record (index));
  }

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_NativeIntegerVector
 * Method:    jni_toRecord
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1toRecord__J
  (JNIEnv * env, jobject, jlong cptr)
{
  Madara::Knowledge_Record * result (0);
  Native_Integer_Vector * current = (Native_Integer_Vector *) cptr;

  if (current)
  {
    result = new Madara::Knowledge_Record (current->to_record ());
  }

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_NativeIntegerVector
 * Method:    jni_toArray
 * Signature: (J)[Ljava/lang/Object;
 */
MADARA_Export jobjectArray JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1toArray
  (JNIEnv * env, jobject, jlong cptr)
{
  jclass kr_class = env->FindClass ("com/madara/KnowledgeRecord");
  jobjectArray list;
  if (kr_class && cptr != 0)
  {
    jmethodID method = env->GetStaticMethodID (kr_class,
      "fromPointer", " (J)Lcom/madara/KnowledgeRecord;");
    Madara::Knowledge_Vector records;
    Native_Integer_Vector * current =
      (Native_Integer_Vector *) cptr;
    current->copy_to (records);
    jsize size = (jsize)records.size ();

    list = env->NewObjectArray ( (jsize)records.size (), kr_class, 0);

    if (method)
    {
      for (jsize i = 0; i < size; ++i)
      {
        std::cout << "record[" << i << "] = " << records[i] << "\n";
        jobject result = env->CallStaticObjectMethod (
          kr_class, method, (jlong)records[i].clone ());
        env->SetObjectArrayElement (list, i, result);
      }
    }
  }
  return list;
}

/*
 * Class:     com_madara_containers_NativeIntegerVector
 * Method:    jni_size
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1size
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);
  Native_Integer_Vector * current = (Native_Integer_Vector *) cptr;

  if (current)
  {
    result = (jlong) current->size ();
  }

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_NativeIntegerVector
 * Method:    jni_resize
 * Signature: (JJ)V
 */
MADARA_Export void JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1resize
  (JNIEnv * env, jobject, jlong cptr, jlong length)
{
  Native_Integer_Vector * current = (Native_Integer_Vector *) cptr;

  if (current)
  {
    current->resize (length);
  }
}

void JNICALL Java_com_madara_containers_NativeIntegerVector_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  Native_Integer_Vector * current = (Native_Integer_Vector *) cptr;

  if (current)
  {
    current->modify ();
  }
}
