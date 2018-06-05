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

#ifndef _Included_com_madara_KnowledgeRecord
#define _Included_com_madara_KnowledgeRecord

#include <jni.h>
#include "madara/MadaraExport.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: ()J
 */
MADARA_EXPORT jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__
  (JNIEnv *, jobject);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecordDeep
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecordDeep
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: (Ljava/lang/String;)J
 */
MADARA_EXPORT jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__Ljava_lang_String_2
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: (D)J
 */
MADARA_EXPORT jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__D
  (JNIEnv *, jobject, jdouble);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord__J
  (JNIEnv *, jobject, jlong);
    
/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: ([D)J
 */
MADARA_EXPORT jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord___3D
  (JNIEnv *, jclass, jdoubleArray);
    
/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: ([J)J
 */
MADARA_EXPORT jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1KnowledgeRecord___3J
  (JNIEnv *, jclass, jlongArray);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_toLongValue
 * Signature: (J)J
 */
MADARA_EXPORT jlong JNICALL
Java_com_madara_KnowledgeRecord_jni_1toLongValue
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_toStringValue
 * Signature: (J)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_com_madara_KnowledgeRecord_jni_1toStringValue
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_toDoubleValue
 * Signature: (J)D
 */
MADARA_EXPORT jdouble JNICALL
Java_com_madara_KnowledgeRecord_jni_1toDoubleValue
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_getType
 * Signature: (J)I
 */
MADARA_EXPORT jint JNICALL
Java_com_madara_KnowledgeRecord_jni_1getType
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_freeKnowledgeRecord
 * Signature: (J)V
 */
MADARA_EXPORT void JNICALL
Java_com_madara_KnowledgeRecord_jni_1freeKnowledgeRecord
  (JNIEnv *, jobject, jlong);
    
/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_toDoubleArray
 * Signature: (J)[D
 */
MADARA_EXPORT jdoubleArray JNICALL
Java_com_madara_KnowledgeRecord_jni_1toDoubleArray
  (JNIEnv *, jclass, jlong);
    
/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_toLongArray
 * Signature: (J)[J
 */
MADARA_EXPORT jlongArray JNICALL
Java_com_madara_KnowledgeRecord_jni_1toLongArray
  (JNIEnv *, jclass, jlong);
  
/*
 * Class:     com_madara_KnowledgeList
 * Method:    jni_freeKnowledgeList
 * Signature: ([JI)V
 */
MADARA_EXPORT void JNICALL
Java_com_madara_KnowledgeList_jni_1freeKnowledgeList
  (JNIEnv *, jobject, jlongArray, jint);
  
/*
 * Class:     com_madara_KnowledgeRecord
 * Method:    jni_isValid
 * Signature: (J)Z
 */
MADARA_EXPORT jboolean JNICALL
Java_com_madara_KnowledgeRecord_jni_1isValid
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_madara_KnowledgeMap
 * Method:    jni_freeKnowledgeMap
 * Signature: ([JI)V
 */
MADARA_EXPORT void JNICALL
Java_com_madara_KnowledgeMap_jni_1freeKnowledgeMap
  (JNIEnv *, jobject, jlongArray, jint);

#ifdef __cplusplus
}
#endif
#endif
