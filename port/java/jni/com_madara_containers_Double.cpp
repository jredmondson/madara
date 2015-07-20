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
#include "com_madara_containers_Double.h"
#include "madara/knowledge_engine/containers/Double.h"

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
typedef containers::Double    Double;

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_Double
 * Signature: ()J
 */
jlong JNICALL Java_com_madara_containers_Double_jni_1Double__
  (JNIEnv * env, jobject)
{
  return (jlong) new Double ();
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_Double
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_Double_jni_1Double__J
  (JNIEnv * env, jobject, jlong cptr)
{
  Double * result (0);
  Double * source = (Double *) cptr;

  if (source)
  {
    result = new Double (*source);
  }

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_freeDouble
 * Signature: (J)V
 */
void JNICALL Java_com_madara_containers_Double_jni_1freeDouble
  (JNIEnv * env, jclass, jlong cptr)
{
  delete (Double *) cptr;
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_set
 * Signature: (JD)V
 */
void JNICALL Java_com_madara_containers_Double_jni_1set
  (JNIEnv * env, jobject, jlong cptr, jdouble value)
{
  Double * current = (Double *) cptr;

  if (current)
  {
    *current = value;
  }
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_getName
 * Signature: (J)Lcom/madara/containers/String;
 */
jstring JNICALL Java_com_madara_containers_Double_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  Double * current = (Double *) cptr;

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
 * Class:     com_madara_containers_Double
 * Method:    jni_setName
 * Signature: (JJJLcom/madara/containers/String;)V
 */
void JNICALL Java_com_madara_containers_Double_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  Double * current = (Double *) cptr;

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
 * Class:     com_madara_containers_Double
 * Method:    jni_toString
 * Signature: (J)Lcom/madara/containers/String;
 */
jstring JNICALL Java_com_madara_containers_Double_jni_1toString
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  Double * current = (Double *) cptr;

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
 * Class:     com_madara_containers_Double
 * Method:    jni_toDouble
 * Signature: (J)D
 */
jdouble JNICALL Java_com_madara_containers_Double_jni_1toDouble
  (JNIEnv * env, jobject, jlong cptr)
{
  jdouble result (0.0);
  Double * current = (Double *) cptr;

  if (current)
  {
    result = current->to_double ();
  }

  return result;
}

/*
 * Class:     com_madara_containers_Double
 * Method:    jni_toLong
 * Signature: (J)J
 */
jlong JNICALL Java_com_madara_containers_Double_jni_1toLong
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);
  Double * current = (Double *) cptr;

  if (current)
  {
    result = current->to_integer ();
  }

  return result;
}

void JNICALL Java_com_madara_containers_Double_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  Double * current = (Double *) cptr;

  if (current)
  {
    current->modify ();
  }
}

void JNICALL Java_com_madara_containers_Double_jni_1setSettings
(JNIEnv *, jobject, jlong cptr, jlong settings_ptr)
{
  Double * current = (Double *)cptr;
  engine::Knowledge_Update_Settings * settings =
    (engine::Knowledge_Update_Settings *)settings_ptr;

  if (current && settings)
  {
    current->set_settings (*settings);
  }
}
