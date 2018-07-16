#ifndef MADARA_KNOWLEDGE_ANY_H_
#define MADARA_KNOWLEDGE_ANY_H_

/**
 * @file Any.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the Any class
 **/

#include <memory>
#include <sstream>
#include <type_traits>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wexceptions"
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
#endif // __GNUC__

#include <boost/version.hpp>
#if BOOST_VERSION > 105600 && !defined(MADARA_NO_BOOST_TYPE_INDEX)
#define MADARA_USE_BOOST_TYPE_INDEX
#endif

#ifdef MADARA_USE_BOOST_TYPE_INDEX
#include <boost/type_index.hpp>
#else
#include <typeindex>
#endif

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

#include "madara/utility/SupportTest.h"
#include "madara/MadaraExport.h"
#include "madara/utility/StdInt.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/IntTypes.h"
#include "madara/exceptions/BadAnyAccess.h"

namespace madara { namespace knowledge {

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
  AnyField(const char *name,
      const TypeHandlers &parent,
      const TypeHandlers &handler,
      size_t data)
    : name_(name), parent_(&parent), handler_(&handler), data_(data) {}

  const char *name() const { return name_; }
  size_t data() const { return data_; }

  const TypeHandlers &parent() const;
  const TypeHandlers &handler() const;
  std::string type_name() const;
private:
  const char *name_ = nullptr;
  const TypeHandlers *parent_ = nullptr;
  const TypeHandlers *handler_ = nullptr;
  size_t data_ = 0;
};

struct compare_any_fields_by_name
{
  bool operator()(const char *l, const char *r) const
  {
    return std::strcmp(l, r) < 0;
  }

  bool operator()(const AnyField &l, const char *r) const
  {
    return (*this)(l.name(), r);
  }

  bool operator()(const char *l, const AnyField &r) const
  {
    return (*this)(l, r.name());
  }

  bool operator()(const AnyField &l, const AnyField &r) const
  {
    return (*this)(l.name(), r.name());
  }
};

/// For internal use. Holds type information for use by Any class.
struct TypeHandlers
{
  type_index tindex;

  typedef void (*destruct_fn_type)(void *);
  destruct_fn_type destruct;

  typedef void *(*clone_fn_type)(void *);
  clone_fn_type clone;

  typedef void (*save_fn_type)(madara_oarchive &, const void *);
  save_fn_type save;

  typedef void (*load_fn_type)(madara_iarchive &, void *);
  load_fn_type load;

  typedef void (*save_json_fn_type)(json_oarchive &, const void *);
  save_json_fn_type save_json;

  typedef void (*load_json_fn_type)(json_iarchive &, void *);
  load_json_fn_type load_json;

  typedef const std::vector<AnyField> &(*list_fields_fn_type)(void *);
  list_fields_fn_type list_fields;

  typedef void (*get_field_fn_type)(const AnyField &field,
      const TypeHandlers *&out_handler, void *&out_ptr, void *);
  get_field_fn_type get_field;

  typedef void (*size_fn_type)(size_t &out, void *);
  size_fn_type size;

  typedef void (*index_int_fn_type)(size_t index,
      const TypeHandlers *&out_handler, void *&out_ptr, void *);
  index_int_fn_type index_int;

  typedef void (*index_str_fn_type)(const char *index,
      const TypeHandlers *&out_handler, void *&out_ptr, void *);
  index_str_fn_type index_str;

  typedef KnowledgeRecord (*to_record_fn_type)(void *);
  to_record_fn_type to_record;

  typedef void (*from_record_fn_type)(
      const KnowledgeRecord &rec, void *);
  from_record_fn_type from_record;

  typedef std::ostream &(*to_ostream_fn_type)(std::ostream &, void *);
  to_ostream_fn_type to_ostream;
};

inline const TypeHandlers &AnyField::parent() const
{
  return *parent_;
}

inline const TypeHandlers &AnyField::handler() const
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

/// Creates a function for deleting the given type. By default, simply call
/// delete. Specialize this function to customize otherwise.
template<typename T>
constexpr auto get_type_handler_destruct(type<T>,
    overload_priority_weakest) ->
  typename std::enable_if<std::is_nothrow_destructible<T>::value,
    TypeHandlers::destruct_fn_type>::type
{
  return [](void *ptr) {
      T *t_ptr = static_cast<T *>(ptr);
      delete t_ptr;
    };
}

/// Creates a function for deleting the given type. By default, simply call
/// new with copy constructor. Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::clone_fn_type get_type_handler_clone(type<T>,
    overload_priority_weakest)
{
  return [](void *ptr) -> void * {
      T *t_ptr = static_cast<T *>(ptr);
      return new T(*t_ptr);
    };
}

/// Creates a function for serializing the given type to a madara_oarchive.
/// By default, simply use the Boost.Serialization << operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::save_fn_type get_type_handler_save(type<T>,
    overload_priority_weakest)
{
  return [](madara_oarchive &archive, const void *ptr) {
      const T &val = *static_cast<const T *>(ptr);
      archive << val;
    };
}

/// Creates a function for unserializing the given type to a madara_iarchive.
/// By default, simply use the Boost.Serialization >> operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::load_fn_type get_type_handler_load(type<T>,
    overload_priority_weakest)
{
  return [](madara_iarchive &archive, void *ptr) {
      T &val = *static_cast<T *>(ptr);
      archive >> val;
    };
}

/// Creates a function for serializing the given type to a json_oarchive.
/// By default, simply use the Boost.Serialization << operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::save_json_fn_type
  get_type_handler_save_json(type<T>, overload_priority_weakest)
{
  return [](json_oarchive &archive, const void *ptr) {
      const T &val = *static_cast<const T *>(ptr);
      archive << val;
    };
}

/// Creates a function for unserializing the given type to a json_iarchive.
/// By default, simply use the Boost.Serialization >> operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::load_json_fn_type
  get_type_handler_load_json(type<T>, overload_priority_weakest)
{
  return [](json_iarchive &archive, void *ptr) {
      T &val = *static_cast<T *>(ptr);
      archive >> val;
    };
}

template<typename T>
constexpr TypeHandlers::list_fields_fn_type
  get_type_handler_list_fields(type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::get_field_fn_type
  get_type_handler_get_field(type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::index_int_fn_type
  get_type_handler_index_int(type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::index_str_fn_type
  get_type_handler_index_str(type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::size_fn_type
  get_type_handler_size(type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::to_record_fn_type
  get_type_handler_to_record(type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::from_record_fn_type
  get_type_handler_from_record(type<T>, overload_priority_weakest)
{
  return nullptr;
}

template<typename T>
constexpr TypeHandlers::to_ostream_fn_type
  get_type_handler_to_ostream(type<T>, overload_priority_weakest)
{
  return nullptr;
}

/// For internal use. Constructs a TypeHandlers containing functions used by Any
template<typename T>
inline const TypeHandlers &get_type_handler(type<T> t)
{
  static const TypeHandlers handler = {
      type_id<T>(),
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
    };
  return handler;
}

/// For internal use. Constructs a TypeHandlers containing functions used by Any
template<typename T>
inline const TypeHandlers &get_type_handler()
{
  return get_type_handler(type<T>{});
}

/// Type trait for checking whether a type as an ADL-visible for_each_field
/// available.
MADARA_MAKE_VAL_SUPPORT_TEST(for_each_field, x,
    for_each_field(::madara::ignore_all<>{}, x));

MADARA_MAKE_VAL_SUPPORT_TEST(size_member, x, (1UL == x.size()));
MADARA_MAKE_VAL_SUPPORT_TEST(int_index, x, (x[1UL] = x[1UL]));
MADARA_MAKE_VAL_SUPPORT_TEST(str_index, x, (x[""] = x[""]));
MADARA_MAKE_VAL_SUPPORT_TEST(int_at_index, x, (x.at(1UL) = x.at(1UL)));
MADARA_MAKE_VAL_SUPPORT_TEST(str_at_index, x, (x.at("") = x.at("")));

MADARA_MAKE_VAL_SUPPORT_TEST(to_ostream, x, (std::cout << x));
MADARA_MAKE_VAL_SUPPORT_TEST(from_istream, x, (std::cin >> x));

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

namespace exceptions {
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
          expected.name() + ", got " +
          knowledge::type_id<Got>().name()
#endif
          ) {}
  };
}

namespace knowledge {

namespace tags {

/**
 * Type tag used with Any to signifiy storing raw unserialized data. The Any
 * will unserialize lazily as needed.
 *
 * Note that this lazy deserialization is not fully type-safe, and might not
 * throw an exception if the wrong type is used. The result may be garbled
 * data, but shouldn't segfault or trample other data.
 **/
constexpr struct raw_data_t {} raw_data;

}

template<typename T, typename = void>
struct is_type_tag_impl : std::false_type {};

template<typename T>
struct is_type_tag_impl<type<T>> : std::true_type {};

template<typename T>
constexpr bool is_type_tag() { return is_type_tag_impl<decay_<T>>::value; }

/**
 * Provides methods common to Any, ConstAny, AnyRef, ConstAnyRef. Use those
 * classes, not this one directly.
 **/
template<typename Impl, typename ValImpl, typename RefImpl>
class BasicConstAny
{
protected:
  BasicConstAny() = default;

  BasicConstAny(const TypeHandlers *handler, void *data)
    : handler_(handler), data_(data) {}

  Impl &impl() { return *static_cast<Impl*>(this); }
  const Impl &impl() const { return *static_cast<const Impl*>(this); }

  friend Impl;
  friend ValImpl;
  friend RefImpl;

  template<typename Impl2, typename ValImpl2, typename RefImpl2>
  friend class BasicConstAny;

  template<typename Impl2, typename ValImpl2, typename RefImpl2, typename CRefImpl2>
  friend class BasicAny;

public:
  /**
   * Test whether this Any holds a value. A default constructed Any is empty.
   *
   * @return true if this Any is empty
   **/
  bool empty() const
  {
    return data_ == nullptr;
  }

  /**
   * Test whether this Any holds unserialized raw data, to be lazily
   * deserialized when first needed.
   *
   * Note that this lazy deserialization is not fully type-safe, and might not
   * throw an exception if the wrong type is used. The result may be garbled
   * data, but shouldn't segfault or trample other data.
   *
   * @return true if this Any holds raw data
   **/
  bool raw() const
  {
    return data_ != nullptr && handler_ == nullptr;
  }

protected:
  template<typename T>
  void check_type(type<T> t) const
  {
    if (!data_) {
      throw exceptions::BadAnyAccess("ref() called on empty Any");
    } else if (!handler_) {
      throw exceptions::BadAnyAccess("ref() called on const Any with raw data");
    } else if (type_id<T>() != handler_->tindex) {
      throw exceptions::BadAnyAccess(t, handler_->tindex);
    }
  }

public:
  /**
   * Create a const reference type from this Any type
   **/
  const RefImpl cref() const
  {
    return AnyRef(handler_, data_);
  }

  /**
   * Create a const reference type from this Any type
   **/
  const RefImpl ref() const
  {
    return cref();
  }

  /**
   * Access the Any's stored value by const reference.
   * If empty() or raw() is true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as const T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  const T &ref(type<T> t) const
  {
    impl().check_type(t);
    return impl().ref_unsafe(t);
  }

  /**
   * Access the Any's stored value by const reference.
   * If empty() or raw() is true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as const T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  const T &ref() const
  {
    return impl().ref(type<T>{});
  }

  /**
   * Access the Any's stored value by const reference.
   * If empty() or raw() is true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as const T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  const T &cref(type<T> t) const
  {
    return impl().ref(t);
  }

  /**
   * Access the Any's stored value by const reference.
   * If empty() or raw() is true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as const T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  const T &cref() const
  {
    return impl().ref(type<T>{});
  }

  /**
   * Reference the stored data as type T, WITHOUT any type checking. Do not
   * call this unless you are certain what type the Any contains. This does not
   * unserialize if the Any contains unserialized raw data.
   *
   * @tparam T the type to treat the stored data as
   * @return a const reference of type T to the stored data
   **/
  template<typename T>
  const T &ref_unsafe(type<T>) const
  {
    return *reinterpret_cast<const T *>(data_);
  }

  /**
   * Reference the stored data as type T, WITHOUT any type checking. Do not
   * call this unless you are certain what type the Any contains. This does not
   * unserialize if the Any contains unserialized raw data.
   *
   * @tparam T the type to treat the stored data as
   * @return a const reference of type T to the stored data
   **/
  template<typename T>
  const T &ref_unsafe() const
  {
    return ref_unsafe(type<T>{});
  }

  /**
   * Serialize this Any to the given buffer. Throws an exception if the buffer
   * size is insufficient.
   *
   * @param data the buffer to serialize to
   * @param size size of the buffer
   * @return the actual number of bytes used during serialization
   **/
  size_t serialize(char *data, size_t size) const
  {
    namespace bio = boost::iostreams;

    bio::array_sink output_sink(data, size);
    bio::stream<bio::array_sink> output_stream(output_sink);

    auto pos = output_stream.tellp();
    impl().serialize(output_stream);
    auto len = output_stream.tellp() - pos;

    return len;
  }

  /**
   * Serialize this Any to the given vector. The vector will be cleared first,
   * and resized as needed.
   *
   * @param vec the vector to serialize to, which will be cleared first
   * @return the actual number of bytes used during serialization
   **/
  size_t serialize(std::vector<char> &vec) const
  {
    namespace bio = boost::iostreams;

    vec.clear();
    auto output_sink = bio::back_inserter(vec);
    bio::stream<decltype(output_sink)> output_stream(output_sink);

    impl().serialize(output_stream);

    return vec.size();
  }

  /**
   * Serialize this Any to the given output stream.
   *
   * @param stream the output stream to serialize to
   **/
  void serialize(std::ostream &stream) const
  {
    madara_oarchive archive(stream);

    impl().serialize(archive);
  }

  /**
   * Serialize this Any to the given madara_oarchive.
   *
   * @param stream the output archive to serialize to
   **/
  void serialize(madara_oarchive &archive) const
  {
    if (handler_) {
      handler_->save(archive, data_);
    } else {
      raw_data_storage *sto = (raw_data_storage *)data_;
      archive.saveBinary<1>(sto->data, sto->size);
    }
  }

  /**
   * Serialize this Any to the given archive. Use this overload for
   * Boost.Serialization archives other than madara_oarchive.
   *
   * @param stream the output archive to serialize to
   **/
  void serialize(json_oarchive &archive) const
  {
    if (handler_) {
      handler_->save_json(archive, data_);
    } else {
      raw_data_storage *sto = (raw_data_storage *)data_;
      archive.saveBinaryValue(sto->data, sto->size);
    }
  }

  std::string to_json() const
  {
    std::ostringstream stream;
    {
      cereal::JSONOutputArchive json(stream);
      json.makeArray();
      impl().serialize(json);
    }
    return stream.str();
  }

  /**
   * Returns true if the stored type supports list_fields(), and the ref()
   * overloads that retrieve fields. False otherwise.
   **/
  bool supports_fields() const
  {
    return handler_ && handler_->list_fields && handler_->get_field;
  }

  /**
   * Get a list of fields the stored object has. This operation is O(n log n)
   * for n fields the first time it is called for a type, but O(1)
   * subsequently, as it returns the same cached list.
   *
   * Use the returned list to access fields using ref(AnyField)
   *
   * Returns empty fields list if type doesn't support fields.
   **/
  const std::vector<AnyField> &list_fields() const
  {
    if (!supports_fields()) {
      static std::vector<AnyField> empty;
      return empty;
    }
    return handler_->list_fields(data_);
  }

  /**
   * Return the AnyField for the given name. Uses list_fields() internally.
   * O(log n) string comparisons for n fields, excluding time for lsit_fields().
   *
   * throws BadAnyAccess if stored type doesn't have the named field.
   **/
  AnyField find_field(const char *name) const
  {
    using exceptions::BadAnyAccess;

    if (!name) {
      throw BadAnyAccess("Null name pointer passed to find_field");
    }

    const auto &fields = list_fields();
    auto found = std::equal_range(fields.begin(), fields.end(), name,
        compare_any_fields_by_name());
    if (found.first == fields.end() || found.first == found.second) {
      throw BadAnyAccess(std::string("Type in Any does not contain field \"") +
          name + "\"");
    }

    return *found.first;
  }

  /**
   * Verifies that the given field belongs to the stored type. Throws
   * BadAnyAccess if not. If it does belong, simply returns the given field.
   **/
  AnyField find_field(const AnyField &field) const
  {
    using exceptions::BadAnyAccess;

    if (!handler_ || field.parent().tindex != handler_->tindex) {
      throw BadAnyAccess(std::string("Type in Any does not contain field \"") +
          field.name() + "\"");
    }
    return field;
  }

  /**
   * Return the AnyField for the given name. Uses list_fields() internally.
   * O(log n) string comparisons for n fields, excluding time for lsit_fields().
   *
   * throws BadAnyAccess if stored type doesn't have the named field.
   **/
  AnyField find_field(const std::string &name) const
  {
    return find_field(name.c_str());
  }

  /**
   * Access a field by const reference, using an AnyField object.
   *
   * Get the AnyField objects for a given type by calling list_fields()
   **/
  RefImpl ref(const AnyField &field) const
  {
    using exceptions::BadAnyAccess;

    if (!supports_fields())
    {
      throw BadAnyAccess("Tried to access field in Any holding type "
          "that doesn't support them");
    }
    if (field.parent().tindex != handler_->tindex)
    {
      throw BadAnyAccess("Tried to access field in Any from a "
          "class not contained within");
    }
    auto func = handler_->get_field;
    if (!func) {
      return {};
    }
    const TypeHandlers *handler;
    void *data;
    func(field, handler, data, data_);
    return {handler, data};
  }

  /**
   * Acess a field by const reference, by name.
   *
   * Use the ref(AnyField) overload for better efficiency.
   **/
  RefImpl ref(const char *name) const
  {
    return ref(find_field(name));
  }

  /**
   * Acess a field by const reference, by name.
   *
   * Use the ref(AnyField) overload for better efficiency.
   **/
  RefImpl ref(const std::string &name) const
  {
    return ref(find_field(name));
  }

  /**
   * Access a field by const reference, using an AnyField object.
   *
   * Get the AnyField objects for a given type by calling list_fields()
   **/
  RefImpl cref(const AnyField &field) const
  {
    return ref(field);
  }

  /**
   * Acess a field by const reference, by name.
   *
   * Use the cref(AnyField) overload for better efficiency.
   **/
  RefImpl cref(const char *name) const
  {
    return ref(find_field(name));
  }

  /**
   * Acess a field by const reference, by name.
   *
   * Use the cref(AnyField) overload for better efficiency.
   **/
  RefImpl cref(const std::string &name) const
  {
    return ref(find_field(name));
  }

  /**
   * Access the Any's stored value's field by const reference. First, uses
   * ref<Class>(), where Class is the owner of the given pointer-to-member.
   * If empty() or raw() are true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as T&, else throw BadAnyAccess exception
   *
   * Note that Class must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * Then, it applies the pointer-to-member to the reference, to get a
   * reference to the member.
   *
   * @param ptm the pointer-to-member to call
   * @return a reference to the contained value's member
   **/
  template<typename T, typename Class>
  const T &ref(T Class::* ptm) const
  {
    return cref(ptm);
  }

  /**
   * Access the Any's stored value's field by const reference. First, uses
   * ref<Class>(), where Class is the owner of the given pointer-to-member.
   * If empty() or raw() are true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as T&, else throw BadAnyAccess exception
   *
   * Note that Class must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * Then, it applies the pointer-to-member to the reference, to get a
   * reference to the member.
   *
   * @param ptm the pointer-to-member to call
   * @return a reference to the contained value's member
   **/
  template<typename T, typename Class>
  const T &cref(T Class::* ptm) const
  {
    return (&impl().ref(type<Class>{}))->*ptm;
  }

  /**
   * Call a const method of the Any's stored value. First, uses ref<Class>(),
   * where Class is the owner of the given pointer-to-member-function.
   * If empty() or raw() are true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as T&, else throw BadAnyAccess exception
   *
   * Note that Class must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * Then, it calls the pointer-to-member-function on the reference, forwarding
   * the remaining arguments, and returns the result.
   *
   * @param ptmf the pointer-to-member-function to call
   * @param args the arguments to pass to that call
   * @return result of the method called
   **/
  template<typename T, typename Class, typename... Params, typename... Args>
  T ref(T (Class::* ptmf)(Params...) const, Args&&... args) const
  {
    return cref(ptmf, std::forward<Args>(args)...);
  }

  /**
   * Call a const method of the Any's stored value. First, uses ref<Class>(),
   * where Class is the owner of the given pointer-to-member-function.
   * If empty() or raw() are true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as T&, else throw BadAnyAccess exception
   *
   * Note that Class must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * Then, it calls the pointer-to-member-function on the reference, forwarding
   * the remaining arguments, and returns the result.
   *
   * @param ptmf the pointer-to-member-function to call
   * @param args the arguments to pass to that call
   * @return result of the method called
   **/
  template<typename T, typename Class, typename... Params, typename... Args>
  T cref(T (Class::* ptmf)(Params...) const, Args&&... args) const
  {
    return ((&impl().ref(type<Class>{}))->*ptmf)(std::forward<Args>(args)...);
  }

  /**
   * Access a field by const reference, using an AnyField object, with a given
   * type. Throws BadAnyAccess if the type doesn't match the field's type
   * exactly, as described for ref(type<T>).
   *
   * Get the AnyField objects for a given type by calling list_fields()
   **/
  template<typename T>
  const T &ref(const AnyField &field) const
  {
    return ref(field).template ref<T>();
  }

  /**
   * Access a field by const reference, by name, with a given type.
   * Throws BadAnyAccess if the type doesn't match the field's type exactly,
   * as described for ref<T>().
   *
   * Use the ref<T>(AnyField) overload for better efficiency.
   **/
  template<typename T>
  const T &ref(const char *name) const
  {
    return ref(find_field(name)).template ref<T>();
  }

  /**
   * Access a field by const reference, by name, with a given type.
   * Throws BadAnyAccess if the type doesn't match the field's type exactly,
   * as described for ref<T>().
   *
   * Use the ref<T>(AnyField) overload for better efficiency.
   **/
  template<typename T>
  const T& ref(const std::string &name) const
  {
    return ref(find_field(name)).template ref<T>();
  }

  /**
   * Access a field by const reference, using an AnyField object, with a given
   * type. Throws BadAnyAccess if the type doesn't match the field's type
   * exactly, as described for ref(type<T>).
   *
   * Get the AnyField objects for a given type by calling list_fields()
   **/
  template<typename T>
  const T &cref(const AnyField &field) const
  {
    return cref(field).template cref<T>();
  }

  /**
   * Access a field by const reference, by name, with a given type.
   * Throws BadAnyAccess if the type doesn't match the field's type exactly,
   * as described for ref<T>().
   *
   * Use the cref<T>(AnyField) overload for better efficiency.
   **/
  template<typename T>
  const T &cref(const char *name) const
  {
    return cref(find_field(name)).template cref<T>();
  }

  /**
   * Access a field by const reference, by name, with a given type.
   * Throws BadAnyAccess if the type doesn't match the field's type exactly,
   * as described for ref<T>().
   *
   * Use the cref<T>(AnyField) overload for better efficiency.
   **/
  template<typename T>
  const T& cref(const std::string &name) const
  {
    return cref(find_field(name)).template cref<T>();
  }

  /**
   * Copy the stored value, and return it in a new Any
   **/
  ValImpl clone() const;

  /**
   * Forwards to cref(args...). Use to access fields, and access the stored
   * value by typed reference (see tags namespace)
   **/
  template<typename... Args>
  auto operator()(Args&&... args) const ->
    decltype(ref(std::forward<Args>(args)...))
  {
    return ref(std::forward<Args>(args)...);
  }

  /**
   * Returns true if the stored value supports indexing by integer. Else, false.
   **/
  bool supports_int_index() const
  {
    return handler_ && handler_->index_int;
  }

  /**
   * Access an element of the stored value, by numeric index.
   *
   * Throws BadAnyAccess if the stored type cannot be indexed this way.
   *
   * Range checking is determined by stored type, and will be used if available.
   **/
  RefImpl at(size_t i) const
  {
    using exceptions::BadAnyAccess;

    if (!supports_int_index()) {
      throw BadAnyAccess("Type in Any does not support indexing by integer");
    }
    const TypeHandlers *handler = nullptr;
    void *data = nullptr;
    handler_->index_int(i, handler, data, data_);
    return {handler, data};
  }

  /**
   * Returns true if the stored value supports indexing by string. Else, false.
   **/
  bool supports_string_index() const
  {
    return handler_ && handler_->index_str;
  }

  /**
   * Access an element of the stored value, by string index.
   *
   * Throws BadAnyAccess if the stored type cannot be indexed this way.
   *
   * Range checking is determined by stored type, and will be used if available.
   **/
  RefImpl at(const char *i) const
  {
    using exceptions::BadAnyAccess;

    if (!supports_string_index()) {
      throw BadAnyAccess("Type in Any does not support indexing by string");
    }
    const TypeHandlers *handler = nullptr;
    void *data = nullptr;
    handler_->index_str(i, handler, data, data_);
    return {handler, data};
  }

  /**
   * Access an element of the stored value, by string index.
   *
   * Throws BadAnyAccess if the stored type cannot be indexed this way.
   *
   * Range checking is determined by stored type, and will be used if available.
   **/
  RefImpl at(const std::string &i) const
  {
    return at(i.c_str());
  }

  /**
   * Access an element of the stored value, by string index.
   *
   * Throws BadAnyAccess if the stored type cannot be indexed this way.
   *
   * Range checking is determined by stored type, and will be used if available.
   **/
  template<typename T>
  enable_if_<std::is_integral<T>::value, RefImpl> operator[](T i) const
  {
    return at(i);
  }

  /**
   * Access an element of the stored value, by string index.
   *
   * Throws BadAnyAccess if the stored type cannot be indexed this way.
   *
   * Range checking is determined by stored type, and will be used if available.
   **/
  RefImpl operator[](const char *i) const
  {
    return at(i);
  }

  /**
   * Access an element of the stored value, by numeric index.
   *
   * Throws BadAnyAccess if the stored type cannot be indexed this way.
   *
   * Range checking is determined by stored type, and will be used if available.
   **/
  RefImpl operator[](const std::string &i) const
  {
    return at(i);
  }

  /**
   * Returns true if stored type supports size() as a method
   **/
  bool supports_size() const
  {
    return handler_ && handler_->size;
  }

  /**
   * Calls size() on stored type, and returns value.
   *
   * Throws BadAnyAccess if !supports_size()
   **/
  size_t size() const
  {
    using exceptions::BadAnyAccess;

    if (!supports_size()) {
      throw BadAnyAccess("Type in Any does not support size()");
    }
    size_t ret;
    handler_->size(ret, data_);
    return ret;
  }

  /**
   * Returns true if stored type can be converted to a KnowledgeRecord by
   * knowledge_cast.
   **/
  bool supports_to_record() const
  {
    return handler_ && handler_->to_record;
  }

  /**
   * Convert stored value to KnowledgeRecord using knoweldge_cast.
   *
   * Throws BadAnyAccess if !to_record()
   **/
  KnowledgeRecord to_record() const;

  /**
   * Access stored value, and return as value of given type.
   *
   * If T matches stored type exactly, will return a copy of the stored value
   * directly. Otherwise, if the type given supports conversion using
   * knowledge_cast from a KnowledgeRecord, and the stored type supports
   * to_record(), will return knowledge_cast<T>(to_record())
   **/
  template<typename T>
  T to(type<T>) const;

  /**
   * Access stored value, and return as value of given type.
   *
   * If T matches stored type exactly, will return a copy of the stored value
   * directly. Otherwise, if the type given supports conversion using
   * knowledge_cast from a KnowledgeRecord, and the stored type supports
   * `to_record()`, will return `knowledge_cast<T>(to_record())`
   **/
  template<typename T>
  T to() const { return to(type<T>{}); }

  /**
   * Access stored value, and return as integer (int64_t) value.
   *
   * If stored type is an int64_t, returns it directly. Otherwise, if stored
   * value supports `to_record()`, return `knowledge_cast<int64_t>(to_record())`
   **/
  int64_t to_integer() const { return impl().template to<int64_t>(); }

  /**
   * Access stored value, and return as double value.
   *
   * If stored type is an double, returns it directly. Otherwise, if stored
   * value supports `to_record()`, return `knowledge_cast<double>(to_record())`
   **/
  double to_double() const { return impl().template to<double>(); }

  /**
   * Access stored value, and return as std::vector<int64_t> value.
   *
   * If stored type is an a std::vector<int64_t>, returns it directly.
   * Otherwise, if stored value supports `to_record()`, return
   * `knowledge_cast<std::vector<int64_t>>(to_record())`
   **/
  std::vector<int64_t> to_integers() const
  {
    return impl().template to<std::vector<int64_t>>();
  }

  /**
   * Access stored value, and return as std::vector<double> value.
   *
   * If stored type is an a std::vector<double>, returns it directly.
   * Otherwise, if stored value supports `to_record()`, return
   * `knowledge_cast<std::vector<double>>(to_record())`
   **/
  std::vector<double> to_doubles() const
  {
    return impl().template to<std::vector<double>>();
  }

  /**
   * Access stored value, and return as std::string value.
   *
   * If stored type is an std::string, returns it directly. Otherwise, if stored
   * value supports `stream(std::ostream&)`, calls that with a
   * `std::stringstream`. Otherwise, if stored value supports `to_record()`,
   * return `knowledge_cast<std::string>(to_record())`
   **/
  std::string to_string() const {
    if (!handler_ && data_) {
      return impl().template ref<std::string>();
    }

    if (handler_->tindex == type_id<std::string>()) {
      return impl().template ref_unsafe<std::string>();
    }

    if (supports_ostream()) {
      std::stringstream s;
      stream(s);
      return s.str();
    }

    if (!supports_to_record()) {
      throw exceptions::BadAnyAccess("Type stored in Any doesn't "
          "support to_record or stream");
    }

    std::string ret;
    try_knowledge_cast(handler_->to_record(data_), ret);
    return ret;
  }

  /**
   * Access stored value, and return as std::vector<unsigned char> value.
   *
   * If stored type is an a std::vector<unsigned char>, returns it directly.
   * Otherwise, if stored value supports `to_record()`, return
   * `knowledge_cast<std::vector<unsigned char>>(to_record())`
   **/
  std::vector<unsigned char> to_file() const
  {
    return impl().template to<std::vector<unsigned char>>();
  }

  /**
   * Return true if stored type supports `stream(std::ostream&)` directly.
   * If not, falls back to calling `to_record()`, and writing that, and if not
   * that, calls `to_json()` and writes that.
   **/
  bool supports_ostream() const
  {
    return handler_ && handler_->to_ostream;
  }

  /**
   * If `supports_ostream()` is true, stream stored type to `o` using its
   * operator<< implementation.
   * If not, falls back to calling `to_record()`, and writing that, and if not
   * that, calls `to_json()` and writes that.
   **/
  std::ostream &stream(std::ostream &o) const
  {
    if (supports_ostream()) {
      return handler_->to_ostream(o, data_);
    } else if (supports_to_record()) {
      return o << to_record();
    }

    return o << to_json();
  }

  /**
   * If `supports_ostream()` is true, stream stored type to `o` using its
   * operator<< implementation.
   * If not, falls back to calling `to_record()`, and writing that, and if not
   * that, calls `to_json()` and writes that.
   **/
  friend std::ostream &operator<<(std::ostream &o, const BasicConstAny &self)
  {
    return self.stream(o);
  }

protected:
  const TypeHandlers *handler_ = nullptr;
  void *data_ = nullptr;

  struct raw_data_storage
  {
    size_t size;
    char data[1];

    static void *make(const char *data, size_t size)
    {
      auto ret = new char[size + sizeof(size)];
      raw_data_storage *sto = (raw_data_storage *)ret;
      sto->size = size;
      memcpy(&sto->data, data, size);
      return ret;
    }

    static void *clone(const void *orig)
    {
      raw_data_storage *sto = (raw_data_storage *)orig;
      return make(sto->data, sto->size);
    }
  };
};

/**
 * Provides methods common to Any and AnyRef. Use those classes, not this one
 * directly.
 **/
template<typename Impl, typename ValImpl, typename RefImpl, typename CRefImpl>
class BasicAny : public BasicConstAny<Impl, ValImpl, CRefImpl>
{
  using Base = BasicConstAny<Impl, ValImpl, CRefImpl>;
protected:
  using Base::Base;

  Impl &impl() { return *static_cast<Impl*>(this); }
  const Impl &impl() const { return *static_cast<const Impl*>(this); }

  using Base::check_type;

  friend Impl;
  friend ValImpl;
  friend RefImpl;
  friend CRefImpl;

  template<typename Impl2, typename ValImpl2, typename RefImpl2>
  friend class BasicConstAny;

  template<typename Impl2, typename ValImpl2, typename RefImpl2, typename CRefImpl2>
  friend class BasicAny;

public:
  /**
   * Create a reference type from this Any type
   **/
  RefImpl ref() const
  {
    return {this->handler_, this->data_};
  }

  using Base::cref;

  /**
   * Access the Any's stored value by reference.
   * If empty() or raw() are true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return *data_ as T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  T &ref(type<T> t) const
  {
    impl().check_type(t);
    return impl().ref_unsafe(t);
  }

  /**
   * Access the Any's stored value by reference.
   * If empty() or raw() are true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  T &ref() const
  {
    return impl().ref(type<T>{});
  }

  /**
   * Access the Any's stored value's field by reference. First, uses
   * ref<Class>(), where Class is the owner of the given pointer-to-member.
   * If empty() or raw() are true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as T&, else throw BadAnyAccess exception
   *
   * Note that Class must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * Then, it applies the pointer-to-member to the reference, to get a
   * reference to the member.
   *
   * @param ptm the pointer-to-member to call
   * @return a reference to the contained value's member
   **/
  template<typename T, typename Class>
  T &ref(T Class::* ptm) const
  {
    return (&impl().ref(type<Class>{}))->*ptm;
  }

  /**
   * Call a const method of the Any's stored value. First, uses ref<Class>(),
   * where Class is the owner of the given pointer-to-member-function.
   * If empty() or raw() are true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as T&, else throw BadAnyAccess exception
   *
   * Note that Class must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * Then, it calls the pointer-to-member-function on the reference, forwarding
   * the remaining arguments, and returns the result.
   *
   * @param ptmf the pointer-to-member-function to call
   * @param args the arguments to pass to that call
   * @return result of the method called
   **/
  template<typename T, typename Class, typename... Params, typename... Args>
  T ref(T (Class::* ptmf)(Params...) const, Args&&... args) const
  {
    return ((&impl().ref(type<Class>{}))->*ptmf)(std::forward<Args>(args)...);
  }

  /**
   * Call a method of the Any's stored value. First, uses ref<Class>(), where
   * Class is the owner of the given pointer-to-member-function.
   * If empty() or raw() are true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as T&, else throw BadAnyAccess exception
   *
   * Note that Class must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * Then, it calls the pointer-to-member-function on the reference, forwarding
   * the remaining arguments, and returns the result.
   *
   * @param ptmf the pointer-to-member-function to call
   * @param args the arguments to pass to that call
   * @return result of the method called
   **/
  template<typename T, typename Class, typename... Params, typename... Args>
  T ref(T (Class::* ptmf)(Params...), Args&&... args) const
  {
    return ((&impl().ref(type<Class>{}))->*ptmf)(std::forward<Args>(args)...);
  }

  /**
   * Reference the stored data as type T, WITHOUT any type checking. Do not
   * call this unless you are certain what type the Any contains. This does not
   * unserialize if the Any contains unserialized raw data.
   *
   * @tparam T the type to treat the stored data as
   * @return a reference of type T to the stored data
   **/
  template<typename T>
  T &ref_unsafe() const
  {
    return impl().ref_unsafe(type<T>{});
  }

  /**
   * Reference the stored data as type T, WITHOUT any type checking. Do not
   * call this unless you are certain what type the Any contains. This does not
   * unserialize if the Any contains unserialized raw data.
   *
   * @tparam T the type to treat the stored data as
   * @return a reference of type T to the stored data
   **/
  template<typename T>
  T &ref_unsafe(type<T>) const
  {
    return *reinterpret_cast<T *>(this->data_);
  }

  /**
   * Assign to the stored object. Does not replace the stored object, it
   * assigns to it using its operator=, and as such, cannot change the type
   * stored by this Any.
   *
   * If the given type matches the type stored, it will be assigned to
   * directly, forwarding the argument to avoid copying if possible.
   *
   * If not, but the type does support the from_record() operation, and the
   * given value supports knowledge_cast(), the given value will be converted
   * to a KnowledgeRecord, and from_record called.
   *
   * Otherwise, a BadAnyAccess exception is thrown.
   **/
  template<typename T>
  void assign(T &&t) const;

  using Base::find_field;
  using Base::list_fields;

  /**
   * Access a field by reference, using an AnyField object.
   *
   * Get the AnyField objects for a given type by calling list_fields()
   **/
  RefImpl ref(const AnyField &field) const
  {
    auto ret = Base::ref(field);
    return {ret.handler_, ret.data_};
  }

  /**
   * Acess a field by reference, by name.
   *
   * Use the ref(AnyField) overload for better efficiency.
   **/
  RefImpl ref(const char *name) const
  {
    return ref(find_field(name));
  }

  /**
   * Acess a field by reference, by name.
   *
   * Use the ref(AnyField) overload for better efficiency.
   **/
  RefImpl ref(const std::string &name) const
  {
    return ref(find_field(name));
  }

  /**
   * Access a field by reference, using an AnyField object, with a given type.
   * Throws BadAnyAccess if the type doesn't match the field's type exactly,
   * as described for ref(type<T>).
   *
   * Get the AnyField objects for a given type by calling list_fields()
   **/
  template<typename T>
  T &ref(const AnyField &field) const
  {
    return ref(field).template ref<T>();
  }

  /**
   * Access a field by reference, by name, with a given type.
   * Throws BadAnyAccess if the type doesn't match the field's type exactly,
   * as described for ref<T>().
   *
   * Use the ref<T>(AnyField) overload for better efficiency.
   **/
  template<typename T>
  T &ref(const char *name) const
  {
    return ref(find_field(name)).template ref<T>();
  }

  /**
   * Access a field by reference, by name, with a given type.
   * Throws BadAnyAccess if the type doesn't match the field's type exactly,
   * as described for ref<T>().
   *
   * Use the ref<T>(AnyField) overload for better efficiency.
   **/
  template<typename T>
  T &ref(const std::string &name) const
  {
    return ref(find_field(name)).template ref<T>();
  }

  /**
   * Forwards to ref(args...). Use to access fields, and access the stored
   * value by typed reference (see tags namespace)
   **/
  template<typename... Args>
  auto operator()(Args&&... args) const ->
    decltype(ref(std::forward<Args>(args)...))
  {
    return ref(std::forward<Args>(args)...);
  }

  /**
   * Access an element of the stored value, by numeric index.
   *
   * Throws BadAnyAccess if the stored type cannot be indexed this way.
   *
   * Range checking is determined by stored type, and will be used if available.
   **/
  RefImpl at(size_t i) const
  {
    auto r = Base::at(i);
    return {r.handler_, r.data_};
  }

  /**
   * Access an element of the stored value, by string index.
   *
   * Throws BadAnyAccess if the stored type cannot be indexed this way.
   *
   * Range checking is determined by stored type, and will be used if available.
   **/
  RefImpl at(const char *i) const
  {
    auto r = Base::at(i);
    return {r.handler_, r.data_};
  }

  /**
   * Access an element of the stored value, by string index.
   *
   * Throws BadAnyAccess if the stored type cannot be indexed this way.
   *
   * Range checking is determined by stored type, and will be used if available.
   **/
  RefImpl at(const std::string &i) const
  {
    return at(i.c_str());
  }

  /**
   * Access an element of the stored value, by numeric index.
   *
   * Throws BadAnyAccess if the stored type cannot be indexed this way.
   *
   * Range checking is determined by stored type, and will be used if available.
   **/
  template<typename T>
  enable_if_<std::is_integral<T>::value, RefImpl> operator[](T i) const
  {
    return at(i);
  }

  /**
   * Access an element of the stored value, by string index.
   *
   * Throws BadAnyAccess if the stored type cannot be indexed this way.
   *
   * Range checking is determined by stored type, and will be used if available.
   **/
  RefImpl operator[](const char *i) const
  {
    return at(i);
  }

  /**
   * Access an element of the stored value, by string index.
   *
   * Throws BadAnyAccess if the stored type cannot be indexed this way.
   *
   * Range checking is determined by stored type, and will be used if available.
   **/
  RefImpl operator[](const std::string &i) const
  {
    return at(i);
  }

  /**
   * Returns true if the stored value supports from_record()
   * In general, this is any type of stored_value for which this is well-formed:
   *
   * ```
   * knowledge_cast(rec, stored_value)
   * ```
   **/
  bool supports_from_record() const
  {
    return this->handler_ && this->handler_->from_record;
  }

  /**
   * Set the stored value from the given KnowledgeRecord. This will not change
   * the stored type; it converts the record to the stored type, if possible.
   * Throws a BadAnyAccess if raw() or empty() are true, or if the stored type
   * doesn't support conversion from a KnowledgeRecord
   **/
  void from_record(const KnowledgeRecord &rec) const;
};

/**
 * A general purpose type which can refer to any type which is supported by Any.
 * The referenced object need not actually be contained within an Any. This
 * reference can read from the object, and convert it to other types, but cannot
 * modify it.
 **/
class ConstAnyRef : public BasicConstAny<ConstAnyRef, Any, ConstAnyRef>
{
  using Base = BasicConstAny<ConstAnyRef, Any, ConstAnyRef>;

  template<typename Impl2, typename ValImpl2, typename RefImpl2>
  friend class ::madara::knowledge::BasicConstAny;

  template<typename Impl2, typename ValImpl2, typename RefImpl2, typename CRefImpl2>
  friend class ::madara::knowledge::BasicAny;
protected:
  using Base::Base;

public:
  /**
   * Construct an empty ConstAnyRef. Call target() before calling any other
   * method besides empty().
   **/
  ConstAnyRef() = default;

  /**
   * Create a const version of the given AnyRef
   **/
  ConstAnyRef(const AnyRef &other);

  /**
   * Construct as a reference to the given Any
   **/
  ConstAnyRef(const Any &other);

  /**
   * Change the object this ConstAnyRef refers to. It must be a type which could
   * be stored in an Any, but the object need actually be stored in an Any
   **/
  template<typename T>
  void target(T &t)
  {
    handler_ = &get_type_handler<T>();
    data_ = (void*)&t;
  }

  template<typename Impl2, typename Base2>
  friend class BasicOwningAny;

  friend class Any;
};

/**
 * A general purpose type which can refer to any type which is supported by Any.
 * The referenced object need not actually be contained within an Any. This
 * reference may be used to modify the referenced object, but cannot change the
 * type of the referenced object.
 **/
class AnyRef : public BasicAny<AnyRef, Any, AnyRef, ConstAnyRef>
{
  using Base = BasicAny<AnyRef, Any, AnyRef, ConstAnyRef>;

  template<typename Impl2, typename ValImpl2, typename RefImpl2>
  friend class ::madara::knowledge::BasicConstAny;

  template<typename Impl2, typename ValImpl2, typename RefImpl2, typename CRefImpl2>
  friend class ::madara::knowledge::BasicAny;
protected:
  using Base::Base;

public:
  /**
   * Construct an empty ConstAnyRef. Call target() before calling any other
   * method besides empty().
   **/
  AnyRef() = default;

  /**
   * Construct as a reference to the given Any
   **/
  AnyRef(const Any &other);

  /**
   * Forwards to assign(T), as long as T is not an Any or AnyRef, modifying
   * the object this refers to.
   **/
  template<typename T>
  auto operator=(T &&val) ->
    enable_if_<!is_convertible<decay_<T>, ConstAnyRef>()>
  {
    return this->assign(std::forward<T>(val));
  }

  /**
   * Change the object this ConstAnyRef refers to. It must be a type which could
   * be stored in an Any, but the object need actually be stored in an Any
   **/
  template<typename T>
  void target(T &t)
  {
    handler_ = &get_type_handler<T>();
    data_ = (void*)&t;
  }

  template<typename Impl2, typename Base2>
  friend class BasicOwningAny;

  friend class Any;
};

inline ConstAnyRef::ConstAnyRef(const AnyRef &other)
  : Base(other.handler_, other.data_) {}

/**
 * Class which defines methods common to ConstAny and Any. Use those classes
 * instead of this class directly.
 **/
template<typename Impl, typename Base>
class BasicOwningAny : public Base
{
public:
  /**
   * Default constructor. Creates an empty Any.
   **/
  BasicOwningAny() = default;

  /**
   * Copy constructor. Will clone any data stored inside.
   **/
  BasicOwningAny(const BasicOwningAny &other)
    : Base(other.handler_,
        other.data_ ?
        (other.handler_ ?
          other.handler_->clone(other.data_) :
          Base::raw_data_storage::clone(other.data_)) : nullptr) {}

  /**
   * Construct from a ConstAnyRef. Will clone the data it refers to.
   **/
  BasicOwningAny(const ConstAnyRef &other)
    : Base(other.handler_,
        other.data_ ?
        (other.handler_ ?
          other.handler_->clone(other.data_) :
          Base::raw_data_storage::clone(other.data_)) : nullptr) {}

  /**
   * Construct from a AnyRef. Will clone the data it refers to.
   **/
  BasicOwningAny(const AnyRef &other)
    : BasicOwningAny(ConstAnyRef(other)) {}

  /**
   * Copy assignment operator. Will clone any data stored inside.
   **/
  BasicOwningAny &operator=(const BasicOwningAny &other)
  {
    void *data = other.data_ ?
      (other.handler_ ?
        other.handler_->clone(other.data_) :
        Base::raw_data_storage::clone(other.data_)) : nullptr;

    clear();
    this->handler_ = other.handler_;
    this->data_ = data;
    return *this;
  }

  /**
   * Move constructor. Other Any will be left empty.
   **/
  BasicOwningAny(BasicOwningAny &&other) noexcept :
    Base(take_ptr(other.handler_),
      take_ptr(other.data_)) {}

  /**
   * Move assignment operator. Other Any will be left empty.
   **/
  BasicOwningAny &operator=(BasicOwningAny &&other) noexcept
  {
    if (this != &other) {
      using std::swap;
      swap(this->data_, other.data_);
      swap(this->handler_, other.handler_);
    }
    return *this;
  }

  /**
   * Destructor. Deletes the stored data.
   **/
  ~BasicOwningAny() noexcept
  {
    clear();
  }

  /**
   * Clear and free the data inside this Any. Safe to call if already empty.
   **/
  void clear() noexcept
  {
    if (!this->data_) {
      return;
    }
    if (this->handler_)
    {
      this->handler_->destruct((void*)this->data_);
      this->handler_ = nullptr;
    } else {
      delete [] (char*)this->data_;
    }
    this->data_ = nullptr;
  }

  /**
   * Construct from any compatible type. The argument will be moved into
   * the new Any if it supports it, and the argument is an rvalue reference.
   * Otherwise, it will be copied.
   **/
  template<typename T>
  explicit BasicOwningAny(T &&t, enable_if_<
    !is_type_tag<T>() &&
    !is_convertible<T, ConstAnyRef>(), int> = 0)
    : Base(&get_type_handler(type<decay_<T>>{}),
        reinterpret_cast<void*>(
            new decay_<T>(std::forward<T>(t))))
  {}

  /**
   * Construct any compatible type in-place. The first argument is a
   * madara::type struct which provides the type as a template parameter,
   * but otherwise holds no data. The remaining arguments are forwarded to
   * that type's constructor arguments.
   *
   * Example:
   * @code{.cpp}
   * using madara::knowledge;
   *
   * // constructs a vector in-place with 42 copies of a string
   * Any a(type<std::vector<std::string>>{}, 42, "Don't panic");
   * @endcode
   **/
  template<typename T, typename... Args>
  explicit BasicOwningAny(type<T> t, Args&&... args)
    : Base(&get_type_handler(t),
        reinterpret_cast<void*>(new T(std::forward<Args>(args)...))) {}

  /**
   * Construct any compatible type in-place. The first argument is a
   * madara::type struct which provides the type as a template parameter,
   * but otherwise holds no data. The second argument is an initializer list
   * that will be passed along to that type's constructor.
   *
   * Example:
   * @code{.cpp}
   * using madara::knowledge;
   *
   * // constructs a vector in-place with several strings
   * Any a(type<std::vector<std::string>>{}, {"mostly", "harmless"});
   * @endcode
   **/
  template<typename T, typename I>
  explicit BasicOwningAny(type<T> t, std::initializer_list<I> init)
    : BasicOwningAny(t, init.begin(), init.end()) {}

  /**
   * Construct with serialized data, for lazy deserialization when first needed.
   *
   * Note that this lazy deserialization is not fully type-safe, and might not
   * throw an exception if the wrong type is used. The result may be garbled
   * data, but shouldn't segfault or trample other data.
   *
   * The first parameter is a type tag, which is available by-value from the
   * global `madara::knowledge::raw_data`.
   *
   * @param data a pointer to the serialized data to copy into this Any
   * @param size the amount of data to copy
   **/
  explicit BasicOwningAny(tags::raw_data_t, const char *data, size_t size)
    : Base(nullptr, Base::raw_data_storage::make(data, size)) {}

  /**
   * Construct any compatible type in-place. The first argument is a
   * madara::type struct which provides the type as a template parameter,
   * but otherwise holds no data. The remaining arguments are forwarded to
   * that type's constructor arguments.
   *
   * Example:
   * @code{.cpp}
   * using madara::knowledge;
   *
   * Any a;
   * // constructs a vector in-place with 42 copies of a string
   * a.emplace(type<std::vector<std::string>>{}, 42, "Don't panic");
   * @endcode
   **/
  template<typename T, typename... Args>
  T &emplace(type<T>, Args&&... args)
  {
    std::unique_ptr<T> ptr(new T(std::forward<Args>(args)...));
    const auto &handler = get_type_handler(type<decay_<T>>{});

    clear();

    this->handler_ = &handler;
    T *ret = ptr.release();
    this->data_ = reinterpret_cast<void*>(ret);
    return *ret;
  }

  /**
   * Construct any compatible type in-place. The first argument is a
   * madara::type struct which provides the type as a template parameter,
   * but otherwise holds no data. The second argument is an initializer list
   * that will be passed along to that type's constructor.
   *
   * Example:
   * @code{.cpp}
   * using madara::knowledge;
   *
   * Any a;
   * // constructs a vector in-place with several strings
   * a.emplace(type<std::vector<std::string>>{}, {"mostly", "harmless"});
   * @endcode
   **/
  template<typename T, typename I>
  T &emplace(type<T> t, std::initializer_list<I> init)
  {
    return emplace(t, init.begin(), init.end());
  }

  /**
   * Construct any compatible type in-place. The arguments are forwarded to
   * the given type's constructor arguments.
   *
   * Example:
   * @code{.cpp}
   * using madara::knowledge;
   *
   * Any a;
   * // constructs a vector in-place with 42 copies of a string
   * a.emplace<std::vector<std::string>>(42, "Don't panic");
   * @endcode
   **/
  template<typename T, typename... Args>
  T &emplace(Args&&... args)
  {
    return emplace(type<T>{}, std::forward<Args>(args)...);
  }

  /**
   * Construct any compatible type in-place. The first argument is a
   * madara::type struct which provides the type as a template parameter,
   * but otherwise holds no data. The second argument is an initializer list
   * that will be passed along to that type's constructor.
   *
   * Example:
   * @code{.cpp}
   * using madara::knowledge;
   *
   * Any a;
   * // constructs a vector in-place with several strings
   * a.emplace<std::vector<std::string>>({"mostly", "harmless"});
   * @endcode
   **/
  template<typename T, typename I>
  T &emplace(std::initializer_list<I> init)
  {
    return emplace(type<T>{}, init);
  }

  /**
   * Set from any compatible type. The argument will be moved into
   * this Any if it supports it, and the argument is an rvalue reference.
   * Otherwise, it will be copied.
   **/
  template<typename T>
  T &set(T &&t)
  {
    return emplace(type<decay_<T>>{},
           std::forward<T>(t));
  }

  /**
   * Store serialized data, for lazy deserialization when first needed.
   *
   * Note that this lazy deserialization is not fully type-safe, and might not
   * throw an exception if the wrong type is used. The result may be garbled
   * data, but shouldn't segfault or trample other data.
   *
   * @param data a pointer to the serialized data to copy into this Any
   * @param size the amount of data to copy
   **/
  void set_raw(const char *data, size_t size)
  {
    clear();
    this->data_ = Base::raw_data_storage::make(data, size);
  }

  /**
   * Store serialized data, for lazy deserialization when first needed.
   *
   * Note that this lazy deserialization is not fully type-safe, and might not
   * throw an exception if the wrong type is used. The result may be garbled
   * data, but shouldn't segfault or trample other data.
   *
   * The first parameter is a type tag, which is available by-value from the
   * global `madara::knowledge::raw_data`.
   *
   * @param data a pointer to the serialized data to copy into this Any
   * @param size the amount of data to copy
   **/
  void set(tags::raw_data_t, const char *data, size_t size)
  {
    set_raw(data, size);
  }

  /**
   * Unserialize the given type from the given character array, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  size_t unserialize(type<T> t, const char *data, size_t size)
  {
    namespace bio = boost::iostreams;

    bio::array_source input_source(data, size);
    bio::stream<bio::array_source> input_stream(input_source);

    auto pos = input_stream.tellg();
    unserialize(t, input_stream);
    auto len = input_stream.tellg() - pos;

    return len;
  }

  /**
   * Unserialize the given type from the given character array, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  size_t unserialize(const char *data, size_t size)
  {
    return unserialize(type<T>{}, data, size);
  }

  /**
   * Unserialize the given type from the given input stream, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(type<T> t, std::istream &stream)
  {
    madara_iarchive archive(stream);

    unserialize(t, archive);
  }

  /**
   * Unserialize the given type from the given input stream, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(std::istream &stream)
  {
    unserialize(type<T>{}, stream);
  }

  /**
   * Unserialize the given type from the given madara_iarchive, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(type<T> t, madara_iarchive &archive)
  {
    const TypeHandlers &handler = get_type_handler(t);
    std::unique_ptr<T> ptr(new T{});

    handler.load(archive, (void*)ptr.get());

    clear();
    this->data_ = reinterpret_cast<void*>(ptr.release());
    this->handler_ = &handler;
  }

  /**
   * Unserialize the given type from the given madara_iarchive, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(madara_iarchive &archive)
  {
    unserialize(type<T>{}, archive);
  }

  /**
   * Unserialize the given type from the given archive, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(type<T> t, json_iarchive &archive)
  {
    const TypeHandlers &handler = get_type_handler(t);
    std::unique_ptr<T> ptr(new T{});

    handler.load_json(archive, (void*)ptr.get());

    clear();
    this->data_ = reinterpret_cast<void*>(ptr.release());
    this->handler_ = &handler;
  }

  /**
   * Unserialize the given type from the given archive, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(json_iarchive &archive)
  {
    unserialize(type<T>{}, archive);
  }

  /**
   * Access the Any's stored value by reference.
   * If empty() is true, throw BadAnyAccess exception; else,
   * If raw() is true, try to deserialize using T, and store deserialized
   * data if successful, else throw BadAnyAccess exception.
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return *data_ as T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  T &ref(type<T> t)
  {
    if (!this->data_) {
      throw exceptions::BadAnyAccess("ref() called on empty Any");
    } else if (!this->handler_) {
      typename Base::raw_data_storage *sto =
        (typename Base::raw_data_storage *)this->data_;
      unserialize(t, sto->data, sto->size);
    } else if (type_id<T>() != this->handler_->tindex) {
      throw exceptions::BadAnyAccess(t, this->handler_->tindex);
    }
    return this->impl().ref_unsafe(t);
  }

  /**
   * Access the Any's stored value by reference.
   * If empty() is true, throw BadAnyAccess exception; else,
   * If raw() is true, try to deserialize using T, and store deserialized
   * data if successful, else throw BadAnyAccess exception.
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  T &ref()
  {
    return ref(type<T>{});
  }

  using Base::ref;

  /**
   * Take the Any's stored value, leaving it empty. On moveable types, this
   * will not copy the value.
   *
   * If empty() is true, throw BadAnyAccess exception; else,
   * If raw() is true, try to deserialize using T, and store deserialized
   * data if successful, else throw BadAnyAccess exception.
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * take and return the data, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return the formerly contained value
   **/
  template<typename T>
  T take(type<T> t)
  {
    T ret(std::move(ref(t)));
    clear();
    return ret;
  }

  /**
   * Take the Any's stored value, leaving it empty. On moveable types, this
   * will not copy the value.
   *
   * If empty() is true, throw BadAnyAccess exception; else,
   * If raw() is true, try to deserialize using T, and store deserialized
   * data if successful, else throw BadAnyAccess exception.
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * take and return the data, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return the formerly contained value
   **/
  template<typename T>
  T take()
  {
    return take(type<T>{});
  }

private:
  template<typename T>
  static T *take_ptr(T *&in)
  {
    T *ret = in;
    in = nullptr;
    return ret;
  }

  template<typename T>
  static const T *take_ptr(const T *&in)
  {
    const T *ret = in;
    in = nullptr;
    return ret;
  }
};

/**
 * A general purpose type which can store any type which is:
 *
 *  * Default constructible
 *  * Copy constructible
 *  * Serializable by Boost.Serialization, or implements the for_each_field
 *    free function.
 *
 * This class is used by KnowledgeRecord and KnowledgeBase to store (nearly)
 * arbitrary types. This class owns the value it stores, and will automatically
 * destruct the object when the Any gets destructed, like std::unique_ptr.
 *
 * It is similar in principle to Boost.Any (and C++17 std::any), but
 * incorporates serialization support, and a different interface.
 *
 * This class cannot modify the stored object, but it can replace what object
 * is stored, which will destroy the previously stored object.
 * A `const ConstAny` cannot change at all once constructed, until the
 * `const ConstAny` itself is destructed.
 **/
class ConstAny : public BasicOwningAny<ConstAny,
  BasicConstAny<ConstAny, Any, ConstAnyRef>>
{
  using Base = BasicOwningAny<ConstAny,
    BasicConstAny<ConstAny, Any, ConstAnyRef>>;

public:
  using Base::Base;
};

/**
 * A general purpose type which can store any type which is:
 *
 *  * Default constructible
 *  * Copy constructible
 *  * Serializable by Boost.Serialization, or implements the for_each_field
 *    free function.
 *
 * This class is used by KnowledgeRecord and KnowledgeBase to store (nearly)
 * arbitrary types. This class owns the value it stores, and will automatically
 * destruct the object when the Any gets destructed, like std::unique_ptr.
 *
 * It is similar in principle to Boost.Any (and C++17 std::any), but
 * incorporates serialization support, and a different interface.
 *
 * Note that, a `const Any` can modify the object constructed, it simply
 * cannot change which object it holds. Use the ConstAny class to hold a const
 * object which cannot be modifid, and `const ConstAny` to hold a const
 * object that will not be destructed until the `const ConstAny` is destructed.
 **/
class Any : public BasicOwningAny<Any,
  BasicAny<Any, Any, AnyRef, ConstAnyRef>>
{
  using Base = BasicOwningAny<Any,
    BasicAny<Any, Any, AnyRef, ConstAnyRef>>;

public:
  using Base::Base;

};

template<typename Impl, typename ValImpl, typename RefImpl>
inline ValImpl BasicConstAny<Impl, ValImpl, RefImpl>::clone() const
{
  return this->impl();
}

inline ConstAnyRef::ConstAnyRef(const Any &other)
  : Base(other.handler_, other.data_) {}

inline AnyRef::AnyRef(const Any &other)
  : Base(other.handler_, other.data_) {}

} // namespace knowledge

namespace utility { inline namespace core {
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
// and at(int). Takes precedence over operator[] version above if available.
template<typename T,
  enable_if_<knowledge::supports_int_at_index<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::index_int_fn_type
  get_type_handler_index_int(type<T>, overload_priority<4>)
{
  return [](size_t index,
      const knowledge::TypeHandlers *&handler,
      void *&out_ptr,
      void *ptr)
    {
      T &val = *static_cast<T *>(ptr);
      using I = decltype(val.at(index));
      handler = &knowledge::get_type_handler<decay_<I>>();
      out_ptr = &val.at(index);
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

// Implement get_type_handler_str_index for type supporting for_each_field
// and at(const char *) or operator[](const std::string &).
// Takes precedence over operator[] version above if available.
template<typename T,
  enable_if_<knowledge::supports_str_at_index<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::index_str_fn_type
  get_type_handler_index_str(type<T>, overload_priority<4>)
{
  return [](const char *index,
      const knowledge::TypeHandlers *&handler,
      void *&out_ptr,
      void *ptr)
    {
      T &val = *static_cast<T *>(ptr);
      using I = decltype(val.at(index));
      handler = &knowledge::get_type_handler<decay_<I>>();
      out_ptr = &val.at(index);
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

} // namespace madara

namespace cereal
{

// Implement Cereal library serialization for types supporting for_each_field
template<typename Archive, typename T>
auto serialize(Archive &ar, T &&val) ->
  ::madara::enable_if_<::madara::knowledge::supports_for_each_field<T>::value>
{
  for_each_field(::madara::knowledge::do_serialize<Archive>{&ar},
      std::forward<T>(val));
}

}

#endif  // MADARA_KNOWLEDGE_ANY_H_
