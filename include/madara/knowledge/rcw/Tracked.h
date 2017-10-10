

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
#include "madara/utility/stdint.h"
#include "madara/MADARA_export.h"
#include "madara/knowledge/VariableReference.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/knowledge_cast.h"
#include "madara/knowledge/ContextGuard.h"

namespace madara { namespace knowledge { namespace rcw
{
  /// Used internally by Tracked
  template<class T, class Impl>
  class TrackedExtra
  {
  public:
    const T& get() const;
    T& get_mut();
    void set(T val);

    void modify();
    bool is_dirty() const;
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
    T val_;
    char dirty_;

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
    explicit operator const T& () const
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
    Tracked &operator=(T val)
    {
      this->set(val);
      return *this;
    }

    Tracked &operator=(const Tracked &) = delete;

    void modify()
    {
      dirty_ = true;
    }

    bool is_dirty() const
    {
      return dirty_;
    }

    void clear_dirty()
    {
      dirty_ = false;
    }

    using Base::get;
    using Base::get_mut;
    using Base::set;
    using Base::modify;
    using Base::is_dirty;
    using Base::clear_dirty;

    explicit operator bool()
    {
      return val_ ? true : false;
    }

    template<class U, typename std::enable_if<std::is_convertible<T, U>::value, void*>::type = nullptr>
    operator Tracked<U>() const
    {
      Tracked<U> ret(get());
      if (!is_dirty()) ret.clear_dirty();
      return ret;
    }

    friend void swap(Tracked &lhs, Tracked &rhs)
    {
      using std::swap;
      swap(lhs.val_, rhs.val_);
      swap(lhs.dirty_, rhs.dirty_);
    }

    friend void swap(Tracked &lhs, T &rhs)
    {
      using std::swap;
      swap(lhs.val_, rhs);
      lhs.dirty_ = true;
    }

    friend void swap(T &lhs, Tracked &rhs)
    {
      using std::swap;
      swap(lhs, rhs.val_);
      rhs.dirty_ = true;
    }
  };

  template<class T>
  Tracked<typename std::decay<T>::type> track(T &&val)
  {
    return Tracked<typename std::decay<T>::type>(std::forward<T>(val));
  }

#define MADARA_AUTOTYPE_BODY(body) \
  -> decltype(body) { return (body); }

#define MADARA_MAKE_BINARY_COMPARE_OP(op) \
  template<class T, class U> \
  auto operator op(const Tracked<T> &lhs, const Tracked<U> &rhs) \
    MADARA_AUTOTYPE_BODY(lhs.get() op rhs.get()) \
  template<class T, class U> \
  auto operator op(const Tracked<T> &lhs, U &&rhs) \
    MADARA_AUTOTYPE_BODY(lhs.get() op std::forward<U>(rhs)) \
  template<class T, class U> \
  auto operator op(T &&lhs, const Tracked<U> &rhs) \
    MADARA_AUTOTYPE_BODY(std::forward<T>(lhs) op rhs.get())

  MADARA_MAKE_BINARY_COMPARE_OP(==)
  MADARA_MAKE_BINARY_COMPARE_OP(!=)
  MADARA_MAKE_BINARY_COMPARE_OP(<=)
  MADARA_MAKE_BINARY_COMPARE_OP(>=)
  MADARA_MAKE_BINARY_COMPARE_OP(<)
  MADARA_MAKE_BINARY_COMPARE_OP(>)

#define MADARA_MAKE_COMPOUND_OP(op) \
  template<class T, class U> \
  auto operator op##=(Tracked<T> &lhs, const Tracked<U> &rhs) \
    MADARA_AUTOTYPE_BODY(lhs.get_mut() op##= rhs.get()) \
  template<class T, class U> \
  auto operator op##=(Tracked<T> &lhs, U &&rhs) \
    MADARA_AUTOTYPE_BODY(lhs.get_mut() op##= std::forward<U>(rhs)) \
  template<class T, class U> \
  auto operator op##=(T &lhs, const Tracked<U> &rhs) \
    MADARA_AUTOTYPE_BODY(lhs op##= rhs.get())

#define MADARA_MAKE_BINARY_ARITH_OP(op) \
  template<class T, class U> \
  auto operator op(const Tracked<T> &lhs, const Tracked<U> &rhs) \
    MADARA_AUTOTYPE_BODY(lhs.get() op rhs.get()) \
  template<class T, class U> \
  auto operator op(const Tracked<T> &lhs, U &&rhs) \
    MADARA_AUTOTYPE_BODY(lhs.get() op std::forward<U>(rhs)) \
  template<class T, class U> \
  auto operator op(T &&lhs, const Tracked<U> &rhs) \
    MADARA_AUTOTYPE_BODY(std::forward<T>(lhs) op rhs.get()) \
  MADARA_MAKE_COMPOUND_OP(op)

  MADARA_MAKE_BINARY_ARITH_OP(+)
  MADARA_MAKE_BINARY_ARITH_OP(-)
  MADARA_MAKE_BINARY_ARITH_OP(*)
  MADARA_MAKE_BINARY_ARITH_OP(/)
  MADARA_MAKE_BINARY_ARITH_OP(%)
  MADARA_MAKE_BINARY_ARITH_OP(&)
  MADARA_MAKE_BINARY_ARITH_OP(|)
  MADARA_MAKE_BINARY_ARITH_OP(^)
  MADARA_MAKE_BINARY_ARITH_OP(<<)
  MADARA_MAKE_BINARY_ARITH_OP(>>)

#define MADARA_MAKE_UNARY_ARITH_OP(op) \
  template<class T> \
  auto operator op(const Tracked<T> &lhs) \
    MADARA_AUTOTYPE_BODY(op lhs.get())

  MADARA_MAKE_UNARY_ARITH_OP(+);
  MADARA_MAKE_UNARY_ARITH_OP(-);
  MADARA_MAKE_UNARY_ARITH_OP(!);
  MADARA_MAKE_UNARY_ARITH_OP(~);

#define MADARA_MAKE_INCDEC_OP(op) \
  template<class T> auto operator op(Tracked<T> &lhs) \
    MADARA_AUTOTYPE_BODY(op lhs.get_mut()) \
  template<class T> auto operator op(Tracked<T> &lhs, int) \
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

  template<typename T, typename Impl>
  class TrackedCollection
  {
  private:
    Impl &impl() { return static_cast<Impl&>(*this); }
    const Impl &impl() const { return static_cast<const Impl&>(*this); }

  public:
    typedef typename T::value_type value_type;
    typedef typename T::const_iterator const_iterator;

    void set(size_t i, value_type val)
    {
      impl().val_.at(i) = val;
      impl().modify(i);
    }

    const value_type &at(size_t i) const
    {
      return impl().val_.at(i);
    }

    const value_type &operator[](size_t i) const
    {
      return impl().val_[i];
    }

    const value_type &get(size_t i) const
    {
      return at(i);
    }

    value_type &at_mut(size_t i)
    {
      impl().modify(i);
      return impl().val_.at(i);
    }

    value_type &get_mut(size_t i)
    {
      impl().modify(i);
      return at_mut(i);
    }

    size_t size() { return impl().val_.size(); }
    bool empty() { return impl().val_.empty(); }
    size_t max_size() { return impl().val_.max_size(); }

    const value_type& front() const
    {
      return at(0);
    }

    const value_type& back() const
    {
      return at(size() - 1);
    }

    value_type& front_mut()
    {
      return at_mut(0);
    }
    value_type& back()
    {
      return at_mut(size() - 1);
    }

    const_iterator begin() const
    {
      return impl().val_.begin();
    }

    const_iterator cbegin() const
    {
      return begin();
    }

    const_iterator end() const
    {
      return impl().val_.end();
    }

    const_iterator cend() const
    {
      return end();
    }
  };

  template<typename Char, typename Impl>
  class TrackedExtra<std::basic_string<Char>, Impl> : public TrackedCollection<std::basic_string<Char>, Impl>
  {
  private:
    Impl &impl() { return static_cast<Impl&>(*this); }
    const Impl &impl() const { return static_cast<const Impl&>(*this); }
    template<class U, class I> friend class TrackedCollection;
  public:
    typedef typename std::basic_string<Char>::value_type value_type;
    typedef typename std::basic_string<Char>::const_iterator const_iterator;

    void modify(size_t i)
    {
      impl().modify();
    }

    bool is_dirty(size_t i) const
    {
      impl().is_dirty();
    }

    bool clear_dirty(size_t i) const
    {
      impl().clear_dirty();
    }

    void resize(size_t count)
    {
      impl().val_.resize(count);
      impl().modify();
    }

    size_t capacity() const
    {
      return impl().val_.capacity();
    }

    void reserve(size_t count)
    {
      impl().val_.reserve(count);
    }

    void push_back(const value_type &value)
    {
      impl().val_.push_back(value);
      impl().modify();
    }

    void pop_back()
    {
      impl().val_.pop_back();
      impl().modify();
    }

    void clear()
    {
      impl().val_.clear();
      impl().modify();
    }

    const value_type *c_str() const { return impl().val_.c_str(); }
    const value_type *data() const { return impl().val_.data(); }
  };

  typedef Tracked<std::string> TrackedString;

  template<typename T>
  bool is_dirty(const Tracked<T> &t)
  {
    return t.is_dirty();
  }

  template<typename T>
  void clear_dirty(Tracked<T> &t)
  {
    t.clear_dirty();
  }

  template<typename T>
  bool is_dirty(const std::vector<Tracked<T>> &t, size_t i)
  {
    return is_dirty(t[i]);
  }

  template<typename T>
  void clear_dirty(std::vector<Tracked<T>> &t, size_t i)
  {
    clear_dirty(t[i]);
  }

  template<typename T>
  const T &get_value(const Tracked<T> &t)
  {
    return t.get();
  }

  template<typename T>
  void set_value(Tracked<T> &t, const T &v)
  {
    t.set(v);
  }

  const char get_value(const Tracked<std::string> &t, size_t i)
  {
    return t.get(i);
  }

  void set_value(Tracked<std::string> &t, size_t i, char v)
  {
    t.set(i, v);
  }

  template<typename T>
  class Tracked<std::vector<T>> : public TrackedCollection<std::vector<T>, Tracked<std::vector<T>>>
  {
  private:
    typedef std::vector<T> Vec;
    typedef TrackedCollection<Vec, Tracked<std::vector<T>>> Base;
    Vec val_;
    std::vector<uint64_t> dirty_;
    bool all_dirty_;
    bool size_dirty_;

    static std::pair<size_t, uint64_t> to_dirty_bit(size_t i)
    {
      size_t idx = i >> 6;
      size_t shift = i - (idx << 6);
      return {idx, 1 << shift};
    }

    static size_t to_dirty_size(size_t i)
    {
      return (i + 63) >> 6;
    }

    friend class TrackedCollection<std::vector<T>, Tracked<std::vector<T>>>;

  public:
    typedef typename std::vector<T>::value_type value_type;
    typedef typename std::vector<T>::const_iterator const_iterator;

    Tracked() : val_(), dirty_() {}
    explicit Tracked(std::vector<T> val)
      : val_(val), dirty_(to_dirty_size(val.size())),
        size_dirty_(false), all_dirty_(false) {}

    const std::vector<T>& get() const
    {
      return val_;
    }

    std::vector<T>& get_mut()
    {
      modify();
      return val_;
    }

    const std::vector<T>& operator*() const
    {
      return get();
    }

    const std::vector<T>* operator->() const
    {
      return &get();
    }

    void set(const std::vector<T> &val)
    {
      val_ = val;
      dirty_.resize(to_dirty_size(val_.size()));
      modify();
    }

    Tracked &operator=(const std::vector<T> &val)
    {
      set(val);
      return *this;
    }

    void modify()
    {
      all_dirty_ = true;
    }

    bool is_all_dirty() const
    {
      return all_dirty_;
    }

    bool is_size_dirty() const
    {
      return size_dirty_;
    }

    bool is_dirty() const
    {
      if (all_dirty_)
        return true;
      for (const uint64_t &cur : dirty_)
        if (cur)
          return true;
      return false;
    }

    void clear_dirty()
    {
      all_dirty_ = false;
      size_dirty_ = false;
      for (uint64_t &cur : dirty_)
        cur = 0;
    }

    void modify(size_t i)
    {
      std::pair<size_t, uint64_t> x = to_dirty_bit(i);
      dirty_.at(x.first) |= x.second;
    }

    void clear_dirty(size_t i)
    {
      std::pair<size_t, uint64_t> x = to_dirty_bit(i);
      dirty_.at(x.first) &= ~x.second;
    }

    bool is_dirty(size_t i) const
    {
      std::pair<size_t, uint64_t> x = to_dirty_bit(i);
      return dirty_.at(x.first) & x.second;
    }

    void resize(size_t count)
    {
      val_.resize(count);
      dirty_.resize(to_dirty_size(val_.size()));
      all_dirty_ = true;
    }

    size_t capacity() const
    {
        size_t vc = val_.capacity();
        size_t dc = to_dirty_size(dirty_.capacity());
        return vc > dc ? dc : vc;
    }

    void reserve(size_t count)
    {
      val_.reserve(count);
      dirty_.reserve(to_dirty_size(count));
    }

    void push_back(const value_type &value)
    {
      val_.push_back(value);
      dirty_.resize(to_dirty_size(val_.size()));
      size_dirty_ = true;
      modify(val_.size() - 1);
    }

    void pop_back()
    {
      val_.pop_back();
      dirty_.resize(to_dirty_size(val_.size()));
      size_dirty_ = true;
    }

    void clear()
    {
      val_.clear();
      dirty_.clear();
      all_dirty_ = true;
    }

    using Base::get;
    using Base::get_mut;
    using Base::set;

    friend bool operator==(const Tracked &lhs, const Tracked &rhs) { return lhs.val_ == rhs.val_; }
    friend bool operator!=(const Tracked &lhs, const Tracked &rhs) { return lhs.val_ != rhs.val_; }
    friend bool operator<=(const Tracked &lhs, const Tracked &rhs) { return lhs.val_ <= rhs.val_; }
    friend bool operator>=(const Tracked &lhs, const Tracked &rhs) { return lhs.val_ >= rhs.val_; }
    friend bool operator<(const Tracked &lhs, const Tracked &rhs) { return lhs.val_ < rhs.val_; }
    friend bool operator>(const Tracked &lhs, const Tracked &rhs) { return lhs.val_ > rhs.val_; }

    friend void swap(Tracked &lhs, Tracked &rhs)
    {
      using std::swap;
      swap(lhs.val_, rhs.val_);
      swap(lhs.dirty_, rhs.dirty_);
      swap(lhs.all_dirty_, rhs.all_dirty_);
      swap(lhs.size_dirty_, rhs.size_dirty_);
    }
  };

  template<typename T>
  bool is_dirty(const Tracked<std::vector<T>> &t, size_t i)
  {
    return t.is_dirty(i);
  }

  template<typename T>
  bool is_all_dirty(const Tracked<std::vector<T>> &t)
  {
    return t.is_all_dirty();
  }

  template<typename T>
  bool is_size_dirty(const Tracked<std::vector<T>> &t)
  {
    return t.is_size_dirty();
  }

  template<typename T>
  void clear_dirty(Tracked<std::vector<T>> &t, size_t i)
  {
    t.clear_dirty(i);
  }

  template<typename T>
  const T &get_value(const Tracked<std::vector<T>> &t, size_t i)
  {
    return t.get(i);
  }

  template<typename T>
  void set_value(Tracked<std::vector<T>> &t, size_t i, const T &v)
  {
    t.set(i, v);
  }

  typedef Tracked<std::vector<int64_t>> TrackedIntVector;
  typedef Tracked<std::vector<double>> TrackedDoubleVector;
} } } // end namespace knowledge::rcw

#endif  // INCL_MADARA_RCW_TRACKED_H
