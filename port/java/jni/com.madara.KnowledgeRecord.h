/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _Included_com_madara_KnowledgeRecord
#define _Included_com_madara_KnowledgeRecord

#include <jni.h>
#include "madara/MADARA_export.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: ()J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__
  (JNIEnv *, jobject);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecordDeep
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecordDeep
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: (Ljava/lang/String;)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__Ljava_lang_String_2
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: (D)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__D
  (JNIEnv *, jobject, jdouble);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__J
  (JNIEnv *, jobject, jlong);
    
/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: ([D)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord___3D
  (JNIEnv *, jclass, jdoubleArray);
    
/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: ([J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord___3J
  (JNIEnv *, jclass, jlongArray);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_toLongValue
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeRecord_jni_1toLongValue
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_toStringValue
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_KnowledgeRecord_jni_1toStringValue
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_toDoubleValue
 * Signature: (J)D
 */
MADARA_Export jdouble JNICALL Java_com_madara_KnowledgeRecord_jni_1toDoubleValue
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_getType
 * Signature: (J)I
 */
MADARA_Export jint JNICALL Java_com_madara_KnowledgeRecord_jni_1getType
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_freeKnowledgeRecord
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeRecord_jni_1freeKnowledgeRecord
  (JNIEnv *, jobject, jlong);
    
/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_toDoubleArray
 * Signature: (J)[D
 */
MADARA_Export jdoubleArray JNICALL Java_com_madara_KnowledgeRecord_jni_1toDoubleArray
  (JNIEnv *, jclass, jlong);
    
/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_toLongArray
 * Signature: (J)[J
 */
MADARA_Export jlongArray JNICALL Java_com_madara_KnowledgeRecord_jni_1toLongArray
  (JNIEnv *, jclass, jlong);
  
/*
 * Class:     com_madara_KnowledgeList
 * Method:    jni_freeKnowledgeList
 * Signature: ([JI)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeList_jni_1freeKnowledgeList
  (JNIEnv *, jobject, jlongArray, jint);
  
/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_isValid
 * Signature: (J)Z
 */
MADARA_Export jboolean JNICALL Java_com_madara_KnowledgeRecord_jni_1isValid
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeMap
 * Method:    jni_freeKnowledgeMap
 * Signature: ([JI)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeMap_jni_1freeKnowledgeMap
  (JNIEnv *, jobject, jlongArray, jint);

#ifdef __cplusplus
}
#endif
#endif
