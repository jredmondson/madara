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
class RefAny;
class CRefAny;

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
  const type_index &type_index() const;
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

  typedef void (*index_int_fn_type)(size_t index,
      const TypeHandlers *&out_handler, void *&out_ptr, void *);
  index_int_fn_type index_int;

  typedef void (*size_fn_type)(size_t &out, void *);
  size_fn_type size;
};

inline const TypeHandlers &AnyField::parent() const
{
  return *parent_;
}

inline const TypeHandlers &AnyField::handler() const
{
  return *handler_;
}

inline const type_index &AnyField::type_index() const
{
  return handler_->tindex;
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
constexpr TypeHandlers::size_fn_type
  get_type_handler_size(type<T>, overload_priority_weakest)
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
      //get_type_handler_find_field(t, select_overload()),
      get_type_handler_list_fields(t, select_overload()),
      get_type_handler_get_field(t, select_overload()),
      get_type_handler_index_int(t, select_overload()),
      get_type_handler_size(t, select_overload()),
      //get_type_handler_set_field(t, select_overload()),
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

MADARA_MAKE_VAL_SUPPORT_TEST(size_member, x, (0UL == x.size()));
MADARA_MAKE_VAL_SUPPORT_TEST(int_index, x, (x[0UL] = x[0UL]));
MADARA_MAKE_VAL_SUPPORT_TEST(str_index, x, (x[""] = x[""]));
MADARA_MAKE_VAL_SUPPORT_TEST(int_at_index, x, (x.at(0UL) = x.at(0UL)));
MADARA_MAKE_VAL_SUPPORT_TEST(str_at_index, x, (x.at("") = x.at("")));

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

constexpr struct json_t {} json;

}

template<typename T, typename = void>
struct is_type_tag_impl : std::false_type {};

template<typename T>
struct is_type_tag_impl<type<T>> : std::true_type {};

template<typename T>
constexpr bool is_type_tag() { return is_type_tag_impl<decay_<T>>::value; }

template<typename Impl, typename RefImpl>
class BasicConstAny
{
protected:
  BasicConstAny() = default;

  BasicConstAny(const TypeHandlers *handler, void *data)
    : handler_(handler), data_(data) {}

  Impl &impl() { return *static_cast<Impl*>(this); }
  const Impl &impl() const { return *static_cast<const Impl*>(this); }

  friend Impl;
  friend RefImpl;

  template<typename Impl2, typename RefImpl2>
  friend class BasicConstAny;

  template<typename Impl2, typename RefImpl2, typename CRefImpl2>
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
  const RefImpl cref() const
  {
    return RefAny(handler_, data_);
  }

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

  bool supports_fields() const
  {
    return handler_ && handler_->list_fields && handler_->get_field;
  }

  const std::vector<AnyField> &list_fields() const
  {
    if (!supports_fields()) {
      static std::vector<AnyField> empty;
      return empty;
    }
    return handler_->list_fields(data_);
  }

  AnyField find_field(const char *name) const
  {
    using exceptions::BadAnyAccess;

    if (!name) {
      throw BadAnyAccess("Null name pointer passed to find_field");
    }

    /*
    auto func = handler_->find_field;
    if (!func) {
      throw BadAnyAccess("Type in Any does not support find_field");
    }

    AnyField ret;
    func(name, ret, data_);
    if (ret.name() == nullptr) {
      throw BadAnyAccess(std::string("Type in Any does not contain field \"") +
          name + "\"");
    }*/
    const auto &fields = list_fields();
    auto found = std::equal_range(fields.begin(), fields.end(), name,
        compare_any_fields_by_name());
    if (found.first == fields.end() || found.first == found.second) {
      throw BadAnyAccess(std::string("Type in Any does not contain field \"") +
          name + "\"");
    }
    /*
    if (found.first + 1 != found.second) {
      throw BadAnyAccess(std::string("Type in Any contains multiple "
          "fields named \"") + name + "\"");
    }*/
    return *found.first;
  }

  AnyField find_field(AnyField field) const
  {
    using exceptions::BadAnyAccess;

    if (!handler_ || field.parent().tindex != handler_->tindex) {
      throw BadAnyAccess(std::string("Type in Any does not contain field \"") +
          field.name() + "\"");
    }
    return field;
  }

  AnyField find_field(const std::string &name) const
  {
    return find_field(name.c_str());
  }

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

  RefImpl ref(const char *name) const
  {
    return ref(find_field(name));
  }

  RefImpl ref(const std::string &name) const
  {
    return ref(find_field(name));
  }

  RefImpl cref(const AnyField &field) const
  {
    return ref(field);
  }

  RefImpl cref(const char *name) const
  {
    return ref(find_field(name));
  }

  RefImpl cref(const std::string &name) const
  {
    return ref(find_field(name));
  }

  template<typename T>
  const T &ref(const AnyField &field) const
  {
    return ref(field).template ref<T>();
  }

  template<typename T>
  const T &ref(const char *name) const
  {
    return ref(find_field(name)).template ref<T>();
  }

  template<typename T>
  const T& ref(const std::string &name) const
  {
    return ref(find_field(name)).template ref<T>();
  }

  template<typename T>
  const T &cref(const AnyField &field) const
  {
    return cref(field).template cref<T>();
  }

  template<typename T>
  const T &cref(const char *name) const
  {
    return cref(find_field(name)).template cref<T>();
  }

  template<typename T>
  const T& cref(const std::string &name) const
  {
    return cref(find_field(name)).template cref<T>();
  }

  Any clone() const;

  std::string operator()(tags::json_t)
  {
    return to_json();
  }

  class proxy
  {
    RefImpl target;

    proxy(RefImpl t) : target(t) {}
    proxy(const proxy &t) = delete;
    proxy(proxy &&t) = default;

  public:
    RefImpl *operator->()
    {
      return &target;
    }

    template<typename T>
    auto operator()(T &&t) ->
      decltype(target(std::forward<T>(t)))
    {
      return target(std::forward<T>(t));
    }

    template<typename T>
    auto operator[](T &&t) ->
      decltype(target[std::forward<T>(t)])
    {
      return target[std::forward<T>(t)];
    }

    template<typename Impl2, typename RefImpl2>
    friend class BasicConstAny;
  };

  Impl *operator->() const { return &this->impl(); }

  template<typename T,
    enable_if_<!is_type_tag<T>() &&
      !is_same_decayed<T, tags::json_t>(), int> = 0>
  proxy operator()(T &&t) const
  {
    return proxy(ref(std::forward<T>(t)));
  }

  template<typename T>
  const T &operator()(type<T> t) const
  {
    return cref(t);
  }

  bool supports_int_index() const
  {
    return handler_ && handler_->index_int;
  }

  RefImpl operator[](size_t i) const
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

  bool supports_size() const
  {
    return handler_ && handler_->size;
  }

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
 * A general purpose type which can store any type which is:
 *
 *  * Default constructible
 *  * Copy constructible
 *  * Serializable by Boost.Serialization, or implements the for_each_field
 *    free function.
 *
 * This class is used by KnowledgeRecord and KnowledgeBase to store (nearly)
 * arbitrary types.
 *
 * It is similar in principle to Boost.Any (and C++17 std::any), but
 * incorporates serialization support, and a different interface.
 **/
template<typename Impl, typename RefImpl, typename CRefImpl>
class BasicAny : public BasicConstAny<Impl, CRefImpl>
{
  using Base = BasicConstAny<Impl, CRefImpl>;
protected:
  using Base::Base;

  Impl &impl() { return *static_cast<Impl*>(this); }
  const Impl &impl() const { return *static_cast<const Impl*>(this); }

  using Base::check_type;

  friend Impl;
  friend RefImpl;
  friend CRefImpl;

  template<typename Impl2, typename RefImpl2>
  friend class BasicConstAny;

  template<typename Impl2, typename RefImpl2, typename CRefImpl2>
  friend class BasicAny;

public:
  RefImpl ref()
  {
    return {this->handler_, this->data_};
  }

  using Base::ref;
  using Base::cref;

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
    impl().check_type(t);
    return impl().ref_unsafe(t);
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
    return impl().ref(type<T>{});
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
  T &ref_unsafe()
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
  T &ref_unsafe(type<T>)
  {
    return *reinterpret_cast<T *>(this->data_);
  }

  template<typename T>
  decay_<T> &assign(T &&t)
  {
    return ref<decay_<T>>() = std::forward<T>(t);
  }

  using Base::ref_unsafe;
  using Base::find_field;
  using Base::list_fields;

  RefImpl ref(const AnyField &field) const
  {
    auto ret = Base::ref(field);
    return {ret.handler_, ret.data_};
  }

  RefImpl ref(const char *name) const
  {
    return ref(find_field(name));
  }

  RefImpl ref(const std::string &name) const
  {
    return ref(find_field(name));
  }

  template<typename T>
  T &ref(const AnyField &field) const
  {
    return ref(field).template ref<T>();
  }

  template<typename T>
  T &ref(const char *name) const
  {
    return ref(find_field(name)).template ref<T>();
  }

  template<typename T>
  T &ref(const std::string &name) const
  {
    return ref(find_field(name)).template ref<T>();
  }

  void set_field(const AnyField &field, Impl any)
  {
    ref(field) = std::move(any);
  }

  void set_field(const char *name, Impl any)
  {
    return set_field(find_field(name), std::move(any));
  }

  void set_field(const std::string &name, Impl any)
  {
    return set_field(find_field(name), std::move(any));
  }

  template<typename N, typename T>
  auto set_field(N &&name, T &&val) ->
    enable_if_<!is_convertible<T, Impl>(),
      decltype(set_field(find_field(std::forward<N>(name)),
              Impl(std::forward<T>(val))), void())>
  {
    set_field(find_field(std::forward<N>(name)), Impl(std::forward<T>(val)));
  }

  class proxy
  {
    RefImpl target;

    proxy(RefImpl t) : target(t) {}
    proxy(const proxy &t) = delete;
    proxy(proxy &&t) = default;

  public:
    template<typename T>
    decay_<T> &operator=(T &&val)
    {
      return target.assign(std::forward<T>(val));
    }

    RefImpl *operator->()
    {
      return &target;
    }

    template<typename T>
    auto operator()(T &&t) ->
      decltype(target(std::forward<T>(t)))
    {
      return target(std::forward<T>(t));
    }

    template<typename T>
    auto operator[](T &&t) ->
      decltype(target[std::forward<T>(t)])
    {
      return target[std::forward<T>(t)];
    }

    template<typename Impl2, typename RefImpl2, typename CRefImpl2>
    friend class BasicAny;
  };

  Impl *operator->() { return &this->impl(); }

  template<typename T,
    enable_if_<!is_type_tag<T>() &&
      !is_same_decayed<T, tags::json_t>(), int> = 0>
  proxy operator()(T &&t)
  {
    return proxy(ref(std::forward<T>(t)));
  }

  template<typename T>
  T &operator()(type<T> t)
  {
    return ref(t);
  }

  RefImpl operator[](size_t i) const
  {
    auto r = Base::operator[](i);
    return {r.handler_, r.data_};
  }

  using Base::operator();
};

class CRefAny : public BasicConstAny<CRefAny, CRefAny>
{
  using Base = BasicConstAny<CRefAny, CRefAny>;
protected:
  using Base::Base;

public:
  CRefAny(const RefAny &other);

  CRefAny(const Any &other);

  template<typename T>
  void set(T &t)
  {
    handler_ = &get_type_handler<T>();
    data_ = (void*)&t;
  }

  friend class Any;
};

class RefAny : public BasicAny<RefAny, RefAny, CRefAny>
{
  using Base = BasicAny<RefAny, RefAny, CRefAny>;
protected:
  using Base::Base;

public:
  RefAny(const Any &other);

  template<typename T>
  void set(T &t)
  {
    handler_ = &get_type_handler<T>();
    data_ = (void*)&t;
  }

  friend class Any;
};

inline CRefAny::CRefAny(const RefAny &other)
  : Base(other.handler_, other.data_) {}

class Any : public BasicAny<Any, RefAny, CRefAny>
{
  using Base = BasicAny<Any, RefAny, CRefAny>;
public:
  /**
   * Default constructor. Creates an empty Any.
   **/
  Any() = default;

  /**
   * Copy constructor. Will clone any data stored inside.
   **/
  Any(const Any &other)
    : Base(other.handler_,
        other.data_ ?
        (other.handler_ ?
          other.handler_->clone(other.data_) :
          raw_data_storage::clone(other.data_)) : nullptr) {}

  Any(const CRefAny &other)
    : Base(other.handler_,
        other.data_ ?
        (other.handler_ ?
          other.handler_->clone(other.data_) :
          raw_data_storage::clone(other.data_)) : nullptr) {}

  Any(const RefAny &other)
    : Any(CRefAny(other)) {}

  /**
   * Copy assignment operator. Will clone any data stored inside.
   **/
  Any &operator=(const Any &other)
  {
    void *data = other.data_ ?
      (other.handler_ ?
        other.handler_->clone(other.data_) :
        raw_data_storage::clone(other.data_)) : nullptr;

    clear();
    handler_ = other.handler_;
    data_ = data;
    return *this;
  }

  /**
   * Move constructor. Other Any will be left empty.
   **/
  Any(Any &&other) noexcept :
    Base(take_ptr(other.handler_),
      take_ptr(other.data_)) {}

  /**
   * Move assignment operator. Other Any will be left empty.
   **/
  Any &operator=(Any &&other) noexcept
  {
    if (this != &other) {
      using std::swap;
      swap(data_, other.data_);
      swap(handler_, other.handler_);
    }
    return *this;
  }

  /**
   * Destructor. Deletes the stored data.
   **/
  ~Any() noexcept
  {
    clear();
  }

  /**
   * Clear and free the data inside this Any. Safe to call if already empty.
   **/
  void clear() noexcept
  {
    if (!data_) {
      return;
    }
    if (handler_)
    {
      handler_->destruct((void*)data_);
      handler_ = nullptr;
    } else {
      delete [] (char*)data_;
    }
    data_ = nullptr;
  }

  /**
   * Construct from any compatible type. The argument will be moved into
   * the new Any if it supports it, and the argument is an rvalue reference.
   * Otherwise, it will be copied.
   **/
  template<typename T>
  explicit Any(T &&t, enable_if_<
    !is_type_tag<T>() &&
    !is_convertible<T, Any>(), int> = 0)
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
  explicit Any(type<T> t, Args&&... args)
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
  explicit Any(type<T> t, std::initializer_list<I> init)
    : Any(t, init.begin(), init.end()) {}

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
  explicit Any(tags::raw_data_t, const char *data, size_t size)
    : Base(nullptr, raw_data_storage::make(data, size)) {}

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
    if (!data_) {
      throw exceptions::BadAnyAccess("ref() called on empty Any");
    } else if (!handler_) {
      raw_data_storage *sto = (raw_data_storage *)data_;
      unserialize(t, sto->data, sto->size);
    } else if (type_id<T>() != handler_->tindex) {
      throw exceptions::BadAnyAccess(t, handler_->tindex);
    }
    return impl().ref_unsafe(t);
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

    handler_ = &handler;
    T *ret = ptr.release();
    data_ = reinterpret_cast<void*>(ret);
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
    data_ = raw_data_storage::make(data, size);
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
    data_ = reinterpret_cast<void*>(ptr.release());
    handler_ = &handler;
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
    data_ = reinterpret_cast<void*>(ptr.release());
    handler_ = &handler;
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

template<typename Impl, typename RefImpl>
inline Any BasicConstAny<Impl, RefImpl>::clone() const
{
  return *this;
}

inline CRefAny::CRefAny(const Any &other)
  : Base(other.handler_, other.data_) {}

inline RefAny::RefAny(const Any &other)
  : Base(other.handler_, other.data_) {}

} // namespace knowledge

namespace utility { inline namespace core {
// Must define below in same namespace as type<> struct for ADL

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

template<typename T>
inline std::vector<knowledge::AnyField> get_fields(T &val)
{
  std::vector<knowledge::AnyField> ret;
  for_each_field(do_list_fields{&knowledge::get_type_handler<T>(),
      &ret, 0}, val);
  std::sort(ret.begin(), ret.end(), knowledge::compare_any_fields_by_name());
  return ret;
}

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

template<typename T,
  enable_if_<knowledge::supports_for_each_field<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::get_field_fn_type
  get_type_handler_get_field(type<T>, overload_priority<8>)
{
  return [](const knowledge::AnyField &field,
      const knowledge::TypeHandlers *&handler,
      void *&out_ptr,
      void *ptr) {
      T &val = *static_cast<T *>(ptr);
      for_each_field(do_get_field{&field, &handler, &out_ptr, 0}, val);
    };
}

template<typename T,
  enable_if_<knowledge::supports_int_index<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::index_int_fn_type
  get_type_handler_index_int(type<T>, overload_priority<8>)
{
  return [](size_t index,
      const knowledge::TypeHandlers *&handler,
      void *&out_ptr,
      void *ptr) {
      T &val = *static_cast<T *>(ptr);
      using I = decltype(val[index]);
      handler = &knowledge::get_type_handler<decay_<I>>();
      out_ptr = &val[index];
    };
}

template<typename T,
  enable_if_<knowledge::supports_int_at_index<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::index_int_fn_type
  get_type_handler_index_int(type<T>, overload_priority<4>)
{
  return [](size_t index,
      const knowledge::TypeHandlers *&handler,
      void *&out_ptr,
      void *ptr) {
      T &val = *static_cast<T *>(ptr);
      using I = decltype(val.at(index));
      handler = &knowledge::get_type_handler<decay_<I>>();
      out_ptr = &val.at(index);
    };
}

template<typename T,
  enable_if_<knowledge::supports_size_member<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::size_fn_type
  get_type_handler_size(type<T>, overload_priority<4>)
{
  return [](size_t &out, void *ptr) {
      T &val = *static_cast<T *>(ptr);
      out = val.size();
    };
}

} } // namespace utility::core

} // namespace madara

namespace cereal
{

template<typename Archive, typename T>
auto serialize(Archive &ar, T &&val) ->
  ::madara::enable_if_<::madara::knowledge::supports_for_each_field<T>::value>
{
  for_each_field(::madara::knowledge::do_serialize<Archive>{&ar},
      std::forward<T>(val));
}

}

#endif  // MADARA_KNOWLEDGE_ANY_H_
