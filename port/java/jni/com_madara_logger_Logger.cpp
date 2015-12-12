#include "com_madara_logger_Logger.h"
#include "madara/logger/Logger.h"

namespace logger = madara::logger;
typedef logger::Logger  Logger;

jlong JNICALL Java_com_madara_logger_Logger_jni_1Logger__
  (JNIEnv *, jobject)
{
  return (jlong) new Logger ();
}

void JNICALL Java_com_madara_logger_Logger_jni_1freeLogger
  (JNIEnv *, jclass, jlong cptr)
{
  delete (Logger *)cptr;
}


void JNICALL Java_com_madara_logger_Logger_jni_1setLevel
  (JNIEnv *, jobject, jlong cptr, jint level)
{
  Logger * current = (Logger *)cptr;

  if (current)
  {
    current->set_level ((int)level);
  }
}


jint JNICALL Java_com_madara_logger_Logger_jni_1getLevel
  (JNIEnv *, jobject, jlong cptr)
{
  jint result (0);
  Logger * current = (Logger *)cptr;

  if (current)
  {
    result = (jint)current->get_level ();
  }

  return result;
}


jstring JNICALL Java_com_madara_logger_Logger_jni_1getTag
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  Logger * current = (Logger *)cptr;

  if (current)
  {
    result = env->NewStringUTF (current->get_tag ().c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}


void JNICALL Java_com_madara_logger_Logger_jni_1setTag
  (JNIEnv * env, jobject, jlong cptr, jstring tag)
{
  Logger * current = (Logger *)cptr;

  if (current)
  {
    const char * str_tag = env->GetStringUTFChars (tag, 0);

    current->set_tag (str_tag);

    env->ReleaseStringUTFChars (tag, str_tag);
  }
}


void JNICALL Java_com_madara_logger_Logger_jni_1addTerm
  (JNIEnv *, jobject, jlong cptr)
{
  Logger * current = (Logger *)cptr;

  if (current)
  {
    current->add_term ();
  }
}


void JNICALL Java_com_madara_logger_Logger_jni_1addSyslog
  (JNIEnv *, jobject, jlong cptr)
{
  Logger * current = (Logger *)cptr;

  if (current)
  {
    current->add_syslog ();
  }
}


void JNICALL Java_com_madara_logger_Logger_jni_1clear
  (JNIEnv *, jobject, jlong cptr)
{
  Logger * current = (Logger *)cptr;

  if (current)
  {
    current->clear ();
  }
}


void JNICALL Java_com_madara_logger_Logger_jni_1addFile
  (JNIEnv * env, jobject, jlong cptr, jstring filename)
{
  Logger * current = (Logger *)cptr;

  if (current)
  {
    const char * str_filename = env->GetStringUTFChars (filename, 0);

    current->add_file (str_filename);

    env->ReleaseStringUTFChars (filename, str_filename);
  }
}

void JNICALL Java_com_madara_logger_Logger_jni_1log
(JNIEnv * env, jobject, jlong cptr, jint level, jstring message)
{
  Logger * current = (Logger *)cptr;

  if (current)
  {
    const char * str_message = env->GetStringUTFChars (message, 0);

    current->log ((jint)level, str_message);

    env->ReleaseStringUTFChars (message, str_message);
  }
}

void JNICALL Java_com_madara_logger_Logger_jni_1setTimestampFormat
(JNIEnv * env, jobject, jlong cptr, jstring format)
{
  Logger * current = (Logger *)cptr;

  if (current)
  {
    const char * str_format = env->GetStringUTFChars (format, 0);

    current->log (0, "madara::logger::(java) Setting timestamp to \"%s\"\n",
      str_format);

    current->set_timestamp_format (str_format);

    env->ReleaseStringUTFChars (format, str_format);
  }
}
