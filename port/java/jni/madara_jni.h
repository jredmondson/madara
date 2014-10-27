
#ifndef _Included_com_madara_JNI
#define _Included_com_madara_JNI

#include <jni.h>
#include "madara/MADARA_export.h"

#ifdef _MADARA_ANDROID_
MADARA_Export jint JNI_OnLoad(JavaVM *vm, void *reserved);
MADARA_Export void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved);
#endif

MADARA_Export JNIEnv* madara_jni_get_env();

MADARA_Export JavaVM* madara_jni_jvm();

MADARA_Export jclass madara_jni_class();
MADARA_Export jclass jni_string_cls();

MADARA_Export JNIEnv* jni_attach();
MADARA_Export void jni_detach();

#endif