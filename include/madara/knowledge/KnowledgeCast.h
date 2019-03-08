/**
 * Copyright (c) 2015 Carnegie Mellon University. All Rights Reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:

 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following acknowledgments
 * and disclaimers.

 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.

 * 3. The names "Carnegie Mellon University," "SEI" and/or "Software
 * Engineering Institute" shall not be used to endorse or promote
 * products derived from this software without prior written
 * permission. For written permission, please contact
 * permission@sei.cmu.edu.

 * 4. Products derived from this software may not be called "SEI" nor
 * may "SEI" appear in their names without prior written permission of
 * permission@sei.cmu.edu.

 * 5. Redistributions of any form whatsoever must retain the following
 * acknowledgment:

 * This material is based upon work funded and supported by the
 * Department of Defense under Contract No. FA8721-05-C-0003 with
 * Carnegie Mellon University for the operation of the Software
 * Engineering Institute, a federally funded research and development
 * center.

 * Any opinions, findings and conclusions or recommendations expressed
 * in this material are those of the author(s) and do not necessarily
 * reflect the views of the United States Department of Defense.

 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE
 * ENGINEERING INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS"
 * BASIS. CARNEGIE MELLON UNIVERSITY MAKES NO WARRANTIES OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT
 * LIMITED TO, WARRANTY OF FITNESS FOR PURPOSE OR MERCHANTABILITY,
 * EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE MATERIAL. CARNEGIE
 * MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND WITH
 * RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT
 * INFRINGEMENT.

 * This material has been approved for public release and unlimited
 * distribution.

 * DM-0002494
**/

#ifndef _MADARA_KNOWLEDGE_KNOWLEDGE_CAST_H_
#define _MADARA_KNOWLEDGE_KNOWLEDGE_CAST_H_

#include <string>
#include <cstring>
#include <type_traits>
#include <stdbool.h>
#include <madara/knowledge/KnowledgeRecord.h>
#include <madara/utility/SupportTest.h>

namespace madara
{
namespace knowledge
{
#ifdef DOXYGEN

/**
 * Convert a KnowledgeRecord into a specified type.
 *
 * Supports std::string, bool, all arithmetic primitives, and std::vector of
 * arithmetic primitives. This is implemented using the appropriate "to_..."
 * methods provided by KnowledgeRecord, so refer to those for details on
 * type coercion.
 *
 * Internally this overload calls the version which takes type<Out> as first
 * parameter, so to support custom types, overload that form.
 *
 * @tparam Out the type to try to convert to (must be specified explicitly)
 * @param in the KnowledgeRecord to read from
 * @return a value of type Out
 **/
template<class Out>
inline Out knowledge_cast(const KnowledgeRecord& in);

/**
 * Convert a KnowledgeRecord into a specified type, using the "type" helper
 * struct to specify type.
 *
 * Supports std::string, bool, all arithmetic primitives, and std::vector of
 * arithmetic primitives. This is implemented using the appropriate "to_..."
 * methods provided by KnowledgeRecord, so refer to those for details on
 * type coercion.
 *
 * To support custom types, overload this form with other Out types. These
 * overloads should exist in the same namespace as the types themselves so
 * they can be found with ADL.
 *
 * @param t an instance of the type helper struct to infer target type
 * @param in the KnowledgeRecord to read from
 * @return a value of type Out
 * @tparam Out the type to try to convert to (inferred from t)
 **/
template<class Out>
inline Out knowledge_cast(type<T> t, const KnowledgeRecord& in);

template<class Out>
/**
 * Convert a KnowledgeRecord into a specified type, using an output parameter.
 *
 * Supports std::string, bool, all arithmetic primitives, and STL containers of
 * arithmetic primitives. This is implemented using the appropriate "to_..."
 * methods provided by KnowledgeRecord, so refer to those for details on
 * type coercion.
 *
 * For most types, this calls the form which takes a type struct, and returns
 * by value, putting that value into out. For STL containers, it modifies those
 * containers inplace. For resizable containers, such as std::vector, the
 * container will be resized to fit the value. For unresizable containers,
 * such as std::array and native C arrays, excess values will be discarded,
 * and any extra values in the target will be zeroed.
 *
 * @param in the KnowledgeRecord to read from
 * @param out output parameter to put converted value
 * @return a value of type Out
 * @tparam Out the type to try to convert to (inferred from out)
 **/
inline Out& knowledge_cast(const KnowledgeRecord& in, Out& out);

/**
 * Convert a given value into a KnowledgeRecord
 *
 * Supports std::string, bool, all arithmetic primitives, and std::vector of
 * arithmetic primitives.
 *
 * @param in the value to convert
 * @return a new KnowledgeRecord initialized from in
 * @tparam In the type to convert from (inferred from in)
 **/
template<class In>
inline KnowledgeRecord knowledge_cast(const In& in);

#else

/// By default, call constructor of target class;
/// for other semantics, define specializations
template<class O>
inline auto knowledge_cast(type<O>, const KnowledgeRecord& in) ->
    typename std::enable_if<
        std::is_constructible<O, const KnowledgeRecord&>::value, O>::type
{
  return O{in};
}

/// Convert KnowledgeRecord to floating point
template<class O>
inline auto knowledge_cast(type<O>, const KnowledgeRecord& in)
    -> enable_if_<is_floating_point<O>(), O>
{
  return static_cast<O>(in.to_double());
}

/// Convert KnowledgeRecord to integer
template<class O>
inline auto knowledge_cast(type<O>, const KnowledgeRecord& in)
    -> enable_if_<is_integral<O>() || is_enum<O>(), O>
{
  return static_cast<O>(in.to_integer());
}

/// Convert KnowledgeRecord to bool (using in.is_true())
inline bool knowledge_cast(type<bool>, const KnowledgeRecord& in)
{
  return in.is_true();
}

/// Convert KnowledgeRecord to a std::string
inline std::string knowledge_cast(type<std::string>, const KnowledgeRecord& in)
{
  return in.to_string();
}

/// Convert KnowledgeRecord to a std::vector<int64_t>
inline std::vector<int64_t> knowledge_cast(
    type<std::vector<int64_t>>, const KnowledgeRecord& in)
{
  return in.to_integers();
}

/// Convert KnowledgeRecord to a std::vector of integral types besides int64_t
template<typename T>
inline auto knowledge_cast(type<std::vector<T>>, const KnowledgeRecord& in) ->
    typename std::enable_if<std::is_integral<T>::value, std::vector<T>>::type
{
  auto vec = in.to_integers();
  return {std::begin(vec), std::end(vec)};
}

/// Convert KnowledgeRecord to a std::vector<double>
inline std::vector<double> knowledge_cast(
    type<std::vector<double>>, const KnowledgeRecord& in)
{
  return in.to_doubles();
}

/// Convert KnowledgeRecord to a std::vector of float type other than double
template<typename T>
inline auto knowledge_cast(type<std::vector<T>>, const KnowledgeRecord& in) ->
    typename std::enable_if<std::is_floating_point<T>::value,
        std::vector<T>>::type
{
  auto vec = in.to_doubles();
  return {std::begin(vec), std::end(vec)};
}

/// Internal implementation details. Users of MADARA should not directly use
/// the contents of this namespace.
namespace impl
{
template<typename T>
inline auto get_size(const T& c) -> decltype(c.size())
{
  return c.size();
}

template<typename T, size_t N>
inline size_t get_size(const T (&arr)[N])
{
  (void)arr;
  return N;
}

MADARA_MAKE_VAL_SUPPORT_TEST(resize, x, x.resize(size_t{}));

template<typename T>
inline auto resize_or_clear(T& c, size_t n) ->
    typename std::enable_if<!supports_resize<T&>::value,
        decltype(c[0], get_size(c), size_t{})>::type
{
  using elem_type = typename std::decay<decltype(c[0])>::type;
  size_t curn = get_size(c);
  for (; n < curn; ++n)
  {
    c[n] = elem_type{};
  }
  return curn;
}

template<typename T>
inline auto resize_or_clear(T& c, size_t n) ->
    typename std::enable_if<supports_resize<T&>::value, size_t>::type
{
  c.resize(n);
  return n;
}

template<typename T>
inline auto share_array(const KnowledgeRecord& in) ->
    typename std::enable_if<std::is_integral<typename std::decay<decltype(
                                *std::begin(std::declval<T&>()))>::type>::value,
        decltype(in.share_integers())>::type
{
  return in.share_integers();
}

template<typename T>
inline auto to_array(const KnowledgeRecord& in) ->
    typename std::enable_if<std::is_integral<typename std::decay<decltype(
                                *std::begin(std::declval<T&>()))>::type>::value,
        decltype(in.to_integers())>::type
{
  return in.to_integers();
}

template<typename T>
inline auto share_array(const KnowledgeRecord& in) ->
    typename std::enable_if<std::is_floating_point<typename std::decay<decltype(
                                *std::begin(std::declval<T&>()))>::type>::value,
        decltype(in.share_doubles())>::type
{
  return in.share_doubles();
}

template<typename T>
inline auto to_array(const KnowledgeRecord& in) ->
    typename std::enable_if<std::is_floating_point<typename std::decay<decltype(
                                *std::begin(std::declval<T&>()))>::type>::value,
        decltype(in.to_doubles())>::type
{
  return in.to_doubles();
}

template<typename T>
struct simple_span
{
  T* ptr;
  size_t len;

  T* begin() const
  {
    return ptr;
  }
  const T* cbegin() const
  {
    return ptr;
  }
  T* end() const
  {
    return ptr + len;
  }
  const T* cend() const
  {
    return ptr + len;
  }

  T& operator[](size_t i) const
  {
    return ptr[i];
  }

  size_t size() const
  {
    return len;
  }
};

template<typename T>
simple_span<T> make_span(T* ptr, size_t size)
{
  return {ptr, size};
}

MADARA_MAKE_VAL_SUPPORT_TEST(target_container, x,
    (enable_if_<std::is_arithmetic<
            typename std::decay<decltype(*std::begin(x))>::type>::value>(),
        impl::get_size(x)));
}

/// Convert KnowledgeRecord via an output parameter. This overload calls the
/// underlying version that returns by value, and sets to out
template<typename T>
inline auto knowledge_cast(const KnowledgeRecord& in, T& out) ->
    typename std::enable_if<!impl::supports_target_container<T&>::value,
        decltype(out = knowledge_cast(type<T>{}, in))>::type
{
  return (out = knowledge_cast(type<T>{}, in));
}

namespace impl
{
template<typename T>
struct is_basic_string : std::false_type
{
};

template<typename CharT, typename Traits, typename Allocator>
struct is_basic_string<std::basic_string<CharT, Traits, Allocator>>
  : std::true_type
{
};
}

/// Conert KnowlegeRecord into an existing std::basic_string<...>
template<typename CharT, typename Traits, typename Allocator>
inline std::basic_string<CharT, Traits, Allocator>& knowledge_cast(
    const KnowledgeRecord& in, std::basic_string<CharT, Traits, Allocator>& out)
{
  static_assert(std::is_same<std::string,
                    std::basic_string<CharT, Traits, Allocator>>::value,
      "knowledge_cast only supports std::string, "
      "not other std::basic_string types");
  return (out = knowledge_cast(type<std::string>{}, in));
}

/// Conert KnowlegeRecord into an existing container (such as std::vector,
/// std::array, and native C arrays)
template<typename T>
inline auto knowledge_cast(const KnowledgeRecord& in, T& out) ->
    typename std::enable_if<impl::supports_target_container<T&>::value &&
                                !impl::is_basic_string<T>::value,
        decltype(out)>::type
{
  auto ints = impl::share_array<T>(in);
  if (ints)
  {
    size_t count = ints->size();
    count = std::min(impl::resize_or_clear(out, count), count);
    std::copy_n(std::begin(*ints), count, std::begin(out));
  }
  else
  {
    auto ints_arr = impl::to_array<T>(in);
    size_t count = ints_arr.size();
    count = std::min(impl::resize_or_clear(out, count), count);
    std::copy_n(std::begin(ints_arr), count, std::begin(out));
  }
  return out;
}

/// Convert KnowledgeRecord into an existing native C array, passed via
/// a pointer and size
template<typename T>
inline auto knowledge_cast(const KnowledgeRecord& in, T* out, size_t size)
    -> decltype(
        knowledge_cast(in, std::declval<impl::simple_span<T>&>()), (T*)nullptr)
{
  auto span = impl::make_span(out, size);
  knowledge_cast(in, span);
  return out;
}

/// Identity NOP for converting KnowledgeRecord to KnowledgeRecord
inline KnowledgeRecord knowledge_cast(
    type<KnowledgeRecord>, const KnowledgeRecord& in)
{
  return in;
}

/// For explicit type form, call through to form taking type struct
template<typename T>
inline auto knowledge_cast(const KnowledgeRecord& in)
    -> decltype(knowledge_cast(type<T>{}, in))
{
  return knowledge_cast(type<T>{}, in);
}

/// Convert a value to KnowledgeRecord
/// By default, if no other overload specified, this overload just calls
/// KnowledgeRecord constructor
template<class O>
inline auto knowledge_cast(O&& in) ->
    typename std::decay<decltype(KnowledgeRecord{std::forward<O>(in)})>::type
{
  return KnowledgeRecord{std::forward<O>(in)};
}

/// Construct a KnowledgeRecord with integer array type, from a container of
/// integers, except char types
template<typename T>
inline auto knowledge_cast(const T& in) -> typename std::enable_if<
    std::is_integral<typename std::decay<decltype(in[0])>::type>::value &&
        !std::is_same<typename std::decay<decltype(in[0])>::type,
            char>::value &&
        !std::is_same<typename std::decay<decltype(in[0])>::type,
            unsigned char>::value &&
        !std::is_same<T, std::vector<int64_t>>::value,
    typename std::decay<decltype(KnowledgeRecord{
        tags::integers, std::begin(in), std::end(in)})>::type>::type
{
  return KnowledgeRecord{tags::integers, std::begin(in), std::end(in)};
}

/// Construct a KnowledgeRecord with floating point array type
template<typename T>
inline auto knowledge_cast(const T& in) -> typename std::enable_if<
    std::is_floating_point<typename std::decay<decltype(in[0])>::type>::value &&
        !std::is_same<T, std::vector<double>>::value,
    typename std::decay<decltype(KnowledgeRecord{
        tags::doubles, std::begin(in), std::end(in)})>::type>::type
{
  return KnowledgeRecord{tags::doubles, std::begin(in), std::end(in)};
}

/// Construct a KnowledgeRecord with string type
template<typename T>
inline auto knowledge_cast(const T& in) -> typename std::enable_if<
    std::is_same<typename std::decay<decltype(in[0])>::type, char>::value &&
        !std::is_same<T, std::string>::value,
    typename std::decay<decltype(KnowledgeRecord{
        tags::string, std::begin(in), std::end(in)})>::type>::type
{
  return KnowledgeRecord{tags::string, std::begin(in), std::end(in)};
}

/// Construct a KnowledgeRecord with binary type
template<typename T>
inline auto knowledge_cast(const T& in) -> typename std::enable_if<
    std::is_same<typename std::decay<decltype(in[0])>::type,
        unsigned char>::value &&
        !std::is_same<T, std::vector<unsigned char>>::value,
    typename std::decay<decltype(KnowledgeRecord{
        tags::binary, std::begin(in), std::end(in)})>::type>::type
{
  return KnowledgeRecord{tags::binary, std::begin(in), std::end(in)};
}

template<typename T>
inline auto knowledge_cast(const KnowledgeRecord& in, T&& out)
    -> decltype(*out = knowledge_cast(
                    type<typename decay_<T>::container_type::value_type>{}, in))
{
  return (*out = knowledge_cast(
              type<typename decay_<T>::container_type::value_type>{}, in));
}

/// Identity NOP
inline KnowledgeRecord& knowledge_cast(KnowledgeRecord& in)
{
  return in;
}

/// Identity NOP
inline const KnowledgeRecord& knowledge_cast(const KnowledgeRecord& in)
{
  return in;
}

/// Identity NOP
inline const KnowledgeRecord& knowledge_cast(
    const KnowledgeRecord& in, KnowledgeRecord& out)
{
  return out = in;
}

#endif  // !DOXYGEN

/// Generates comparison operators for KnowledgeRecords, in combination with
/// types that can be knowledge_cast into KnowledgeRecords
#define MADARA_KNOWLEDGE_COMPARE_OP(op)                                       \
  template<typename T, typename std::enable_if<                               \
                           !std::is_convertible<T, KnowledgeRecord>::value && \
                               std::is_fundamental<T>::value,                 \
                           void*>::type = nullptr>                            \
  inline auto operator op(const KnowledgeRecord& l, const T& r)               \
      ->decltype(knowledge_cast<T>(l) op r)                                   \
  {                                                                           \
    return knowledge_cast<T>(l) op r;                                         \
  }                                                                           \
                                                                              \
  template<typename T, typename std::enable_if<                               \
                           !std::is_convertible<T, KnowledgeRecord>::value && \
                               std::is_fundamental<T>::value,                 \
                           void*>::type = nullptr>                            \
  inline auto operator op(const T& l, const KnowledgeRecord& r)               \
      ->decltype(l op knowledge_cast<T>(r))                                   \
  {                                                                           \
    return l op knowledge_cast<T>(r);                                         \
  }                                                                           \
                                                                              \
  inline bool operator op(const KnowledgeRecord& l, std::nullptr_t)           \
  {                                                                           \
    return l op 0;                                                            \
  }                                                                           \
                                                                              \
  inline bool operator op(const KnowledgeRecord& l, const char* r)            \
  {                                                                           \
    auto s = l.share_string();                                                \
    if (s)                                                                    \
    {                                                                         \
      return s->compare(r) op 0;                                              \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      return l.to_string().compare(r) op 0;                                   \
    }                                                                         \
  }                                                                           \
                                                                              \
  inline bool operator op(const char* l, const KnowledgeRecord& r)            \
  {                                                                           \
    auto s = r.share_string();                                                \
    if (s)                                                                    \
    {                                                                         \
      return std::strcmp(l, s->c_str()) op 0;                                 \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      return std::strcmp(l, r.to_string().c_str()) op 0;                      \
    }                                                                         \
  }                                                                           \
                                                                              \
  inline bool operator op(const KnowledgeRecord& l, const std::string& r)     \
  {                                                                           \
    auto s = l.share_string();                                                \
    if (s)                                                                    \
    {                                                                         \
      return s->compare(r) op 0;                                              \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      return l.to_string().compare(r) op 0;                                   \
    }                                                                         \
  }                                                                           \
                                                                              \
  inline bool operator op(const std::string& l, const KnowledgeRecord& r)     \
  {                                                                           \
    auto s = r.share_string();                                                \
    if (s)                                                                    \
    {                                                                         \
      return l.compare(*s) op 0;                                              \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      return l.compare(r.to_string()) op 0;                                   \
    }                                                                         \
  }

MADARA_KNOWLEDGE_COMPARE_OP(==)
MADARA_KNOWLEDGE_COMPARE_OP(!=)
MADARA_KNOWLEDGE_COMPARE_OP(<=)
MADARA_KNOWLEDGE_COMPARE_OP(>=)
MADARA_KNOWLEDGE_COMPARE_OP(<)
MADARA_KNOWLEDGE_COMPARE_OP(>)

/// Generates binary math operators for KnowledgeRecords, in combination with
/// types that can be knowledge_cast into KnowledgeRecord
#define MADARA_KNOWLEDGE_BINARY_OP(op)                                        \
  template<typename T, typename std::enable_if<                               \
                           !std::is_convertible<T, KnowledgeRecord>::value && \
                               std::is_fundamental<T>::value,                 \
                           void*>::type = nullptr>                            \
  inline auto operator op(const KnowledgeRecord& l, const T& r)               \
      ->decltype(l op knowledge_cast(r))                                      \
  {                                                                           \
    return l op knowledge_cast(r);                                            \
  }                                                                           \
                                                                              \
  template<typename T, typename std::enable_if<                               \
                           !std::is_convertible<T, KnowledgeRecord>::value && \
                               std::is_fundamental<T>::value,                 \
                           void*>::type = nullptr>                            \
  inline auto operator op(const T& l, const KnowledgeRecord& r)               \
      ->decltype(knowledge_cast(l) op r)                                      \
  {                                                                           \
    return knowledge_cast(l) op r;                                            \
  }

MADARA_KNOWLEDGE_BINARY_OP(+)
MADARA_KNOWLEDGE_BINARY_OP(-)
MADARA_KNOWLEDGE_BINARY_OP(/)
MADARA_KNOWLEDGE_BINARY_OP(*)
MADARA_KNOWLEDGE_BINARY_OP(%)

/// Generates composite assignment operators for KnowledgeRecords, in
/// combination with types that can be knowledge_cast into KnowledgeRecord
#define MADARA_KNOWLEDGE_COMPOSITE_OP(op)                                     \
  template<typename T, typename std::enable_if<                               \
                           !std::is_convertible<T, KnowledgeRecord>::value && \
                               std::is_fundamental<T>::value,                 \
                           void*>::type = nullptr>                            \
  inline auto operator op(KnowledgeRecord& l, const T& r)                     \
      ->decltype(l op knowledge_cast(r))                                      \
  {                                                                           \
    return l op knowledge_cast(r);                                            \
  }

MADARA_KNOWLEDGE_COMPOSITE_OP(+=)
MADARA_KNOWLEDGE_COMPOSITE_OP(-=)
MADARA_KNOWLEDGE_COMPOSITE_OP(/=)
MADARA_KNOWLEDGE_COMPOSITE_OP(*=)
MADARA_KNOWLEDGE_COMPOSITE_OP(%=)

MADARA_MAKE_VAL_SUPPORT_TEST(cast_to_record, x, knowledge_cast(x));
MADARA_MAKE_VAL_SUPPORT_TEST(
    cast_from_record, x, knowledge_cast(KnowledgeRecord{}, x));

template<typename T>
inline auto try_knowledge_cast(T&& in)
    -> enable_if_<supports_cast_to_record<T>::value, KnowledgeRecord>
{
  return knowledge_cast(std::forward<T>(in));
}

template<typename T>
inline auto try_knowledge_cast(T &&)
    -> enable_if_<!supports_cast_to_record<T>::value, KnowledgeRecord>
{
  return {};
}

template<typename T>
inline auto try_knowledge_cast(const KnowledgeRecord& rec, T& out)
    -> enable_if_<supports_cast_from_record<T>::value, bool>
{
  knowledge_cast(rec, out);
  return true;
}

template<typename T>
inline auto try_knowledge_cast(const KnowledgeRecord&, T&)
    -> enable_if_<!supports_cast_from_record<T>::value, bool>
{
  return false;
}

template<typename Impl, typename ValImpl, typename RefImpl>
template<typename T>
inline T BasicConstAny<Impl, ValImpl, RefImpl>::to(type<T>) const
{
  using exceptions::BadAnyAccess;

  if (!handler_ || !data_)
  {
    throw BadAnyAccess("Any::to<T>: Any is empty");
  }

  if (handler_->tindex == type_id<T>())
  {
    return impl().template ref_unsafe<T>();
  }

  if (supports_cast_from_record<T>::value)
  {
    if (!supports_to_record())
    {
      throw BadAnyAccess("Any::to<T>: Type stored in Any doesn't "
                         "support to_record");
    }

    T ret;
    try_knowledge_cast(handler_->to_record(data_), ret);
    return ret;
  }
  else
  {
    throw BadAnyAccess(
        "Any::to<T>: Type stored doesn't match type "
        "requested, and doesn't support knowledge_cast from record");
  }
}

template<typename Impl, typename ValImpl, typename RefImpl, typename CRefImpl>
template<typename T>
inline void BasicAny<Impl, ValImpl, RefImpl, CRefImpl>::assign(T&& t) const
{
  if (!this->handler_ && this->data_)
  {
    impl().template ref<decay_<T>>() = std::forward<T>(t);
    return;
  }

  if (this->handler_->tindex == type_id<T>())
  {
    ref<decay_<T>>() = std::forward<T>(t);
    return;
  }

  if (supports_cast_to_record<T>::value)
  {
    if (!supports_from_record())
    {
      throw exceptions::BadAnyAccess("Type stored in Any doesn't "
                                     "support to_record");
    }

    KnowledgeRecord rec(try_knowledge_cast(std::forward<T>(t)));
    this->handler_->from_record(rec, this->data_);
  }
  else
  {
    throw exceptions::BadAnyAccess(
        "Type stored doesn't match type "
        "assigning, and doesn't support knowledge_cast to record");
  }
}

template<typename Impl, typename ValImpl, typename RefImpl, typename CRefImpl>
inline void BasicAny<Impl, ValImpl, RefImpl, CRefImpl>::from_record(
    const knowledge::KnowledgeRecord& rec) const
{
  if (!supports_from_record())
  {
    throw exceptions::BadAnyAccess("Type stored in Any doesn't "
                                   "support to_record");
  }
  this->handler_->from_record(rec, this->data_);
}

template<typename OutputIterator>
size_t KnowledgeRecord::get_history_range(
    OutputIterator out, size_t index, size_t count) const
{
  return for_history_range(
      [&out](const KnowledgeRecord& rec) {
        knowledge_cast(rec, *out);
        ++out;
      },
      index, count);
}
}

namespace utility
{
inline namespace core
{
template<typename T,
    enable_if_<knowledge::supports_cast_to_record<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::to_record_fn_type get_type_handler_to_record(
    type<T>, overload_priority<8>)
{
  return [](void* ptr) -> knowledge::KnowledgeRecord {
    T& val = *static_cast<T*>(ptr);
    return knowledge::knowledge_cast(val);
  };
}

template<typename T,
    enable_if_<knowledge::supports_cast_from_record<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::from_record_fn_type
    get_type_handler_from_record(type<T>, overload_priority<8>)
{
  return [](const knowledge::KnowledgeRecord& rec, void* ptr) {
    T& val = *static_cast<T*>(ptr);
    knowledge::knowledge_cast(rec, val);
  };
}
}
}
}

#endif
