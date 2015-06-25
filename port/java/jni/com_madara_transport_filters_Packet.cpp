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
#include "com_madara_transport_filters_Packet.h"
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara_jni.h"

typedef Madara::Knowledge_Map      Knowledge_Map;
typedef Madara::Knowledge_Record   Knowledge_Record;

/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_get
 * Signature: (JLjava/lang/String;)J
 */
jlong JNICALL Java_com_madara_transport_filters_Packet_jni_1get
  (JNIEnv * env, jobject, jlong cptr, jstring index)
{
  // get the C string and the underlying map
  const char *key = env->GetStringUTFChars (index, 0);
  Knowledge_Map * packet = (Knowledge_Map *)cptr;
  Knowledge_Record * result (0);
  
  if (packet)
  {
    // get the record and return the string index
    result = new Knowledge_Record ((*packet)[key]);
  }

  env->ReleaseStringUTFChars (index, key);

  return (jlong) result;
}

/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_set
 * Signature: (JLjava/lang/String;J)V
 */
void JNICALL Java_com_madara_transport_filters_Packet_jni_1set
  (JNIEnv * env, jobject, jlong cptr, jstring index, jlong value)
{
  // get the C string and the underlying map
  const char * key = env->GetStringUTFChars (index, 0);
  Knowledge_Map * packet = (Knowledge_Map *)cptr;
  Knowledge_Record * result = (Knowledge_Record *)value;

  if (packet && result)
  {
    (*packet)[key] = *result;
  }

  // get the record and return the string index
  env->ReleaseStringUTFChars (index, key);
}

/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_get_keys
 * Signature: (J)[Ljava/lang/String;
 */
jobjectArray JNICALL Java_com_madara_transport_filters_Packet_jni_1get_1keys
  (JNIEnv * env, jobject, jlong cptr)
{
  Knowledge_Map * packet = (Knowledge_Map *)cptr;
  jobjectArray result (0);
  
  if (packet)
  {
    result = (jobjectArray) env->NewObjectArray (
      (jsize)packet->size (),
      Madara::Utility::Java::find_class (env, "java/lang/String"),
      env->NewStringUTF (""));
 
    jsize i = 0;
  
    for (Knowledge_Map::const_iterator cur = packet->begin ();
         cur != packet->end (); ++cur, ++i)
    {
      env->SetObjectArrayElement (
        result, i, env->NewStringUTF (cur->first.c_str ()));
    }
  }

  return result;
}
/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_exists
 * Signature: (JLjava/lang/String;)Z
 */
jboolean JNICALL Java_com_madara_transport_filters_Packet_jni_1exists
  (JNIEnv * env, jobject, jlong cptr, jstring index)
{
  jboolean result (false);
  Knowledge_Map * packet = (Knowledge_Map *)cptr;

  if (packet)
  {
    // get the C string and the underlying map
    const char *key = env->GetStringUTFChars (index, 0);

    // get the record and return the string index
    result = packet->find (key) != packet->end (); 
    env->ReleaseStringUTFChars (index, key);
  }

  return result;
}

void JNICALL Java_com_madara_transport_filters_Packet_jni_1clear
  (JNIEnv *, jobject, jlong cptr)
{
  Knowledge_Map * packet = (Knowledge_Map *)cptr;

  if (packet)
  {
    packet->clear ();
  }
}

void JNICALL Java_com_madara_transport_filters_Packet_jni_1erase
  (JNIEnv * env, jobject, jlong cptr, jstring index)
{
  Knowledge_Map * packet = (Knowledge_Map *)cptr;
  const char * key = env->GetStringUTFChars (index, 0);

  if (packet)
  {
    // erase the record
    packet->erase (key);
  }

  env->ReleaseStringUTFChars (index, key);
}
