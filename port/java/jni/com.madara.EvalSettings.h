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

#ifndef _Included_com_madara_EvalSettings
#define _Included_com_madara_EvalSettings

#include <jni.h>
#include "madara/MADARA_export.h"

#ifdef __cplusplus
extern "C" {
#endif

//===================================================================================
//EvalSettings
//===================================================================================

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_getDefaultEvalSettings
 * Signature: (I)J
 */
MADARA_Export jlong JNICALL
Java_com_madara_EvalSettings_jni_1getDefaultEvalSettings
  (JNIEnv *, jclass, jint);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_evalSettings
 * Signature: ()J
 */
MADARA_Export jlong JNICALL
Java_com_madara_EvalSettings_jni_1evalSettings__
  (JNIEnv *, jclass);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_evalSettings
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL
Java_com_madara_EvalSettings_jni_1evalSettings__J
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_setDelaySendingModifieds
 * Signature: (JZ)V
 */
MADARA_Export void JNICALL
Java_com_madara_EvalSettings_jni_1setDelaySendingModifieds
  (JNIEnv *, jclass, jlong, jboolean);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_getDelaySendingModifieds
 * Signature: (J)Z
 */
MADARA_Export jboolean JNICALL
Java_com_madara_EvalSettings_jni_1getDelaySendingModifieds
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_setPrePrintStatement
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL
Java_com_madara_EvalSettings_jni_1setPrePrintStatement
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_getPrePrintStatement
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL
Java_com_madara_EvalSettings_jni_1getPrePrintStatement
  (JNIEnv *, jclass, jlong);
  
/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_setPostPrintStatement
 * Signature: (JLjava/lang/String;)V
 */
MADARA_Export void JNICALL
Java_com_madara_EvalSettings_jni_1setPostPrintStatement
  (JNIEnv *, jclass, jlong, jstring);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_getPostPrintStatement
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL
Java_com_madara_EvalSettings_jni_1getPostPrintStatement
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_setAlwaysOverwrite
 * Signature: (JZ)V
 */
MADARA_Export void JNICALL
Java_com_madara_EvalSettings_jni_1setAlwaysOverwrite
  (JNIEnv *, jclass, jlong, jboolean);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_getAlwaysOverwrite
 * Signature: (J)Z
 */
MADARA_Export jboolean JNICALL
Java_com_madara_EvalSettings_jni_1getAlwaysOverwrite
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_setTreatGlobalsAsLocals
 * Signature: (JZ)V
 */
MADARA_Export void JNICALL
Java_com_madara_EvalSettings_jni_1setTreatGlobalsAsLocals
  (JNIEnv *, jclass, jlong, jboolean);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_getTreatGlobalsAsLocals
 * Signature: (J)Z
 */
MADARA_Export jboolean JNICALL
Java_com_madara_EvalSettings_jni_1getTreatGlobalsAsLocals
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_setDefaultClockIncrement
 * Signature: (JJ)V
 */
MADARA_Export void JNICALL
Java_com_madara_EvalSettings_jni_1setClockIncrement
  (JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_getDefaultClockIncrement
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL
Java_com_madara_EvalSettings_jni_1getClockIncrement
  (JNIEnv *, jclass, jlong);
    
/*
 * Class:     com_madara_EvalSettings
 * Method:    jni_freeEvalSettings
 * Signature: (J)V
 */
MADARA_Export void JNICALL
Java_com_madara_EvalSettings_jni_1freeEvalSettings
  (JNIEnv *, jclass, jlong);

#ifdef __cplusplus
}
#endif
#endif
