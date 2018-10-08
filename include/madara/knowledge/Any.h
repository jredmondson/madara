#ifndef MADARA_KNOWLEDGE_ANY_H_
#define MADARA_KNOWLEDGE_ANY_H_

/**
 * @file Any.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the Any and ConstAny classes
 **/

#include <memory>
#include <sstream>
#include <map>
#include <functional>
#include <type_traits>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

#include "madara/utility/SupportTest.h"
#include "madara/MadaraExport.h"
#include "madara/utility/StdInt.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/IntTypes.h"
#include "madara/exceptions/BadAnyAccess.h"
#include "TypeHandlers.h"
#include "ConstAnyRef.h"
#include "AnyRef.h"

namespace madara
{
namespace knowledge
{
namespace tags
{
struct from_tag_t
{
};
constexpr from_tag_t from_tag;
}

/**
 * Class which defines methods common to ConstAny and Any. Use those classes
 * instead of this class directly.
 **/
template<typename Impl, typename BaseImpl>
class BasicOwningAny : public BaseImpl, public AnyRegistry
{
  using Base = BaseImpl;

protected:
  using Base::Base;

public:
  /**
   * Default constructor. Creates an empty Any.
   **/
  BasicOwningAny() = default;

  /**
   * Copy constructor. Will clone any data stored inside.
   **/
  BasicOwningAny(const BasicOwningAny& other)
    : Base(other.handler_, other.data_ && other.handler_
                               ? other.handler_->clone(other.data_)
                               : nullptr)
  {
  }

  /**
   * Construct from a ConstAnyRef. Will clone the data it refers to.
   **/
  BasicOwningAny(const ConstAnyRef& other)
    : Base(other.handler_, other.data_ && other.handler_
                               ? other.handler_->clone(other.data_)
                               : nullptr)
  {
  }

  /**
   * Construct from a AnyRef. Will clone the data it refers to.
   **/
  BasicOwningAny(const AnyRef& other) : BasicOwningAny(ConstAnyRef(other)) {}

  /**
   * Copy assignment operator. Will clone any data stored inside.
   **/
  BasicOwningAny& operator=(const BasicOwningAny& other)
  {
    void* data = other.handler_ && other.data_
                     ? other.handler_->clone(other.data_)
                     : nullptr;

    clear();
    this->handler_ = other.handler_;
    this->data_ = data;
    return *this;
  }

  /**
   * Move constructor. Other Any will be left empty.
   **/
  BasicOwningAny(BasicOwningAny&& other) noexcept
    : Base(take_ptr(other.handler_), take_ptr(other.data_))
  {
  }

  /**
   * Move assignment operator. Other Any will be left empty.
   **/
  BasicOwningAny& operator=(BasicOwningAny&& other) noexcept
  {
    if (this != &other)
    {
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
    if (!this->data_)
    {
      return;
    }
    if (this->handler_)
    {
      this->handler_->destruct((void*)this->data_);
      this->handler_ = nullptr;
    }
    else
    {
      delete[](char*) this->data_;
    }
    this->data_ = nullptr;
  }

  /**
   * Construct from any compatible type. The argument will be moved into
   * the new Any if it supports it, and the argument is an rvalue reference.
   * Otherwise, it will be copied.
   **/
  template<typename T,
      typename std::enable_if<
          !is_type_tag<T>() && !is_convertible<T, ConstAnyRef>(), int>::type =
          0>
  explicit BasicOwningAny(T&& t)
    : Base(&get_type_handler(type<decay_<T>>{}),
          reinterpret_cast<void*>(new decay_<T>(std::forward<T>(t))))
  {
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
   * // constructs a vector in-place with 42 copies of a string
   * Any a(type<std::vector<std::string>>{}, 42, "Don't panic");
   * @endcode
   **/
  template<typename T, typename... Args>
  explicit BasicOwningAny(type<T> t, Args&&... args)
    : Base(&get_type_handler(t),
          reinterpret_cast<void*>(new T(std::forward<Args>(args)...)))
  {
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
   * // constructs a vector in-place with several strings
   * Any a(type<std::vector<std::string>>{}, {"mostly", "harmless"});
   * @endcode
   **/
  template<typename T, typename I>
  explicit BasicOwningAny(type<T> t, std::initializer_list<I> init)
    : BasicOwningAny(t, init.begin(), init.end())
  {
  }

  BasicOwningAny(tags::from_tag_t, const char* tag);

  BasicOwningAny(tags::from_tag_t, const std::string& tag);

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
  T& emplace(type<T>, Args&&... args)
  {
    std::unique_ptr<T> ptr(new T(std::forward<Args>(args)...));
    const auto& handler = get_type_handler(type<decay_<T>>{});

    clear();

    this->handler_ = &handler;
    T* ret = ptr.release();
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
  T& emplace(type<T> t, std::initializer_list<I> init)
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
  T& emplace(Args&&... args)
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
  T& emplace(std::initializer_list<I> init)
  {
    return emplace(type<T>{}, init);
  }

  void emplace(const char* tag);
  void emplace(const std::string& tag);

  /**
   * Set from any compatible type. The argument will be moved into
   * this Any if it supports it, and the argument is an rvalue reference.
   * Otherwise, it will be copied.
   **/
  template<typename T>
  T& set(T&& t)
  {
    return emplace(type<decay_<T>>{}, std::forward<T>(t));
  }

  /**
   * Unserialize the given type from the given character array, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  size_t unserialize(type<T> t, const char* data, size_t size)
  {
    const TypeHandlers& handler = get_type_handler(t);
    std::unique_ptr<T> ptr(new T{});

    handler.load(data, size, (void*)ptr.get(), AnyRegistry::get_type_name<T>());

    clear();
    this->data_ = reinterpret_cast<void*>(ptr.release());
    this->handler_ = &handler;

    return size;
  }

  /**
   * Unserialize the given type from the given character array, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  size_t unserialize(const char* data, size_t size)
  {
    return unserialize(type<T>{}, data, size);
  }

#if 0
  /**
   * Unserialize the given type from the given input stream, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(type<T> t, std::istream &stream)
  {
    const TypeHandlers &handler = get_type_handler(t);
    std::unique_ptr<T> ptr(new T{});

    handler.load(stream, (void*)ptr.get(), AnyRegistry::get_type_name<T>());

    clear();
    this->data_ = reinterpret_cast<void*>(ptr.release());
    this->handler_ = &handler;
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
#endif

  void unserialize(const char* type, const char* data, size_t size);

  /**
   * Unserialize the given type from the given character array, and store into
   * this Any, using saved type tag to determine type. This operation provides
   * the strong exception-guarantee: if an exception is throw during
   * unserialization, this Any will not be modified.
   *
   * Use with data serialized by tagged_serialize()
   **/
  size_t tagged_unserialize(const char* data, size_t size)
  {
    namespace bio = boost::iostreams;

    bio::array_source input_source(data, size);
    bio::stream<bio::array_source> input_stream(input_source);

    auto pos = input_stream.tellg();
    madara_iarchive archive(input_stream);
    std::string tag;
    archive >> tag;
    auto len = input_stream.tellg() - pos;

    unserialize(tag.c_str(), data + len, size - len);

    return len;
  }

#if 0
  /**
   * Unserialize the given type from the given input stream, and store into
   * this Any, using saved type tag to determine type. This operation provides
   * the strong exception-guarantee: if an exception is throw during
   * unserialization, this Any will not be modified.
   *
   * Use with data serialized by tagged_serialize()
   **/
  void tagged_unserialize(std::istream &stream)
  {
    madara_iarchive archive(stream);
    std::string tag;
    archive >> tag;
    unserialize(tag.c_str(), stream);
  }
#endif

  /**
   * Unserialize the given type from JSON from the given stream, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize_json(type<T> t, std::istream& i)
  {
    const TypeHandlers& handler = get_type_handler(t);

    using exceptions::BadAnyAccess;
    if (!handler.load_json)
    {
      throw BadAnyAccess(std::string("Type ") +
                         AnyRegistry::get_type_name<T>() +
                         " does not support unserialize_json");
    }

    std::unique_ptr<T> ptr(new T{});

    handler.load_json(i, (void*)ptr.get(), AnyRegistry::get_type_name<T>());

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
  void unserialize_json(std::istream& i)
  {
    unserialize(type<T>{}, i);
  }

  /**
   * Unserialize the given type from the given archive, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  void unserialize_json(const char* type, std::istream& i);

  void unserialize_json(const char* type, const char* json, size_t size)
  {
    namespace bio = boost::iostreams;

    bio::array_source input_source(json, size);
    bio::stream<bio::array_source> input_stream(input_source);

    unserialize_json(type, input_stream);
  }

  void unserialize_json(const char* type, const char* json)
  {
    unserialize_json(type, json, std::strlen(json));
  }

  void unserialize_json(const char* type, const std::string& json)
  {
    unserialize_json(type, json.c_str(), json.size());
  }

protected:
  template<typename T>
  static T* take_ptr(T*& in)
  {
    T* ret = in;
    in = nullptr;
    return ret;
  }

  template<typename T>
  static const T* take_ptr(const T*& in)
  {
    const T* ret = in;
    in = nullptr;
    return ret;
  }

  template<typename Impl2, typename Base2>
  friend class ::madara::knowledge::BasicOwningAny;
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
class ConstAny
  : public BasicOwningAny<ConstAny, BasicConstAny<ConstAny, Any, ConstAnyRef>>
{
  using Base =
      BasicOwningAny<ConstAny, BasicConstAny<ConstAny, Any, ConstAnyRef>>;

public:
  using Base::Base;

  static ConstAny construct(const char* name)
  {
    return construct_const(name);
  }

  ConstAny() = default;
  ConstAny(const Any& other);
  ConstAny(Any&& other) noexcept;

  friend class Any;
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
class Any : public BasicOwningAny<Any, BasicAny<Any, Any, AnyRef, ConstAnyRef>>
{
  using Base = BasicOwningAny<Any, BasicAny<Any, Any, AnyRef, ConstAnyRef>>;

public:
  using Base::Base;

  Any() = default;
  Any(const ConstAny& other);
  Any(ConstAny&& other) noexcept;

  friend class ConstAny;
  friend class AnyRegistry;
};

template<typename T>
inline bool AnyRegistry::register_type(const char* name)
{
  auto& ptr = *get_type_name_ptr<T>();
  if (ptr == nullptr)
  {
    ptr = name;
  }

  return register_type_impl(name, &get_type_handler<T>());
}

template<typename Impl, typename Base>
inline void BasicOwningAny<Impl, Base>::unserialize(
    const char* type, const char* data, size_t size)
{
  Any any(construct(type));

  any.handler_->load(data, size, any.data_, type);

  using std::swap;
  swap(this->handler_, any.handler_);
  swap(this->data_, any.data_);
}

template<typename Impl, typename Base>
inline void BasicOwningAny<Impl, Base>::unserialize_json(
    const char* type, std::istream& i)
{
  Any any(construct(type));

  using exceptions::BadAnyAccess;
  if (!any.handler_->load_json)
  {
    throw BadAnyAccess(
        std::string("Type ") + type + " does not support unserialize_json");
  }

  any.handler_->load_json(i, any.data_, type);

  using std::swap;
  swap(this->handler_, any.handler_);
  swap(this->data_, any.data_);
}

inline ConstAnyRef::ConstAnyRef(const Any& other)
  : Base(other.handler_, other.data_)
{
}

inline AnyRef::AnyRef(const Any& other) : Base(other.handler_, other.data_) {}

inline ConstAnyRef::ConstAnyRef(const ConstAny& other)
  : Base(other.handler_, other.data_)
{
}

inline ConstAny::ConstAny(const Any& other) : ConstAny(other.cref()) {}

inline ConstAny::ConstAny(Any&& other) noexcept
  : Base(take_ptr(other.handler_), take_ptr(other.data_))
{
}

inline Any::Any(const ConstAny& other) : Any(other.cref()) {}

inline Any::Any(ConstAny&& other) noexcept
  : Base(take_ptr(other.handler_), take_ptr(other.data_))
{
}

template<typename Impl, typename Base>
inline BasicOwningAny<Impl, Base>::BasicOwningAny(
    tags::from_tag_t, const char* tag)
  : BasicOwningAny(construct(tag))
{
}

template<typename Impl, typename Base>
inline BasicOwningAny<Impl, Base>::BasicOwningAny(
    tags::from_tag_t, const std::string& tag)
  : BasicOwningAny(tag.c_str())
{
}

template<typename Impl, typename Base>
inline void BasicOwningAny<Impl, Base>::emplace(const char* tag)
{
  *this = construct(tag);
}

template<typename Impl, typename Base>
inline void BasicOwningAny<Impl, Base>::emplace(const std::string& tag)
{
  emplace(tag.c_str());
}

}  // namespace knowledge

}  // namespace madara

#include "DefaultTypeHandlers.h"

#endif  // MADARA_KNOWLEDGE_ANY_H_
