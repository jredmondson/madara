#ifndef MADARA_CAPNOBJECT_ANY_H_
#define MADARA_CAPNOBJECT_ANY_H_

/**
 * @file CapnObject.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the CapnObject class and related support
 **/

#include <memory>
#include <sstream>
#include <functional>
#include <type_traits>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif

#include "capnp/message.h"
#include "capnp/serialize.h"
#include "capnp/dynamic.h"
#include "capnp/schema.h"
#include "capnp/compat/json.h"
#include "kj/io.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC__

#include "madara/utility/SupportTest.h"
#include "madara/MadaraExport.h"
#include "madara/logger/GlobalLogger.h"
#include "TypeHandlers.h"

namespace madara { namespace knowledge {

/**
 * Helper template to ensure given type is no-throw destructible. If an
 * exception is thrown during destruction, it will be logged, but otherwise
 * ignored, so use this with care.
 **/
template<typename T>
struct NoThrowDestruct : T
{
  using T::T;

  ~NoThrowDestruct() noexcept try {
    return;
  } catch (const std::exception &e) {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ERROR,
      "NoThrowDestruct" \
      " unexpected exception thrown: %s\n", e.what());
    return;
  } catch (...) {
    madara_logger_ptr_log (logger::global_logger.get (), logger::LOG_ERROR,
      "NoThrowDestruct" \
      " unexpected unknown exception thrown\n");
    return;
  }
};

/**
 * Base class for various Cap'n proto types.
 **/
class BaseCapnObject
{
public:
  /**
   * Size of held buffer containing a Cap'n proto type.
   **/
  size_t size() const { return size_; }

  /**
   * Pointer to buffer containing a Cap'n proto type.
   **/
  const char *data() const { return data_.get(); }

  /**
   * Returns the tag of the this object.
   **/
  virtual const char *tag() const = 0;

  virtual ~BaseCapnObject() = default;

protected:
  char *data() { return data_.get(); }

  std::shared_ptr<char> mk_copy(const char *data, size_t size, size_t extra)
  {
    return std::shared_ptr<char>(
        (char *)memcpy((char *)operator new(size + extra), data, size));
  }

  std::shared_ptr<char> read_from(std::istream &i, size_t size, size_t extra)
  {
    std::shared_ptr<char> ret((char *)operator new(size + extra));
    i.read(ret.get(), size);
    return ret;
  }

  template<typename T, typename... Args>
  typename T::Reader reader(type<T>, Args&&... args) const
  {
    return reader_->template getRoot<T>(std::forward<Args>(args)...);
  }

  BaseCapnObject() = default;

  BaseCapnObject(const char *data, size_t size, size_t extra = 0,
      bool init_reader = true)
    : data_(mk_copy(data, size, extra)), size_(size),
      reader_(init_reader ? mk_reader() : nullptr) {}

  BaseCapnObject(std::istream &i, size_t size, size_t extra = 0,
      bool init_reader = true)
    : data_(read_from(i, size, extra)), size_(size),
      reader_(init_reader ? mk_reader() : nullptr) {}

  std::shared_ptr<char> data_;
  size_t size_;

  using reader_type = NoThrowDestruct<capnp::FlatArrayMessageReader>;
  using reader_sptr = std::shared_ptr<reader_type>;

  mutable reader_sptr reader_;

  reader_sptr mk_reader() const
  {
    return std::make_shared<reader_type>(
        kj::ArrayPtr<const capnp::word>((const capnp::word *)data_.get(),
          size_ / sizeof(capnp::word)));
  }
};

/**
 * Holds a Cap'n proto object of unknown schema, and unregistered tag.
 **/
class GenericCapnObject : public BaseCapnObject
{
  using Base = BaseCapnObject;
public:
  /**
   * Default constructor, holding no data. Object in this state cannot be used
   * for any purpose except to be assigned to.
   **/
  GenericCapnObject() = default;

  /**
   * Construct with given tag and char buffer. Contents of pointed to tag and
   * buffer will be copied into this new object.
   **/
  GenericCapnObject(const char *tag, const char *d, size_t s)
    : Base(d, s, std::strlen(tag) + 1, false),
      tag_((const char *)std::strcpy(data() + size(), tag)) {}

  /**
   * Construct with given tag and bytes from input stream. Contents of pointed
   * to tag will be copied into this new object.
   **/
  GenericCapnObject(const char *tag, std::istream &i, size_t s)
    : Base(i, s, std::strlen(tag) + 1, false),
      tag_((const char *)std::strcpy(data() + size(), tag)) {}

  /**
   * Construct with given tag and contents of given Cap'n Proto message builder.
   * Pointed to tag and data of builder will be copied into this new object.
   **/
  template<typename Builder>
  GenericCapnObject(const char * tag, Builder &&builder,
      decltype(capnp::writeMessage(
          std::declval<kj::VectorOutputStream&>(),
          std::declval<decay_<Builder>&>()), 0) = 0
    ) {
    kj::VectorOutputStream vec;
    capnp::writeMessage(vec, std::forward<Builder>(builder));

    auto data = vec.getArray();
    *this = GenericCapnObject(tag, data.asChars().begin(), data.size());
  }

  /**
   * Interpret held data as given Cap'n Proto type, and return the appropriate
   * reader.
   **/
  template<typename T>
  typename T::Reader reader(type<T> t) const
  {
    init_reader();
    return Base::reader(t);
  }

  /**
   * Interpret held data as given Cap'n Proto type, and return the appropriate
   * reader.
   **/
  template<typename T>
  typename T::Reader reader() const
  {
    return reader(type<T>{});
  }

  /**
   * Interpret held data with given Cap'n Proto schema, and return a
   * DyanmicStruct::Reader pointing to that data.
   **/
  capnp::DynamicStruct::Reader reader(capnp::StructSchema schema) const
  {
    init_reader();
    return Base::reader(type<capnp::DynamicStruct>{}, schema);
  }

  /**
   * Returns the tag stored in this object.
   **/
  const char *tag() const override { return tag_; }

private:
  void init_reader() const
  {
    if (!reader_) {
      reader_ = mk_reader();
    }
  }

  const char *tag_;
};

/**
 * Stores a specific statically-known Cap'n Proto type. Type must be registered
 * with the Any system; use `Any::register_type<CapnObject<T>>("Tag")`
 **/
template<typename T>
class CapnObject : public BaseCapnObject
{
  using Base = BaseCapnObject;

public:
  /**
   * Default constructor, holding no data. Object in this state cannot be used
   * for any purpose except to be assigned to or to call tag().
   **/
  CapnObject() = default;

  /**
   * Construct with given char buffer. Contents of buffer will be copied into
   * this new object.
   **/
  CapnObject(const char *d, size_t s) : Base(d, s) {}

  /**
   * Construct with data from given input stream.
   **/
  CapnObject(std::istream &i, size_t s) : Base(i, s) {}

  /**
   * Construct from given Cap'n Proto message builder. Data will be copied into
   * this new object.
   **/
  template<typename Builder>
  CapnObject(Builder &&builder,
      decltype(capnp::writeMessage(
          std::declval<kj::VectorOutputStream&>(),
          std::declval<decay_<Builder>&>()), 0) = 0
    ) {
    kj::VectorOutputStream vec;
    capnp::writeMessage(vec, std::forward<Builder>(builder));

    auto data = vec.getArray();
    *this = CapnObject(data.asChars().begin(), data.size());
  }

  /**
   * Get a Cap'n Proto reader for the held data.
   **/
  typename T::Reader reader() const
  {
    return Base::reader(type<T>{});
  }

  /**
   * Get the registered tag for this type.
   **/
  const char *tag() const override {
    return AnyRegistry::get_type_name<CapnObject<T>>();
  }
};

/**
 * Stores a Cap'n Proto object with a given tag and schema. Typically, you
 * should registered the tags and schemas used with this object with the Any
 * system: `Any::register_schema("Tag", struct_schema)`
 **/
class RegCapnObject : public BaseCapnObject
{
  using Base = BaseCapnObject;

public:
  RegCapnObject() = default;

  /**
   * Construct with no data, but with given tag and schema. Warning: the tag is
   * NOT copied, so ensure its lifetime exceeds that of this object. Typically,
   * this will be the tag registered with the Any system.
   **/
  RegCapnObject(const char *tag, capnp::StructSchema schema) :
    Base(), tag_(tag), schema_(schema) {}

  /**
   * Construct given tag and schema, and data copied from given char buffer.
   * Warning: the tag is NOT copied, so ensure its lifetime exceeds that of
   * this object. Typically, this will be the tag registered with the Any
   * system.
   **/
  RegCapnObject(const char *tag, capnp::StructSchema schema,
      const char *d, size_t s) :
    Base(d, s), tag_(tag), schema_(schema) {}

  /**
   * Construct given tag and schema, and data copied from given input stream.
   * Warning: the tag is NOT copied, so ensure its lifetime exceeds that of
   * this object. Typically, this will be the tag registered with the Any
   * system.
   **/
  RegCapnObject(const char *tag, capnp::StructSchema schema, std::istream &i,
    size_t s) : Base(i, s), tag_(tag), schema_(schema) {}

  /**
   * Construct given tag and schema, and data copied from given Cap'n Proto
   * message stream. Warning: the tag is NOT copied, so ensure its lifetime
   * exceeds that of this object. Typically, this will be the tag registered
   * with the Any system.
   **/
  template<typename Msg>
  RegCapnObject(const char *tag, capnp::StructSchema schema, Msg &&msg,
      decltype(capnp::writeMessage(
          std::declval<kj::VectorOutputStream&>(),
          std::declval<decay_<Msg>&>()), 0) = 0)
  {
    kj::VectorOutputStream vec;
    capnp::writeMessage(vec, std::forward<Msg>(msg));

    auto data = vec.getArray();
    *this = RegCapnObject(tag, schema, data.asChars().begin(), data.size());
  }
  /**
   * Construct with no data, but with given tag.  The tag must be registered
   * with the Any system.
   **/
  RegCapnObject(const char *tag) : Base()
  {
    const auto &pair = AnyRegistry::lookup_schema(tag);
    tag_ = pair.first;
    schema_ = pair.second;
  }

  /**
   * Construct given tag, and data copied from given char buffer.
   * The tag must be registered with the Any system.
   **/
  RegCapnObject(const char *tag, const char *d, size_t s) : Base(d, s)
  {
    const auto &pair = AnyRegistry::lookup_schema(tag);
    tag_ = pair.first;
    schema_ = pair.second;
  }

  /**
   * Construct given tag, and data copied from given input stream.
   * The tag must be registered with the Any system.
   **/
  RegCapnObject(const char *tag, std::istream &i, size_t s) : Base(i, s)
  {
    const auto &pair = AnyRegistry::lookup_schema(tag);
    tag_ = pair.first;
    schema_ = pair.second;
  }

  /**
   * Construct given tag, and data copied from given Cap'n Proto message stream.
   * The tag must already be registered with the Any system.
   **/
  template<typename Msg>
  RegCapnObject(const char *tag, Msg &&msg,
      decltype(capnp::writeMessage(
          std::declval<kj::VectorOutputStream&>(),
          std::declval<decay_<Msg>&>()), 0) = 0)
  {
    const auto &pair = AnyRegistry::lookup_schema(tag);
    *this = RegCapnObject(pair.first, pair.second, std::forward<Msg>(msg));
  }

  /**
   * Get a reader for the held type, with the held schema
   **/
  typename capnp::DynamicStruct::Reader reader() const
  {
    return Base::reader(type<capnp::DynamicStruct>{}, schema_);
  }

  /**
   * Interpret held data as given Cap'n Proto type, and return the appropriate
   * reader. Will throw if type doesn't match schema of this message.
   **/
  template<typename T>
  typename T::Reader reader(type<T>) const
  {
    return reader().template as<T>();
  }

  /**
   * Interpret held data as given Cap'n Proto type, and return the appropriate
   * reader. Will throw if type doesn't match schema of this message.
   **/
  template<typename T>
  typename T::Reader reader() const
  {
    return reader(type<T>{});
  }

  /**
   * Get the registered tag of this object's schema
   **/
  const char *tag() const override { return tag_; }

  /**
   * Get the Cap'n Proto schema of this object
   **/
  const capnp::StructSchema &schema() const { return schema_; }

private:
  const char *tag_;
  capnp::StructSchema schema_;
};

} // namespace knowledge

namespace utility { inline namespace core {

template<typename T>
inline auto get_type_handler_save(type<knowledge::CapnObject<T>>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::save_fn_type
{
  return [](std::ostream &o, const void *ptr) {
      using knowledge::CapnObject;
      const CapnObject<T> &val = *static_cast<const CapnObject<T> *>(ptr);
      o.write(val.data(), val.size());
    };
}

inline auto get_type_handler_save(type<knowledge::GenericCapnObject>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::save_fn_type
{
  return [](std::ostream &o, const void *ptr) {
      using knowledge::GenericCapnObject;
      const GenericCapnObject &val = *static_cast<const GenericCapnObject *>(ptr);
      o.write(val.data(), val.size());
    };
}

inline auto get_type_handler_save(type<knowledge::RegCapnObject>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::save_fn_type
{
  return [](std::ostream &o, const void *ptr) {
      using knowledge::RegCapnObject;
      const RegCapnObject &val = *static_cast<const RegCapnObject *>(ptr);
      o.write(val.data(), val.size());
    };
}

template<typename T>
inline auto get_type_handler_load(type<knowledge::CapnObject<T>>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::load_fn_type
{
  return [](const char *in, size_t size, void *ptr, const char *) {
      using knowledge::CapnObject;
      CapnObject<T> &val = *static_cast<CapnObject<T> *>(ptr);
      val = {in, size};
    };
}

inline auto get_type_handler_load(type<knowledge::GenericCapnObject>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::load_fn_type
{
  return [](const char *in, size_t size, void *ptr, const char *tag) {
      using knowledge::GenericCapnObject;
      GenericCapnObject &val = *static_cast<GenericCapnObject *>(ptr);
      val = {tag, in, size};
    };
}

inline auto get_type_handler_load(type<knowledge::RegCapnObject>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::load_fn_type
{
  return [](const char *in, size_t size, void *ptr, const char *) {
      using knowledge::RegCapnObject;
      RegCapnObject &val = *static_cast<RegCapnObject *>(ptr);
      val = {val.tag(), val.schema(), in, size};
    };
}

template<typename T>
inline auto get_type_handler_save_json(type<knowledge::CapnObject<T>>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::save_json_fn_type
{
  return [](std::ostream &o, const void *ptr) {
      using knowledge::CapnObject;
      const CapnObject<T> &val = *static_cast<const CapnObject<T> *>(ptr);

      capnp::JsonCodec json;
      kj::String enc = json.encode(val.reader());
      o << enc.cStr();
    };
}

inline auto get_type_handler_save_json(type<knowledge::GenericCapnObject>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::save_json_fn_type
{
  return [](std::ostream &o, const void *ptr) {
      using knowledge::GenericCapnObject;
      const GenericCapnObject &val = *static_cast<const GenericCapnObject *>(ptr);
      o << "\"GenericCapnObject<" << val.tag() << ">\"";
    };
}

inline auto get_type_handler_save_json(type<knowledge::RegCapnObject>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::save_json_fn_type
{
  return [](std::ostream &o, const void *ptr) {
      using knowledge::RegCapnObject;
      const RegCapnObject &val = *static_cast<const RegCapnObject *>(ptr);

      capnp::JsonCodec json;
      kj::String enc = json.encode(
          capnp::DynamicValue::Reader(val.reader()), capnp::Type(val.schema()));
      o << enc.cStr();
    };
}

template<typename T>
inline auto get_type_handler_load_json(type<knowledge::CapnObject<T>>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::load_json_fn_type
{
  return [](std::istream &i, void *ptr, const char *) {
      using knowledge::CapnObject;
      CapnObject<T> &val = *static_cast<CapnObject<T> *>(ptr);

      capnp::MallocMessageBuilder buffer;
      auto builder = buffer.initRoot<T>();

      std::string s(std::istreambuf_iterator<char>(i), {});

      capnp::JsonCodec json;
      json.decode({s.data(), s.size()}, builder);

      kj::VectorOutputStream vec;
      capnp::writeMessage(vec, buffer);

      auto data = vec.getArray();
      CapnObject<T> obj(data.asChars().begin(), data.size());

      val = std::move(obj);
    };
}

inline auto get_type_handler_load_json(type<knowledge::GenericCapnObject>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::load_json_fn_type
{
  return nullptr;
}

inline auto get_type_handler_load_json(type<knowledge::RegCapnObject>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::load_json_fn_type
{
  return [](std::istream &i, void *ptr, const char *) {
      using knowledge::RegCapnObject;
      RegCapnObject &val = *static_cast<RegCapnObject *>(ptr);

      capnp::MallocMessageBuilder buffer;
      auto builder = buffer.initRoot<capnp::DynamicStruct>(val.schema());

      std::string s(std::istreambuf_iterator<char>(i), {});

      capnp::JsonCodec json;
      json.decode({s.data(), s.size()}, builder);

      kj::VectorOutputStream vec;
      capnp::writeMessage(vec, buffer);

      auto data = vec.getArray();
      RegCapnObject obj(val.tag(), val.schema(),
                        data.asChars().begin(), data.size());

      val = std::move(obj);
    };
}

template<typename T>
inline auto get_type_handler_get_reader(type<knowledge::CapnObject<T>>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::get_reader_fn_type
{
  return [](capnp::DynamicStruct::Reader *reader, capnp::StructSchema *schema,
      const char **data, size_t *size, void *ptr)
    {
      using knowledge::CapnObject;
      const CapnObject<T> &val = *static_cast<const CapnObject<T> *>(ptr);

      if (reader) { *reader = val.reader(); }
      if (schema) { *schema = capnp::Schema::from<T>(); }
      if (data) { *data = val.data(); }
      if (size) { *size = val.size(); }

      return true;
    };
}

inline auto get_type_handler_get_reader(type<knowledge::RegCapnObject>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::get_reader_fn_type
{
  return [](capnp::DynamicStruct::Reader *reader, capnp::StructSchema *schema,
      const char **data, size_t *size, void *ptr)
    {
      using knowledge::RegCapnObject;
      const RegCapnObject &val = *static_cast<const RegCapnObject *>(ptr);

      if (reader) { *reader = val.reader(); }
      if (schema) { *schema = val.schema(); }
      if (data) { *data = val.data(); }
      if (size) { *size = val.size(); }

      return true;
    };
}

inline auto get_type_handler_get_reader(type<knowledge::GenericCapnObject>,
    overload_priority<8>) ->
  knowledge::TypeHandlers::get_reader_fn_type
{
  return [](capnp::DynamicStruct::Reader *reader, capnp::StructSchema *schema,
      const char **data, size_t *size, void *ptr)
    {
      using knowledge::GenericCapnObject;
      const GenericCapnObject &val =
        *static_cast<const GenericCapnObject *>(ptr);

      if (reader && !schema) { return false;}
      if (reader && schema) { *reader = val.reader(*schema); }
      if (data) { *data = val.data(); }
      if (size) { *size = val.size(); }
      return true;
    };
}

/// Creates a function for getting type's tag
template<typename T>
constexpr auto get_type_handler_tag(type<T>,
    overload_priority<8>) ->
  enable_if_<is_base_of<knowledge::BaseCapnObject, T>(),
    knowledge::TypeHandlers::tag_fn_type>
{
  return [](void *ptr) -> const char * {
      using knowledge::BaseCapnObject;

      const BaseCapnObject &val = *static_cast<const BaseCapnObject *>(ptr);
      return val.tag();
    };
}

}}

} // namespace madara

#endif  // MADARA_CAPNOBJECT_ANY_H_
