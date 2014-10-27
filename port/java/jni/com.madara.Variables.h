/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _Included_com_madara_Variables
#define _Included_com_madara_Variables

#include <jni.h>
#include "madara/MADARA_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_madara_Variables
 * Method:    jni_evaluate
 * Signature: (JJJ)J
 */
MADARA_Export jlong JNICALL Java_com_madara_Variables_jni_1evaluate__JJJ
  (JNIEnv *, jobject, jlong, jlong, jlong);

/*
 * Class:     com_madara_Variables
 * Method:    jni_compile
 * Signature: (JLjava/lang/String;)J
 */
MADARA_Export jlong JNICALL Java_com_madara_Variables_jni_1compile
  (JNIEnv *, jobject, jlong, jstring);
  
/*
 * Class:     com_madara_Variables
 * Method:    jni_get
 * Signature: (JLjava/lang/String;)J
 */
MADARA_Export jlong JNICALL Java_com_madara_Variables_jni_1get
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_Variables
 * Method:    jni_set
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_Export void JNICALL Java_com_madara_Variables_jni_1set
  (JNIEnv *, jobject, jlong, jstring, jlong);

#ifdef __cplusplus
}
#endif
#endif
