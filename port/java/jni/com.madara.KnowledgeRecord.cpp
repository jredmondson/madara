/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "com.madara.KnowledgeRecord.h"
#include "madara/knowledge_engine/Knowledge_Record.h"

#include <stdio.h>

jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__
  (JNIEnv *, jobject)
{
  return (jlong) new Madara::Knowledge_Record();
}

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecordDeep
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecordDeep (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Knowledge_Record* ret = new Madara::Knowledge_Record(*(Madara::Knowledge_Record*)cptr);
  return (jlong)ret;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_KnowledgeRecord
* Signature: (Ljava/lang/String;)J
*/
jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__Ljava_lang_String_2 (JNIEnv * env, jobject obj, jstring data)
{
  const char *nativeData = env->GetStringUTFChars(data, 0);

  Madara::Knowledge_Record* ret = new Madara::Knowledge_Record(nativeData);

  env->ReleaseStringUTFChars(data, nativeData);

  return (jlong)ret;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_KnowledgeRecord
* Signature: (D)J
*/
jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__D (JNIEnv * env, jobject obj, jdouble data)
{
  return (jlong) (new Madara::Knowledge_Record(data));
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_KnowledgeRecord
* Signature: (J)J
*/
jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__J (JNIEnv * env, jobject obj, jlong data)
{
  return (jlong) (new Madara::Knowledge_Record(Madara::Knowledge_Record::Integer(data)));
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_KnowledgeRecord
* Signature: ([D)J
*/
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord___3D (JNIEnv * env, jclass cls, jdoubleArray data)
{
  jsize len = env->GetArrayLength(data);
  jboolean isCopy;
  jdouble* dblArray = env->GetDoubleArrayElements(data, &isCopy);
  std::vector<double> dblVector(len);
  for (int x = 0; x < len; x++)
    dblVector[x] = dblArray[x];
  jlong ret = (jlong) (new Madara::Knowledge_Record(dblVector));
  if (isCopy)
    env->ReleaseDoubleArrayElements(data, dblArray, JNI_ABORT);
  return ret;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_KnowledgeRecord
* Signature: ([J)J
*/
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord___3J (JNIEnv * env, jclass cls, jlongArray data)
{
  jsize len = env->GetArrayLength(data);
  jboolean isCopy;
  jlong* intArray = env->GetLongArrayElements(data, &isCopy);
  std::vector<Madara::Knowledge_Record::Integer> intVector(len);
  for (int x = 0; x < len; x++)
    intVector[x] = intArray[x];
  jlong ret = (jlong) (new Madara::Knowledge_Record(intVector));
  if (isCopy)
    env->ReleaseLongArrayElements(data, intArray, JNI_ABORT);
  return ret;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_toLongValue
* Signature: (J)J
*/
jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1toLongValue (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Knowledge_Record record = *(Madara::Knowledge_Record*)cptr;
  return (jlong) record.to_integer();
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_toStringValue
* Signature: (J)Ljava/lang/String;
*/
jstring JNICALL Java_com_madara_KnowledgeRecord_jni_1toStringValue (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Knowledge_Record record = *(Madara::Knowledge_Record*)cptr;
  return env->NewStringUTF(record.to_string().c_str());
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_toDoubleValue
* Signature: (J)D
*/
jdouble JNICALL Java_com_madara_KnowledgeRecord_jni_1toDoubleValue (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Knowledge_Record record = *(Madara::Knowledge_Record*)cptr;
  return (jdouble) record.to_double();
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_getType
* Signature: (J)I
*/
jint JNICALL Java_com_madara_KnowledgeRecord_jni_1getType (JNIEnv *env, jobject obj, jlong cptr)
{
  Madara::Knowledge_Record record = *(Madara::Knowledge_Record*)cptr;
  return record.type();
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_freeKnowledgeRecord
* Signature: (J)V
*/
void JNICALL Java_com_madara_KnowledgeRecord_jni_1freeKnowledgeRecord (JNIEnv * env, jobject obj, jlong cptr)
{
  Madara::Knowledge_Record* record = (Madara::Knowledge_Record*)cptr;
  if (record)
    delete record;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_toDoubleArray
* Signature: (J)[D
*/
MADARA_Export jdoubleArray JNICALL Java_com_madara_KnowledgeRecord_jni_1toDoubleArray (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Knowledge_Record record = *(Madara::Knowledge_Record*)cptr;
  std::vector<double> vec = record.to_doubles();

  jdouble * tmp = new jdouble [vec.size()];

  for (unsigned int x = 0; x < vec.size(); x++)
    tmp[x] = (jdouble) vec[x];

  jdoubleArray ret = env->NewDoubleArray((jsize)vec.size());
  env->SetDoubleArrayRegion(ret, 0, (jsize)vec.size(), tmp);

  delete [] tmp;

  return ret;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_toLongArray
* Signature: (J)[J
*/
MADARA_Export jlongArray JNICALL Java_com_madara_KnowledgeRecord_jni_1toLongArray (JNIEnv * env, jclass cls, jlong cptr)
{
  Madara::Knowledge_Record record = *(Madara::Knowledge_Record*)cptr;
  std::vector<Madara::Knowledge_Record::Integer> vec = record.to_integers();

  jlong * tmp = new jlong [vec.size()];

  for (unsigned int x = 0; x < vec.size(); x++)
    tmp[x] = (jlong) vec[x];

  jlongArray ret = env->NewLongArray((jsize)vec.size());
  env->SetLongArrayRegion(ret, 0, (jsize)vec.size(), tmp);

  delete [] tmp;

  return ret;
}

/*
* Class:     com_madara_KnowledgeList
* Method:    jni_freeKnowledgeList
* Signature: ([JI)V
*/
void JNICALL Java_com_madara_KnowledgeList_jni_1freeKnowledgeList (JNIEnv *env, jobject obj, jlongArray records, jint length)
{
  jboolean jniNoCopy = JNI_FALSE;
  jlong* nativeRecords = env->GetLongArrayElements(records, &jniNoCopy);
  for (int x = 0; x < length; x++)
  {
    Java_com_madara_KnowledgeRecord_jni_1freeKnowledgeRecord(env, obj, nativeRecords[x]);
  }
}

/*
* Class:     com_madara_KnowledgeMap
* Method:    jni_freeKnowledgeMap
* Signature: ([JI)V
*/
void JNICALL Java_com_madara_KnowledgeMap_jni_1freeKnowledgeMap (JNIEnv * env, jobject obj, jlongArray records, jint length)
{
  //KnowledgeMap#free is exactly the same as KnowledgeList#free, no reason to re-implement
  Java_com_madara_KnowledgeList_jni_1freeKnowledgeList(env, obj, records, length);
}


