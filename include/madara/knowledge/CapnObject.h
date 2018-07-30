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

#include "capnp/serialize.h"
#include "capnp/compat/json.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC__

#include "madara/utility/SupportTest.h"
#include "madara/MadaraExport.h"
#include "madara/logger/GlobalLogger.h"
#include "TypeHandlers.h"

namespace madara { namespace knowledge {

template<typename Impl>
class BasicCapnObject
{
public:
  size_t size() const { return size_; }

  const char *data() const { return data_.get(); }

protected:
  char *data() { return data_.get(); }

  std::shared_ptr<char> mk_copy(const char *data, size_t size, size_t extra)
  {
    return std::shared_ptr<char>(
        (char *)memcpy((char *)operator new(size + extra), data, size));
  }

  std::shared_ptr<char> read_from(std::istream &i, size_t size)
  {
    std::shared_ptr<char> ret((char *)operator new(size));
    i.read(ret.get(), size);
    return ret;
  }

  BasicCapnObject() = default;

  BasicCapnObject(const char *data, size_t size, size_t extra = 0)
    : data_(mk_copy(data, size, extra)), size_(size) {}

  BasicCapnObject(std::istream &i, size_t size)
    : data_(read_from(i, size)), size_(size) {}

  Impl &impl() { return *static_cast<Impl *>(this); }
  const Impl &impl() const { return *static_cast<const Impl *>(this); }

  std::shared_ptr<char> data_;
  size_t size_;
};

class UnknownCapnObject : public BasicCapnObject<UnknownCapnObject>
{
  using Base = BasicCapnObject<UnknownCapnObject>;
public:
  UnknownCapnObject(const char *tag, const char *d, size_t s)
    : Base(d, s, std::strlen(tag) + 1),
      tag_((const char *)std::strcpy(data() + size(), tag)) {}

  template<typename T>
  typename T::Reader reader(type<T>) const
  {
    capnp::FlatArrayMessageReader reader(
        kj::ArrayPtr<const capnp::word>((const capnp::word *)data(),
          size() / sizeof(capnp::word)));
    return reader.template getRoot<T>();
  }

  const char *tag() const { return tag_; }

private:
  const char *tag_;
};

template<typename T>
class CapnObject : public BasicCapnObject<CapnObject<T>>
{
  using Base = BasicCapnObject<CapnObject<T>>;

public:
  CapnObject() = default;

  CapnObject(const char *d, size_t s) : Base(d, s) {}

  CapnObject(std::istream &i, size_t s) : Base(i, s) {}

  typename T::Reader reader() const
  {
    capnp::FlatArrayMessageReader reader(
        kj::ArrayPtr<const capnp::word>((const capnp::word *)this->data(),
          this->size() / sizeof(capnp::word)));
    return reader.template getRoot<T>();
  }

  static const char *tag() { return AnyRegistry::get_type_name<T>(); }
};

} // namespace knowledge

namespace utility { inline namespace core {

template<typename T>
constexpr auto get_type_handler_save(type<knowledge::CapnObject<T>>,
    overload_priority<12>) ->
  knowledge::TypeHandlers::save_fn_type
{
  return [](std::ostream &o, const void *ptr) {
      using knowledge::CapnObject;
      const CapnObject<T> &val = *static_cast<const CapnObject<T> *>(ptr);
      knowledge::madara_oarchive archive(o);
      archive << val.size();
      o.write(val.data(), val.size());
    };
}

template<typename T>
constexpr auto get_type_handler_load(type<knowledge::CapnObject<T>>,
    overload_priority<12>) ->
  knowledge::TypeHandlers::load_fn_type
{
  return [](std::istream &i, void *ptr) {
      using knowledge::CapnObject;
      CapnObject<T> &val = *static_cast<CapnObject<T> *>(ptr);
      knowledge::madara_iarchive archive(i);
      size_t size;
      archive >> size;
      val = {i, size};
    };
}

template<typename T>
constexpr auto get_type_handler_save_json(type<knowledge::CapnObject<T>>,
    overload_priority<12>) ->
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

template<typename T>
constexpr auto get_type_handler_load_json(type<knowledge::CapnObject<T>>,
    overload_priority<12>) ->
  knowledge::TypeHandlers::load_json_fn_type
{
  return [](std::istream &i, void *ptr) {
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

}}

} // namespace madara

#endif  // MADARA_CAPNOBJECT_ANY_H_
