#include "Any.h"

#ifndef MADARA_KNOWLEDGE_CONST_ANY_REF_H_
#define MADARA_KNOWLEDGE_CONST_ANY_REF_H_

/**
 * @file CosntAnyRef.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the CosntAnyRef class
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
#include "CapnObject.h"

namespace madara
{
namespace knowledge
{
template<typename Impl, typename ValImpl, typename RefImpl, typename CRefImpl>
class BasicAny;
/**
 * Provides methods common to Any, ConstAny, AnyRef, ConstAnyRef. Use those
 * classes, not this one directly.
 **/
template<typename Impl, typename ValImpl, typename RefImpl>
class BasicConstAny
{
protected:
  BasicConstAny() = default;

  BasicConstAny(const TypeHandlers* handler, void* data)
    : handler_(handler), data_(data)
  {
  }

  Impl& impl()
  {
    return *static_cast<Impl*>(this);
  }
  const Impl& impl() const
  {
    return *static_cast<const Impl*>(this);
  }

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

protected:
  template<typename T>
  void check_type(type<T> t) const
  {
    if(!data_)
    {
      throw exceptions::BadAnyAccess("ref() called on empty Any");
    }
    else if(!handler_)
    {
      throw exceptions::BadAnyAccess("ref() called on const Any with raw data");
    }
    else if(type_id<T>() != handler_->tindex)
    {
      throw exceptions::BadAnyAccess(t, handler_->tindex);
    }
  }

  template<typename T>
  bool try_type(type<T>) const
  {
    return data_ && handler_ && type_id<T>() == handler_->tindex;
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
  const T& ref(type<T> t) const
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
  const T& ref() const
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
  const T& cref(type<T> t) const
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
  const T& cref() const
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
  const T& ref_unsafe(type<T>) const
  {
    return *reinterpret_cast<const T*>(data_);
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
  const T& ref_unsafe() const
  {
    return ref_unsafe(type<T>{});
  }

  /**
   * Access the Any's stored value by const pointer.
   * If empty() or raw() is true, return nullptr; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as const T&, else return nullptr
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a pointer to the contained value
   **/
  template<typename T>
  const T* ptr(type<T> t) const
  {
    if(!impl().try_type(t))
    {
      return nullptr;
    }
    return &impl().ref_unsafe(t);
  }

  /**
   * Access the Any's stored value by const pointer.
   * If empty() or raw() is true, return nullptr; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as const T&, else return nullptr
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a pointer to the contained value
   **/
  template<typename T>
  const T* ptr() const
  {
    return ptr(type<T>{});
  }

  /**
   * Access the Any's stored value by const pointer.
   * If empty() or raw() is true, return nullptr; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as const T&, else return nullptr
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a pointer to the contained value
   **/
  template<typename T>
  const T* cptr(type<T> t) const
  {
    return ptr(t);
  }

  /**
   * Access the Any's stored value by const pointer.
   * If empty() or raw() is true, return nullptr; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as const T&, else return nullptr
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a pointer to the contained value
   **/
  template<typename T>
  const T* cptr() const
  {
    return ptr(type<T>{});
  }

  /**
   * Get a statically typed reader for a Cap'n Proto object stored inside this
   * object. Throws BadAnyAccess if the wrong type is given.
   **/
  template<typename T>
  typename T::Reader reader(type<T>) const
  {
    auto capn = cptr<CapnObject<T>>();
    if(capn)
    {
      return capn->reader();
    }
    auto reg_capn = cptr<RegCapnObject>();
    if(reg_capn)
    {
      return reg_capn->reader().template as<T>();
    }
    auto gen_capn = cptr<GenericCapnObject>();
    if(gen_capn)
    {
      return gen_capn->reader(capnp::Schema::from<T>()).template as<T>();
    }
    throw exceptions::BadAnyAccess("reader<T>(): not a Cap'n Proto message");
  }

  /**
   * Get a statically typed reader for a Cap'n Proto object stored inside this
   * object. Throws BadAnyAccess if the wrong type is given.
   **/
  template<typename T>
  typename T::Reader reader() const
  {
    return reader(type<T>{});
  }

  /**
   * Get a DynamicStruct reader for a Cap'n Proto object stored inside this
   * object. Throws BadAnyAccess if the schema isn't known (i.e., this is an
   * unregistered type), or if this isn't a Cap'n Proto object.
   **/
  capnp::DynamicStruct::Reader reader() const
  {
    using exceptions::BadAnyAccess;

    if(!handler_->get_reader)
    {
      throw BadAnyAccess("Any does not contain a Cap'n Proto message");
    }
    capnp::DynamicStruct::Reader ret;
    if(!handler_->get_reader(&ret, nullptr, nullptr, nullptr, data_))
    {
      throw BadAnyAccess("Unknown schema for Cap'n Proto message.");
    }
    return ret;
  }

  /**
   * Get a DynamicStruct reader for a Cap'n Proto object stored inside this
   * object. Uses the given schema if the schema isn't known internally (i.e.,
   * this is an unregistered type). Otherwise, ignores the given schema.
   **/
  capnp::DynamicStruct::Reader reader(capnp::StructSchema schema) const
  {
    using exceptions::BadAnyAccess;

    if(!handler_->get_reader)
    {
      throw BadAnyAccess("Any does not contain a Cap'n Proto message");
    }
    capnp::DynamicStruct::Reader ret;
    if(!handler_->get_reader(&ret, &schema, nullptr, nullptr, data_))
    {
      throw BadAnyAccess("Wrong schema for Cap'n Proto message");
    }
    return ret;
  }

  kj::ArrayPtr<const capnp::word> get_capnp_buffer() const
  {
    using exceptions::BadAnyAccess;

    if(!handler_->get_reader)
    {
      throw BadAnyAccess("Any does not contain a Cap'n Proto message");
    }
    const char* data;
    size_t size;
    if(!handler_->get_reader(nullptr, nullptr, &data, &size, data_))
    {
      throw BadAnyAccess("Unknown error getting Cap'n Proto object's buffer");
    }
    return {(const capnp::word*)data, size / 8};
  }

  /**
   * Serialize this Any to the given buffer. Throws an exception if the buffer
   * size is insufficient.
   *
   * @param data the buffer to serialize to
   * @param size size of the buffer
   * @return the actual number of bytes used during serialization
   **/
  size_t serialize(char* data, size_t size) const
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
  size_t serialize(std::vector<char>& vec) const
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
  void serialize(std::ostream& stream) const
  {
    if(!handler_->save)
    {
      throw exceptions::BadAnyAccess(std::string("Type ") + tag() +
                                     "does not "
                                     "support serialization");
    }
    handler_->save(stream, data_);
  }

  /**
   * Serialize this Any to the given buffer with a tag string recording the
   * saved type, for use with tagged_unserialize. Throws an exception if the
   * buffer size is insufficient.
   *
   * @param data the buffer to serialize to
   * @param size size of the buffer
   * @return the actual number of bytes used during serialization
   **/
  size_t tagged_serialize(char* data, size_t size) const
  {
    namespace bio = boost::iostreams;

    bio::array_sink output_sink(data, size);
    bio::stream<bio::array_sink> output_stream(output_sink);

    auto pos = output_stream.tellp();
    impl().tagged_serialize(output_stream);
    auto len = output_stream.tellp() - pos;

    return len;
  }

  /**
   * Serialize this Any to the given buffer with a tag string recording the
   * saved type, for use with tagged_unserialize. Throws an exception if the
   * buffer size is insufficient.
   *
   * @param tag tag to use
   * @param data the buffer to serialize to
   * @param size size of the buffer
   * @return the actual number of bytes used during serialization
   **/
  size_t tagged_serialize(const char* tag, char* data, size_t size) const
  {
    namespace bio = boost::iostreams;

    bio::array_sink output_sink(data, size);
    bio::stream<bio::array_sink> output_stream(output_sink);

    auto pos = output_stream.tellp();
    impl().tagged_serialize(tag, output_stream);
    auto len = output_stream.tellp() - pos;

    return len;
  }

  /**
   * Serialize this Any to the given vector with a tag string recording the
   * saved type, for use with tagged_unserialize. The vector will be cleared
   * first, and resized as needed.
   *
   * @param vec the vector to serialize to, which will be cleared first
   * @return the actual number of bytes used during serialization
   **/
  size_t tagged_serialize(std::vector<char>& vec) const
  {
    namespace bio = boost::iostreams;

    vec.clear();
    auto output_sink = bio::back_inserter(vec);
    bio::stream<decltype(output_sink)> output_stream(output_sink);

    impl().tagged_serialize(output_stream);

    return vec.size();
  }

  /**
   * Serialize this Any to the given vector with a tag string recording the
   * saved type, for use with tagged_unserialize. The vector will be cleared
   * first, and resized as needed.
   *
   * @param tag tag to use
   * @param vec the vector to serialize to, which will be cleared first
   * @return the actual number of bytes used during serialization
   **/
  size_t tagged_serialize(const char* tag, std::vector<char>& vec) const
  {
    namespace bio = boost::iostreams;

    vec.clear();
    auto output_sink = bio::back_inserter(vec);
    bio::stream<decltype(output_sink)> output_stream(output_sink);

    impl().tagged_serialize(tag, output_stream);

    return vec.size();
  }

  /**
   * Serialize this Any to the given std::ostream with a tag string recording
   * the saved type, for use with tagged_unserialize.
   *
   * @param stream the output archive to serialize to
   **/
  void tagged_serialize(std::ostream& stream) const
  {
    using exceptions::BadAnyAccess;

    auto t = this->tag();
    if(t == nullptr)
    {
      throw BadAnyAccess(
          std::string("tagged_serialize(): unregistered type: ") +
          handler_->tindex.name());
    }

    tagged_serialize(t, stream);
  }

  /**
   * Serialize this Any to the given std::ostream with a tag string recording
   * the saved type, for use with tagged_unserialize.
   *
   * @param tag tag to use
   * @param stream the output archive to serialize to
   **/
  void tagged_serialize(const char* tag, std::ostream& stream) const
  {
    madara_oarchive archive(stream);
    std::string stag(tag);
    /*
    const size_t align = sizeof(capnp::word);
    size_t over = stag.size() % align;
    if(over > 0) {
      size_t pad = align - over;
      stag.resize(stag.size() + pad, '\0');
    }*/
    archive << stag;

    serialize(stream);
  }

  /**
   * Serialize this Any as JSON to the given std::ostream.
   *
   * @param o the output stream to serialize to
   **/
  void serialize_json(std::ostream& o) const
  {
    if(!handler_->save_json)
    {
      throw exceptions::BadAnyAccess(std::string("Type ") + tag() +
                                     "does not "
                                     "support JSON serialization");
    }
    handler_->save_json(o, data_);
  }

  /**
   * Serialize this Any in JSON format to a string, and return it.
   **/
  std::string to_json() const
  {
    std::ostringstream stream;
    serialize_json(stream);
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
  const std::vector<AnyField>& list_fields() const
  {
    if(!supports_fields())
    {
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
  AnyField find_field(const char* name) const
  {
    using exceptions::BadAnyAccess;

    if(!name)
    {
      throw BadAnyAccess("Null name pointer passed to find_field");
    }

    const auto& fields = list_fields();
    auto found = std::equal_range(
        fields.begin(), fields.end(), name, compare_any_fields_by_name());
    if(found.first == fields.end() || found.first == found.second)
    {
      throw BadAnyAccess(
          std::string("Type in Any does not contain field \"") + name + "\"");
    }

    return *found.first;
  }

  /**
   * Verifies that the given field belongs to the stored type. Throws
   * BadAnyAccess if not. If it does belong, simply returns the given field.
   **/
  AnyField find_field(const AnyField& field) const
  {
    using exceptions::BadAnyAccess;

    if(!handler_ || field.parent().tindex != handler_->tindex)
    {
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
  AnyField find_field(const std::string& name) const
  {
    return find_field(name.c_str());
  }

  /**
   * Access a field by const reference, using an AnyField object.
   *
   * Get the AnyField objects for a given type by calling list_fields()
   **/
  RefImpl ref(const AnyField& field) const
  {
    using exceptions::BadAnyAccess;

    if(!supports_fields())
    {
      throw BadAnyAccess("Tried to access field in Any holding type "
                         "that doesn't support them");
    }
    if(field.parent().tindex != handler_->tindex)
    {
      throw BadAnyAccess("Tried to access field in Any from a "
                         "class not contained within");
    }
    auto func = handler_->get_field;
    if(!func)
    {
      return {};
    }
    const TypeHandlers* handler;
    void* data;
    func(field, handler, data, data_);
    return {handler, data};
  }

  /**
   * Acess a field by const reference, by name.
   *
   * Use the ref(AnyField) overload for better efficiency.
   **/
  RefImpl ref(const char* name) const
  {
    return ref(find_field(name));
  }

  /**
   * Acess a field by const reference, by name.
   *
   * Use the ref(AnyField) overload for better efficiency.
   **/
  RefImpl ref(const std::string& name) const
  {
    return ref(find_field(name));
  }

  /**
   * Access a field by const reference, using an AnyField object.
   *
   * Get the AnyField objects for a given type by calling list_fields()
   **/
  RefImpl cref(const AnyField& field) const
  {
    return ref(field);
  }

  /**
   * Acess a field by const reference, by name.
   *
   * Use the cref(AnyField) overload for better efficiency.
   **/
  RefImpl cref(const char* name) const
  {
    return ref(find_field(name));
  }

  /**
   * Acess a field by const reference, by name.
   *
   * Use the cref(AnyField) overload for better efficiency.
   **/
  RefImpl cref(const std::string& name) const
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
  const T& ref(T Class::*ptm) const
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
  const T& cref(T Class::*ptm) const
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
  T ref(T (Class::*ptmf)(Params...) const, Args&&... args) const
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
  T cref(T (Class::*ptmf)(Params...) const, Args&&... args) const
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
  const T& ref(const AnyField& field) const
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
  const T& ref(const char* name) const
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
  const T& ref(const std::string& name) const
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
  const T& cref(const AnyField& field) const
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
  const T& cref(const char* name) const
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
  const T& cref(const std::string& name) const
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
  auto operator()(Args&&... args) const
      -> decltype(ref(std::forward<Args>(args)...))
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

    if(!supports_int_index())
    {
      throw BadAnyAccess("Type in Any does not support indexing by integer");
    }
    const TypeHandlers* handler = nullptr;
    void* data = nullptr;
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
  RefImpl at(const char* i) const
  {
    using exceptions::BadAnyAccess;

    if(!supports_string_index())
    {
      throw BadAnyAccess("Type in Any does not support indexing by string");
    }
    const TypeHandlers* handler = nullptr;
    void* data = nullptr;
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
  RefImpl at(const std::string& i) const
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
  RefImpl operator[](const char* i) const
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
  RefImpl operator[](const std::string& i) const
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

    if(!supports_size())
    {
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
   * Convert stored value to KnowledgeRecord using knoweldge_cast. If the type
   * cannot be converted to any of native KnowledgeRecord types, the returned
   * KnowledgeRecord will contain a clone of this Any.
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
  T to() const
  {
    return to(type<T>{});
  }

  /**
   * Access stored value, and return as integer (int64_t) value.
   *
   * If stored type is an int64_t, returns it directly. Otherwise, if stored
   * value supports `to_record()`, return `knowledge_cast<int64_t>(to_record())`
   **/
  int64_t to_integer() const
  {
    return impl().template to<int64_t>();
  }

  /**
   * Access stored value, and return as double value.
   *
   * If stored type is an double, returns it directly. Otherwise, if stored
   * value supports `to_record()`, return `knowledge_cast<double>(to_record())`
   **/
  double to_double() const
  {
    return impl().template to<double>();
  }

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
  std::string to_string() const
  {
    if(!handler_ && data_)
    {
      return impl().template ref<std::string>();
    }

    if(handler_->tindex == type_id<std::string>())
    {
      return impl().template ref_unsafe<std::string>();
    }

    if(supports_ostream())
    {
      std::stringstream s;
      stream(s);
      return s.str();
    }

    if(supports_to_record())
    {
      std::string ret;
      try_knowledge_cast(handler_->to_record(data_), ret);
      return ret;
    }

    return to_json();
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
  std::ostream& stream(std::ostream& o) const
  {
    if(supports_ostream())
    {
      return handler_->to_ostream(o, data_);
    }
    else if(supports_to_record())
    {
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
  friend std::ostream& operator<<(std::ostream& o, const BasicConstAny& self)
  {
    return self.stream(o);
  }

  const char* tag() const
  {
    if(!handler_)
    {
      return nullptr;
    }
    return handler_->tag(data_);
  }

  std::pair<const TypeHandlers*, void*> get_pointers() const
  {
    return {handler_, data_};
  }

protected:
  const TypeHandlers* handler_ = nullptr;
  void* data_ = nullptr;

  friend Impl;
  friend ValImpl;
  friend RefImpl;

  template<typename Impl2, typename ValImpl2, typename RefImpl2>
  friend class BasicConstAny;

  template<typename Impl2, typename ValImpl2, typename RefImpl2,
      typename CRefImpl2>
  friend class BasicAny;
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
  ConstAnyRef(const AnyRef& other);

  /**
   * Construct as a reference to the given Any
   **/
  ConstAnyRef(const Any& other);

  /**
   * Construct as a reference to the given Any
   **/
  ConstAnyRef(const ConstAny& other);

  /**
   * Change the object this ConstAnyRef refers to. It must be a type which could
   * be stored in an Any, but the object need actually be stored in an Any
   **/
  template<typename T>
  void target(T& t)
  {
    handler_ = &get_type_handler<T>();
    data_ = (void*)&t;
  }

  static ConstAnyRef from_pointers_unsafe(
      const TypeHandlers* handler, void* data)
  {
    return {handler, data};
  }

  template<typename Impl2, typename Base2>
  friend class BasicOwningAny;

  friend class Any;

  template<typename Impl2, typename ValImpl2, typename RefImpl2>
  friend class ::madara::knowledge::BasicConstAny;

  template<typename Impl2, typename ValImpl2, typename RefImpl2,
      typename CRefImpl2>
  friend class ::madara::knowledge::BasicAny;
};

template<typename Impl, typename ValImpl, typename RefImpl>
inline ValImpl BasicConstAny<Impl, ValImpl, RefImpl>::clone() const
{
  return this->impl();
}

}  // namespace knowledge

}  // namespace madara

#endif  // MADARA_KNOWLEDGE_CONST_ANY_REF_H_
