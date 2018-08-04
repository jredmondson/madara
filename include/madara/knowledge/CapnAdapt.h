#ifndef MADARA_KNOWLEDGE_CAPNADAPT_H_
#define MADARA_KNOWLEDGE_CAPNADAPT_H_

/**
 * @file CapnAdapt.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file provides mechanisms to adapt existing types to map to Cap'n Proto
 * generated types
 **/

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/rem.hpp>
#include <boost/preprocessor/facilities/overload.hpp>

#include "madara/utility/SupportTest.h"

#define KJ_HEADER_WARNINGS 0

#include "kj/io.h"
#include "kj/std/iostream.h"
#include "capnp/serialize.h"
#include "capnp/schema-loader.h"

namespace madara { namespace knowledge {

template<typename T, typename C>
struct CapnPrimitive {
  using CapnType = C;
  using FieldType = T;

  T (C::Reader::* get)() const;
  void (C::Builder::* set)(T);
};

template<typename C, typename T>
CapnPrimitive<T, C> MakeCapnPrimitive(
    T (C::Reader::* get)() const,
    void (C::Builder::* set)(T)
  )
{
  return {get, set};
}

template<typename R, typename B, typename C>
struct CapnStruct {
  using CapnType = C;
  using FieldReader = R;
  using FieldBuilder = B;

  R (C::Reader::* get)() const;
  B (C::Builder::* get_builder)();
  void (C::Builder::* set)(R);
  B (C::Builder::* init)();
  bool (C::Reader::* has)() const;
};

template<typename C, typename R, typename B>
CapnStruct<R, B, C> MakeCapnStruct(
    R (C::Reader::* get)() const,
    B (C::Builder::* get_builder)(),
    void (C::Builder::* set)(R),
    B (C::Builder::* init)(),
    bool (C::Reader::* has)() const
  )
{
  return {get, get_builder, set, init, has};
}

template<typename C>
struct CapnString {
  using CapnType = C;
  using FieldReader = typename capnp::Text::Reader;
  using FieldBuilder = typename capnp::Text::Builder;

  typename capnp::Text::Reader (C::Reader::* get)() const;
  typename capnp::Text::Builder (C::Builder::* get_builder)();
  void (C::Builder::* set)(typename capnp::Text::Reader);
  typename capnp::Text::Builder (C::Builder::* init)(unsigned int);
  bool (C::Reader::* has)() const;
};

template<typename C>
CapnString<C> MakeCapnString(
    typename capnp::Text::Reader (C::Reader::* get)() const,
    typename capnp::Text::Builder (C::Builder::* get_builder)(),
    void (C::Builder::* set)(typename capnp::Text::Reader),
    typename capnp::Text::Builder (C::Builder::* init)(unsigned int),
    bool (C::Reader::* has)() const
  )
{
  return {get, get_builder, set, init, has};
}

template<typename R, typename B, typename C>
struct CapnList {
  using CapnType = C;
  using FieldReader = R;
  using FieldBuilder = B;

  R (C::Reader::* get)() const;
  B (C::Builder::* get_builder)();
  B (C::Builder::* init)(unsigned int);
  bool (C::Reader::* has)() const;
};

template<typename C, typename R, typename B>
CapnList<R, B, C> MakeCapnList(
    R (C::Reader::* get)() const,
    B (C::Builder::* get_builder)(),
    B (C::Builder::* init)(unsigned int),
    bool (C::Reader::* has)() const
  )
{
  return {get, get_builder, init, has};
}

template<typename T, typename C>
inline void capn_set(typename C::Builder &builder,
    const CapnPrimitive<T, C> &prim, const T &val)
{
  (builder.*(prim.set))(val);
}

template<typename T, typename C>
inline void capn_get(typename C::Reader &reader,
    const CapnPrimitive<T, C> &prim, T &val)
{
  val = (reader.*(prim.get))();
}

template<typename C>
inline void capn_set(typename C::Builder &builder,
    const CapnString<C> &prim,
    const std::string &val)
{
  (builder.*(prim.set))(val);
}

template<typename C>
inline void capn_get(typename C::Reader &reader,
    const CapnString<C> &prim, std::string &val)
{
  val = (reader.*(prim.get))();
}

template<typename T, typename B>
struct do_capn_struct_set
{
  const T *val;
  B *builder;

  template<typename Ptm, typename Capn>
  void operator()(const char *, Ptm ptm, const Capn &capn)
  {
    capn_set(*builder, capn, invoke_(ptm, *const_cast<T *>(val)));
  }
};

template<typename T, typename R, typename B, typename C>
inline void capn_set(typename C::Builder &builder,
    const CapnStruct<R, B, C> &info, const T &val)
{
  (builder.*(info.init))();
  auto sub_builder{(builder.*(info.get_builder))()};
  for_each_member(type<T>{}, do_capn_struct_set<T, B>{
      &val, &sub_builder});
}

template<typename T, typename R>
struct do_capn_struct_get
{
  T *val;
  R *reader;

  template<typename Ptm, typename Capn>
  void operator()(const char *, Ptm ptm, const Capn &capn)
  {
    capn_get(*reader, capn, invoke_(ptm, *val));
  }
};

template<typename T, typename R, typename B, typename C>
inline void capn_get(typename C::Reader &reader,
    const CapnStruct<R, B, C> &info, T &val)
{
  auto sub_reader{(reader.*(info.get))()};
  for_each_member(type<T>{}, do_capn_struct_get<T, R>{
      &val, &sub_reader});
}

template<typename T,
  enable_if_<supports_for_each_member<T>::value, int> = 0>
auto infer_capn_type(type<T>) -> typename decltype(for_each_member(type<T>{},
                ignore_all<void>()))::self;

template<typename B, typename T>
inline auto capn_set(B &builder, const T &val) ->
  enable_if_<supports_for_each_member<T>::value>
{
  for_each_member(type<T>{},
      do_capn_struct_set<T, decay_<decltype(builder)>>{
      &val, &builder});
}

template<typename R, typename T>
inline auto capn_get(R &reader, T &val) ->
  enable_if_<supports_for_each_member<T>::value>
{
  for_each_member(type<T>{}, do_capn_struct_get<T, decay_<decltype(reader)>>{
      &val, &reader});
}

MADARA_MAKE_VAL_SUPPORT_TEST(capn_set, x, capn_set(std::declval<int&>(), x));
MADARA_MAKE_VAL_SUPPORT_TEST(capn_get, x, capn_get(std::declval<int&>(), x));
MADARA_MAKE_VAL_SUPPORT_TEST(infer_capn, x,
    infer_capn(type<decay_<decltype(x)>>()));

template<typename T,
  enable_if_<supports_infer_capn<T>::value, int> = 0>
auto infer_capn_type(type<std::vector<T>>) -> capnp::List<T>;

template<typename T, typename R, typename B, typename C>
inline auto capn_set(typename C::Builder &builder,
    const CapnList<R, B, C> &info, const std::vector<T> &val) ->
  enable_if_<supports_capn_set<T>::value && !std::is_arithmetic<T>::value>
{
  (builder.*(info.init))(val.size());
  auto list_builder{(builder.*(info.get_builder))()};
  size_t i = 0;
  for (const auto & cur : val) {
    auto elem_builder = list_builder[i];
    capn_set(elem_builder, cur);
    ++i;
  }
}

template<typename T, typename R, typename B, typename C>
inline auto capn_get(typename C::Reader &reader,
    const CapnList<R, B, C> &info, std::vector<T> &val) ->
  enable_if_<supports_capn_get<T>::value && !std::is_arithmetic<T>::value>
{
  auto list_reader{(reader.*(info.get))()};
  val.resize(list_reader.size());
  size_t i = 0;
  for (auto cur : list_reader) {
    capn_get(cur, val[i]);
    ++i;
  }
}

template<typename T, typename R, typename B, typename C>
inline auto capn_set(typename C::Builder &builder,
    const CapnList<R, B, C> &info, const std::vector<T> &val) ->
  enable_if_<std::is_arithmetic<T>::value>
{
  (builder.*(info.init))(val.size());
  auto list_builder{(builder.*(info.get_builder))()};
  size_t i = 0;
  for (const auto & cur : val) {
    list_builder.set(i, cur);
    ++i;
  }
}

template<typename T, typename R, typename B, typename C>
inline auto capn_get(typename C::Reader &reader,
    const CapnList<R, B, C> &info, std::vector<T> &val) ->
  enable_if_<std::is_arithmetic<T>::value>
{
  auto list_reader{(reader.*(info.get))()};
  val.resize(list_reader.size());
  size_t i = 0;
  for (auto cur : list_reader) {
    val[i] = cur;
    ++i;
  }
}

template<typename T, typename R, typename B, typename C, size_t N>
inline auto capn_set(typename C::Builder &builder,
    const CapnList<R, B, C> &info, const std::array<T, N> &val) ->
  enable_if_<supports_capn_set<T>::value && !std::is_arithmetic<T>::value>
{
  (builder.*(info.init))(val.size());
  auto list_builder{(builder.*(info.get_builder))()};
  size_t i = 0;
  for (const auto & cur : val) {
    auto elem_builder = list_builder[i];
    capn_set(elem_builder, cur);
    ++i;
  }
}

template<typename T, typename R, typename B, typename C, size_t N>
inline auto capn_get(typename C::Reader &reader,
    const CapnList<R, B, C> &info, std::array<T, N> &val) ->
  enable_if_<supports_capn_get<T>::value && !std::is_arithmetic<T>::value>
{
  auto list_reader{(reader.*(info.get))()};
  size_t i = 0;
  for (auto cur : list_reader) {
    capn_get(cur, val[i]);
    ++i;
    if (i >= N) {
      break;
    }
  }
  for (; i < N; ++i) {
    val[i] = T();
  }
}

template<typename T, typename R, typename B, typename C, size_t N>
inline auto capn_set(typename C::Builder &builder,
    const CapnList<R, B, C> &info, const std::array<T, N> &val) ->
  enable_if_<std::is_arithmetic<T>::value>
{
  (builder.*(info.init))(val.size());
  auto list_builder{(builder.*(info.get_builder))()};
  size_t i = 0;
  for (const auto & cur : val) {
    list_builder.set(i, cur);
    ++i;
  }
}

template<typename T, typename R, typename B, typename C, size_t N>
inline auto capn_get(typename C::Reader &reader,
    const CapnList<R, B, C> &info, std::array<T, N> &val) ->
  enable_if_<std::is_arithmetic<T>::value>
{
  auto list_reader{(reader.*(info.get))()};
  size_t i = 0;
  for (auto cur : list_reader) {
    val[i] = cur;
    ++i;
    if (i >= N) {
      break;
    }
  }
  for (; i < N; ++i) {
    val[i] = T();
  }
}

} // namespace knowledge

namespace utility { inline namespace core {

  template<typename T>
  inline auto get_type_handler_save(type<T>,
      overload_priority<8>) ->
    enable_if_<knowledge::supports_capn_set<T>::value,
      knowledge::TypeHandlers::save_fn_type>
  {
    return [](std::ostream &o, const void *ptr) {
        using namespace knowledge;

        const T &val = *static_cast<const T *>(ptr);

        capnp::MallocMessageBuilder msg;

        using CapnType = decltype(infer_capn_type(type<T>{}));
        auto builder = msg.template initRoot<CapnType>();

        capn_set(builder, val);

        size_t size = capnp::computeSerializedSizeInWords(msg)
                         * sizeof(capnp::word);
        knowledge::madara_oarchive archive(o);
        archive << size;

        kj::std::StdOutputStream stream{o};
        capnp::writeMessage(stream, msg);
      };
  }

  template<typename T>
  inline auto get_type_handler_load(type<T>,
      overload_priority<8>) ->
    enable_if_<knowledge::supports_capn_get<T>::value,
      knowledge::TypeHandlers::load_fn_type>
  {
    return [](std::istream &i, void *ptr, const char *) {
        using namespace knowledge;

        T &val = *static_cast<T *>(ptr);

        knowledge::madara_iarchive archive(i);
        size_t size;
        archive >> size;
        (void)size;

        kj::std::StdInputStream stream{i};
        capnp::InputStreamMessageReader msg(stream);

        using CapnType = decltype(infer_capn_type(type<T>{}));
        auto reader = msg.template getRoot<CapnType>();

        capn_get(reader, val);
      };
  }

} } }

/* ************************************************************************** */
/* ********************* Derived from Boost v.1.67 ************************** */
/* ************************************************************************** */

 /* **************************************************************************
  *                                                                          *
  *     This section (C) Copyright Paul Mensonides 2012.                     *
  *     Distributed under the Boost Software License, Version 1.0. (See      *
  *     accompanying file LICENSE_1_0.txt or copy at                         *
  *     http://www.boost.org/LICENSE_1_0.txt)                                *
  *                                                                          *
  ************************************************************************** */

/* *****************************************************************************
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 **************************************************************************** */

#define MADARA_PP_VARIADIC_SEQ_TO_SEQ(vseq) \
  BOOST_PP_CAT(MADARA_PP_VARIADIC_SEQ_TO_SEQ_A vseq, 0)

#define MADARA_PP_VARIADIC_SEQ_TO_SEQ_A(...) \
  ((__VA_ARGS__)) MADARA_PP_VARIADIC_SEQ_TO_SEQ_B

#define MADARA_PP_VARIADIC_SEQ_TO_SEQ_B(...) \
  ((__VA_ARGS__)) MADARA_PP_VARIADIC_SEQ_TO_SEQ_A

#define MADARA_PP_VARIADIC_SEQ_TO_SEQ_A0
#define MADARA_PP_VARIADIC_SEQ_TO_SEQ_B0

/* ************************************************************************** */
/* ****************** End of derived from Boost v.1.67 ********************** */
/* ************************************************************************** */

#define MADARA_CAPNPRIM_DETECT(TYPE, FIELD) \
  MADARA_MAKE_VAL_SUPPORT_TEST(madara_capn_struct_##TYPE##_has##FIELD, \
      MADARA_x, MADARA_x.has##FIELD()); \
  template<typename T, ::madara::enable_if_< \
    !supports_madara_capn_struct_##TYPE##_has##FIELD< \
      typename T::Builder>::value, int> = 0> \
  MADARA_AUTORET_FUNC(get_capnproto_type_info_##TYPE##_##FIELD, \
    (type<T>, ::madara::overload_priority_weakest), \
    ::madara::knowledge::MakeCapnPrimitive<T>( \
            &T::Reader::get##FIELD, \
            &T::Builder::set##FIELD \
      ))

#define MADARA_CAPNSTRUCT_DETECT(TYPE, FIELD) \
  template<typename T> \
  MADARA_AUTORET_FUNC(get_capnproto_type_info_##TYPE##_##FIELD, \
    (type<T>, ::madara::overload_priority<8>), \
    ::madara::knowledge::MakeCapnStruct<T>( \
            &T::Reader::get##FIELD, \
            &T::Builder::get##FIELD, \
            &T::Builder::set##FIELD, \
            &T::Builder::init##FIELD, \
            &T::Reader::has##FIELD \
      ))

#define MADARA_CAPNSTRING_DETECT(TYPE, FIELD) \
  template<typename T> \
  MADARA_AUTORET_FUNC(get_capnproto_type_info_##TYPE##_##FIELD, \
    (type<T>, ::madara::overload_priority<4>), \
    ::madara::knowledge::MakeCapnString<T>( \
            &T::Reader::get##FIELD, \
            &T::Builder::get##FIELD, \
            &T::Builder::set##FIELD, \
            &T::Builder::init##FIELD, \
            &T::Reader::has##FIELD \
      ))

#define MADARA_CAPNLIST_DETECT(TYPE, FIELD) \
  template<typename T> \
  MADARA_AUTORET_FUNC(get_capnproto_type_info_##TYPE##_##FIELD, \
    (type<T>, ::madara::overload_priority<8>), \
    ::madara::knowledge::MakeCapnList<T>( \
            &T::Reader::get##FIELD, \
            &T::Builder::get##FIELD, \
            &T::Builder::init##FIELD, \
            &T::Reader::has##FIELD \
      ))

#define MADARA_CAPN_DETECTORS(TYPE, FIELD) \
  MADARA_CAPNPRIM_DETECT(TYPE, FIELD) \
  MADARA_CAPNSTRUCT_DETECT(TYPE, FIELD) \
  MADARA_CAPNSTRING_DETECT(TYPE, FIELD) \
  MADARA_CAPNLIST_DETECT(TYPE, FIELD)

#define MADARA_PP_ARGS2(T0, T1) \
  (BOOST_PP_TUPLE_REM_CTOR(T0), \
   BOOST_PP_TUPLE_REM_CTOR(T1))

#define MADARA_CAPN_DETECTORS_AUX_3(TYPE, FIELD, CAPNFIELD) \
  MADARA_CAPN_DETECTORS(TYPE, CAPNFIELD)

#define MADARA_CAPN_DETECTORS_AUX_4(TYPE, FIELD, CAPNFIELD, ACCESSOR) \
  MADARA_CAPN_DETECTORS(TYPE, CAPNFIELD)

#define MADARA_CAPN_DETECTORS_AUX(...) \
  BOOST_PP_OVERLOAD(MADARA_CAPN_DETECTORS_AUX_, __VA_ARGS__)(__VA_ARGS__)

#define MADARA_CAPN_DETECTORS_FOR_EACH(R, DATA, ELEM) \
  BOOST_PP_EXPAND(MADARA_CAPN_DETECTORS_AUX MADARA_PP_ARGS2(DATA, ELEM))

#define MADARA_CAPN_DO_DETECT(TYPE, CAPNTYPE, FIELD) \
  get_capnproto_type_info_##TYPE##_##FIELD( \
      type<CAPNTYPE>{}, ::madara::select_overload())

#define MADARA_CAPN_MEMBER_5(FUNC, TYPE, CAPNTYPE, FIELD, CAPNFIELD) \
    FUNC(#FIELD, &TYPE::FIELD, \
         MADARA_CAPN_DO_DETECT(TYPE, CAPNTYPE, CAPNFIELD));

#define MADARA_CAPN_MEMBER_6(FUNC, TYPE, CAPNTYPE, FIELD, CAPNFIELD, ACCESSOR) \
    FUNC(#FIELD, ACCESSOR, \
         MADARA_CAPN_DO_DETECT(TYPE, CAPNTYPE, CAPNFIELD));

#define MADARA_CAPN_MEMBER(...) \
  BOOST_PP_OVERLOAD(MADARA_CAPN_MEMBER_, __VA_ARGS__)(__VA_ARGS__)

#define MADARA_CAPN_MEMBERS_FOR_EACH(R, DATA, ELEM) \
  BOOST_PP_EXPAND(MADARA_CAPN_MEMBER MADARA_PP_ARGS2(DATA, ELEM))

#define MADARA_CAPN_MEMBERS_IMPL(TYPE, CAPNTYPE, FIELDS) \
  BOOST_PP_SEQ_FOR_EACH(MADARA_CAPN_DETECTORS_FOR_EACH, (TYPE), FIELDS); \
  \
  template<typename MADARA_FunType> \
  ::madara::type<CAPNTYPE> for_each_member( \
      ::madara::type<TYPE>, \
      MADARA_FunType&& MADARA_fun) \
  { \
    BOOST_PP_SEQ_FOR_EACH(MADARA_CAPN_MEMBERS_FOR_EACH, \
        (MADARA_fun, TYPE, CAPNTYPE), FIELDS); \
    return {}; \
  }

#define MADARA_CAPN_MEMBERS(TYPE, CAPNTYPE, FIELDS) \
  MADARA_CAPN_MEMBERS_IMPL(TYPE, CAPNTYPE, \
      MADARA_PP_VARIADIC_SEQ_TO_SEQ(FIELDS))

#endif // MADARA_KNOWLEDGE_CAPNADAPT_H_
