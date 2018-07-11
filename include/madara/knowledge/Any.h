#ifndef MADARA_KNOWLEDGE_ANY_H_
#define MADARA_KNOWLEDGE_ANY_H_

/**
 * @file Any.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the Any class
 **/

#include <memory>
#include <type_traits>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/polymorphic_oarchive.hpp>
#include <boost/archive/polymorphic_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/stack.hpp>
#include <boost/serialization/queue.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/bitset.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/base_object.hpp>

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

using madara_oarchive = boost::archive::binary_oarchive;
using madara_iarchive = boost::archive::binary_iarchive;

using boost::archive::polymorphic_oarchive;
using boost::archive::polymorphic_iarchive;

#ifdef MADARA_USE_BOOST_TYPE_INDEX
using type_index = boost::typeindex::type_index;
using boost::typeindex::type_id;
#else
using type_index = std::type_index;

template<typename T>
constexpr type_index type_id()
{
  return type_index(typeid(T));
}
#endif

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

  typedef void (*save_polymorphic_fn_type)(polymorphic_oarchive &, const void *);
  save_polymorphic_fn_type save_polymorphic;

  typedef void (*load_polymorphic_fn_type)(polymorphic_iarchive &, void *);
  load_polymorphic_fn_type load_polymorphic;
};

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

template<typename T>
constexpr TypeHandlers::clone_fn_type get_type_handler_clone(type<T>,
    overload_priority_weakest)
{
  return [](void *ptr) -> void * {
      T *t_ptr = static_cast<T *>(ptr);
      return new T(*t_ptr);
    };
}

template<typename T>
constexpr TypeHandlers::save_fn_type get_type_handler_save(type<T>,
    overload_priority_weakest)
{
  return [](madara_oarchive &archive, const void *ptr) {
      const T &val = *static_cast<const T *>(ptr);
      archive << val;
    };
}

template<typename T>
constexpr TypeHandlers::load_fn_type get_type_handler_load(type<T>,
    overload_priority_weakest)
{
  return [](madara_iarchive &archive, void *ptr) {
      T &val = *static_cast<T *>(ptr);
      archive >> val;
    };
}

template<typename T>
constexpr TypeHandlers::save_polymorphic_fn_type
  get_type_handler_save_polymorphic(type<T>, overload_priority_weakest)
{
  return [](polymorphic_oarchive &archive, const void *ptr) {
      const T &val = *static_cast<const T *>(ptr);
      archive << val;
    };
}

template<typename T>
constexpr TypeHandlers::load_polymorphic_fn_type
  get_type_handler_load_polymorphic(type<T>, overload_priority_weakest)
{
  return [](polymorphic_iarchive &archive, void *ptr) {
      T &val = *static_cast<T *>(ptr);
      archive >> val;
    };
}

} // namespace knowledge

MADARA_MAKE_VAL_SUPPORT_TEST(forEachField, x,
    forEachField(::madara::ignore_all<>{}, x));

template<typename Archive>
struct do_serialize
{
  Archive *ar;

  template<typename T>
  void operator()(const char *, T &val)
  {
    *ar & val;
  }
};

template<typename T>
struct for_each_serialization_wrapper_type
{
  T *ptr;
};

template<typename T,
  enable_if_<supports_forEachField<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::save_fn_type get_type_handler_save(type<T>,
    overload_priority<8>)
{
  return [](knowledge::madara_oarchive &archive, const void *ptr) {
      const T &val = *static_cast<const T *>(ptr);
      for_each_serialization_wrapper_type<T> wrapper{const_cast<T *>(&val)};
      archive << wrapper;
    };
}

template<typename T,
  enable_if_<supports_forEachField<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::load_fn_type get_type_handler_load(type<T>,
    overload_priority<8>)
{
  return [](knowledge::madara_iarchive &archive, void *ptr) {
      T &val = *static_cast<T *>(ptr);
      for_each_serialization_wrapper_type<T> wrapper{&val};
      archive >> wrapper;
    };
}

template<typename T,
  enable_if_<supports_forEachField<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::save_polymorphic_fn_type
  get_type_handler_save_polymorphic(type<T>, overload_priority<8>)
{
  return [](knowledge::polymorphic_oarchive &archive, const void *ptr) {
      const T &val = *static_cast<const T *>(ptr);
      for_each_serialization_wrapper_type<T> wrapper{const_cast<T *>(&val)};
      archive << wrapper;
    };
}

template<typename T,
  enable_if_<supports_forEachField<T>::value, int> = 0>
constexpr knowledge::TypeHandlers::load_polymorphic_fn_type
  get_type_handler_load_polymorphic(type<T>, overload_priority<8>)
{
  return [](knowledge::polymorphic_iarchive &archive, void *ptr) {
      T &val = *static_cast<T *>(ptr);
      for_each_serialization_wrapper_type<T> wrapper{&val};
      archive >> wrapper;
    };
}

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

template<typename T>
inline const TypeHandlers &get_type_handler(type<T> t)
{
  static const TypeHandlers handler = {
      type_id<T>(),
      get_type_handler_destruct(t, select_overload()),
      get_type_handler_clone(t, select_overload()),
      get_type_handler_save(type<T>{}, select_overload()),
      get_type_handler_load(type<T>{}, select_overload()),
      get_type_handler_save_polymorphic(t, select_overload()),
      get_type_handler_load_polymorphic(t, select_overload()),
    };
  return handler;
}

template<typename T>
inline const TypeHandlers &get_type_handler()
{
  return get_type_handler(type<T>{});
}

constexpr struct raw_data_t {} raw_data;

class Any
{
public:
  Any() = default;

  Any(const Any &other)
    : handler_(other.handler_),
      data_(other.data_ ?
        (handler_ ?
          handler_->clone(other.data_) :
          raw_data_storage::clone(other.data_)) : nullptr) {}

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

  Any(Any &&other) noexcept :
    handler_(take_ptr(other.handler_)),
    data_(take_ptr(other.data_)) {}

  Any &operator=(Any &&other) noexcept
  {
    if (this != &other) {
      using std::swap;
      swap(data_, other.data_);
      swap(handler_, other.handler_);
    }
    return *this;
  }

  ~Any() noexcept
  {
    clear();
  }

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

  template<typename T, enable_if_<!is_same_decayed<T, Any>(), int> = 0>
  explicit Any(T &&t)
    : handler_(&get_type_handler(type<decay_<T>>{})),
      data_(reinterpret_cast<void*>(
            new decay_<T>(std::forward<T>(t))))
  {}

  template<typename T, typename... Args>
  explicit Any(type<T> t, Args&&... args)
    : handler_(&get_type_handler(t)),
      data_(reinterpret_cast<void*>(new T(std::forward<Args>(args)...))) {}

  template<typename T, typename I>
  explicit Any(type<T> t, std::initializer_list<I> init)
    : Any(t, init.begin(), init.end()) {}

  /// Store serialized data, for lazy deserialization by ref() later
  explicit Any(raw_data_t, const char *data, size_t size)
    : data_(raw_data_storage::make(data, size)) {}

  bool empty() const
  {
    return data_ == nullptr;
  }

  bool raw() const
  {
    return data_ != nullptr && handler_ == nullptr;
  }

  template<typename T>
  T &get_unsafe()
  {
    return get_unsafe(type<T>{});
  }

  size_t serialize(char *data, size_t size) const
  {
    namespace bio = boost::iostreams;

    bio::array_sink output_sink(data, size);
    bio::stream<bio::array_sink> output_stream(output_sink);

    return serialize(output_stream);
  }

  size_t serialize(std::vector<char> &vec) const
  {
    namespace bio = boost::iostreams;

    vec.clear();
    auto output_sink = bio::back_inserter(vec);
    bio::stream<decltype(output_sink)> output_stream(output_sink);

    return serialize(output_stream);
  }

  size_t serialize(std::ostream &stream) const
  {
    madara_oarchive archive(stream);

    auto pos = stream.tellp();
    serialize(archive);
    auto len = stream.tellp() - pos;

    return len;
  }

  void serialize(madara_oarchive &archive) const
  {
    if (handler_) {
      handler_->save(archive, data_);
    } else {
      raw_data_storage *sto = (raw_data_storage *)data_;
      archive.save_binary(sto->data, sto->size);
    }
  }

  void serialize(polymorphic_oarchive &archive) const
  {
    if (handler_) {
      handler_->save_polymorphic(archive, data_);
    } else {
      raw_data_storage *sto = (raw_data_storage *)data_;
      archive.save_binary(sto->data, sto->size);
    }
  }

  template<typename T>
  size_t unserialize(type<T> t, const char *data, size_t size)
  {
    namespace bio = boost::iostreams;

    bio::array_source input_source(data, size);
    bio::stream<bio::array_source> input_stream(input_source);

    return unserialize(t, input_stream);
  }

  template<typename T>
  size_t unserialize(const char *data, size_t size)
  {
    return unserialize(type<T>{}, data, size);
  }

  template<typename T>
  size_t unserialize(type<T> t, std::istream &stream)
  {
    madara_iarchive archive(stream);

    auto pos = stream.tellg();
    unserialize(t, archive);
    auto len = stream.tellg() - pos;

    return len;
  }

  template<typename T>
  size_t unserialize(std::istream &stream)
  {
    return unserialize(type<T>{}, stream);
  }

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

  template<typename T>
  void unserialize(madara_iarchive &archive)
  {
    unserialize(type<T>{}, archive);
  }

  template<typename T>
  void unserialize(type<T> t, polymorphic_iarchive &archive)
  {
    const TypeHandlers &handler = get_type_handler(t);
    std::unique_ptr<T> ptr(new T{});

    handler.load_polymorphic(archive, (void*)ptr.ref());

    clear();
    data_ = reinterpret_cast<void*>(ptr.release());
    handler_ = &handler;
  }

  template<typename T>
  void unserialize(polymorphic_iarchive &archive)
  {
    unserialize(type<T>{}, archive);
  }

  /// Get contents as type T, with no checking
  template<typename T>
  T &get_unsafe(type<T>)
  {
    return *reinterpret_cast<T *>(data_);
  }

  /// Get contents as type T, with no checking
  template<typename T>
  const T &get_unsafe(type<T>) const
  {
    return *reinterpret_cast<const T *>(data_);
  }

  /// If data_ is null, throw exception; else,
  /// If handler_ is null, try to deserialize using T, store corresponding
  /// handler and deserialized data if successful, else throw exception
  /// Otherwise, check type_id<T> matches handler_->tindex; if so,
  /// return *data_ as T&, else throw exception
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
    return get_unsafe(t);
  }

  template<typename T>
  T &ref()
  {
    return ref(type<T>{});
  }

  template<typename T>
  T take(type<T> t)
  {
    T ret(std::move(ref(t)));
    clear();
    return ret;
  }

  template<typename T>
  T take()
  {
    return take(type<T>{});
  }

  template<typename T>
  const T &ref(type<T> t) const
  {
    if (!data_) {
      throw exceptions::BadAnyAccess("ref() called on empty Any");
    } else if (!handler_) {
      throw exceptions::BadAnyAccess("ref() called on const Any with raw data");
    } else if (type_id<T>() != handler_->tindex) {
      throw exceptions::BadAnyAccess(t, handler_->tindex);
    }
    return get_unsafe(t);
  }

  template<typename T>
  const T &ref() const
  {
    return ref(type<T>{});
  }

  template<typename T>
  const T &cref(type<T> t) const
  {
    return ref(t);
  }

  template<typename T>
  const T &cref() const
  {
    return ref(type<T>{});
  }

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

  template<typename T, typename I>
  T &emplace(type<T> t, std::initializer_list<I> init)
  {
    return emplace(t, init.begin(), init.end());
  }

  template<typename T, typename... Args>
  T &emplace(Args&&... args)
  {
    return emplace(type<T>{}, std::forward<Args>(args)...);
  }

  template<typename T, typename I>
  T &emplace(std::initializer_list<I> init)
  {
    return emplace(type<T>{}, init);
  }

  template<typename T, typename U>
  T &store(type<T> t, U &&u)
  {
    return emplace(t, std::forward<U>(u));
  }

  template<typename T, typename U>
  T &store(U &&u)
  {
    return store(type<T>{}, std::forward<U>(u));
  }

  template<typename T>
  T &store(T &&t)
  {
    return store(type<decay_<T>>{},
           std::forward<T>(t));
  }

  /// Store serialized data, for lazy deserialization by ref() later
  void store_raw(const char *data, size_t size)
  {
    clear();
    data_ = raw_data_storage::make(data, size);
  }

  /// Store serialized data, for lazy deserialization by ref() later
  void store(raw_data_t, const char *data, size_t size)
  {
    store_raw(data, size);
  }

private:
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

} } // namespace madara::knowledge

namespace boost { namespace serialization {

  template<typename Archive, typename T>
  void serialize(Archive &ar,
      ::madara::for_each_serialization_wrapper_type<T> &t, const unsigned int)
  {
    forEachField(::madara::do_serialize<Archive>{&ar}, *t.ptr);
  }

}}
#endif  // MADARA_KNOWLEDGE_ANY_H_
