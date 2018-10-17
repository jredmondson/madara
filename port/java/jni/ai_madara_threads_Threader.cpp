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
#include "ai_madara_threads_Threader.h"
#include "madara/threads/Threader.h"

namespace knowledge = madara::knowledge;
namespace threads = madara::threads;
typedef threads::Threader Threader;
typedef knowledge::KnowledgeBase KnowledgeBase;

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_Threader
 * Signature: ()J
 */
jlong JNICALL Java_ai_madara_threads_Threader_jni_1Threader__(JNIEnv*, jobject)
{
  return (jlong) new Threader();
}

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_Threader
 * Signature: (J)J
 */
jlong JNICALL Java_ai_madara_threads_Threader_jni_1Threader__J(
    JNIEnv* env, jobject, jlong kb)
{
  Threader* result(0);
  KnowledgeBase* knowledge = (KnowledgeBase*)kb;

  if(knowledge)
  {
    result = new Threader(*knowledge);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Threader::copyConstructor: "
        "Threader object is released already");
  }

  return (jlong)result;
}

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_freeThreader
 * Signature: (J)V
 */
void JNICALL Java_ai_madara_threads_Threader_jni_1freeThreader(
    JNIEnv*, jobject, jlong cptr)
{
  delete (Threader*)cptr;
}

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_run
 * Signature: (JLjava/lang/String;Ljava/lang/Object;Z)V
 */
void JNICALL Java_ai_madara_threads_Threader_jni_1run(JNIEnv* env, jobject,
    jlong cptr, jstring name, jobject obj, jboolean paused)
{
  Threader* current = (Threader*)cptr;

  if(current)
  {
    const char* str_name = env->GetStringUTFChars(name, 0);

    current->run(str_name, obj, paused);

    env->ReleaseStringUTFChars(name, str_name);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Threader::run: "
        "Threader object is released already");
  }
}

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_runhz
 * Signature: (JDLjava/lang/String;Ljava/lang/Object;Z)V
 */
void JNICALL Java_ai_madara_threads_Threader_jni_1runhz(JNIEnv* env, jobject,
    jlong cptr, jdouble hertz, jstring name, jobject obj, jboolean paused)
{
  Threader* current = (Threader*)cptr;

  if(current)
  {
    const char* str_name = env->GetStringUTFChars(name, 0);

    current->run(hertz, str_name, obj, paused);

    env->ReleaseStringUTFChars(name, str_name);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Threader::runhz: "
        "Threader object is released already");
  }
}

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_pauseThread
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_ai_madara_threads_Threader_jni_1pauseThread(
    JNIEnv* env, jobject, jlong cptr, jstring name)
{
  Threader* current = (Threader*)cptr;

  if(current)
  {
    const char* str_name = env->GetStringUTFChars(name, 0);

    current->pause(str_name);

    env->ReleaseStringUTFChars(name, str_name);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Threader::pauseThread: "
        "Threader object is released already");
  }
}

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_pause
 * Signature: (J)V
 */
void JNICALL Java_ai_madara_threads_Threader_jni_1pause(
    JNIEnv* env, jobject, jlong cptr)
{
  Threader* current = (Threader*)cptr;

  if(current)
  {
    current->pause();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Threader::pause: "
        "Threader object is released already");
  }
}

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_waitThread
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_ai_madara_threads_Threader_jni_1waitThread(
    JNIEnv* env, jobject, jlong cptr, jstring name)
{
  Threader* current = (Threader*)cptr;

  if(current)
  {
    const char* str_name = env->GetStringUTFChars(name, 0);

    current->wait(str_name);

    env->ReleaseStringUTFChars(name, str_name);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Threader::waitThread: "
        "Threader object is released already");
  }
}

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_wait
 * Signature: (J)V
 */
void JNICALL Java_ai_madara_threads_Threader_jni_1wait(
    JNIEnv* env, jobject, jlong cptr)
{
  Threader* current = (Threader*)cptr;

  if(current)
  {
    current->wait();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Threader::wait: "
        "Threader object is released already");
  }
}

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_terminateThread
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_ai_madara_threads_Threader_jni_1terminateThread(
    JNIEnv* env, jobject, jlong cptr, jstring name)
{
  Threader* current = (Threader*)cptr;

  if(current)
  {
    const char* str_name = env->GetStringUTFChars(name, 0);

    current->terminate(str_name);

    env->ReleaseStringUTFChars(name, str_name);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Threader::terminateThread: "
        "Threader object is released already");
  }
}

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_terminate
 * Signature: (J)V
 */
void JNICALL Java_ai_madara_threads_Threader_jni_1terminate(
    JNIEnv* env, jobject, jlong cptr)
{
  Threader* current = (Threader*)cptr;

  if(current)
  {
    current->terminate();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Threader::terminate: "
        "Threader object is released already");
  }
}

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_resumeThread
 * Signature: (JLjava/lang/String;)V
 */
void JNICALL Java_ai_madara_threads_Threader_jni_1resumeThread(
    JNIEnv* env, jobject, jlong cptr, jstring name)
{
  Threader* current = (Threader*)cptr;

  if(current)
  {
    const char* str_name = env->GetStringUTFChars(name, 0);

    current->resume(str_name);

    env->ReleaseStringUTFChars(name, str_name);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Threader::resumeThread: "
        "Threader object is released already");
  }
}

/*
 * Class:     ai_madara_threads_Threader
 * Method:    jni_resume
 * Signature: (J)V
 */
void JNICALL Java_ai_madara_threads_Threader_jni_1resume(
    JNIEnv* env, jobject, jlong cptr)
{
  Threader* current = (Threader*)cptr;

  if(current)
  {
    current->resume();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Threader::resume: "
        "Threader object is released already");
  }
}
