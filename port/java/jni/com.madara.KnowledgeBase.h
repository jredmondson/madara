/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _Included_com_madara_KnowledgeBase_Impl
#define _Included_com_madara_KnowledgeBase_Impl

#include <jni.h>
#include "madara/MADARA_export.h"

#ifdef __cplusplus
extern "C" {
#endif


//===================================================================================
//KnowledgeBase
//===================================================================================
/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_KnowledgeBase
 * Signature: ()J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1KnowledgeBase__
  (JNIEnv *, jobject);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_KnowledgeBase
 * Signature: (Ljava/lang/String;ILjava/lang/String;)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1KnowledgeBase__Ljava_lang_String_2ILjava_lang_String_2
  (JNIEnv *, jobject, jstring, jint, jstring);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_KnowledgeBase
 * Signature: (Ljava/lang/String;J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1KnowledgeBase__Ljava_lang_String_2J
  (JNIEnv *, jobject, jstring, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_KnowledgeBase
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1KnowledgeBase__J
  (JNIEnv *, jobject, jlong);
  
/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_evaluate
 * Signature: (JLjava/lang/String;J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1evaluate__JLjava_lang_String_2J
  (JNIEnv *, jobject, jlong, jstring, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_evaluate
 * Signature: (JJJ)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1evaluate__JJJ
  (JNIEnv *, jobject, jlong, jlong, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_evaluateNoReturn
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1evaluateNoReturn__JLjava_lang_String_2J
  (JNIEnv *, jobject, jlong, jstring, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_evaluateNoReturn
 * Signature: (JJJ)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1evaluateNoReturn__JJJ
  (JNIEnv *, jobject, jlong, jlong, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_compile
 * Signature: (JLjava/lang/String;)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1compile
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_defineFunction
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_defineFunction
 * Signature: (JLjava/lang/String;Ljava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2Ljava_lang_String_2
  (JNIEnv *, jobject, jlong, jstring, jstring);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_defineFunction
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2J
  (JNIEnv *, jobject, jlong, jstring, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_defineFunction
 * Signature: (JLjava/lang/String;Lcom/madara/MadaraFunction;)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2Lcom_madara_MadaraFunction_2
  (JNIEnv *, jobject, jlong, jstring, jobject);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_clear
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1clear
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_exists
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export jboolean JNICALL Java_com_madara_KnowledgeBase_jni_1exists
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_get
 * Signature: (JLjava/lang/String;)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1get
  (JNIEnv *, jobject, jlong, jstring);
  
/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_setInteger
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1setInteger
  (JNIEnv *, jclass, jlong, jstring, jlong);
    
/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_setDouble
 * Signature: (JLjava/lang/String;D)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1setDouble
  (JNIEnv *, jclass, jlong, jstring, jdouble);
    
/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_setString
 * Signature: (JLjava/lang/String;Ljava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1setString
  (JNIEnv *, jclass, jlong, jstring, jstring);
    
/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_setIntegerArray
 * Signature: (JLjava/lang/String;[J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1setIntegerArray
  (JNIEnv *, jclass, jlong, jstring, jlongArray);
    
/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_setDoubleArray
 * Signature: (JLjava/lang/String;[D)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1setDoubleArray
  (JNIEnv *, jclass, jlong, jstring, jdoubleArray);
  
/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_setIntegerSettings
 * Signature: (JLjava/lang/String;JJ)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1setIntegerSettings
  (JNIEnv *, jclass, jlong, jstring, jlong, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_setDoubleSettings
 * Signature: (JLjava/lang/String;DJ)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1setDoubleSettings
  (JNIEnv *, jclass, jlong, jstring, jdouble, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_setStringSettings
 * Signature: (JLjava/lang/String;Ljava/lang/String;J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1setStringSettings
  (JNIEnv *, jclass, jlong, jstring, jstring, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_setIntegerArraySettings
 * Signature: (JLjava/lang/String;[JJ)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1setIntegerArraySettings
  (JNIEnv *, jclass, jlong, jstring, jlongArray, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_setDoubleArraySettings
 * Signature: (JLjava/lang/String;[DJ)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1setDoubleArraySettings
  (JNIEnv *, jclass, jlong, jstring, jdoubleArray, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_sendModifieds
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1sendModifieds__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_sendModifieds
 * Signature: (JJ)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1sendModifieds__JJ
  (JNIEnv *, jobject, jlong, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_print
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1print__JLjava_lang_String_2
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_print
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1print__J
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_wait
 * Signature: (JLjava/lang/String;J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1wait__JLjava_lang_String_2J
  (JNIEnv *, jobject, jlong, jstring, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_wait
 * Signature: (JJJ)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1wait__JJJ
  (JNIEnv *, jobject, jlong, jlong, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_waitNoReturn
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1waitNoReturn__JLjava_lang_String_2J
  (JNIEnv *, jobject, jlong, jstring, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_waitNoReturn
 * Signature: (JJJ)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1waitNoReturn__JJJ
  (JNIEnv *, jobject, jlong, jlong, jlong);  

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_freeKnowledgeBase
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1freeKnowledgeBase
  (JNIEnv *, jobject, jlong);
  
/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_toKnowledgeList
 * Signature: (JLjava/lang/String;II)[J
 */
MADARA_Export jlongArray JNICALL Java_com_madara_KnowledgeBase_jni_1toKnowledgeList
  (JNIEnv *, jobject, jlong, jstring, jint, jint);
  
/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_saveContext
 * Signature: (JLjava/lang/String;)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1saveContext
  (JNIEnv *, jobject, jlong, jstring);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_saveCheckpoint
 * Signature: (JLjava/lang/String;Z)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1saveCheckpoint
  (JNIEnv *, jobject, jlong, jstring, jboolean);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_loadContext
 * Signature: (JLjava/lang/String;ZJ)J
 */
MADARA_Export jlong JNICALL Java_com_madara_KnowledgeBase_jni_1loadContext
  (JNIEnv *, jobject, jlong, jstring, jboolean, jlong);

/*
 * Class:     com_madara_KnowledgeBase
 * Method:    jni_toKnowledgeMap
 * Signature: (JLjava/lang/String;Lcom/madara/KnowledgeBase/MapReturn;)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_jni_1toKnowledgeMap
  (JNIEnv *, jobject, jlong, jstring, jobject);
  
//===================================================================================
  
//===================================================================================
//CompiledExpression
//===================================================================================
/*
 * Class:     com_madara_KnowledgeBase_CompiledExpression
 * Method:    jni_freeCompiledExpression
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_KnowledgeBase_00024CompiledExpression_jni_1freeCompiledExpression
  (JNIEnv *, jobject, jlong);  
//===================================================================================

#ifdef __cplusplus
}
#endif
#endif
