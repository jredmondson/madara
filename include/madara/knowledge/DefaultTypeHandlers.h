#include "Any.h"

#ifndef MADARA_KNOWLEDGE_DEFAULT_TYPE_HANDLERS_H_
#define MADARA_KNOWLEDGE_DEFAULT_TYPE_HANDLERS_H_

/**
 * @file DefaultTypeHandlers.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains TypeHandlers free functions for various types
 **/

#include <memory>
#include <sstream>
#include <map>
#include <functional>
#include <type_traits>

#include "madara/utility/SupportTest.h"
#include "madara/MadaraExport.h"
#include "madara/utility/StdInt.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/IntTypes.h"
#include "madara/exceptions/BadAnyAccess.h"

namespace madara { namespace utility { inline namespace core {
// Must define below in same namespace as type<> struct for ADL

constexpr bool madara_ignore_for_each_field(...) { return false; }

template<typename T>
constexpr bool madara_use_cereal(type<T>)
{
  return (knowledge::supports_for_each_field<T>::value &&
          !madara_ignore_for_each_field(type<T>{})) ||
         std::is_arithmetic<T>::value || std::is_enum<T>::value;
}

#define MADARA_IGNORE_FOR_EACH_FIELD(...) \
  constexpr bool madara_ignore_for_each_field(::madara::type<__VA_ARGS__>){return true;}

template<typename C, typename A>
MADARA_IGNORE_FOR_EACH_FIELD(std::basic_string<C, A>)

template<typename T, typename A>
MADARA_IGNORE_FOR_EACH_FIELD(std::vector<T, A>)

template<typename T, typename A>
MADARA_IGNORE_FOR_EACH_FIELD(std::list<T, A>)

template<typename T, typename A>
MADARA_IGNORE_FOR_EACH_FIELD(std::deque<T, A>)

template<typename T, typename A>
MADARA_IGNORE_FOR_EACH_FIELD(std::queue<T, A>)

template<typename T, typename A>
MADARA_IGNORE_FOR_EACH_FIELD(std::stack<T, A>)

template<size_t N>
MADARA_IGNORE_FOR_EACH_FIELD(std::bitset<N>)

template<typename T, size_t N>
MADARA_IGNORE_FOR_EACH_FIELD(std::array<T, N>)

template<typename K, typename V, typename C, typename A>
MADARA_IGNORE_FOR_EACH_FIELD(std::map<K, V, C, A>)

template<typename K, typename V, typename H, typename A>
MADARA_IGNORE_FOR_EACH_FIELD(std::unordered_map<K, V, H, A>)

template<typename V, typename C, typename A>
MADARA_IGNORE_FOR_EACH_FIELD(std::set<V, C, A>)

template<typename V, typename H, typename A>
MADARA_IGNORE_FOR_EACH_FIELD(std::unordered_set<V, H, A>)

#define MADARA_USE_CEREAL(...) \
  constexpr bool madara_use_cereal(::madara::type<__VA_ARGS__>){return true;}

template<typename C, typename A>
MADARA_USE_CEREAL(std::basic_string<C, A>)

template<typename T, typename A,
  enable_if_<madara_use_cereal(type<T>{}), int> = 0>
MADARA_USE_CEREAL(std::vector<T, A>)

template<typename T, typename A>
MADARA_USE_CEREAL(std::list<T, A>)

template<typename T, typename A>
MADARA_USE_CEREAL(std::deque<T, A>)

template<typename T, typename A>
MADARA_USE_CEREAL(std::queue<T, A>)

template<typename T, typename A>
MADARA_USE_CEREAL(std::stack<T, A>)

template<size_t N>
MADARA_USE_CEREAL(std::bitset<N>)

template<typename T, size_t N,
  enable_if_<madara_use_cereal(type<T>{}), int> = 0>
MADARA_USE_CEREAL(std::array<T, N>)

template<typename K, typename V, typename C, typename A>
MADARA_USE_CEREAL(std::map<K, V, C, A>)

template<typename K, typename V, typename H, typename A>
MADARA_USE_CEREAL(std::unordered_map<K, V, H, A>)

template<typename V, typename C, typename A>
MADARA_USE_CEREAL(std::set<V, C, A>)

template<typename V, typename H, typename A>
MADARA_USE_CEREAL(std::unordered_set<V, H, A>)

/// Creates a function for serializing the given type to a madara_oarchive.
/// By default, simply use the Boost.Serialization << operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr auto get_type_handler_save(type<T>, overload_priority<12>) ->
  enable_if_<madara_use_cereal(type<T>{}),
    knowledge::TypeHandlers::save_fn_type>
{
  return [](std::ostream &o, const void *ptr) {
      const T &val = *static_cast<const T *>(ptr);
      knowledge::madara_oarchive archive(o);
      archive << val;
    };
}

/// Creates a function for unserializing the given type to a madara_iarchive.
/// By default, simply use the Boost.Serialization >> operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr auto get_type_handler_load(type<T>, overload_priority<12>) ->
  enable_if_<madara_use_cereal(type<T>{}),
    knowledge::TypeHandlers::load_fn_type>
{
  return [](std::istream &i, void *ptr, const char *) {
      T &val = *static_cast<T *>(ptr);
      knowledge::madara_iarchive archive(i);
      archive >> val;
    };
}

/// Creates a function for serializing the given type to a json_oarchive.
/// By default, simply use the Boost.Serialization << operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr auto get_type_handler_save_json(type<T>, overload_priority<12>) ->
  knowledge::TypeHandlers::save_json_fn_type
{
  return [](std::ostream &o, const void *ptr) {
      const T &val = *static_cast<const T *>(ptr);
      {
        knowledge::json_oarchive archive(o);
        const char *tag = knowledge::AnyRegistry::get_type_name<T>();
        archive.setNextName(tag ?
            (std::string("Any<") + tag + ">").c_str() :
            "Any<UKNOWN_ANY_TYPE>");
        archive << val;
      }
    };
}

/// Creates a function for unserializing the given type to a json_iarchive.
/// By default, simply use the Boost.Serialization >> operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr auto get_type_handler_load_json(type<T>, overload_priority<12>) ->
  knowledge::TypeHandlers::load_json_fn_type
{
  return [](std::istream &i, void *ptr, const char *) {
      T &val = *static_cast<T *>(ptr);
      {
        knowledge::json_iarchive archive(i);
        archive >> val;
      }
    };
}

//template<typename T>
//constexpr auto get_type_handler_save_capn(type<T>, overload_priority<12>) ->
  //enable_if_<knowledge::supports_capn_builder<T>::value,
  //knowledge::TypeHandlers::save_capn_fn_type>
//{
  //return [](capnp::MessageBuilder &msgbuild, const void *ptr) {
      //(void)msgbuild; (void)ptr;
    //};
//}

//template<typename T>
//constexpr auto get_type_handler_load_capn(type<T>, overload_priority<12>) ->
  //enable_if_<knowledge::supports_capn_reader<T>::value,
  //knowledge::TypeHandlers::load_capn_fn_type>
//{
  //return [](capnp::MessageReader &msgread, void *ptr) {
      //(void)msgread; (void)ptr;
    //};
//}

// Functor to list fields of type which supports for_each_field
struct do_list_fields
{
  const knowledge::TypeHandlers *parent;
  std::vector<knowledge::AnyField> *out;
  size_t cur;

  template<typename T>
  void operator()(const char *name, T &)
  {
    out->emplace_back(name, *parent, knowledge::get_type_handler<T>(), cur);
    ++cur;
  }
};

// Returns a sorted vector of AnyField for a type supporting for_each_field
template<typename T>
inline std::vector<knowledge::AnyField> get_fields(T &val)
{
  std::vector<knowledge::AnyField> ret;
  for_each_field(do_list_fields{&knowledge::get_type_handler<T>(),
      &ret, 0}, val);
  std::sort(ret.begin(), ret.end(), knowledge::compare_any_fields_by_name());
  return ret;
}

// Implement get_type_handler_list_fields for type supporting for_each_field
template<typename T,
  enable_if_<knowledge::supports_for_each_field<T>::value &&
    !madara_ignore_for_each_field(type<decay_<T>>{}), int> = 0>
constexpr knowledge::TypeHandlers::list_fields_fn_type
  get_type_handler_list_fields(type<T>, overload_priority<8>)
{
  return [](void *ptr) -> const std::vector<knowledge::AnyField> & {
      static const std::vector<knowledge::AnyField> fields =
        get_fields<T>(*static_cast<T *>(ptr));
      return fields;
    };
}

// Functor to get field of type which supports for_each_field
struct do_get_field
{
  const knowledge::AnyField *field;
  const knowledge::TypeHandlers **handler;
  void **ptr;
  size_t cur;

  template<typename T>
  void operator()(const char *, T &val)
  {
    if (cur == field->data()) {
      *handler = &knowledge::get_type_handler<T>();
      *ptr = (void*) &val;
    }
    ++cur;
  }
};

// Implement get_type_handler_get_field for type supporting for_each_field
template<typename T,
  enable_if_<knowledge::supports_for_each_field<T>::value &&
    !madara_ignore_for_each_field(type<decay_<T>>{}), int> = 0>
constexpr knowledge::TypeHandlers::get_field_fn_type
  get_type_handler_get_field(type<T>, overload_priority<8>)
{
  return [](const knowledge::AnyField &field,
      const knowledge::TypeHandlers *&handler,
      void *&out_ptr,
      void *ptr)
    {
      T &val = *static_cast<T *>(ptr);
      for_each_field(do_get_field{&field, &handler, &out_ptr, 0}, val);
    };
}

// Functor to list fields of type which supports for_each_member
template<typename Class>
struct do_list_members
{
  const knowledge::TypeHandlers *parent;
  std::vector<knowledge::AnyField> *out;
  size_t cur;

  template<typename Field, typename Other>
  void operator()(const char *name, Field field, Other)
  {
    using T = decay_<decltype(invoke_(field, std::declval<Class&>()))>;
    out->emplace_back(name, *parent, knowledge::get_type_handler<T>(), cur);
    ++cur;
  }
};

// Returns a sorted vector of AnyField for a type supporting for_each_member
template<typename T>
inline std::vector<knowledge::AnyField> get_members(type<T>)
{
  std::vector<knowledge::AnyField> ret;
  for_each_member(type<T>{}, do_list_members<T>{
      &knowledge::get_type_handler<T>(),
      &ret, 0});
  std::sort(ret.begin(), ret.end(), knowledge::compare_any_fields_by_name());
  return ret;
}

// Implement get_type_handler_list_fields for type supporting for_each_member
template<typename T,
  enable_if_<knowledge::supports_for_each_member<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::list_fields_fn_type
  get_type_handler_list_fields(type<T>, overload_priority<8>)
{
  return [](void *) -> const std::vector<knowledge::AnyField> & {
      static const std::vector<knowledge::AnyField> fields =
        get_members(type<T>{});
      return fields;
    };
}

// Functor to get field of type which supports for_each_member
template<typename Class>
struct do_get_member
{
  Class *val;
  const knowledge::AnyField *field;
  const knowledge::TypeHandlers **handler;
  void **ptr;
  size_t cur;

  template<typename Field, typename Other>
  void operator()(const char *, Field ptm, Other)
  {
    if (cur == field->data()) {
      using T = decay_<decltype(invoke_(ptm, std::declval<Class&>()))>;
      *handler = &knowledge::get_type_handler<T>();
      *ptr = (void*) &(invoke_(ptm, *val));
    }
    ++cur;
  }
};

// Implement get_type_handler_get_field for type supporting for_each_member
template<typename T,
  enable_if_<knowledge::supports_for_each_member<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::get_field_fn_type
  get_type_handler_get_field(type<T>, overload_priority<8>)
{
  return [](const knowledge::AnyField &field,
      const knowledge::TypeHandlers *&handler,
      void *&out_ptr,
      void *ptr)
    {
      T &val = *static_cast<T *>(ptr);
      for_each_member(type<T>{}, do_get_member<T>{
          &val, &field, &handler, &out_ptr, 0});
    };
}

// Implement get_type_handler_int_index for type supporting operator[](int)
template<typename T,
  enable_if_<knowledge::supports_int_index<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::index_int_fn_type
  get_type_handler_index_int(type<T>, overload_priority<8>)
{
  return [](size_t index,
      const knowledge::TypeHandlers *&handler,
      void *&out_ptr,
      void *ptr)
    {
      T &val = *static_cast<T *>(ptr);
      using I = decltype(val[index]);
      handler = &knowledge::get_type_handler<decay_<I>>();
      out_ptr = &val[index];
    };
}

// Implement get_type_handler_int_index for type supporting .at(int).
// Takes precedence over operator[] version above
template<typename T,
  enable_if_<knowledge::supports_int_at_index<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::index_int_fn_type
  get_type_handler_index_int(type<T>, overload_priority<6>)
{
  return [](size_t index,
      const knowledge::TypeHandlers *&handler,
      void *&out_ptr,
      void *ptr)
    {
      T &val = *static_cast<T *>(ptr);
      using I = decltype(val[index]);
      handler = &knowledge::get_type_handler<decay_<I>>();
      out_ptr = &val.at(index);
    };
}

// Implement get_type_handler_int_index for type supporting resizing. Resizes
// on demand. Takes precedence over non-resizing versions.
template<typename T,
  enable_if_<
    knowledge::supports_int_index<T>::value &&
    knowledge::supports_size_member<T>::value &&
    knowledge::supports_resize_member<T>::value
  , int> = 0>
constexpr knowledge::TypeHandlers::index_int_fn_type
  get_type_handler_index_int(type<T>, overload_priority<4>)
{
  return [](size_t index,
      const knowledge::TypeHandlers *&handler,
      void *&out_ptr,
      void *ptr)
    {
      T &val = *static_cast<T *>(ptr);
      using I = decltype(val[index]);
      handler = &knowledge::get_type_handler<decay_<I>>();
      if (val.size() < index) {
        val.resize(index + 1);
      }
      out_ptr = &val[index];
    };
}

// Implement get_type_handler_str_index for type supporting
// operator[](const char *) or operator[](const std::string &)
template<typename T,
  enable_if_<knowledge::supports_str_index<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::index_str_fn_type
  get_type_handler_index_str(type<T>, overload_priority<8>)
{
  return [](const char *index,
      const knowledge::TypeHandlers *&handler,
      void *&out_ptr,
      void *ptr)
    {
      T &val = *static_cast<T *>(ptr);
      using I = decltype(val[index]);
      handler = &knowledge::get_type_handler<decay_<I>>();
      out_ptr = &val[index];
    };
}

// Implement get_type_handler_size for type supporting size() method
template<typename T,
  enable_if_<knowledge::supports_size_member<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::size_fn_type
  get_type_handler_size(type<T>, overload_priority<8>)
{
  return [](size_t &out, void *ptr)
    {
      T &val = *static_cast<T *>(ptr);
      out = val.size();
    };
}

// Implement get_type_handler_to_ostream for type supporting streaming to
// ostreams (i.e., std::cout << value)
template<typename T,
  enable_if_<knowledge::supports_to_ostream<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::to_ostream_fn_type
  get_type_handler_to_ostream(type<T>, overload_priority<8>)
{
  return [](std::ostream &o, void *ptr) -> std::ostream &
    {
      T &val = *static_cast<T *>(ptr);
      return o << val;
    };
}

} } // namespace utility::core

namespace knowledge {

/// Functor to pass to for_each_field to serialize a type to Cereal archives
template<typename Archive>
struct do_serialize
{
  Archive *ar;

  template<typename T>
  void operator()(const char *name, T &val)
  {
    (*ar)(cereal::make_nvp(name, val));
  }
};

/// Functor to pass to for_each_member to serialize a type to Cereal archives
template<typename T, typename Archive>
struct do_serialize_member
{
  T *val;
  Archive *ar;

  template<typename Field, typename Other>
  void operator()(const char *name, Field field, Other)
  {
    (*ar)(cereal::make_nvp(name, invoke_(field, *val)));
  }
};

} // namespace knowledge

} // namespace madara

namespace cereal
{

using ::madara::utility::core::madara_ignore_for_each_field;

// Implement Cereal library serialization for types supporting for_each_field
template<typename Archive, typename T>
auto serialize(Archive &ar, T &&val) ->
  ::madara::enable_if_<::madara::knowledge::supports_for_each_field<T>::value &&
    !madara_ignore_for_each_field(::madara::type<::madara::decay_<T>>{})>// &&
    //!::madara::knowledge::use_cereal_directly<::madara::decay_<T>>::value>
{
  for_each_field(::madara::knowledge::do_serialize<Archive>{&ar},
      std::forward<T>(val));
}

// Implement Cereal library serialization for types supporting for_each_member
template<typename Archive, typename T,
  ::madara::enable_if_<
    !madara_use_cereal(::madara::type<::madara::decay_<T>>{}) && (
      ::madara::is_same<Archive, ::madara::knowledge::json_iarchive>() ||
      ::madara::is_same<Archive, ::madara::knowledge::json_oarchive>()),
    int> = 0>
auto serialize(Archive &ar, T &&val) ->
  ::madara::enable_if_<::madara::knowledge::supports_for_each_member<T>::value>
{
  using DT = ::madara::decay_<T>;
  for_each_member(::madara::type<DT>{},
      ::madara::knowledge::do_serialize_member<DT, Archive>{&val, &ar});
}

}

#endif  // MADARA_KNOWLEDGE_DEFAULT_TYPE_HANDLERS_H_
