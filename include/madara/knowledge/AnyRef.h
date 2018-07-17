#ifndef MADARA_KNOWLEDGE_ANY_REF_H_
#define MADARA_KNOWLEDGE_ANY_REF_H_

/**
 * @file AnyRef.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the AnyRef class
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

namespace madara { namespace knowledge {

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
   * Take the Any's stored value, leaving it in its moved-from state. On
   * moveable types, this will not copy the value.
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
  T take(type<T> t) const
  {
    return T(std::move(this->ref(t)));
  }

  /**
   * Take the Any's stored value, leaving it in its moved-from state. On
   * moveable types, this will not copy the value.
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
  T take() const
  {
    return take(type<T>{});
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
 * reference may be used to modify the referenced object, but cannot change the
 * type of the referenced object.
 **/
class AnyRef : public BasicAny<AnyRef, Any, AnyRef, ConstAnyRef>
{
  using Base = BasicAny<AnyRef, Any, AnyRef, ConstAnyRef>;

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

  template<typename Impl2, typename ValImpl2, typename RefImpl2>
  friend class ::madara::knowledge::BasicConstAny;

  template<typename Impl2, typename ValImpl2, typename RefImpl2, typename CRefImpl2>
  friend class ::madara::knowledge::BasicAny;
};

inline ConstAnyRef::ConstAnyRef(const AnyRef &other)
  : Base(other.handler_, other.data_) {}

} // namespace knowledge

} // namespace madara

#endif  // MADARA_KNOWLEDGE_ANY_REF_H_
