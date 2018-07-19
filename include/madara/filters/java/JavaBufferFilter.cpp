
#ifdef  _MADARA_JAVA_

#include <string.h>

#include "JavaBufferFilter.h"

#include "madara/utility/java/Acquire_VM.h"
#include "madara/utility/Utility.h"


madara::filters::JavaBufferFilter::JavaBufferFilter (
  logger::Logger & logger, jobject obj)
  : logger_ (&logger)
{
  madara::utility::java::Acquire_VM jvm;

  if (jvm.env)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "JavaBufferFilter::constructor:" \
      " allocating global reference for object\n");

    obj_ = (jobject)jvm.env->NewGlobalRef (obj);

    if (obj_)
    {
      madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
        "JavaBufferFilter::constructor:" \
        " allocating global reference for object's class\n");

      class_ = (jclass)jvm.env->NewGlobalRef (jvm.env->GetObjectClass (obj_));
      if (class_)
      {
        madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
          "JavaBufferFilter::constructor:" \
          " class and object obtained successfully\n");
      }
      else
      {
        madara_logger_ptr_log (logger_, logger::LOG_ERROR,
          "JavaBufferFilter::constructor:" \
          " ERROR: class object inaccessible\n");
      }
    }
    else
    {
      madara_logger_ptr_log (logger_, logger::LOG_ERROR,
        "JavaBufferFilter::constructor:" \
        " ERROR: object is invalid\n");
    }
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "JavaBufferFilter::constructor:" \
      " ERROR: unable to acquire JAVA environment\n");
  }
}


madara::filters::JavaBufferFilter::~JavaBufferFilter ()
{
  madara::utility::java::Acquire_VM jvm;
  if (jvm.env)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
      "JavaBufferFilter::destructor:" \
      " Deleting global references\n");

    jvm.env->DeleteGlobalRef (obj_);
    jvm.env->DeleteGlobalRef (class_);
  }
}

int
madara::filters::JavaBufferFilter::encode (
  unsigned char * source, int size, int max_size) const
{
  madara::utility::java::Acquire_VM jvm;
  jint result (0);

  madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
    "JavaBufferFilter::encode:" \
    " Obtaining user-defined encode method\n");

  jmethodID call = jvm.env->GetMethodID (class_, "encode", "([BJJ)J");

  if (call)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "JavaBufferFilter::encode:" \
      " Creating a Java byte array and copying values\n");

    // obtain a Java buffer
    jboolean isCopy;
    jbyteArray new_array = jvm.env->NewByteArray (max_size);
    void * data = jvm.env->GetPrimitiveArrayCritical ((jarray)new_array, &isCopy);

    // populate the Java buffer with the contents our source buffer
    memcpy (data, source, (size_t)size);

    madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
      "JavaBufferFilter::encode:" \
      " Calling user-defined encode method\n");
    result = jvm.env->CallIntMethod (obj_, call, new_array, size, max_size);

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "JavaBufferFilter::encode:" \
      " Copying values from java byte array to the C buffer\n");

    // copy the Java memory to the caller buffer
    memcpy (source, data, (size_t)result);

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "JavaBufferFilter::encode:" \
      " Deleting local Java variables\n");

    // clean up the Java code
    jvm.env->ReleaseByteArrayElements (new_array, (jbyte *)data, JNI_ABORT);
    jvm.env->DeleteLocalRef (new_array);
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "JavaBufferFilter::encode:" \
      " ERROR: Unable to find user-defined encode method\n");
  }

  return result;
}

int
madara::filters::JavaBufferFilter::decode (
  unsigned char * source, int size, int max_size) const
{
  madara::utility::java::Acquire_VM jvm;
  jint result (0);

  madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
    "JavaBufferFilter::decode:" \
    " Obtaining user-defined decode method\n");

  jmethodID call = jvm.env->GetMethodID (class_, "decode", "([BJJ)J");

  if (call)
  {
    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "JavaBufferFilter::decode:" \
      " Creating a Java byte array and copying values\n");

    // obtain a Java buffer
    jboolean isCopy;
    jbyteArray new_array = jvm.env->NewByteArray (max_size);
    void * data = jvm.env->GetPrimitiveArrayCritical ((jarray)new_array, &isCopy);

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "JavaBufferFilter::decode:" \
      " Copying values from java byte array to the C buffer\n");

    // populate the Java buffer with the contents our source buffer
    memcpy (data, source, (size_t)size);

    madara_logger_ptr_log (logger_, logger::LOG_MAJOR,
      "JavaBufferFilter::decode:" \
      " Calling user-defined decode method\n");
    result = jvm.env->CallIntMethod (obj_, call, new_array, size, max_size);

    // copy the Java memory to the caller buffer
    memcpy (source, data, (size_t)result);

    madara_logger_ptr_log (logger_, logger::LOG_MINOR,
      "JavaBufferFilter::decode:" \
      " Deleting local Java variables\n");

    // clean up the Java code
    jvm.env->ReleaseByteArrayElements (new_array, (jbyte *)data, JNI_ABORT);
    jvm.env->DeleteLocalRef (new_array);
  }
  else
  {
    madara_logger_ptr_log (logger_, logger::LOG_EMERGENCY,
      "JavaBufferFilter::decode:" \
      " ERROR: Unable to find user-defined decode method\n");
  }

  return result;
}

std::string
madara::filters::JavaBufferFilter::get_id (void)
{
  return "javanull";
}

uint32_t
madara::filters::JavaBufferFilter::get_version (void)
{
  return madara::utility::get_uint_version ("1.0.0");
}

#endif // _MADARA_JAVA_
