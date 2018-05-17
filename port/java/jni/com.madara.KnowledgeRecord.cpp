/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "com.madara.KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeRecord.h"

#include <stdio.h>

// define useful shorthands
namespace knowledge = madara::knowledge;
typedef madara::knowledge::KnowledgeRecord  KnowledgeRecord;
typedef KnowledgeRecord::Integer Integer;


jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__
  (JNIEnv *, jobject)
{
  return (jlong) new KnowledgeRecord ();
}

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecordDeep
 * Signature: (J)J
 */
jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecordDeep (JNIEnv *, jobject, jlong cptr)
{
  KnowledgeRecord * result (0);
  KnowledgeRecord * source = (KnowledgeRecord *) cptr;

  if (source)
  {
    result = new KnowledgeRecord (*source);
  }

  return (jlong) result;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_KnowledgeRecord
* Signature: (Ljava/lang/String;)J
*/
jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__Ljava_lang_String_2 (JNIEnv * env, jobject, jstring data)
{
  const char *nativeData = env->GetStringUTFChars (data, 0);

  KnowledgeRecord * result = new KnowledgeRecord (nativeData);

  env->ReleaseStringUTFChars (data, nativeData);

  return (jlong)result;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_KnowledgeRecord
* Signature: (D)J
*/
jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__D (JNIEnv *, jobject, jdouble data)
{
  return (jlong) (new KnowledgeRecord (data));
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_KnowledgeRecord
* Signature: (J)J
*/
jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__J (JNIEnv *, jobject, jlong data)
{
  return (jlong) (new KnowledgeRecord (Integer (data)));
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_KnowledgeRecord
* Signature: ([D)J
*/
jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord___3D (JNIEnv * env, jclass, jdoubleArray data)
{
  jsize len = env->GetArrayLength (data);
  jboolean isCopy;
  std::vector<double> dblVector (len);

  jdouble* dblArray = env->GetDoubleArrayElements (data, &isCopy);

  // copy elements to STL vector
  for (int x = 0; x < len; x++)
    dblVector[x] = dblArray[x];

  jlong result = (jlong) (new KnowledgeRecord (dblVector));

  if (isCopy)
    env->ReleaseDoubleArrayElements (data, dblArray, JNI_ABORT);
  return result;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_KnowledgeRecord
* Signature: ([J)J
*/
jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord___3J (JNIEnv * env, jclass, jlongArray data)
{
  jsize len = env->GetArrayLength (data);
  jboolean isCopy;
  std::vector<KnowledgeRecord::Integer> intVector (len);
  jlong * intArray = env->GetLongArrayElements (data, &isCopy);

  // copy to STL vector
  for (int x = 0; x < len; x++)
    intVector[x] = intArray[x];

  jlong result = (jlong) (new KnowledgeRecord (intVector));

  if (isCopy)
    env->ReleaseLongArrayElements (data, intArray, JNI_ABORT);
  return result;
}

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_isValid
 * Signature: (J)Z
 */
jboolean JNICALL
Java_com_madara_KnowledgeRecord_jni_1isValid
  (JNIEnv *, jobject, jlong cptr)
{
  jboolean result (false);
  KnowledgeRecord * record = (KnowledgeRecord *) cptr;

  if (record)
  {
    result = record->is_valid ();
  }

  return result;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_toLongValue
* Signature: (J)J
*/
jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1toLongValue (JNIEnv *, jobject, jlong cptr)
{
  jlong result (0);
  KnowledgeRecord * record = (KnowledgeRecord *) cptr;

  if (record)
    result = record->to_integer ();

  return result;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_toStringValue
* Signature: (J)Ljava/lang/String;
*/
jstring JNICALL
Java_com_madara_KnowledgeRecord_jni_1toStringValue (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  KnowledgeRecord * record = (KnowledgeRecord *) cptr;

  if (record)
  {
    result = env->NewStringUTF (record->to_string ().c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_toDoubleValue
* Signature: (J)D
*/
jdouble JNICALL
Java_com_madara_KnowledgeRecord_jni_1toDoubleValue (JNIEnv *, jobject, jlong cptr)
{
  jdouble result (0);
  KnowledgeRecord * record = (KnowledgeRecord *) cptr;

  if (record)
    result = record->to_double ();

  return result;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_getType
* Signature: (J)I
*/
jint JNICALL
Java_com_madara_KnowledgeRecord_jni_1getType (JNIEnv *, jobject, jlong cptr)
{
  jint result (0);
  KnowledgeRecord * record = (KnowledgeRecord *) cptr;

  if (record)
    result = record->type ();

  return result;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_freeKnowledgeRecord
* Signature: (J)V
*/
void JNICALL
Java_com_madara_KnowledgeRecord_jni_1freeKnowledgeRecord (JNIEnv *, jobject, jlong cptr)
{
  KnowledgeRecord * record = (KnowledgeRecord *) cptr;

  if (record)
    delete record;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_toDoubleArray
* Signature: (J)[D
*/
jdoubleArray JNICALL
Java_com_madara_KnowledgeRecord_jni_1toDoubleArray (JNIEnv * env, jclass, jlong cptr)
{
  jdoubleArray result (0);
  KnowledgeRecord * record = (KnowledgeRecord *) cptr;

  if (record)
  {
    std::vector<double> vec = record->to_doubles ();

    jdouble * tmp = new jdouble [vec.size ()];

    // copy elements to the temporary array
    for (unsigned int x = 0; x < vec.size (); x++)
      tmp[x] = (jdouble) vec[x];

    // setup the Java array
    result = env->NewDoubleArray ( (jsize)vec.size ());
    env->SetDoubleArrayRegion (result, 0, (jsize)vec.size (), tmp);

    delete [] tmp;
  }

  return result;
}

/*
* Class:     com_madara_KnowledgeRecord
* Method:    jni_toLongArray
* Signature: (J)[J
*/
jlongArray JNICALL
Java_com_madara_KnowledgeRecord_jni_1toLongArray (JNIEnv * env, jclass, jlong cptr)
{
  jlongArray result (0);
  KnowledgeRecord * record = (KnowledgeRecord *) cptr;

  if (record)
  {
    std::vector<KnowledgeRecord::Integer> vec = record->to_integers ();

    jlong * tmp = new jlong [vec.size ()];

    // copy to the temporary array
    for (unsigned int x = 0; x < vec.size (); x++)
      tmp[x] = (jlong) vec[x];

    // setup the Java array
    result = env->NewLongArray ( (jsize)vec.size ());
    env->SetLongArrayRegion (result, 0, (jsize)vec.size (), tmp);

    delete [] tmp;
  }

  return result;
}

/*
* Class:     com_madara_KnowledgeList
* Method:    jni_freeKnowledgeList
* Signature: ([JI)V
*/
void JNICALL
Java_com_madara_KnowledgeList_jni_1freeKnowledgeList (JNIEnv *env, jobject obj, jlongArray records, jint length)
{
  jboolean jniNoCopy = JNI_FALSE;
  jlong * nativeRecords = env->GetLongArrayElements (records, &jniNoCopy);

  // iterate over list and free each record
  for (int x = 0; x < length; x++)
  {
    Java_com_madara_KnowledgeRecord_jni_1freeKnowledgeRecord (env, obj, nativeRecords[x]);
  }
}

/*
* Class:     com_madara_KnowledgeMap
* Method:    jni_freeKnowledgeMap
* Signature: ([JI)V
*/
void JNICALL
Java_com_madara_KnowledgeMap_jni_1freeKnowledgeMap (JNIEnv * env, jobject obj, jlongArray records, jint length)
{
  //KnowledgeMap#free is exactly the same as KnowledgeList#free, no reason to re-implement
  Java_com_madara_KnowledgeList_jni_1freeKnowledgeList (env, obj, records, length);
}


