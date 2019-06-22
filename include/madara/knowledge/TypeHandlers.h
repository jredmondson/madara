#include "Any.h"

#ifndef MADARA_KNOWLEDGE_TYPE_HANDLERS_H_
#define MADARA_KNOWLEDGE_TYPE_HANDLERS_H_

/**
 * @file TypeHandlers.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the TypeHandlers class and related free functions
 **/

#include <memory>
#include <sstream>
#include <map>
#include <functional>
#include <type_traits>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wexceptions"
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wunused-private-field"
#endif
#include "cereal/archives/portable_binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/array.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/stack.hpp"
#include "cereal/types/queue.hpp"
#include "cereal/types/deque.hpp"
#include "cereal/types/list.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/set.hpp"
#include "cereal/types/bitset.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/unordered_set.hpp"
#include "cereal/types/string.hpp"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__

#include <boost/version.hpp>
#if BOOST_VERSION > 105600 && !defined(MADARA_NO_BOOST_TYPE_INDEX)
#define MADARA_USE_BOOST_TYPE_INDEX
#endif

#ifdef MADARA_USE_BOOST_TYPE_INDEX
#include <boost/type_index.hpp>
#else
#include <typeindex>
#endif

#include "madara/utility/SupportTest.h"
#include "madara/MadaraExport.h"
#include "madara/utility/StdInt.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/IntTypes.h"
#include "madara/exceptions/BadAnyAccess.h"
#include "AnyRegistry.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#else
#undef CONST
#endif

#ifdef _USE_CAPNP_
#include "capnp/schema.h"
#include "capnp/dynamic.h"
#endif // _USE_CAPNP_

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__

namespace madara
{
namespace knowledge
{
using madara_oarchive = cereal::PortableBinaryOutputArchive;
using madara_iarchive = cereal::PortableBinaryInputArchive;

using json_oarchive = cereal::JSONOutputArchive;
using json_iarchive = cereal::JSONInputArchive;

#ifdef MADARA_USE_BOOST_TYPE_INDEX
using type_index = boost::typeindex::type_index;
using boost::typeindex::type_id;
#else
using type_index = std::type_index;

template<typename T>
inline type_index type_id()
{
  return type_index(typeid(T));
}
#endif

struct TypeHandlers;
class Any;
class ConstAny;
class AnyRef;
class ConstAnyRef;
class KnowledgeRecord;

/**
 * Represents a field of a type which supports field-access via `Any` types.
 *
 * Created by the `list_fields()` and `find_fields()` methods of `Any` types.
 *
 * These objects are freely and cheaply copyable.
 **/
class AnyField
{
public:
  AnyField() = default;
  AnyField(const char* name, const TypeHandlers& parent,
      const TypeHandlers& handler, size_t data)
    : name_(name), parent_(&parent), handler_(&handler), data_(data)
  {
  }

  const char* name() const
  {
    return name_;
  }
  size_t data() const
  {
    return data_;
  }

  const TypeHandlers& parent() const;
  const TypeHandlers& handler() const;
  std::string type_name() const;

private:
  const char* name_ = nullptr;
  const TypeHandlers* parent_ = nullptr;
  const TypeHandlers* handler_ = nullptr;
  size_t data_ = 0;
};

struct compare_any_fields_by_name
{
  bool operator()(const char* l, const char* r) const
  {
    return std::strcmp(l, r) < 0;
  }

  bool operator()(const AnyField& l, const char* r) const
  {
    return (*this)(l.name(), r);
  }

  bool operator()(const char* l, const AnyField& r) const
  {
    return (*this)(l, r.name());
  }

  bool operator()(const AnyField& l, const AnyField& r) const
  {
    return (*this)(l.name(), r.name());
  }
};

/// For internal use. Holds type information for use by Any class.
struct TypeHandlers
{
  type_index tindex;

  typedef const char* (*tag_fn_type)(void*);
  tag_fn_type tag;

  typedef void* (*construct_default_fn_type)();
  construct_default_fn_type construct_default;

  typedef void (*destruct_fn_type)(void*);
  destruct_fn_type destruct;

  typedef void* (*clone_fn_type)(void*);
  clone_fn_type clone;

  typedef void (*save_fn_type)(std::ostream&, const void*);
  save_fn_type save;

  typedef void (*load_fn_type)(const char*, size_t, void*, const char*);
  load_fn_type load;

  typedef void (*save_json_fn_type)(std::ostream&, const void*);
  save_json_fn_type save_json;

  typedef void (*load_json_fn_type)(std::istream&, void*, const char*);
  load_json_fn_type load_json;

  typedef const std::vector<AnyField>& (*list_fields_fn_type)(void*);
  list_fields_fn_type list_fields;

  typedef void (*get_field_fn_type)(const AnyField& field,
      const TypeHandlers*& out_handler, void*& out_ptr, void*);
  get_field_fn_type get_field;

  typedef void (*size_fn_type)(size_t& out, void*);
  size_fn_type size;

  typedef void (*index_int_fn_type)(
      size_t index, const TypeHandlers*& out_handler, void*& out_ptr, void*);
  index_int_fn_type index_int;

  typedef void (*index_str_fn_type)(const char* index,
      const TypeHandlers*& out_handler, void*& out_ptr, void*);
  index_str_fn_type index_str;

  typedef KnowledgeRecord (*to_record_fn_type)(void*);
  to_record_fn_type to_record;

  typedef void (*from_record_fn_type)(const KnowledgeRecord& rec, void*);
  from_record_fn_type from_record;

  typedef std::ostream& (*to_ostream_fn_type)(std::ostream&, void*);
  to_ostream_fn_type to_ostream;

#ifdef _USE_CAPNP_
  typedef bool (*get_reader_fn_type)(capnp::DynamicStruct::Reader*,
      capnp::StructSchema*, const char**, size_t*, void*);
  get_reader_fn_type get_reader;
#endif // _USE_CAPNP_
};

inline const TypeHandlers& AnyField::parent() const
{
  return *parent_;
}

inline const TypeHandlers& AnyField::handler() const
{
  return *handler_;
}

#ifdef MADARA_USE_BOOST_TYPE_INDEX
inline std::string AnyField::type_name() const
{
  return handler_->tindex.pretty_name();
}
#else
inline std::string AnyField::type_name() const
{
  return handler_->tindex.name();
}
#endif

/// Creates a function for getting type's tag
template<typename T>
constexpr auto get_type_handler_tag(type<T>, overload_priority_weakest)
    -> TypeHandlers::tag_fn_type
{
  return [](void*) -> const char* { return AnyRegistry::get_type_name<T>(); };
}

/// Creates a function for default constructing the given type.
template<typename T>
constexpr auto get_type_handler_construct_default(
    type<T>, overload_priority_weakest) ->
    typename std::enable_if<std::is_constructible<T>::value,
        TypeHandlers::construct_default_fn_type>::type
{
  return []() -> void* { return new T{}; };
}

/// Creates a function for deleting the given type. By default, simply call
/// delete. Specialize this function to customize otherwise.
template<typename T>
constexpr auto get_type_handler_destruct(type<T>, overload_priority_weakest) ->
    typename std::enable_if<std::is_nothrow_destructible<T>::value,
        TypeHandlers::destruct_fn_type>::type
{
  return [](void* ptr) {
    T* t_ptr = static_cast<T*>(ptr);
    delete t_ptr;
  };
}

/// Creates a function for deleting the given type. By default, simply call
/// new with copy constructor. Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::clone_fn_type get_type_handler_clone(
    type<T>, overload_priority_weakest)
{
  return [](void* ptr) -> void* {
    T* t_ptr = static_cast<T*>(ptr);
    return new T(*t_ptr);
  };
}

/// Creates a function for serializing the given type to a madara_oarchive.
/// By default, simply use the Boost.Serialization << operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::save_fn_type get_type_handler_save(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}

/// Creates a function for unserializing the given type to a madara_iarchive.
/// By default, simply use the Boost.Serialization >> operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::load_fn_type get_type_handler_load(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}

// template<typename T>
// constexpr TypeHandlers::save_capn_fn_type get_type_handler_save_capn(type<T>,
// overload_priority_weakest)
//{
// return nullptr;
//}

// template<typename T>
// constexpr TypeHandlers::load_capn_fn_type get_type_handler_load_capn(type<T>,
// overload_priority_weakest)
//{
// return nullptr;
//}

/// Creates a function for serializing the given type to a json_oarchive.
/// By default, simply use the Boost.Serialization << operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::save_json_fn_type get_type_handler_save_json(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}

/// Creates a function for unserializing the given type to a json_iarchive.
/// By default, simply use the Boost.Serialization >> operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::load_json_fn_type get_type_handler_load_json(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::list_fields_fn_type get_type_handler_list_fields(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::get_field_fn_type get_type_handler_get_field(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::index_int_fn_type get_type_handler_index_int(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::index_str_fn_type get_type_handler_index_str(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::size_fn_type get_type_handler_size(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::to_record_fn_type get_type_handler_to_record(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::from_record_fn_type get_type_handler_from_record(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::to_ostream_fn_type get_type_handler_to_ostream(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}

#ifdef _USE_CAPNP_
template<typename T>
constexpr TypeHandlers::get_reader_fn_type get_type_handler_get_reader(
    type<T>, overload_priority_weakest)
{
  return nullptr;
}
#endif  // _USE_CAPNP_

/// For internal use. Constructs a TypeHandlers containing functions used by Any
template<typename T>
inline const TypeHandlers& get_type_handler(type<T> t)
{
  static const TypeHandlers handler{
      type_id<T>(),
      get_type_handler_tag(t, select_overload()),
      get_type_handler_construct_default(t, select_overload()),
      get_type_handler_destruct(t, select_overload()),
      get_type_handler_clone(t, select_overload()),
      get_type_handler_save(type<T>{}, select_overload()),
      get_type_handler_load(type<T>{}, select_overload()),
      get_type_handler_save_json(t, select_overload()),
      get_type_handler_load_json(t, select_overload()),
      get_type_handler_list_fields(t, select_overload()),
      get_type_handler_get_field(t, select_overload()),
      get_type_handler_size(t, select_overload()),
      get_type_handler_index_int(t, select_overload()),
      get_type_handler_index_str(t, select_overload()),
      get_type_handler_to_record(t, select_overload()),
      get_type_handler_from_record(t, select_overload()),
      get_type_handler_to_ostream(t, select_overload()),
#ifdef _USE_CAPNP_
      get_type_handler_get_reader(t, select_overload()),
#endif // _USE_CAPNP_
  };
  return handler;
}

/// For internal use. Constructs a TypeHandlers containing functions used by Any
template<typename T>
inline const TypeHandlers& get_type_handler()
{
  return get_type_handler(type<T>{});
}

/// Type trait for checking whether a type as an ADL-visible for_each_field
/// available.
MADARA_MAKE_VAL_SUPPORT_TEST(
    for_each_field, x, for_each_field(::madara::ignore_all<>{}, x));

MADARA_MAKE_VAL_SUPPORT_TEST(for_each_member, x,
    for_each_member(type<decay_<decltype(x)>>{}, ignore_all<void>{}));

MADARA_MAKE_VAL_SUPPORT_TEST(size_member, x, (1UL == x.size()));
MADARA_MAKE_VAL_SUPPORT_TEST(resize_member, x, (x.resize(1UL)));
MADARA_MAKE_VAL_SUPPORT_TEST(int_index, x, (x[1UL] = x[1UL]));
MADARA_MAKE_VAL_SUPPORT_TEST(str_index, x, (x[""] = x[""]));
MADARA_MAKE_VAL_SUPPORT_TEST(int_at_index, x, (x.at(1UL) = x.at(1UL)));
MADARA_MAKE_VAL_SUPPORT_TEST(str_at_index, x, (x.at("") = x.at("")));

MADARA_MAKE_VAL_SUPPORT_TEST(to_ostream, x, (std::cout << x));
MADARA_MAKE_VAL_SUPPORT_TEST(from_istream, x, (std::cin >> x));

MADARA_MAKE_VAL_SUPPORT_TEST(cereal_portable_binary_serialize, x,
    (std::declval<madara_oarchive>() << x));
MADARA_MAKE_VAL_SUPPORT_TEST(cereal_portable_binary_unserialize, x,
    (std::declval<madara_oarchive>() >> x));
MADARA_MAKE_VAL_SUPPORT_TEST(
    cereal_json_serialize, x, (std::declval<json_oarchive>() << x));
MADARA_MAKE_VAL_SUPPORT_TEST(
    cereal_json_unserialize, x, (std::declval<json_oarchive>() >> x));
MADARA_MAKE_VAL_SUPPORT_TEST(
    capn_builder, x, (typename decay_<decltype(x)>::Builder(nullptr)));
MADARA_MAKE_VAL_SUPPORT_TEST(
    capn_reader, x, (typename decay_<decltype(x)>::Reader()));

}  // namespace knowledge

namespace exceptions
{
/**
 * An exception for misuse of the Any class
 **/
class BadAnyAccess : public MadaraException
{
public:
  using MadaraException::MadaraException;

  template<typename Got>
  BadAnyAccess(type<Got>, knowledge::type_index expected)
    : BadAnyAccess(std::string("Bad Any access: expected ") +
#ifdef MADARA_USE_BOOST_TYPE_INDEX
                   expected.pretty_name() + ", got " +
                   knowledge::type_id<Got>().pretty_name()
#else
                   expected.name() + ", got " + knowledge::type_id<Got>().name()
#endif
      )
  {
  }
};
}  // namespace exceptions

}  // namespace madara

#endif  // MADARA_KNOWLEDGE_TYPE_HANDLERS_H_
