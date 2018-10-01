
#ifndef _Included_ai_madara_JNI_ANY
#define _Included_ai_madara_JNI_ANY

#include <jni.h>
#include "madara/MadaraExport.h"

#undef JNIEXPORT
#define JNIEXPORT MADARA_EXPORT

#include "madara/knowledge/AnyRef.h"
#include "madara/knowledge/Any.h"
#include "madara/knowledge/KnowledgeRecord.h"

namespace madara
{
namespace utility
{
namespace java
{
inline char* jstring_to_new_cstr(JNIEnv* env, jstring& s)
{
  size_t len = env->GetStringUTFLength(s);
  char* ret = new char[len + 1];
  env->GetStringUTFRegion(s, 0, len, ret);
  ret[len] = '\0';
  return ret;
}

struct array_deleter
{
  template<typename T>
  void operator()(T* ptr)
  {
    delete[] ptr;
  }
};

inline knowledge::AnyRef make_anyref(jlong handler, jlong data)
{
  return knowledge::AnyRef::from_pointers_unsafe(
      reinterpret_cast<const knowledge::TypeHandlers*>(handler),
      reinterpret_cast<void*>(data));
}

inline void anyref_out(JNIEnv* env, knowledge::AnyRef ref, jlongArray out)
{
  auto ptrs = ref.get_pointers();
  std::array<jlong, 2> arr{{reinterpret_cast<jlong>(ptrs.first),
      reinterpret_cast<jlong>(ptrs.second)}};
  env->SetLongArrayRegion(out, 0, 2, arr.data());
}

class JavaUTFString
{
public:
  JavaUTFString(JNIEnv* env, jstring str)
    : env_(env), str_(str), chars_(env->GetStringUTFChars(str_, nullptr))
  {
  }

  JavaUTFString(const JavaUTFString&) = delete;
  JavaUTFString(JavaUTFString&&) = delete;
  JavaUTFString& operator=(const JavaUTFString&) = delete;
  JavaUTFString& operator=(JavaUTFString&&) = delete;

  ~JavaUTFString()
  {
    env_->ReleaseStringUTFChars(str_, chars_);
  }

  const char* chars() const
  {
    return chars_;
  }

private:
  JNIEnv* env_;
  jstring str_;
  const char* chars_;
};

class JavaByteArray
{
public:
  JavaByteArray(JNIEnv* env, jbyteArray array)
    : env_(env),
      array_(array),
      data_(env->GetByteArrayElements(array_, nullptr))
  {
  }

  JavaByteArray(const JavaByteArray&) = delete;
  JavaByteArray(JavaByteArray&&) = delete;
  JavaByteArray& operator=(const JavaByteArray&) = delete;
  JavaByteArray& operator=(JavaByteArray&&) = delete;

  ~JavaByteArray()
  {
    env_->ReleaseByteArrayElements(array_, (jbyte*)data_, JNI_ABORT);
  }

  const jbyte* data() const
  {
    return data_;
  }

  size_t size() const
  {
    return env_->GetArrayLength(array_);
  }

private:
  JNIEnv* env_;
  jbyteArray array_;
  const jbyte* data_;
};

class JavaLongArray
{
public:
  JavaLongArray(JNIEnv* env, jlongArray array)
    : env_(env),
      array_(array),
      data_(env->GetLongArrayElements(array_, nullptr))
  {
  }

  JavaLongArray(const JavaLongArray&) = delete;
  JavaLongArray(JavaLongArray&&) = delete;
  JavaLongArray& operator=(const JavaLongArray&) = delete;
  JavaLongArray& operator=(JavaLongArray&&) = delete;

  ~JavaLongArray()
  {
    env_->ReleaseLongArrayElements(array_, (jlong*)data_, JNI_ABORT);
  }

  const jlong* data() const
  {
    return data_;
  }

  size_t size() const
  {
    return env_->GetArrayLength(array_);
  }

private:
  JNIEnv* env_;
  jlongArray array_;
  const jlong* data_;
};

class JavaDoubleArray
{
public:
  JavaDoubleArray(JNIEnv* env, jdoubleArray array)
    : env_(env),
      array_(array),
      data_(env->GetDoubleArrayElements(array_, nullptr))
  {
  }

  JavaDoubleArray(const JavaDoubleArray&) = delete;
  JavaDoubleArray(JavaDoubleArray&&) = delete;
  JavaDoubleArray& operator=(const JavaDoubleArray&) = delete;
  JavaDoubleArray& operator=(JavaDoubleArray&&) = delete;

  ~JavaDoubleArray()
  {
    env_->ReleaseDoubleArrayElements(array_, (double*)data_, JNI_ABORT);
  }

  const jdouble* data() const
  {
    return data_;
  }

  size_t size() const
  {
    return env_->GetArrayLength(array_);
  }

private:
  JNIEnv* env_;
  jdoubleArray array_;
  const jdouble* data_;
};

template<typename Func>
jstring catch_wrap(JNIEnv* env, Func&& func)
{
  try
  {
    func();
    return nullptr;
  }
  catch (const std::exception& e)
  {
    return env->NewStringUTF(e.what());
  }
  catch (...)
  {
    return env->NewStringUTF("Unknown C++ Exception");
  }
}
}
}
}

#endif
