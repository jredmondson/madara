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
/* Header for class ai_madara_knowledge_containers_Counter */

#ifndef _Included_ai_madara_knowledge_containers_Counter
#define _Included_ai_madara_knowledge_containers_Counter
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_Counter
 * Signature: ()J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1Counter__(JNIEnv*, jobject);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_Counter
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1Counter__J(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_freeCounter
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1freeCounter(
    JNIEnv*, jclass, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_set
 * Signature: (JJ)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_containers_Counter_jni_1set(
    JNIEnv*, jobject, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1getName(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1setName(
    JNIEnv*, jobject, jlong, jlong, jlong, jstring);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_toString
 * Signature: (J)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1toString(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_toDouble
 * Signature: (J)D
 */
MADARA_EXPORT jdouble JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1toDouble(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_toLong
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1toLong(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_inc
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_containers_Counter_jni_1inc(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_dec
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL Java_ai_madara_knowledge_containers_Counter_jni_1dec(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_incValue
 * Signature: (JJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1incValue(
    JNIEnv*, jobject, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_decValue
 * Signature: (JJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1decValue(
    JNIEnv*, jobject, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_modify
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1modify(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_resize
 * Signature: (JII)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1resize(
    JNIEnv*, jobject, jlong, jint, jint);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_setSettings
 * Signature: (JJ)V
 */
MADARA_EXPORT void JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1setSettings(
    JNIEnv*, jobject, jlong, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_isTrue
 * Signature: (J)Z
 */
MADARA_EXPORT jboolean JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1isTrue(
    JNIEnv*, jobject, jlong);

/*
 * Class:     ai_madara_knowledge_containers_Counter
 * Method:    jni_isFalse
 * Signature: (J)Z
 */
MADARA_EXPORT jboolean JNICALL
Java_ai_madara_knowledge_containers_Counter_jni_1isFalse(
    JNIEnv*, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif
