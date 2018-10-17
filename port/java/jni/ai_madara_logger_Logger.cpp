#include "ai_madara_logger_Logger.h"
#include "madara/logger/Logger.h"
#include "madara_jni.h"

namespace logger = madara::logger;
typedef logger::Logger Logger;

jlong JNICALL Java_ai_madara_logger_Logger_jni_1Logger__(JNIEnv*, jobject)
{
  return (jlong) new Logger();
}

void JNICALL Java_ai_madara_logger_Logger_jni_1freeLogger(
    JNIEnv*, jclass, jlong cptr)
{
  delete (Logger*)cptr;
}

void JNICALL Java_ai_madara_logger_Logger_jni_1setLevel(
    JNIEnv* env, jobject, jlong cptr, jint level)
{
  Logger* current = (Logger*)cptr;

  if(current)
  {
    current->set_level((int)level);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Logger::setLevel: "
        "Logger object is released already");
  }
}

jint JNICALL Java_ai_madara_logger_Logger_jni_1getLevel(
    JNIEnv* env, jobject, jlong cptr)
{
  jint result(0);
  Logger* current = (Logger*)cptr;

  if(current)
  {
    result = (jint)current->get_level();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Logger::getLevel: "
        "Logger object is released already");
  }

  return result;
}

jstring JNICALL Java_ai_madara_logger_Logger_jni_1getTag(
    JNIEnv* env, jobject, jlong cptr)
{
  jstring result = 0;
  Logger* current = (Logger*)cptr;

  if(current)
  {
    result = env->NewStringUTF(current->get_tag().c_str());
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Logger::getTag: "
        "Logger object is released already");
  }

  return result;
}

void JNICALL Java_ai_madara_logger_Logger_jni_1setTag(
    JNIEnv* env, jobject, jlong cptr, jstring tag)
{
  Logger* current = (Logger*)cptr;

  if(current)
  {
    const char* str_tag = env->GetStringUTFChars(tag, 0);

    current->set_tag(str_tag);

    env->ReleaseStringUTFChars(tag, str_tag);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Logger::setTag: "
        "Logger object is released already");
  }
}

void JNICALL Java_ai_madara_logger_Logger_jni_1addTerm(
    JNIEnv* env, jobject, jlong cptr)
{
  Logger* current = (Logger*)cptr;

  if(current)
  {
    current->add_term();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Logger::addTerm: "
        "Logger object is released already");
  }
}

void JNICALL Java_ai_madara_logger_Logger_jni_1addSyslog(
    JNIEnv* env, jobject, jlong cptr)
{
  Logger* current = (Logger*)cptr;

  if(current)
  {
    current->add_syslog();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Logger::addSysLog: "
        "Logger object is released already");
  }
}

void JNICALL Java_ai_madara_logger_Logger_jni_1clear(
    JNIEnv* env, jobject, jlong cptr)
{
  Logger* current = (Logger*)cptr;

  if(current)
  {
    current->clear();
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Logger::clear: "
        "Logger object is released already");
  }
}

void JNICALL Java_ai_madara_logger_Logger_jni_1addFile(
    JNIEnv* env, jobject, jlong cptr, jstring filename)
{
  Logger* current = (Logger*)cptr;

  if(current)
  {
    const char* str_filename = env->GetStringUTFChars(filename, 0);

    current->add_file(str_filename);

    env->ReleaseStringUTFChars(filename, str_filename);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Logger::addFile: "
        "Logger object is released already");
  }
}

void JNICALL Java_ai_madara_logger_Logger_jni_1log(
    JNIEnv* env, jobject, jlong cptr, jint level, jstring message)
{
  Logger* current = (Logger*)cptr;

  if(current)
  {
    const char* str_message = env->GetStringUTFChars(message, 0);

    current->log((jint)level, str_message);

    env->ReleaseStringUTFChars(message, str_message);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Logger::log: "
        "Logger object is released already");
  }
}

void JNICALL Java_ai_madara_logger_Logger_jni_1setTimestampFormat(
    JNIEnv* env, jobject, jlong cptr, jstring format)
{
  Logger* current = (Logger*)cptr;

  if(current && format)
  {
    const char* str_format = env->GetStringUTFChars(format, 0);

    madara_logger_ptr_log(current, logger::LOG_MAJOR,
        "madara::logger::(java)::setTimestampFormat Setting timestamp to "
        "\"%s\"\n",
        str_format);

    current->set_timestamp_format(str_format);

    env->ReleaseStringUTFChars(format, str_format);
  }
  else
  {
    // user has tried to use a deleted object. Clean up and throw

    madara::utility::java::throw_dead_obj_exception(env,
        "Logger::setTimestampFormat: "
        "Logger or timestamp format objects are released already");
  }
}
