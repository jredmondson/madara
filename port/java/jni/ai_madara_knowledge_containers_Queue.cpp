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
#include "ai_madara_knowledge_containers_Queue.h"
#include "madara/knowledge/containers/Queue.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;
typedef knowledge::KnowledgeRecord KnowledgeRecord;
typedef KnowledgeRecord::Integer Integer;

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_Queue
 * Signature: ()J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1Queue__(
    JNIEnv*, jobject)
{
  return (jlong) new containers::Queue();
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_Queue
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1Queue__J(
    JNIEnv* env, jobject, jlong cptr)
{
  containers::Queue* result(0);
  containers::Queue* source = (containers::Queue*)cptr;

  if (source)
  {
    result = new containers::Queue(*source);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Queue::copyConstructor: "
        "Queue object is released already");
  }

  return (jlong)result;
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_freeQueue
 * Signature: (J)V
 */
void JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1freeQueue(
    JNIEnv*, jclass, jlong cptr)
{
  delete (containers::Queue*)cptr;
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_enqueue
 * Signature: (JJ)Z
 */
jboolean JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1enqueue(
    JNIEnv* env, jobject, jlong cptr, jlong record_ptr)
{
  jboolean result(false);
  containers::Queue* current = (containers::Queue*)cptr;

  if (current)
  {
    KnowledgeRecord* record = (KnowledgeRecord*)record_ptr;

    if (record)
    {
      result = current->enqueue(*record);
    }
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Queue::enqueue: "
        "Queue object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_enqueueDouble
 * Signature: (JD)Z
 */
jboolean JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1enqueueDouble(
    JNIEnv* env, jobject, jlong cptr, jdouble value)
{
  jboolean result(false);
  containers::Queue* current = (containers::Queue*)cptr;

  if (current)
  {
    result = current->enqueue(KnowledgeRecord(value));
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Queue::enqueuDouble: "
        "Queue object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_enqueueLong
 * Signature: (JJ)Z
 */
jboolean JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1enqueueLong(
    JNIEnv* env, jobject, jlong cptr, jlong value)
{
  jboolean result(false);
  containers::Queue* current = (containers::Queue*)cptr;

  if (current)
  {
    result = current->enqueue(KnowledgeRecord(Integer(value)));
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Queue::enqueueLong: "
        "Queue object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_enqueueString
 * Signature: (JLai/madara/knowledge/containers/String;)Z
 */
jboolean JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1enqueueString(
    JNIEnv* env, jobject, jlong cptr, jstring value)
{
  jboolean result(false);
  containers::Queue* current = (containers::Queue*)cptr;

  if (current)
  {
    const char* str_value = env->GetStringUTFChars(value, 0);

    result = current->enqueue(KnowledgeRecord(str_value));

    env->ReleaseStringUTFChars(value, str_value);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Queue::enqueueString: "
        "Queue object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_dequeue
 * Signature: (JZ)J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1dequeue(
    JNIEnv* env, jobject, jlong cptr, jboolean wait)
{
  jlong result(0);
  containers::Queue* current = (containers::Queue*)cptr;

  if (current)
  {
    result = (jlong) new KnowledgeRecord(current->dequeue(wait));
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Queue::dequeue: "
        "Queue object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_inspect
 * Signature: (JI)J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1inspect(
    JNIEnv* env, jobject, jlong cptr, jint position)
{
  jlong result(0);
  containers::Queue* current = (containers::Queue*)cptr;

  if (current)
  {
    result = (jlong) new KnowledgeRecord(current->inspect(position));
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Queue::inspect: "
        "Queue object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String
{
}
 */
jstring JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1getName(
    JNIEnv* env, jobject, jlong cptr)
{
  jstring result = 0;
  containers::Queue* current = (containers::Queue*)cptr;

  if (current)
  {
    result = env->NewStringUTF(current->get_name().c_str());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Queue::getName: "
        "Queue object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String
{
})V
 */
void JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1setName(
    JNIEnv* env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  containers::Queue* current = (containers::Queue*)cptr;

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
        "Queue::setName: "
        "Queue object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_size
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1size(
    JNIEnv* env, jobject, jlong cptr)
{
  jlong result(0);
  containers::Queue* current = (containers::Queue*)cptr;

  if (current)
  {
    result = (jlong)current->size();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Queue::size: "
        "Queue object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_count
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1count(
    JNIEnv* env, jobject, jlong cptr)
{
  jlong result(0);
  containers::Queue* current = (containers::Queue*)cptr;

  if (current)
  {
    result = (jlong)current->count();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Queue::count: "
        "Queue object is released already");
  }

  return result;
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_resize
 * Signature: (JJ)V
 */
void JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1resize(
    JNIEnv* env, jobject, jlong cptr, jlong new_size)
{
  containers::Queue* current = (containers::Queue*)cptr;

  if (current)
  {
    current->resize(new_size);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Queue::resize: "
        "Queue object is released already");
  }
}

/*
 * Class:     ai_madara_knowledge_containers_Queue
 * Method:    jni_clear
 * Signature: (J)V
 */
void JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1clear(
    JNIEnv* env, jobject, jlong cptr)
{
  containers::Queue* current = (containers::Queue*)cptr;

  if (current)
  {
    current->clear();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Queue::clear: "
        "Queue object is released already");
  }
}

void JNICALL Java_ai_madara_knowledge_containers_Queue_jni_1setSettings(
    JNIEnv* env, jobject, jlong cptr, jlong settings_ptr)
{
  containers::Queue* current = (containers::Queue*)cptr;
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
        "Queue::setSettings: "
        "Queue or settings objects are released already");
  }
}
