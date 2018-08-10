#include "madara_jni_any.h"

#include "ai_madara_knowledge_Any.h"
#include "ai_madara_knowledge_AnyRef.h"
#include "ai_madara_knowledge_KnowledgeBase.h"
#include "ai_madara_knowledge_KnowledgeRecord.h"

#include "madara/knowledge/KnowledgeBase.h"

using namespace madara;
using namespace knowledge;
using namespace utility::java;

/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_get_tag
 * Signature: (JJ[Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1get_1tag
  (JNIEnv *env, jclass, jlong handler, jlong data, jobjectArray out)
{
  return catch_wrap(env, [&]() {
    AnyRef a{make_anyref(handler, data)};
    auto s = a.tag();
    if (s) {
      jstring ret = env->NewStringUTF(s);
      env->SetObjectArrayElement(out, 0, ret);
    } else {
      env->SetObjectArrayElement(out, 0, nullptr);
    }
  });
}

/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_field
 * Signature: (Ljava/lang/String;JJ[J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1field
  (JNIEnv *env, jclass, jstring name, jlong handler, jlong data, jlongArray out)
{
  return catch_wrap(env, [&]() {
    AnyRef a{make_anyref(handler, data)};
    JavaUTFString str(env, name);
    anyref_out(env, a.ref(str.chars()), out);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_index
 * Signature: (Ljava/lang/String;JJ[J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1index__Ljava_lang_String_2JJ_3J
  (JNIEnv *env, jclass, jstring key, jlong handler, jlong data, jlongArray out)
{
  return catch_wrap(env, [&]() {
    AnyRef a{make_anyref(handler, data)};
    JavaUTFString str(env, key);
    anyref_out(env, a.at(str.chars()), out);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_index
 * Signature: (JJJ[J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1index__JJJ_3J
  (JNIEnv *env, jclass, jlong index, jlong handler, jlong data, jlongArray out)
{
  return catch_wrap(env, [&]() {
    AnyRef a{make_anyref(handler, data)};
    anyref_out(env, a.at((size_t)index), out);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_assign
 * Signature: (JJJJ)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1assign__JJJJ
  (JNIEnv *env, jclass, jlong lhandler, jlong ldata,
                        jlong rhandler, jlong rdata)
{
  return catch_wrap(env, [&]() {
    AnyRef l{make_anyref(lhandler, ldata)};
    AnyRef r{make_anyref(rhandler, rdata)};

    l.assign(r);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_assign_record
 * Signature: (JJJ)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1assign_1record
  (JNIEnv *env, jclass, jlong handler, jlong data, jlong val)
{
  return catch_wrap(env, [&]() {
    AnyRef l{make_anyref(handler, data)};
    const KnowledgeRecord *record =
      reinterpret_cast<const KnowledgeRecord *>(val);

    l.assign(*record);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_assign
 * Signature: (JJJ)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1assign__JJJ
  (JNIEnv *env, jclass, jlong handler, jlong data, jlong val)
{
  return catch_wrap(env, [&]() {
    AnyRef l{make_anyref(handler, data)};

    l.assign((int64_t)val);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_assign
 * Signature: (JJD)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1assign__JJD
  (JNIEnv *env, jclass, jlong handler, jlong data, jdouble val)
{
  return catch_wrap(env, [&]() {
    AnyRef l{make_anyref(handler, data)};

    l.assign((double)val);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_assign
 * Signature: (JJLjava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1assign__JJLjava_lang_String_2
  (JNIEnv *env, jclass, jlong handler, jlong data, jstring val)
{
  return catch_wrap(env, [&]() {
    AnyRef l{make_anyref(handler, data)};
    JavaUTFString str(env, val);

    l.assign(str.chars());
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_assign
 * Signature: (JJ[J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1assign__JJ_3J
  (JNIEnv *env, jclass, jlong handler, jlong data, jlongArray val)
{
  return catch_wrap(env, [&]() {
    AnyRef l{make_anyref(handler, data)};
    JavaLongArray array(env, val);
    std::vector<int64_t> vec(array.data(), array.data() + array.size());

    l.assign(std::move(vec));
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_assign
 * Signature: (JJ[D)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1assign__JJ_3D
  (JNIEnv *env, jclass, jlong handler, jlong data, jdoubleArray val)
{
  return catch_wrap(env, [&]() {
    AnyRef l{make_anyref(handler, data)};
    JavaDoubleArray array(env, val);
    std::vector<double> vec(array.data(), array.data() + array.size());

    l.assign(std::move(vec));
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_to_record
 * Signature: (JJ[J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1to_1record
  (JNIEnv *env, jclass, jlong handler, jlong data, jlongArray out)
{
  return catch_wrap(env, [&]() {
    AnyRef a{make_anyref(handler, data)};
    jlong ret = reinterpret_cast<jlong>(new KnowledgeRecord(a.to_record()));
    env->SetLongArrayRegion(out, 0, 1, &ret);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_to
 * Signature: (JJ[J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1to__JJ_3J
  (JNIEnv *env, jclass, jlong handler, jlong data, jlongArray out)
{
  return catch_wrap(env, [&]() {
    AnyRef a{make_anyref(handler, data)};
    jlong ret = a.to_integer();
    env->SetLongArrayRegion(out, 0, 1, &ret);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_to
 * Signature: (JJ[D)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1to__JJ_3D
  (JNIEnv *env, jclass, jlong handler, jlong data, jdoubleArray out)
{
  return catch_wrap(env, [&]() {
    AnyRef a{make_anyref(handler, data)};
    jdouble ret = a.to_double();
    env->SetDoubleArrayRegion(out, 0, 1, &ret);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_to
 * Signature: (JJ[Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1to__JJ_3Ljava_lang_String_2
  (JNIEnv *env, jclass, jlong handler, jlong data, jobjectArray out)
{
  return catch_wrap(env, [&]() {
    AnyRef a{make_anyref(handler, data)};
    std::string s = a.to_string();
    jstring ret = env->NewStringUTF(s.c_str());
    env->SetObjectArrayElement(out, 0, ret);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_to
 * Signature: (JJ[[J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1to__JJ_3_3J
  (JNIEnv *env, jclass, jlong handler, jlong data, jobjectArray out)
{
  return catch_wrap(env, [&]() {
    AnyRef a{make_anyref(handler, data)};
    std::vector<int64_t> vals = a.to_integers();
    jlongArray ret = env->NewLongArray(vals.size());
    env->SetLongArrayRegion(ret, 0, vals.size(), (jlong*)vals.data());
    env->SetObjectArrayElement(out, 0, ret);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_to
 * Signature: (JJ[[D)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1to__JJ_3_3D
  (JNIEnv *env, jclass, jlong handler, jlong data, jobjectArray out)
{
  return catch_wrap(env, [&]() {
    AnyRef a{make_anyref(handler, data)};
    std::vector<double> vals = a.to_doubles();
    jdoubleArray ret = env->NewDoubleArray(vals.size());
    env->SetDoubleArrayRegion(ret, 0, vals.size(), vals.data());
    env->SetObjectArrayElement(out, 0, ret);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_size
 * Signature: (JJ[J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1size
  (JNIEnv *env, jclass, jlong handler, jlong data, jlongArray out)
{
  return catch_wrap(env, [&]() {
    AnyRef a{make_anyref(handler, data)};
    jlong ret = a.size();
    env->SetLongArrayRegion(out, 0, 1, &ret);
  });
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_list_fields
 * Signature: (JJ[[Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1list_1fields
  (JNIEnv *env, jclass, jlong handler, jlong data, jobjectArray)
{
  AnyRef a{make_anyref(handler, data)};
  (void)a;
  (void)env;
  return nullptr;
}


#define MADARA_IMPL_REGISTER(...) \
  static std::unique_ptr<const char, array_deleter> t{ \
    jstring_to_new_cstr(env, tag)}; \
  AnyRegistry::register_type<__VA_ARGS__>(t.get())

/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerInt
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerInt
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(int32_t);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerLong
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerLong
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(int64_t);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerShort
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerShort
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(int16_t);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerChar
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerChar
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(char);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerUInt
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerUInt
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(uint32_t);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerULong
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerULong
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(uint64_t);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerUShort
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerUShort
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(uint16_t);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerUChar
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerUChar
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(unsigned char);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerSChar
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerSChar
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(signed char);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerDouble
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerDouble
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(double);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerFloat
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerFloat
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(float);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerString
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerString
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::string);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerIntVector
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerIntVector
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::vector<int32_t>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerLongVector
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerLongVector
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::vector<int64_t>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerShortVector
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerShortVector
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::vector<int16_t>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerCharVector
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerCharVector
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::vector<char>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerUIntVector
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerUIntVector
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::vector<uint32_t>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerULongVector
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerULongVector
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::vector<uint64_t>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerUShortVector
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerUShortVector
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::vector<uint16_t>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerUCharVector
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerUCharVector
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::vector<unsigned char>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerSCharVector
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerSCharVector
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::vector<signed char>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerDoubleVector
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerDoubleVector
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::vector<double>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerFloatVector
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerFloatVector
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::vector<float>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerStringVector
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerStringVector
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::vector<std::string>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerStringToStringMap
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerStringToStringMap
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::map<std::string, std::string>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerStringToLongMap
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerStringToLongMap
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::map<std::string, int64_t>);
}


/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    registerStringToDoubleMap
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_ai_madara_knowledge_AnyRef_registerStringToDoubleMap
  (JNIEnv *env, jclass, jstring tag)
{
  MADARA_IMPL_REGISTER(std::map<std::string, double>);
}

namespace {
  // Use union to inhibit destruction of the Any's stored value
  union NoDestruct {
    Any a;

    NoDestruct(const char *tag) :
      a(Any::construct(tag)) {}
    NoDestruct(Any &&any) : a(std::move(any)) {}
    ~NoDestruct() {}
  };
}

/*
 * Class:     ai_madara_knowledge_Any
 * Method:    jni_construct
 * Signature: (Ljava/lang/String;[J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_Any_jni_1construct
  (JNIEnv *env, jclass, jstring tag, jlongArray out)
{
  return catch_wrap(env, [&]() {
    JavaUTFString str(env, tag);

    NoDestruct u(str.chars());

    anyref_out(env, u.a.ref(), out);
  });
}

/*
 * Class:     ai_madara_knowledge_Any
 * Method:    jni_free
 * Signature: (JJ)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_Any_jni_1free
  (JNIEnv *env, jclass, jlong handler, jlong data)
{
  return catch_wrap(env, [&]() {
    const TypeHandlers *h = reinterpret_cast<const TypeHandlers *>(handler);
    void *d = reinterpret_cast<void *>(data);

    if (h && d) {
      h->destruct(d);
    }
  });
}

/*
 * Class:     ai_madara_knowledge_KnowledgeRecord
 * Method:    jni_KnowledgeRecord
 * Signature: (JJ)J
 */
MADARA_EXPORT jlong JNICALL
Java_ai_madara_knowledge_KnowledgeRecord_jni_1KnowledgeRecord__JJ
  (JNIEnv *, jclass, jlong handler, jlong data)
{
  AnyRef a{make_anyref(handler, data)};
  return (jlong) (new KnowledgeRecord (a.clone()));
}

/*
 * Class:     ai_madara_knowledge_KnowledgeRecord
 * Method:    jni_toAny
 * Signature: (J[J)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_knowledge_KnowledgeRecord_jni_1toAny
  (JNIEnv *env, jclass, jlong cptr, jlongArray out)
{
  KnowledgeRecord * record = (KnowledgeRecord *) cptr;
  if (!record) {
    madara::utility::java::throw_dead_obj_exception(env,
      "KnowledgeRecord::toAny: "
      "KnowledgeRecord object is released already");
    return nullptr;
  }
  return catch_wrap(env, [&]() {
    NoDestruct u(record->to_any());
    anyref_out(env, u.a.ref(), out);
  });
}

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setAny
 * Signature: (JLjava/lang/String;JJ)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setAny
  (JNIEnv *env, jclass, jlong cptr, jstring name, jlong handler, jlong data)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  if (!knowledge)
  {
    // user has tried to use a deleted object. Clean up and throw
    madara::utility::java::throw_dead_obj_exception(env,
      "KnowledgeBase::setAny: KB object is released already");
    return nullptr;
  }
  return catch_wrap(env, [&]() {
    JavaUTFString str(env, name);
    AnyRef a{make_anyref(handler, data)};
    knowledge->emplace_any(str.chars(), a);
  });
}

/*
 * Class:     ai_madara_knowledge_KnowledgeBase
 * Method:    jni_setAnySettings
 * Signature: (JLjava/lang/String;JJJ)Ljava/lang/String;
 */
MADARA_EXPORT jstring JNICALL
Java_ai_madara_knowledge_KnowledgeBase_jni_1setAnySettings
  (JNIEnv *env, jclass, jlong cptr, jstring name,
   jlong handler, jlong data, jlong settings_ptr)
{
  KnowledgeBase * knowledge = (KnowledgeBase *) cptr;
  knowledge::EvalSettings * settings = (knowledge::EvalSettings *)settings_ptr;
  if (!knowledge)
  {
    // user has tried to use a deleted object. Clean up and throw
    madara::utility::java::throw_dead_obj_exception(env,
      "KnowledgeBase::setAny: KB object is released already");
    return nullptr;
  }
  if (!settings)
  {
    madara::utility::java::throw_dead_obj_exception(env,
      "KnowledgeBase::setAny: settings object is released already");
    return nullptr;
  }
  return catch_wrap(env, [&]() {
    JavaUTFString str(env, name);
    AnyRef a{make_anyref(handler, data)};
    knowledge->emplace_any(str.chars(), *settings, a);
  });
}

/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_reader
 * Signature: (JJ[[B)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1reader
  (JNIEnv *env, jclass, jlong handler, jlong data, jobjectArray out)
{
  return catch_wrap(env, [&]() {
    AnyRef a{make_anyref(handler, data)};
    auto buf = a.get_capnp_buffer().asChars();

    jbyteArray ret = env->NewByteArray(buf.size());
    env->SetByteArrayRegion(ret, 0,
        buf.size(), (jbyte*)buf.begin());
    env->SetObjectArrayElement(out, 0, ret);
  });
}

/*
 * Class:     ai_madara_knowledge_Any
 * Method:    jni_emplace_capnp
 * Signature: (Ljava/lang/String;[B[J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_Any_jni_1emplace_1capnp
  (JNIEnv *env, jclass, jstring tag, jbyteArray buf, jlongArray out)
{
  return catch_wrap(env, [&]() {
    JavaUTFString tagstr(env, tag);
    JavaByteArray arr(env, buf);

    GenericCapnObject capn(tagstr.chars(), (char *)arr.data(), arr.size());
    Any a(capn);

    NoDestruct u(std::move(a));

    anyref_out(env, u.a.ref(), out);
  });
}

/*
 * Class:     ai_madara_knowledge_AnyRef
 * Method:    jni_register_tag
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_ai_madara_knowledge_AnyRef_jni_1register_1tag
  (JNIEnv *env, jclass, jstring tag)
{
  return catch_wrap(env, [&]() {
    JavaUTFString tagstr(env, tag);

    std::unique_ptr<std::string> new_tag(new std::string(tagstr.chars()));
    if (AnyRegistry::register_type<GenericCapnObject>(new_tag->c_str())) {
      // OK to leak this; will be needed for rest of process execution
      new_tag.release();
    }
  });
}
