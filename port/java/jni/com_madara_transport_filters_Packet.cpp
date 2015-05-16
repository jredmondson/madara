#include "com_madara_transport_filters_Packet.h"
#include "madara/knowledge_engine/Knowledge_Record.h"

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
      (jsize)packet->size (), env->FindClass ("java/lang/String"),
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
