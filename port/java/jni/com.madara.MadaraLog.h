/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _Included_com_madara_MadaraLog
#define _Included_com_madara_MadaraLog

#include <jni.h>
#include "madara/MADARA_export.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_madara_MadaraLog
 * Method:    jni_setLogLevel
 * Signature: (I)V
 */
MADARA_Export void JNICALL Java_com_madara_MadaraLog_jni_1setLogLevel
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_madara_MadaraLog
 * Method:    jni_getLogLevel
 * Signature: ()I
 */
MADARA_Export jint JNICALL Java_com_madara_MadaraLog_jni_1getLogLevel
  (JNIEnv *, jclass);

#ifdef __cplusplus
}
#endif
#endif
