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
#include "Any.h"

namespace madara { namespace utility { inline namespace core {
// Must define below in same namespace as type<> struct for ADL

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
  enable_if_<knowledge::supports_for_each_field<T>::value, int> = 0>
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
  enable_if_<knowledge::supports_for_each_field<T>::value, int> = 0>
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

// Implement get_type_handler_int_index for type supporting for_each_field
// and operator[](int)
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

// Implement get_type_handler_int_index for type supporting for_each_field
// and at(int). Takes precedence over operator[] version above
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

// Implement get_type_handler_int_index for type supporting for_each_field
// and resizing. Resizes on demand. Takes precedence over non-resizing versions.
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

// Implement get_type_handler_str_index for type supporting for_each_field
// and operator[](const char *) or operator[](const std::string &)
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

// Implement get_type_handler_size for type supporting for_each_field
// and size() method
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

// Implement get_type_handler_to_ostream for type supporting for_each_field
// and streaming to ostreams (i.e., std::cout << value)
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

/// Functor to pass to for_each_field to serialize a type
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

} // namespace knowledge

namespace utility { inline namespace core {

#define MADARA_SERIALIZE_DIRECTLY(...) \
  constexpr bool madara_serialize_directly(type<__VA_ARGS__>) { return true; }

template<typename C, typename A>
MADARA_SERIALIZE_DIRECTLY(std::basic_string<C, A>)

template<typename T, typename A>
MADARA_SERIALIZE_DIRECTLY(std::vector<T, A>)

template<typename T, typename A>
MADARA_SERIALIZE_DIRECTLY(std::list<T, A>)

template<typename T, typename A>
MADARA_SERIALIZE_DIRECTLY(std::deque<T, A>)

template<typename T, typename A>
MADARA_SERIALIZE_DIRECTLY(std::queue<T, A>)

template<typename T, typename A>
MADARA_SERIALIZE_DIRECTLY(std::stack<T, A>)

template<size_t N>
MADARA_SERIALIZE_DIRECTLY(std::bitset<N>)

template<typename T, size_t N>
MADARA_SERIALIZE_DIRECTLY(std::array<T, N>)

template<typename K, typename V, typename C, typename A>
MADARA_SERIALIZE_DIRECTLY(std::map<K, V, C, A>)

template<typename K, typename V, typename H, typename A>
MADARA_SERIALIZE_DIRECTLY(std::unordered_map<K, V, H, A>)

template<typename V, typename C, typename A>
MADARA_SERIALIZE_DIRECTLY(std::set<V, C, A>)

template<typename V, typename H, typename A>
MADARA_SERIALIZE_DIRECTLY(std::unordered_set<V, H, A>)

}}

} // namespace madara

namespace cereal
{

constexpr bool madara_serialize_directly(...) { return false; }

// Implement Cereal library serialization for types supporting for_each_field
template<typename Archive, typename T>
auto serialize(Archive &ar, T &&val) ->
  ::madara::enable_if_<::madara::knowledge::supports_for_each_field<T>::value &&
    !madara_serialize_directly(::madara::type<::madara::decay_<T>>{})>// &&
    //!::madara::knowledge::use_cereal_directly<::madara::decay_<T>>::value>
{
  for_each_field(::madara::knowledge::do_serialize<Archive>{&ar},
      std::forward<T>(val));
}

}

#endif  // MADARA_KNOWLEDGE_DEFAULT_TYPE_HANDLERS_H_
