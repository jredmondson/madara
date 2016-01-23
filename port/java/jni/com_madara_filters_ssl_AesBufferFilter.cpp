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
#include "com_madara_filters_ssl_AesBufferFilter.h"

#include "madara/filters/ssl/AESBufferFilter.h"

#ifdef  _USE_SSL_
namespace filters = madara::filters;
typedef  filters::AESBufferFilter   AESBufferFilter;
#endif

  /*
 * Class:     com_madara_filters_ssl_AesBufferFilter
 * Method:    jni_AesBufferFilter
 * Signature: ()J
 */
jlong JNICALL
Java_com_madara_filters_ssl_AesBufferFilter_jni_1AesBufferFilter__
(JNIEnv *, jobject)
{
  jlong result (0);

#ifdef _USE_SSL_
  result = (jlong) new AESBufferFilter ();
#endif

  return result;
}

/*
 * Class:     com_madara_filters_ssl_AesBufferFilter
 * Method:    jni_AesBufferFilter
 * Signature: (J)J
 */
jlong JNICALL
Java_com_madara_filters_ssl_AesBufferFilter_jni_1AesBufferFilter__J
(JNIEnv *, jobject, jlong cptr)
{
  jlong result (0);

#ifdef _USE_SSL_
  AESBufferFilter * input = (AESBufferFilter *)cptr;
  if (input != 0)
  {
    result = (jlong) new AESBufferFilter (*input);
  }
#endif

  return result;
}

/*
 * Class:     com_madara_filters_ssl_AesBufferFilter
 * Method:    jni_freeAesBufferFilter
 * Signature: (J)V
 */
void JNICALL
Java_com_madara_filters_ssl_AesBufferFilter_jni_1freeAesBufferFilter
(JNIEnv *, jclass, jlong cptr)
{
#ifdef _USE_SSL_
  AESBufferFilter * current = (AESBufferFilter *)cptr;
  delete current;
#endif
}

/*
 * Class:     com_madara_filters_ssl_AesBufferFilter
 * Method:    jni_encode
 * Signature: (J[BJJ)J
 */
jlong JNICALL
Java_com_madara_filters_ssl_AesBufferFilter_jni_1encode
(JNIEnv * env, jobject, jlong cptr, jbyteArray buffer, jlong size, jlong max_size)
{
  jlong result (size);

#ifdef _USE_SSL_
  AESBufferFilter * current = (AESBufferFilter *)cptr;

  if (current)
  {
    unsigned char * elements = (unsigned char *)env->GetByteArrayElements (
      buffer, 0);

    result = current->encode (elements, size, max_size);

    env->ReleaseByteArrayElements (buffer, (jbyte *)elements, 0);
  }

#endif

  return result;
}

/*
 * Class:     com_madara_filters_ssl_AesBufferFilter
 * Method:    jni_decode
 * Signature: (J[BJJ)J
 */
jlong JNICALL
Java_com_madara_filters_ssl_AesBufferFilter_jni_1decode
(JNIEnv * env, jobject, jlong cptr, jbyteArray buffer, jlong size, jlong max_size)
{
  jlong result (size);

#ifdef _USE_SSL_
  AESBufferFilter * current = (AESBufferFilter *)cptr;

  if (current)
  {
    unsigned char * elements = (unsigned char *)env->GetByteArrayElements (
      buffer, 0);

    result = current->decode (elements, size, max_size);

    env->ReleaseByteArrayElements (buffer, (jbyte *)elements, 0);
  }

#endif

  return result;
}

/*
 * Class:     com_madara_filters_ssl_AesBufferFilter
 * Method:    jni_generateKey
 * Signature: (JLjava/lang/String;)I
 */
jint JNICALL
Java_com_madara_filters_ssl_AesBufferFilter_jni_1generateKey
(JNIEnv * env, jobject, jlong cptr, jstring password)
{
  jint result (-1);

#ifdef _USE_SSL_
  AESBufferFilter * current = (AESBufferFilter *)cptr;

  if (current && password)
  {
    const char * nativePassword = env->GetStringUTFChars (password, 0);

    result = current->generate_key (nativePassword);

    env->ReleaseStringUTFChars (password, nativePassword);
  }
#endif

  return result;
}

