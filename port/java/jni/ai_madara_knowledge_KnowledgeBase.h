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
 * @author James Root <james.root@gmail.com>
 *********************************************************************/

#ifndef _Included_ai_madara_knowledge_KnowledgeBaseImpl
#define _Included_ai_madara_knowledge_KnowledgeBaseImpl

#include <jni.h>
#include "madara/MadaraExport.h"

#ifdef __cplusplus
extern "C" {
#endif

//===================================================================================
// KnowledgeBase
//===================================================================================
/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_KnowledgeBase
 * Signature: ()J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1KnowledgeBase__(JNIEnv*, jobject);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_KnowledgeBase
 * Signature: (Ljava/lang/String;ILjava/lang/String;)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1KnowledgeBase__Ljava_lang_String_2ILjava_lang_String_2(
    JNIEnv*, jobject, jstring, jint, jstring);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_KnowledgeBase
 * Signature: (Ljava/lang/String;J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1KnowledgeBase__Ljava_lang_String_2J(
    JNIEnv*, jobject, jstring, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_KnowledgeBase
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1KnowledgeBase__J(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_getID
 * Signature: (J)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL Java_ai_madara_knowledge_KnowledgeBase_jni_1getID(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_attachLogger
 * Signature: (JJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1attachLogger(
    JNIEnv*, jobject, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_attachTransport
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1attachTransport(
    JNIEnv*, jobject, jlong, jstring, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_debugModifieds
 * Signature: (J)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1debugModifieds(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_closeTransports
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1closeTransports(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_getLogger
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1getLogger(JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_evaluate
 * Signature: (JLjava/lang/String;J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1evaluate__JLjava_lang_String_2J(
    JNIEnv*, jobject, jlong, jstring, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_evaluate
 * Signature: (JJJ)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1evaluate__JJJ(
    JNIEnv*, jobject, jlong, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_evaluateNoReturn
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1evaluateNoReturn__JLjava_lang_String_2J(
    JNIEnv*, jobject, jlong, jstring, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_evaluateNoReturn
 * Signature: (JJJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1evaluateNoReturn__JJJ(
    JNIEnv*, jobject, jlong, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_toString
 * Signature:
 * (JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1toString(
    JNIEnv*, jobject, jlong, jstring, jstring, jstring);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_compile
 * Signature: (JLjava/lang/String;)J
 */
MADARA_EXPORT jlong JNICALL Java_ai_madara_knowledge_KnowledgeBase_jni_1compile(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_defineFunction
 * Signature: (JLjava/lang/String;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_defineFunction
 * Signature: (JLjava/lang/String;Ljava/lang/String;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2Ljava_lang_String_2(
    JNIEnv*, jobject, jlong, jstring, jstring);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_defineFunction
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2J(
    JNIEnv*, jobject, jlong, jstring, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_defineFunction
 * Signature: (JLjava/lang/String;Lai/madara/MadaraFunction;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1defineFunction__JLjava_lang_String_2Lai_madara_MadaraFunction_2(
    JNIEnv*, jobject, jlong, jstring, jobject);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_clear
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_KnowledgeBase_jni_1clear(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_exists
 * Signature: (JLjava/lang/String;)V
 */
MADARA_EXPORT jboolean JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1exists(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_get
 * Signature: (JLjava/lang/String;)J
 */
MADARA_EXPORT jlong JNICALL Java_ai_madara_knowledge_KnowledgeBase_jni_1get(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setInteger
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setInteger(
    JNIEnv*, jclass, jlong, jstring, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setDouble
 * Signature: (JLjava/lang/String;D)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setDouble(
    JNIEnv*, jclass, jlong, jstring, jdouble);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setString
 * Signature: (JLjava/lang/String;Ljava/lang/String;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setString(
    JNIEnv*, jclass, jlong, jstring, jstring);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setIntegerArray
 * Signature: (JLjava/lang/String;[J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setIntegerArray(
    JNIEnv*, jclass, jlong, jstring, jlongArray);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setDoubleArray
 * Signature: (JLjava/lang/String;[D)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setDoubleArray(
    JNIEnv*, jclass, jlong, jstring, jdoubleArray);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setFile
 * Signature: (JLjava/lang/String;[B)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_KnowledgeBase_jni_1setFile(
    JNIEnv*, jclass, jlong, jstring, jbyteArray);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setImage
 * Signature: (JLjava/lang/String;[B)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_KnowledgeBase_jni_1setImage(
    JNIEnv*, jclass, jlong, jstring, jbyteArray);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setAny
 * Signature: (JLjava/lang/String;JJ)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setAny(
    JNIEnv*, jclass, jlong, jstring, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setAnySettings
 * Signature: (JLjava/lang/String;JJJ)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setAnySettings(
    JNIEnv*, jclass, jlong, jstring, jlong, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setIntegerSettings
 * Signature: (JLjava/lang/String;JJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setIntegerSettings(
    JNIEnv*, jclass, jlong, jstring, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setDoubleSettings
 * Signature: (JLjava/lang/String;DJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setDoubleSettings(
    JNIEnv*, jclass, jlong, jstring, jdouble, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setStringSettings
 * Signature: (JLjava/lang/String;Ljava/lang/String;J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setStringSettings(
    JNIEnv*, jclass, jlong, jstring, jstring, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setIntegerArraySettings
 * Signature: (JLjava/lang/String;[JJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setIntegerArraySettings(
    JNIEnv*, jclass, jlong, jstring, jlongArray, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setDoubleArraySettings
 * Signature: (JLjava/lang/String;[DJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setDoubleArraySettings(
    JNIEnv*, jclass, jlong, jstring, jdoubleArray, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setFileSettings
 * Signature: (JLjava/lang/String;[BJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setFileSettings(
    JNIEnv*, jclass, jlong, jstring, jbyteArray, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setImageSettings
 * Signature: (JLjava/lang/String;[BJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setImageSettings(
    JNIEnv*, jclass, jlong, jstring, jbyteArray, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_sendModifieds
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1sendModifieds__J(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_sendModifieds
 * Signature: (JJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1sendModifieds__JJ(
    JNIEnv*, jobject, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_clearModifieds
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1clearModifieds(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_print
 * Signature: (JLjava/lang/String;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1print__JLjava_lang_String_2(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_print
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_KnowledgeBase_jni_1print__J(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_wait
 * Signature: (JLjava/lang/String;J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1wait__JLjava_lang_String_2J(
    JNIEnv*, jobject, jlong, jstring, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_wait
 * Signature: (JJJ)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1wait__JJJ(
    JNIEnv*, jobject, jlong, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_waitNoReturn
 * Signature: (JLjava/lang/String;J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1waitNoReturn__JLjava_lang_String_2J(
    JNIEnv*, jobject, jlong, jstring, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_waitNoReturn
 * Signature: (JJJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1waitNoReturn__JJJ(
    JNIEnv*, jobject, jlong, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_freeKnowledgeBase
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1freeKnowledgeBase(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_toKnowledgeList
 * Signature: (JLjava/lang/String;II)[J
 */
MADARA_EXPORT jlongArray JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1toKnowledgeList(
    JNIEnv*, jobject, jlong, jstring, jint, jint);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_saveContext
 * Signature: (JLjava/lang/String;)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1saveContext(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_saveAsKarl
 * Signature: (JLjava/lang/String;)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1saveAsKarl(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_saveCheckpoint
 * Signature: (JLjava/lang/String;Z)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1saveCheckpoint(
    JNIEnv*, jobject, jlong, jstring, jboolean);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_loadContext
 * Signature: (JLjava/lang/String;ZJ)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1loadContext(
    JNIEnv*, jobject, jlong, jstring, jboolean, jlong);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_toKnowledgeMap
 * Signature:
 * (JLjava/lang/String;Lai/madara/knowledge/KnowledgeBase/MapReturn;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1toKnowledgeMap(
    JNIEnv*, jobject, jlong, jstring, jobject);

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_toMap
 * Signature:
 * (JLjava/lang/String;Ljava/lang/String;Lai/madara/knowledge/KnowledgeBase/MapReturn;)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_KnowledgeBase_jni_1toMap(
    JNIEnv*, jobject, jlong, jstring, jstring, jobject);

//===================================================================================

//===================================================================================
// CompiledExpression
//===================================================================================
/*
 * Class:     ai_madara_knowledge_KnowledgeBaseCompiledExpression
 * Method:    jni_freeCompiledExpression
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_KnowledgeBase_00024CompiledExpression_jni_1freeCompiledExpression(
    JNIEnv*, jobject, jlong);
//===================================================================================

#ifdef __cplusplus
}
#endif
#endif
