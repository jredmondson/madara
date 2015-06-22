
#ifdef  _MADARA_JAVA_

#include <string.h>

#include "Java_Buffer_Filter.h"

#include "madara/utility/java/Acquire_VM.h"
#include "madara/utility/Utility.h"


Madara::Filters::Java_Buffer_Filter::Java_Buffer_Filter (
  Logger::Logger & logger, jobject obj)
  : logger_ (&logger)
{
  Madara::Utility::Java::Acquire_VM jvm;

  if (jvm.env)
  {
    logger_->log (Logger::LOG_MINOR,
      "Java_Buffer_Filter::constructor:" \
      " allocating global reference for object\n");

    obj_ = (jobject)jvm.env->NewGlobalRef (obj);

    if (obj_)
    {
      logger_->log (Logger::LOG_MAJOR,
        "Java_Buffer_Filter::constructor:" \
        " allocating global reference for object's class\n");

      class_ = (jclass)jvm.env->NewGlobalRef (jvm.env->GetObjectClass (obj_));
      if (class_)
      {
        logger_->log (Logger::LOG_MAJOR,
          "Java_Buffer_Filter::constructor:" \
          " class and object obtained successfully\n");
      }
      else
      {
        logger_->log (Logger::LOG_ERROR,
          "Java_Buffer_Filter::constructor:" \
          " ERROR: class object inaccessible\n");
      }
    }
    else
    {
      logger_->log (Logger::LOG_ERROR,
        "Java_Buffer_Filter::constructor:" \
        " ERROR: object is invalid\n");
    }
  }
  else
  {
    logger_->log (Logger::LOG_ERROR,
      "Java_Buffer_Filter::constructor:" \
      " ERROR: unable to acquire JAVA environment\n");
  }
}


Madara::Filters::Java_Buffer_Filter::~Java_Buffer_Filter ()
{
  Madara::Utility::Java::Acquire_VM jvm;
  if (jvm.env)
  {
    logger_->log (Logger::LOG_MAJOR,
      "Java_Buffer_Filter::destructor:" \
      " Deleting global references\n");

    jvm.env->DeleteGlobalRef (obj_);
    jvm.env->DeleteGlobalRef (class_);
  }
}

int
Madara::Filters::Java_Buffer_Filter::encode (
  unsigned char * source, int size, int max_size) const
{
  Madara::Utility::Java::Acquire_VM jvm;
  jint result (0);

  logger_->log (Logger::LOG_MAJOR,
    "Java_Buffer_Filter::encode:" \
    " Obtaining user-defined encode method\n");

  jmethodID call = jvm.env->GetMethodID (class_, "encode", "([BJJ)J");

  if (call)
  {
    logger_->log (Logger::LOG_MINOR,
      "Java_Buffer_Filter::encode:" \
      " Creating a Java byte array and copying values\n");

    // obtain a Java buffer
    jboolean isCopy;
    jbyteArray new_array = jvm.env->NewByteArray (max_size);
    void * data = jvm.env->GetPrimitiveArrayCritical ((jarray)new_array, &isCopy);

    // populate the Java buffer with the contents our source buffer
    memcpy (data, source, (size_t)size);

    logger_->log (Logger::LOG_MAJOR,
      "Java_Buffer_Filter::encode:" \
      " Calling user-defined encode method\n");
    result = jvm.env->CallIntMethod (obj_, call, new_array, size, max_size);

    logger_->log (Logger::LOG_MINOR,
      "Java_Buffer_Filter::encode:" \
      " Copying values from java byte array to the C buffer\n");

    // copy the Java memory to the caller buffer
    memcpy (source, data, (size_t)result);

    logger_->log (Logger::LOG_MINOR,
      "Java_Buffer_Filter::encode:" \
      " Deleting local Java variables\n");

    // clean up the Java code
    jvm.env->ReleaseByteArrayElements (new_array, (jbyte *)data, JNI_ABORT);
    jvm.env->DeleteLocalRef (new_array);
  }
  else
  {
    logger_->log (Logger::LOG_EMERGENCY,
      "Java_Buffer_Filter::encode:" \
      " ERROR: Unable to find user-defined encode method\n");
  }

  return result;
}

int
Madara::Filters::Java_Buffer_Filter::decode (
  unsigned char * source, int size, int max_size) const
{
  Madara::Utility::Java::Acquire_VM jvm;
  jint result (0);

  logger_->log (Logger::LOG_MAJOR,
    "Java_Buffer_Filter::decode:" \
    " Obtaining user-defined decode method\n");

  jmethodID call = jvm.env->GetMethodID (class_, "decode", "([BJJ)J");

  if (call)
  {
    logger_->log (Logger::LOG_MINOR,
      "Java_Buffer_Filter::decode:" \
      " Creating a Java byte array and copying values\n");

    // obtain a Java buffer
    jboolean isCopy;
    jbyteArray new_array = jvm.env->NewByteArray (max_size);
    void * data = jvm.env->GetPrimitiveArrayCritical ((jarray)new_array, &isCopy);

    logger_->log (Logger::LOG_MINOR,
      "Java_Buffer_Filter::decode:" \
      " Copying values from java byte array to the C buffer\n");

    // populate the Java buffer with the contents our source buffer
    memcpy (data, source, (size_t)size);

    logger_->log (Logger::LOG_MAJOR,
      "Java_Buffer_Filter::decode:" \
      " Calling user-defined decode method\n");
    result = jvm.env->CallIntMethod (obj_, call, new_array, size, max_size);

    // copy the Java memory to the caller buffer
    memcpy (source, data, (size_t)result);

    logger_->log (Logger::LOG_MINOR,
      "Java_Buffer_Filter::decode:" \
      " Deleting local Java variables\n");

    // clean up the Java code
    jvm.env->ReleaseByteArrayElements (new_array, (jbyte *)data, JNI_ABORT);
    jvm.env->DeleteLocalRef (new_array);
  }
  else
  {
    logger_->log (Logger::LOG_EMERGENCY,
      "Java_Buffer_Filter::decode:" \
      " ERROR: Unable to find user-defined decode method\n");
  }

  return result;
}

#endif // _MADARA_JAVA_
