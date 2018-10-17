

#ifndef INCL_MADARA_RCW_TRACKED_H
#define INCL_MADARA_RCW_TRACKED_H

/**
 * @file Tracked.h
 * @author David Kyle <dskyle@sei.cmu.edu>
 *
 * Implements Tracked class, which wraps types and tracks whether they have
 * been modified
 **/

#include <string>
#include <vector>
#include <map>
#include <list>
#include <type_traits>
#include <initializer_list>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/Functions.h"
#include "madara/utility/StdInt.h"
#include "madara/MadaraExport.h"
#include "madara/knowledge/VariableReference.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/KnowledgeCast.h"
#include "madara/knowledge/ContextGuard.h"

namespace madara
{
namespace knowledge
{
namespace rcw
{
/// Provides default versions of methods below. Specialize to add
/// additional overloads for certain Ts
template<class T, class Impl>
class TrackedExtra
{
public:
  /// Get a const ref to the underlying object
  const T& get() const;

  /// Get a non-const ref to underlying object. Immediately treat it as
  /// modified, since we don't know what the caller will do with it.
  T& get_mut();

  /// Get a non-const ref to underlying object. Immediately treat it as
  /// modified, since we don't know what the caller will do with it.
  /// Synonmym for @get_mut
  T& get_mutable();

  /// Set the underlying object, and mark as modified.
  void set(T val);

  /// Treat object as modified, even if it isn't.
  void modify();

  /// Get modification status
  /// @returns true if object has been modified; false if not
  bool is_dirty() const;

  /// Resets modification status
  void clear_dirty();
};

/// Used internally by Tracked
template<class T, class Impl>
class TrackedCollection;

/**
 * Tracks the modification status of a wrapped object of the given type.
 *
 * See https://sourceforge.net/p/madara/wiki/ReadComputeWrite/ for details.
 *
 * @tparam T type of the underlying object
 **/
template<class T>
class Tracked : public TrackedExtra<T, Tracked<T>>
{
private:
  T val_;       /// Wrapped value
  char dirty_;  /// Flag for modification status

  typedef TrackedExtra<T, Tracked<T>> Base;

  friend class TrackedExtra<T, Tracked<T>>;
  friend class TrackedCollection<T, Tracked<T>>;
  friend class TrackedCollection<T, TrackedExtra<T, Tracked<T>>>;

public:
  /// Default constructor; default constructs wrapped object
  Tracked() : val_(), dirty_(true) {}

  /// Create a Tracked version of the given object, which is copied
  /// @param val the object to copy and track
  explicit Tracked(T val) : val_(val), dirty_(true) {}

  /// Get a const ref to the underlying object
  const T& get() const
  {
    return val_;
  }

  /// Cast to const ref as appropriate
  explicit operator const T&() const
  {
    return get();
  }

  /// Get a non-const ref to underlying object. Immediately treat it as
  /// modified, since we don't know what the caller will do with it.
  T& get_mut()
  {
    modify();
    return val_;
  }

  /// Get a non-const ref to underlying object. Immediately treat it as
  /// modified, since we don't know what the caller will do with it.
  /// Synonmym for @get_mut
  T& get_mutable()
  {
    modify();
    return val_;
  }

  /// Set the underlying object, and mark as modified.
  void set(T val)
  {
    modify();
    val_ = val;
  }

  /// Dereference to underlying object. Only supports const version.
  /// To get a non-const reference, use get_mut().
  const T& operator*() const
  {
    return this->get();
  }

  /// Pass through to underlying object. Only supports const version.
  /// To get a non-const reference, use get_mut().
  const T* operator->() const
  {
    return &this->get();
  }

  /// Assignment writes to the underlying object.
  Tracked& operator=(T val)
  {
    this->set(val);
    return *this;
  }

  /// Delete default assignment so we don't accidentally use it.
  Tracked& operator=(const Tracked&) = delete;

  /// Treat object as modified, even if it isn't.
  void modify()
  {
    dirty_ = true;
  }

  /// Get modification status
  /// @returns true if object has been modified; false if not
  bool is_dirty() const
  {
    return dirty_ != 0;
  }

  /// Resets modification status
  void clear_dirty()
  {
    dirty_ = false;
  }

  using Base::clear_dirty;
  using Base::get;
  using Base::get_mut;
  using Base::is_dirty;
  using Base::modify;
  using Base::set;

  /// Cast to bool as appropriate
  explicit operator bool()
  {
    return val_ ? true : false;
  }

  /// Support conversion to Tracked<U> if U can convert from T
  /// @tparam U other type to potentially convert to
  template<class U, typename std::enable_if<std::is_convertible<T, U>::value,
                        void*>::type = nullptr>
  operator Tracked<U>() const
  {
    Tracked<U> ret(get());
    if(!is_dirty())
      ret.clear_dirty();
    return ret;
  }

  /// Implement swap between two tracked objects of same type
  friend void swap(Tracked& lhs, Tracked& rhs)
  {
    using std::swap;
    swap(lhs.val_, rhs.val_);
    swap(lhs.dirty_, rhs.dirty_);
  }

  /// Implement swap between tracked object, and object of underlying type
  friend void swap(Tracked& lhs, T& rhs)
  {
    using std::swap;
    swap(lhs.val_, rhs);
    lhs.dirty_ = true;
  }

  /// Implement swap between tracked object, and object of underlying type
  friend void swap(T& lhs, Tracked& rhs)
  {
    using std::swap;
    swap(lhs, rhs.val_);
    rhs.dirty_ = true;
  }
};

/// Create a tracked version of a given object
/// @param val the value to track
template<class T>
Tracked<typename std::decay<T>::type> track(T&& val)
{
  return Tracked<typename std::decay<T>::type>(std::forward<T>(val));
}

#define MADARA_AUTOTYPE_BODY(body) \
  ->decltype(body)                 \
  {                                \
    return (body);                 \
  }

#define MADARA_MAKE_BINARY_COMPARE_OP(op)                                     \
  template<class T, class U>                                                  \
  auto operator op(const Tracked<T>& lhs, const Tracked<U>& rhs)              \
      MADARA_AUTOTYPE_BODY(lhs.get() op rhs.get()) template<class T, class U> \
      auto operator op(const Tracked<T>& lhs, U&& rhs) MADARA_AUTOTYPE_BODY(  \
          lhs.get() op std::forward<U>(rhs)) template<class T, class U>       \
      auto operator op(T&& lhs, const Tracked<U>& rhs)                        \
          MADARA_AUTOTYPE_BODY(std::forward<T>(lhs) op rhs.get())

MADARA_MAKE_BINARY_COMPARE_OP(==)
MADARA_MAKE_BINARY_COMPARE_OP(!=)
MADARA_MAKE_BINARY_COMPARE_OP(<=)
MADARA_MAKE_BINARY_COMPARE_OP(>=)
MADARA_MAKE_BINARY_COMPARE_OP(<)
MADARA_MAKE_BINARY_COMPARE_OP(>)

#define MADARA_MAKE_COMPOUND_OP(op)                                  \
  template<class T, class U>                                         \
  auto operator op##=(Tracked<T>& lhs, const Tracked<U>& rhs)        \
      MADARA_AUTOTYPE_BODY(                                          \
          lhs.get_mut() op## = rhs.get()) template<class T, class U> \
      auto operator op##=(Tracked<T>& lhs, U&& rhs)                  \
          MADARA_AUTOTYPE_BODY(lhs.get_mut() op## = std::forward<U>( \
                                   rhs)) template<class T, class U>  \
          auto operator op##=(T& lhs, const Tracked<U>& rhs)         \
              MADARA_AUTOTYPE_BODY(lhs op## = rhs.get())

#define MADARA_MAKE_BINARY_ARITH_OP(op)                                       \
  template<class T, class U>                                                  \
  auto operator op(const Tracked<T>& lhs, const Tracked<U>& rhs)              \
      MADARA_AUTOTYPE_BODY(lhs.get() op rhs.get()) template<class T, class U> \
      auto operator op(const Tracked<T>& lhs, U&& rhs) MADARA_AUTOTYPE_BODY(  \
          lhs.get() op std::forward<U>(rhs)) template<class T, class U>       \
      auto operator op(T&& lhs, const Tracked<U>& rhs)                        \
          MADARA_AUTOTYPE_BODY(std::forward<T>(lhs) op rhs.get())             \
              MADARA_MAKE_COMPOUND_OP(op)

MADARA_MAKE_BINARY_ARITH_OP(+)
MADARA_MAKE_BINARY_ARITH_OP(-)
MADARA_MAKE_BINARY_ARITH_OP(*)
MADARA_MAKE_BINARY_ARITH_OP(/)
MADARA_MAKE_BINARY_ARITH_OP(%)
MADARA_MAKE_BINARY_ARITH_OP(&)
MADARA_MAKE_BINARY_ARITH_OP(|)
MADARA_MAKE_BINARY_ARITH_OP (^)
MADARA_MAKE_BINARY_ARITH_OP(<<)
MADARA_MAKE_BINARY_ARITH_OP(>>)

#define MADARA_MAKE_UNARY_ARITH_OP(op) \
  template<class T>                    \
  auto operator op(const Tracked<T>& lhs) MADARA_AUTOTYPE_BODY(op lhs.get())

MADARA_MAKE_UNARY_ARITH_OP(+);
MADARA_MAKE_UNARY_ARITH_OP(-);
MADARA_MAKE_UNARY_ARITH_OP(!);
MADARA_MAKE_UNARY_ARITH_OP(~);

#define MADARA_MAKE_INCDEC_OP(op)                              \
  template<class T>                                            \
  auto operator op(Tracked<T>& lhs)                            \
      MADARA_AUTOTYPE_BODY(op lhs.get_mut()) template<class T> \
      auto operator op(Tracked<T>& lhs, int)                   \
          MADARA_AUTOTYPE_BODY(lhs.get_mut() op)

MADARA_MAKE_INCDEC_OP(++);
MADARA_MAKE_INCDEC_OP(--);

typedef Tracked<bool> TrackedBool;
typedef Tracked<char> TrackedChar;
typedef Tracked<short> TrackedShort;
typedef Tracked<int> TrackedInt;
typedef Tracked<long> TrackedLong;
typedef Tracked<long long> TrackedLongLong;
typedef Tracked<unsigned char> TrackedUChar;
typedef Tracked<signed char> TrackedSChar;
typedef Tracked<unsigned short> TrackedUShort;
typedef Tracked<unsigned int> TrackedUInt;
typedef Tracked<unsigned long> TrackedULong;
typedef Tracked<unsigned long long> TrackedULongLong;

typedef Tracked<int8_t> TrackedI8;
typedef Tracked<uint8_t> TrackedU8;
typedef Tracked<int16_t> TrackedI16;
typedef Tracked<uint16_t> TrackedU16;
typedef Tracked<int32_t> TrackedI32;
typedef Tracked<uint32_t> TrackedU32;
typedef Tracked<int64_t> TrackedI64;
typedef Tracked<uint64_t> TrackedU64;

typedef Tracked<float> TrackedFloat;
typedef Tracked<double> TrackedDouble;
typedef Tracked<long double> TrackedLongDouble;

/// Implements methods appropriate for std library collections
template<typename T, typename Impl>
class TrackedCollection
{
private:
  Impl& impl()
  {
    return static_cast<Impl&>(*this);
  }
  const Impl& impl() const
  {
    return static_cast<const Impl&>(*this);
  }

public:
  typedef typename T::value_type value_type;
  typedef typename T::const_iterator const_iterator;

  /// Set value at index @i
  /// @param i index to set
  /// @param val value to set to
  void set(size_t i, value_type val)
  {
    impl().val_.at(i) = val;
    impl().modify(i);
  }

  /// Pass through at method to underlying collection
  const value_type& at(size_t i) const
  {
    return impl().val_.at(i);
  }

  /// Pass through operator[] to underlying collection
  const value_type& operator[](size_t i) const
  {
    return impl().val_[i];
  }

  /// Synonym for @see at
  const value_type& get(size_t i) const
  {
    return at(i);
  }

  /// Pass through to non-const version of at method of underlying collection
  /// Immediately mark that index as modified when called
  value_type& at_mut(size_t i)
  {
    impl().modify(i);
    return impl().val_.at(i);
  }

  /// Synonym for @see at_mut
  value_type& at_mutable(size_t i)
  {
    return at_mut(i);
  }

  /// Synonym for @see at_mut
  value_type& get_mut(size_t i)
  {
    return at_mut(i);
  }

  /// Synonym for @see at_mut
  value_type& get_mutable(size_t i)
  {
    return at_mut(i);
  }

  /// Pass through to size method of underlying collection
  size_t size()
  {
    return impl().val_.size();
  }
  /// Pass through to empty method of underlying collection
  bool empty()
  {
    return impl().val_.empty();
  }
  /// Pass through to max_size method of underlying collection
  size_t max_size()
  {
    return impl().val_.max_size();
  }

  /// Equivalent to front method of underlying collection
  const value_type& front() const
  {
    return at(0);
  }

  /// Equivalent to back method of underlying collection
  const value_type& back() const
  {
    return at(size() - 1);
  }

  /// Equivalent to non-const front method of underlying collection
  value_type& front_mut()
  {
    return at_mut(0);
  }
  /// Equivalent to non-const back method of underlying collection
  value_type& back()
  {
    return at_mut(size() - 1);
  }

  /// Pass through to const begin method  of underlying collection
  const_iterator begin() const
  {
    return impl().val_.begin();
  }

  /// Pass through to cbegin method  of underlying collection
  const_iterator cbegin() const
  {
    return begin();
  }

  /// Pass through to const end method  of underlying collection
  const_iterator end() const
  {
    return impl().val_.end();
  }

  /// Pass through to cend method  of underlying collection
  const_iterator cend() const
  {
    return end();
  }
};

/// Adds functionality specific to std::basic_string
template<typename Char, typename Impl>
class TrackedExtra<std::basic_string<Char>, Impl>
  : public TrackedCollection<std::basic_string<Char>, Impl>
{
private:
  Impl& impl()
  {
    return static_cast<Impl&>(*this);
  }
  const Impl& impl() const
  {
    return static_cast<const Impl&>(*this);
  }
  template<class U, class I>
  friend class TrackedCollection;

public:
  typedef typename std::basic_string<Char>::value_type value_type;
  typedef typename std::basic_string<Char>::const_iterator const_iterator;

  /// Flag this as modified; we don't track individual characters as dirty,
  /// so flags entire string.
  void modify(size_t)
  {
    impl().modify();
  }

  /// Check dirty status for given index, which is same as overall dirty
  /// status, since we don't track per character
  bool is_dirty(size_t) const
  {
    return impl().is_dirty();
  }

  /// Clear dirty status for given index, which is same as overall dirty
  /// status, since we don't track per character
  bool clear_dirty(size_t) const
  {
    return impl().clear_dirty();
  }

  /// Pass through resize to underlying string
  void resize(size_t count)
  {
    impl().val_.resize(count);
    impl().modify();
  }

  /// Pass through capacity to underlying string
  size_t capacity() const
  {
    return impl().val_.capacity();
  }

  /// Pass through reserve to underlying string
  void reserve(size_t count)
  {
    impl().val_.reserve(count);
  }

  /// Pass through push_back to underlying string
  void push_back(const value_type& value)
  {
    impl().val_.push_back(value);
    impl().modify();
  }

  /// Pass through pop_back to underlying string
  void pop_back()
  {
    impl().val_.pop_back();
    impl().modify();
  }

  /// Pass through clear to underlying string
  void clear()
  {
    impl().val_.clear();
    impl().modify();
  }

  /// Pass through c_str data method underlying string
  const value_type* c_str() const
  {
    return impl().val_.c_str();
  }
  /// Pass through data method to underlying string
  const value_type* data() const
  {
    return impl().val_.data();
  }
};

typedef Tracked<std::string> TrackedString;

/// Returns dirty flag of Tracked types
template<typename T>
bool is_dirty(const Tracked<T>& t)
{
  return t.is_dirty();
}

/// Clears dirty flag of Tracked types
template<typename T>
void clear_dirty(Tracked<T>& t)
{
  t.clear_dirty();
}

/// Returns dirty flag of given index of vector of Tracked types
template<typename T>
bool is_dirty(const std::vector<Tracked<T>>& t, size_t i)
{
  return is_dirty(t[i]);
}

/// Clears dirty flag of given index of vector of Tracked types
template<typename T>
void clear_dirty(std::vector<Tracked<T>>& t, size_t i)
{
  clear_dirty(t[i]);
}

/// Get value of Tracked type
template<typename T>
auto get_value(const Tracked<T>& t) -> decltype(get_value(t.get()))
{
  return get_value(t.get());
}

/// Set value of Tracked type
template<typename T>
void set_value(Tracked<T>& t, decltype(get_value(t.get())) v)
{
  set_value(t.get_mut(), v);
}

/// Get value of given index of Tracked string type
char get_value(const Tracked<std::string>& t, size_t i)
{
  return get_value(t.get(i));
}

/// Set value of given index of Tracked string type
void set_value(Tracked<std::string>& t, size_t i, char v)
{
  set_value(t.get_mut(i), v);
}

/// Implement functionality specific to std::vectors
template<typename T>
class Tracked<std::vector<T>>
  : public TrackedCollection<std::vector<T>, Tracked<std::vector<T>>>
{
private:
  typedef std::vector<T> Vec;
  typedef TrackedCollection<Vec, Tracked<std::vector<T>>> Base;

  /// the vector we're wrapping
  Vec val_;

  /// dirty status for each entry
  std::vector<uint64_t> dirty_;

  /// dirty status to treat entire vector as modified
  bool all_dirty_;

  /// dirty status for size of vector
  bool size_dirty_;

  /// Get dirty flag bit offset into dirty_ vector, plus mask
  static std::pair<size_t, uint64_t> to_dirty_bit(size_t i)
  {
    size_t idx = i >> 6;
    size_t shift = i - (idx << 6);
    return {idx, 1 << shift};
  }

  /// Get size of dirty_ vector needed to fit i-th dirty bit
  static size_t to_dirty_size(size_t i)
  {
    return (i + 63) >> 6;
  }

  friend class TrackedCollection<std::vector<T>, Tracked<std::vector<T>>>;

public:
  typedef typename std::vector<T>::value_type value_type;
  typedef typename std::vector<T>::const_iterator const_iterator;

  /// Default constructor, with empty vector
  Tracked() : val_(), dirty_(), all_dirty_(true), size_dirty_(true) {}
  /// Construct from an existing vector
  explicit Tracked(std::vector<T> val)
    : val_(val),
      dirty_(to_dirty_size(val.size())),
      all_dirty_(true),
      size_dirty_(true)
  {
  }

  /// Get const reference to underlying vector
  const std::vector<T>& get() const
  {
    return val_;
  }

  /// Get non-const reference to underlying vector; mark all elements as
  /// dirty immediately. Usually, you should use the get_mut(index) version
  std::vector<T>& get_mut()
  {
    modify();
    return val_;
  }

  /// Get non-const reference to underlying vector; mark all elements as
  /// dirty immediately. Usually, you should use the get_mut(index) version
  /// Synonym for get_mut
  std::vector<T>& get_mutable()
  {
    modify();
    return val_;
  }

  /// Dereference to const reference
  const std::vector<T>& operator*() const
  {
    return get();
  }

  /// Use to call const methods on underlying vector
  const std::vector<T>* operator->() const
  {
    return &get();
  }

  /// Set entire vector to new vector
  void set(const std::vector<T>& val)
  {
    val_ = val;
    dirty_.resize(to_dirty_size(val_.size()));
    modify();
  }

  /// Set entire vector to new vector
  Tracked& operator=(const std::vector<T>& val)
  {
    set(val);
    return *this;
  }

  /// Flag entire vector as modified
  void modify()
  {
    all_dirty_ = true;
  }

  /// Has entire vector been marked as modified? Note that this will still
  /// return false if every entry has been marked modified individually.
  /// This only returns true if modified(), get_mut(), or set(std::vector)
  /// has been called.
  bool is_all_dirty() const
  {
    return all_dirty_;
  }

  /// Has the size of this vector changed?
  bool is_size_dirty() const
  {
    return size_dirty_;
  }

  /// Has anything about this vector changed?
  bool is_dirty() const
  {
    if(all_dirty_)
      return true;
    for(const uint64_t& cur : dirty_)
      if(cur)
        return true;
    return false;
  }

  /// Clear all modification tracking
  void clear_dirty()
  {
    all_dirty_ = false;
    size_dirty_ = false;
    for(uint64_t& cur : dirty_)
      cur = 0;
  }

  /// Mark i-th element as modified
  void modify(size_t i)
  {
    dirty_.resize(to_dirty_size(val_.size()));
    std::pair<size_t, uint64_t> x = to_dirty_bit(i);
    dirty_.at(x.first) |= x.second;
  }

  /// Clear i-th element modification status
  void clear_dirty(size_t i)
  {
    dirty_.resize(to_dirty_size(val_.size()));
    std::pair<size_t, uint64_t> x = to_dirty_bit(i);
    dirty_.at(x.first) &= ~x.second;
  }

  /// Check i-th element modification status
  bool is_dirty(size_t i) const
  {
    std::pair<size_t, uint64_t> x = to_dirty_bit(i);
    if(x.first >= dirty_.size())
      return false;
    return (dirty_.at(x.first) & x.second) != 0;
  }

  /// Pass through resize method to underlying vector
  void resize(size_t count)
  {
    val_.resize(count);
    dirty_.resize(to_dirty_size(val_.size()));
    all_dirty_ = true;
  }

  /// Pass through capacity method to underlying vector
  size_t capacity() const
  {
    size_t vc = val_.capacity();
    size_t dc = to_dirty_size(dirty_.capacity());
    return vc > dc ? dc : vc;
  }

  /// Pass through reserve method to underlying vector
  void reserve(size_t count)
  {
    val_.reserve(count);
    dirty_.reserve(to_dirty_size(count));
  }

  /// Pass through push_back method to underlying vector
  void push_back(const value_type& value)
  {
    val_.push_back(value);
    dirty_.resize(to_dirty_size(val_.size()));
    size_dirty_ = true;
    modify(val_.size() - 1);
  }

  /// Pass through pop_back method to underlying vector
  void pop_back()
  {
    val_.pop_back();
    dirty_.resize(to_dirty_size(val_.size()));
    size_dirty_ = true;
  }

  /// Pass through clear method to underlying vector
  void clear()
  {
    val_.clear();
    dirty_.clear();
    all_dirty_ = true;
  }

  using Base::get;
  using Base::get_mut;
  using Base::get_mutable;
  using Base::set;

  friend bool operator==(const Tracked& lhs, const Tracked& rhs)
  {
    return lhs.val_ == rhs.val_;
  }
  friend bool operator!=(const Tracked& lhs, const Tracked& rhs)
  {
    return lhs.val_ != rhs.val_;
  }
  friend bool operator<=(const Tracked& lhs, const Tracked& rhs)
  {
    return lhs.val_ <= rhs.val_;
  }
  friend bool operator>=(const Tracked& lhs, const Tracked& rhs)
  {
    return lhs.val_ >= rhs.val_;
  }
  friend bool operator<(const Tracked& lhs, const Tracked& rhs)
  {
    return lhs.val_ < rhs.val_;
  }
  friend bool operator>(const Tracked& lhs, const Tracked& rhs)
  {
    return lhs.val_ > rhs.val_;
  }

  /// Implement swap for tracked vectors
  friend void swap(Tracked& lhs, Tracked& rhs)
  {
    using std::swap;
    swap(lhs.val_, rhs.val_);
    swap(lhs.dirty_, rhs.dirty_);
    swap(lhs.all_dirty_, rhs.all_dirty_);
    swap(lhs.size_dirty_, rhs.size_dirty_);
  }
};

/// Return dirty flag at index of Tracked vector
template<typename T>
bool is_dirty(const Tracked<std::vector<T>>& t, size_t i)
{
  return t.is_dirty(i);
}

/// Return global dirty flag of Tracked vector
template<typename T>
bool is_all_dirty(const Tracked<std::vector<T>>& t)
{
  return t.is_all_dirty();
}

/// Return size changed dirty flag of Tracked vector
template<typename T>
bool is_size_dirty(const Tracked<std::vector<T>>& t)
{
  return t.is_size_dirty();
}

/// Clear dirty flag at index of Tracked vector
template<typename T>
void clear_dirty(Tracked<std::vector<T>>& t, size_t i)
{
  t.clear_dirty(i);
}

/// Get value at index of Tracked vector
template<typename T>
const T& get_value(const Tracked<std::vector<T>>& t, size_t i)
{
  return t.get(i);
}

/// Set value at index of Tracked vector
template<typename T>
void set_value(Tracked<std::vector<T>>& t, size_t i, const T& v)
{
  t.set(i, v);
}

typedef Tracked<std::vector<int64_t>> TrackedIntVector;
typedef Tracked<std::vector<double>> TrackedDoubleVector;
}
}
}  // end namespace knowledge::rcw

#endif  // INCL_MADARA_RCW_TRACKED_H
