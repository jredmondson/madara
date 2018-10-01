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
#include <jni.h>
#include "madara/MadaraExport.h"
/* Header for class ai_madara_knowledge_containers_Map */

#ifndef _Included_ai_madara_knowledge_containers_Map
#define _Included_ai_madara_knowledge_containers_Map
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_Map
 * Signature: ()J
 */
MADARA_EXPORT jlong JNICALL Java_ai_madara_knowledge_containers_Map_jni_1Map__(
    JNIEnv*, jobject);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_Map
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL Java_ai_madara_knowledge_containers_Map_jni_1Map__J(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_freeMap
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1freeMap(
    JNIEnv*, jclass, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_clear
 * Signature: (JZ)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1clear(
    JNIEnv*, jobject, jlong, jboolean);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_erase
 * Signature: (JLjava/lang/String;)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1erase(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_getDelimiter
 * Signature: (J)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1getDelimiter(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_setDelimiter
 * Signature: (JLjava/lang/String;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1setDelimiter(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_keys
 * Signature: (J)[Ljava/lang/String;
 */
MADARA_EXPORT jobjectArray JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1keys(JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_sync
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1sync(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_set
 * Signature: (JLjava/lang/String;Ljava/lang/String;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1setString__JLjava_lang_String_2Ljava_lang_String_2(
    JNIEnv*, jobject, jlong, jstring, jstring);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_set
 * Signature: (JLjava/lang/String;D)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1setDouble__JLjava_lang_String_2D(
    JNIEnv*, jobject, jlong, jstring, jdouble);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_set
 * Signature: (JLjava/lang/String;JJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1set__JLjava_lang_String_2JJ(
    JNIEnv*, jobject, jlong, jstring, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1getName(JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1setName(
    JNIEnv*, jobject, jlong, jlong, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_get
 * Signature: (JLjava/lang/String;)J
 */
MADARA_EXPORT jlong JNICALL Java_ai_madara_knowledge_containers_Map_jni_1get(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_toRecord
 * Signature: (JLjava/lang/String;)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1toRecord(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    modify
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_containers_Map_jni_1modify(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    modifyIndex
 * Signature: (JLjava/lang/String;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1modifyIndex(
    JNIEnv*, jobject, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_containers_IntegerVector
 * Method:    jni_setSettings
 * Signature: (JJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1setSettings(
    JNIEnv*, jobject, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_isTrue
 * Signature: (J)Z
 */
MADARA_EXPORT jboolean JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1isTrue(JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Map
 * Method:    jni_isFalse
 * Signature: (J)Z
 */
MADARA_EXPORT jboolean JNICALL
Java_ai_madara_knowledge_containers_Map_jni_1isFalse(JNIEnv*, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
