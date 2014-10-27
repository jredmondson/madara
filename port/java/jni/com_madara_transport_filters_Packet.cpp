#include "com_madara_transport_filters_Packet.h"
#include "madara/knowledge_engine/Knowledge_Record.h"

/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_get
 * Signature: (JLjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_madara_transport_filters_Packet_jni_1get
  (JNIEnv * env, jobject, jlong cptr, jstring index)
{
  // get the C string and the underlying map
  const char *key = env->GetStringUTFChars(index, 0);
  Madara::Knowledge_Map * packet = (Madara::Knowledge_Map *)cptr;

  
  // get the record and return the string index
  Madara::Knowledge_Record * result = new Madara::Knowledge_Record((*packet)[key]);
  env->ReleaseStringUTFChars(index, key);

  return (jlong) result;
}

/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_set
 * Signature: (JLjava/lang/String;J)V
 */
JNIEXPORT void JNICALL Java_com_madara_transport_filters_Packet_jni_1set
  (JNIEnv * env, jobject, jlong cptr, jstring index, jlong value)
{
  // get the C string and the underlying map
  const char *key = env->GetStringUTFChars(index, 0);
  Madara::Knowledge_Map * packet = (Madara::Knowledge_Map *)cptr;
  Madara::Knowledge_Record * result = (Madara::Knowledge_Record *)value;

  (*packet)[key] = *result;
  
  // get the record and return the string index
  env->ReleaseStringUTFChars(index, key);
}

/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_get_keys
 * Signature: (J)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_madara_transport_filters_Packet_jni_1get_1keys
  (JNIEnv * env, jobject, jlong cptr)
{
  Madara::Knowledge_Map * packet = (Madara::Knowledge_Map *)cptr;
  jobjectArray result = (jobjectArray) env->NewObjectArray(
    (jsize)packet->size(), env->FindClass("java/lang/String"),
    env->NewStringUTF(""));
 
  jsize i = 0;
  
  for (Madara::Knowledge_Map::const_iterator cur = packet->begin ();
       cur != packet->end (); ++cur, ++i)
  {
    env->SetObjectArrayElement(
      result, i, env->NewStringUTF(cur->first.c_str ()));
  }

  return result;
}
/*
 * Class:     com_madara_transport_filters_Packet
 * Method:    jni_exists
 * Signature: (JLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_madara_transport_filters_Packet_jni_1exists
  (JNIEnv * env, jobject, jlong cptr, jstring index)
{
  jboolean result (false);

  // get the C string and the underlying map
  const char *key = env->GetStringUTFChars(index, 0);
  Madara::Knowledge_Map * packet = (Madara::Knowledge_Map *)cptr;

  // get the record and return the string index
  result = packet->find (key) != packet->end (); 
  env->ReleaseStringUTFChars(index, key);

  return result;
}

void JNICALL Java_com_madara_transport_filters_Packet_jni_1clear
  (JNIEnv *, jobject, jlong cptr)
{
  Madara::Knowledge_Map * packet = (Madara::Knowledge_Map *)cptr;

  packet->clear ();
}

void JNICALL Java_com_madara_transport_filters_Packet_jni_1erase
  (JNIEnv * env, jobject, jlong cptr, jstring index)
{
  // get the C string and the underlying map
  const char *key = env->GetStringUTFChars(index, 0);
  Madara::Knowledge_Map * packet = (Madara::Knowledge_Map *)cptr;

  // erase the record
  packet->erase (key);
  env->ReleaseStringUTFChars(index, key);
}
