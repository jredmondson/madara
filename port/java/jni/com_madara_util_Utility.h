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
#include "madara/MADARA_export.h"
/* Header for class com_madara_util_Utility */

#ifndef _Included_com_madara_util_Utility
#define _Included_com_madara_util_Utility
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_madara_util_Utility
 * Method:    jni_getVersion
 * Signature: ()Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_util_Utility_jni_1getVersion
  (JNIEnv *, jclass);

/*
* Class:     com_madara_util_Utility
* Method:    jni_getTime
* Signature: ()J
*/
MADARA_Export jlong JNICALL Java_com_madara_util_Utility_jni_1getTime
  (JNIEnv *, jclass);

/*
* Class:     com_madara_util_Utility
* Method:    jni_sleep
* Signature: (D)V
*/
MADARA_Export void JNICALL Java_com_madara_util_Utility_jni_1sleep
(JNIEnv *, jclass, jdouble);

#ifdef __cplusplus
}
#endif
#endif
