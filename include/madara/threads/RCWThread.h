

#ifndef INCL_MADARA_THREADS_RCWTHREAD_H
#define INCL_MADARA_THREADS_RCWTHREAD_H

/**
 * @file RCWThread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * Base class for Run-Compute-Write threads
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
#include "madara/knowledge/containers/Collection.h"
#include "madara/threads/BaseThread.h"

namespace madara
{
  namespace rcw
  {
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

    template<class T, class Impl>
    class TrackedCollection;

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
      Tracked() : val_(), dirty_(true) {}
      explicit Tracked(T val) : val_(val), dirty_(true) {}

      const T& get() const
      {
        return val_;
      }

      explicit operator const T& () const
      {
        return get();
      }

      T& get_mut()
      {
        modify();
        return val_;
      }

      void set(T val)
      {
        modify();
        val_ = val;
      }

      const T& operator*() const
      {
        return this->get();
      }

      const T* operator->() const
      {
        return &this->get();
      }

      Tracked &operator=(T val)
      {
        this->set(val);
        return *this;
      }

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

    template<typename T>
    const T &get_value(const T &t)
    {
      return t;
    }

    template<typename T>
    void set_value(T &t, const T &v)
    {
      t = v;
    }

    template<typename T>
    auto get_value(const std::vector<T> &t, size_t i) ->
      decltype(get_value(t[i]))
    {
      return get_value(t[i]);
    }

    template<typename T, typename V>
    void set_value(std::vector<T> &t, size_t i, V v)
    {
      set_value(t[i], v);
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
        std::cout << "push_back " << value << std::endl;
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

    template<class T, bool RD = true, bool WR = true, class dummy = void>
    class Tracker
    {
      static_assert(sizeof(T) < 0, "Type unsupported for adding to Transaction");
    };

    template<class T, bool RD = true, bool WR = true, class dummy = void>
    class PrefixTracker
    {
      static_assert(sizeof(T) < 0, "Type unsupported for adding to Transaction with prefix");
    };

    class BaseTracker
    {
    public:
      virtual ~BaseTracker() {}

    private:
      knowledge::VariableReference ref_;

      BaseTracker(knowledge::VariableReference ref) : ref_(ref) {}

      virtual void pull() = 0;
      virtual void push(knowledge::KnowledgeBase &kb) = 0;
      virtual void force_push(knowledge::KnowledgeBase &kb) = 0;

      const knowledge::KnowledgeRecord &get() const
      {
        return get(ref_);
      }

      knowledge::KnowledgeRecord &get_mut()
      {
        return get_mut(ref_);
      }

      static const knowledge::KnowledgeRecord &get(const knowledge::VariableReference &ref)
      {
        return *ref.record_;
      }

      static knowledge::KnowledgeRecord &get_mut(const knowledge::VariableReference &ref)
      {
        return *ref.record_;
      }

      void set(knowledge::KnowledgeBase &kb, knowledge::KnowledgeRecord rec)
      {
        return set(kb, ref_, rec);
      }

      static void set(knowledge::KnowledgeBase &kb, const knowledge::VariableReference &ref, knowledge::KnowledgeRecord rec)
      {
        kb.get_context().set_unsafe_impl(ref, rec, knowledge::EvalSettings());
      }

      template<typename I>
      auto set_index(knowledge::KnowledgeBase &kb, size_t idx, I val) ->
        typename std::enable_if<std::is_integral<I>::value>::type
      {
        kb.get_context().set_index_unsafe_impl(ref_, idx, val, knowledge::EvalSettings());
      }

      template<typename I>
      auto set_index(knowledge::KnowledgeBase &kb, size_t idx, I val) ->
        typename std::enable_if<std::is_floating_point<I>::value>::type
      {
        kb.get_context().set_index_unsafe_impl(ref_, idx, val, knowledge::EvalSettings());
      }

      void post_set(knowledge::KnowledgeBase &kb)
      {
        post_set(kb, ref_);
      }

      void post_set(knowledge::KnowledgeBase &kb, const knowledge::VariableReference &ref)
      {
        kb.get_context().mark_and_signal(ref.name_.get_ptr (), ref.record_, knowledge::EvalSettings());
      }

      friend class Transaction;

      template<class, bool, bool, class>
      friend class Tracker;

      template<class, bool, bool, class>
      friend class PrefixTracker;
    };

    #define MADARA_MAKE_SUPPORT_TEST(name, var, expr) template <typename T> \
    struct supports_##name##_impl { \
        template<typename U> static auto test(U *var) -> decltype((expr), std::true_type()); \
        template<typename U> static auto test(...) -> std::false_type; \
        using type = decltype(test<T>(0)); \
    }; \
    template <typename T> struct supports_##name : supports_##name##_impl<T>::type {}

    MADARA_MAKE_SUPPORT_TEST(is_dirty, p, (is_dirty(*p), clear_dirty(*p)));
    MADARA_MAKE_SUPPORT_TEST(is_all_dirty, p, (is_all_dirty(*p), clear_dirty(*p)));
    MADARA_MAKE_SUPPORT_TEST(is_size_dirty, p, (is_size_dirty(*p), clear_dirty(*p)));
    MADARA_MAKE_SUPPORT_TEST(indexed_is_dirty, p, (is_dirty(*p, 0), clear_dirty(*p, 0)));

    MADARA_MAKE_SUPPORT_TEST(get_value, p, (set_value(*p, get_value(*p))));
    MADARA_MAKE_SUPPORT_TEST(indexed_get_value, p, (set_value(*p, 0, get_value(*p, 0))));
    MADARA_MAKE_SUPPORT_TEST(const_iter, p, (p->cbegin(), p->cend()));
    MADARA_MAKE_SUPPORT_TEST(size, p, (p->size()));

    MADARA_MAKE_SUPPORT_TEST(knowledge_cast, p, (
          knowledge::knowledge_cast(get_value(*p)),
          knowledge::knowledge_cast<decltype(get_value(*p))>(
            std::declval<knowledge::KnowledgeRecord>())));

    MADARA_MAKE_SUPPORT_TEST(self_eq, p, (
          get_value(*p) == get_value(*p),
          get_value(*p) != get_value(*p)));

    template<class T>
    class Tracker<T, false, false, void>
      : public BaseTracker
    {
    private:
      static_assert(sizeof(T) < 0, "Cannot create tracker that can neither read nor write");
    };

    template<class T>
    class Tracker<T, true, true, typename std::enable_if<
                       supports_get_value<T>::value &&
                       supports_knowledge_cast<T>::value &&
                       supports_self_eq<T>::value &&
                       !supports_is_dirty<T>::value>::type>
      : public BaseTracker
    {
    private:
      typedef typename std::decay<decltype(get_value(std::declval<T>()))>::type V;

      T *tracked_;
      V orig_;

      static const bool can_read = true;
      static const bool can_write = true;

      Tracker(T *tracked, knowledge::VariableReference ref)
        : BaseTracker(ref), tracked_(tracked), orig_() {}

      virtual void pull()
      {
        orig_ = knowledge::knowledge_cast<V>(get());
        set_value(*tracked_, orig_);
      }

      virtual void push(knowledge::KnowledgeBase &kb)
      {
        if (get_value(*tracked_) != get_value(orig_)) {
          Tracker::force_push(kb);
        }
      }

      virtual void force_push(knowledge::KnowledgeBase &kb)
      {
        set(kb, knowledge::knowledge_cast(get_value(*tracked_)));
        post_set(kb);
      }

      friend class Transaction;
    };

    template<class T>
    class Tracker<T, true, false, typename std::enable_if<
                       supports_get_value<T>::value &&
                       supports_knowledge_cast<T>::value>::type>
      : public BaseTracker
    {
    private:
      typedef typename std::decay<decltype(get_value(std::declval<T>()))>::type V;

      T *tracked_;

      static const bool can_read = true;
      static const bool can_write = false;

      Tracker(T *tracked, knowledge::VariableReference ref)
        : BaseTracker(ref), tracked_(tracked) {}

      virtual void pull()
      {
        set_value(*tracked_, knowledge::knowledge_cast<V>(get()));
      }

      virtual void push(knowledge::KnowledgeBase &kb) {}

      virtual void force_push(knowledge::KnowledgeBase &kb) {}

      friend class Transaction;
    };

    template<class T>
    class Tracker<T, false, true, typename std::enable_if<
                       supports_get_value<T>::value &&
                       supports_knowledge_cast<T>::value &&
                       supports_self_eq<T>::value &&
                       !supports_is_dirty<T>::value>::type>
      : public BaseTracker
    {
    private:
      typedef typename std::decay<decltype(get_value(std::declval<T>()))>::type V;

      T *tracked_;

      static const bool can_read = false;
      static const bool can_write = true;

      Tracker(T *tracked, knowledge::VariableReference ref)
        : BaseTracker(ref), tracked_(tracked) {}

      virtual void pull()
      {
        set_value(*tracked_, V());
      }

      virtual void push(knowledge::KnowledgeBase &kb)
      {
        if (get_value(*tracked_) != V()) {
          Tracker::force_push(kb);
        }
      }

      virtual void force_push(knowledge::KnowledgeBase &kb)
      {
        set(kb, knowledge::knowledge_cast(get_value(*tracked_)));
        post_set(kb);
      }

      friend class Transaction;
    };

    template<class T>
    class Tracker<T, true, true, typename std::enable_if<
                       supports_get_value<T>::value &&
                       supports_knowledge_cast<T>::value &&
                       supports_is_dirty<T>::value &&
                       !supports_indexed_is_dirty<T>::value>::type >
      : public BaseTracker
    {
    private:
      T *tracked_;

      static const bool can_read = true;
      static const bool can_write = true;

      typedef typename std::decay<decltype(get_value(std::declval<T>()))>::type V;

      Tracker(T *tracked, knowledge::VariableReference ref)
        : BaseTracker(ref), tracked_(tracked) {}

      virtual void pull()
      {
        V val = knowledge::knowledge_cast<V>(get());
        set_value(*tracked_, val);
        clear_dirty(*tracked_);
      }

      virtual void push(knowledge::KnowledgeBase &kb)
      {
        if (is_dirty(*tracked_)) {
          Tracker::force_push(kb);
        }
      }

      virtual void force_push(knowledge::KnowledgeBase &kb)
      {
        set(kb, knowledge::knowledge_cast(get_value(*tracked_)));
        post_set(kb);
      }

      friend class Transaction;
    };

    template<class T>
    class Tracker<T, false, true, typename std::enable_if<
                       supports_get_value<T>::value &&
                       supports_knowledge_cast<T>::value &&
                       supports_is_dirty<T>::value &&
                       !supports_indexed_is_dirty<T>::value>::type >
      : public BaseTracker
    {
    private:
      T *tracked_;

      typedef typename std::decay<decltype(get_value(std::declval<T>()))>::type V;

      static const bool can_read = false;
      static const bool can_write = true;

      Tracker(T *tracked, knowledge::VariableReference ref)
        : BaseTracker(ref), tracked_(tracked) {}

      virtual void pull()
      {
        set_value(*tracked_, V());
        clear_dirty(*tracked_);
      }

      virtual void push(knowledge::KnowledgeBase &kb)
      {
        if (is_dirty(*tracked_)) {
          Tracker::force_push(kb);
        }
      }

      virtual void force_push(knowledge::KnowledgeBase &kb)
      {
        set(kb, knowledge::knowledge_cast(get_value(*tracked_)));
        post_set(kb);
      }

      friend class Transaction;
    };

    template<class T, bool RD, bool WR>
    class Tracker<T, RD, WR, typename std::enable_if<
                       supports_indexed_get_value<T>::value &&
                       supports_size<T>::value &&
                       !supports_is_all_dirty<T>::value &&
                       !supports_is_size_dirty<T>::value &&
                       supports_indexed_is_dirty<T>::value>::type >
      : public BaseTracker
    {
    private:
      static_assert(RD == true, "Write-only not supported for containers of tracked items");

      T *tracked_;
      size_t orig_size_;

      typedef typename std::decay<decltype(get_value(std::declval<T>(), 0))>::type V;

      static const bool can_read = RD;
      static const bool can_write = WR;

      Tracker(T *tracked, knowledge::VariableReference ref)
        : BaseTracker(ref), tracked_(tracked), orig_size_() {}

      virtual void pull()
      {
        std::vector<double> val = get().to_doubles();
        orig_size_ = val.size();
        size_t n = orig_size_;
        tracked_->resize(n);
        for (size_t i = 0; i < n; ++i) {
          set_value(*tracked_, i, (V) val[i]);
          clear_dirty(*tracked_, i);
        }
      }

      virtual void push(knowledge::KnowledgeBase &kb)
      {
        if (can_write) {
          size_t n = tracked_->size();
          if (tracked_->size() != orig_size_) {
            get_mut().resize(n);
          }
          for (size_t i = 0; i < n; ++i) {
            if (is_dirty(*tracked_, i)) {
              set_index(kb, i, get_value(*tracked_, i));
            }
          }
          post_set(kb);
        }
      }

      virtual void force_push(knowledge::KnowledgeBase &kb)
      {
        if (can_write) {
          size_t n = tracked_->size();
          if (tracked_->size() != orig_size_) {
            get_mut().resize(n);
          }
          for (size_t i = 0; i < n; ++i) {
            set_index(kb, i, get_value(*tracked_, i));
          }
          post_set(kb);
        }
      }

      friend class Transaction;
    };

    template<class T, bool RD, bool WR>
    class Tracker<T, RD, WR, typename std::enable_if<
                       supports_get_value<T>::value &&
                       supports_indexed_get_value<T>::value &&
                       supports_size<T>::value &&
                       supports_knowledge_cast<T>::value &&
                       supports_is_dirty<T>::value &&
                       supports_is_all_dirty<T>::value &&
                       supports_is_size_dirty<T>::value &&
                       supports_indexed_is_dirty<T>::value>::type >
      : public BaseTracker
    {
    private:
      T *tracked_;

      typedef typename std::decay<decltype(get_value(std::declval<T>()))>::type V;

      static const bool can_read = RD;
      static const bool can_write = WR;

      Tracker(T *tracked, knowledge::VariableReference ref)
        : BaseTracker(ref), tracked_(tracked) {}

      virtual void pull()
      {
        if (can_read) {
          V val = knowledge::knowledge_cast<V>(get());
          set_value(*tracked_, val);
        } else {
          set_value(*tracked_, V());
        }
        clear_dirty(*tracked_);
      }

      virtual void push(knowledge::KnowledgeBase &kb)
      {
        if (can_write) {
          if (is_all_dirty(*tracked_)) {
            return Tracker::force_push(kb);
          }
          size_t n = tracked_->size();
          if (is_size_dirty(*tracked_) && n < get().size()) {
            get_mut().resize(n);
          }
          for (size_t i = 0; i < n; ++i) {
            if (is_dirty(*tracked_, i)) {
              set_index(kb, i, get_value(*tracked_, i));
            }
          }
          post_set(kb);
        }
      }

      virtual void force_push(knowledge::KnowledgeBase &kb)
      {
        if (can_write) {
          set(kb, knowledge::knowledge_cast(get_value(*tracked_)));
          post_set(kb);
        }
      }

      friend class Transaction;
    };

    template<class T>
    class PrefixTracker<T, false, false, void>
      : public BaseTracker
    {
    private:
      static_assert(sizeof(T) < 0, "Cannot create prefix tracker that can neither read nor write");
    };

    template<class T, bool RD, bool WR>
    class PrefixTracker<T, RD, WR, typename std::enable_if<
                       supports_get_value<T>::value &&
                       supports_indexed_get_value<T>::value &&
                       supports_size<T>::value &&
                       //supports_knowledge_cast<T>::value &&
                       supports_is_dirty<T>::value &&
                       supports_is_all_dirty<T>::value &&
                       supports_is_size_dirty<T>::value &&
                       supports_indexed_is_dirty<T>::value>::type >
      : public BaseTracker
    {
    private:
      T *tracked_;
      std::string prefix_;
      knowledge::KnowledgeBase kb_;
      std::vector<knowledge::VariableReference> elems_;

      typedef typename std::decay<decltype(get_value(std::declval<T>()[0]))>::type V;

      static const bool can_read = RD;
      static const bool can_write = WR;

      PrefixTracker(T *tracked, const std::string &prefix, knowledge::KnowledgeBase &kb)
        : BaseTracker(kb.get_ref(prefix + ".size")),
          tracked_(tracked), prefix_(prefix), kb_(kb), elems_()
      {
        update_elems();
      }

      void update_elems()
      {
        const size_t n = tracked_->size();
        if (elems_.size() == n)
          return;
        if (elems_.size() > n) {
          elems_.resize(n);
          return;
        }
        std::ostringstream name;
        elems_.reserve(n);
        name << prefix_ << ".";
        const auto pos = name.tellp();
        for (size_t i = elems_.size(); i < n; ++i) {
          name << i;
          elems_.push_back(kb_.get_ref(name.str()));
          name.seekp(pos);
        }
      }

      virtual void pull()
      {
        const size_t n = get().to_integer();
        tracked_->resize(n);
        update_elems();
        for (size_t i = 0; i < n; ++i) {
          if (can_read) {
            V val = knowledge::knowledge_cast<V>(get(elems_[i]));
            set_value(*tracked_, i, val);
          } else {
            set_value(*tracked_, i, V());
          }
        }
        clear_dirty(*tracked_);
      }

      virtual void push(knowledge::KnowledgeBase &kb)
      {
        if (can_write) {
          const size_t n = tracked_->size();
          if (is_all_dirty(*tracked_)) {
            return force_push(kb);
          }
          if (is_size_dirty(*tracked_)) {
            set(kb, (int64_t)n);
            post_set(kb);
          }

          update_elems();
          for (size_t i = 0; i < n; ++i) {
            if (is_dirty(*tracked_, i)) {
              set(kb, elems_[i], knowledge::knowledge_cast(get_value(*tracked_, i)));
              post_set(kb, elems_[i]);
            }
          }
        }
      }

      virtual void force_push(knowledge::KnowledgeBase &kb)
      {
        if (can_write) {
          const size_t n = tracked_->size();
          set(kb, (int64_t)n);
          post_set(kb);

          update_elems();
          for (size_t i = 0; i < n; ++i) {
            set(kb, elems_[i], knowledge::knowledge_cast(tracked_->at(i)));
            post_set(kb, elems_[i]);
          }
        }
      }

      friend class Transaction;
    };

    template<typename T>
    struct type {};

    template<class Base, size_t N>
    class PolyStore
    {
    private:
      char store_[N];
    public:
      template<typename Derived, typename... Args>
      PolyStore(type<Derived>, Args&&... args) {
        static_assert(sizeof(Derived) <= N, "Type too big for this PolyStore");
        static_assert(std::is_base_of<Base, Derived>::value, "Type is not derived from Base type of this PolyStore");
        new(&store_) Derived(std::forward<Args>(args)...);
      }

      const Base& operator*() const { return *(const Base*)(const void*)(&store_); }
      Base& operator*() { return *(Base*)(void*)(&store_); }
      const Base* operator->() const { return &operator*(); }
      Base* operator->() { return &operator*(); }

      ~PolyStore() {
        (*this)->~Base();
      }
    };

    class Transaction
    {
    private:
      typedef std::unique_ptr<BaseTracker> uptr_t;
      knowledge::KnowledgeBase kb_;
      std::vector<uptr_t> trackers_;

    public:
      Transaction(const knowledge::KnowledgeBase &kb) : kb_(kb), trackers_() {}

      void push(void)
      {
        knowledge::ContextGuard guard(kb_);
        for (auto &t : trackers_) {
          t->push(kb_);
        }
      }

      void pull(void)
      {
        knowledge::ContextGuard guard(kb_);
        for (auto &t : trackers_) {
          t->pull();
        }
      }

    private:
      template<class Builder, class T, class dummy = void>
      class BuilderBase
      {
      public:
        Builder &init();
      };

      template<class Builder, class T>
      class BuilderBase<Builder, T, 
          typename std::enable_if<
                       supports_get_value<T>::value &&
                       supports_indexed_get_value<T>::value &&
                       supports_size<T>::value>::type>
      {
        typedef typename std::decay<decltype(get_value(std::declval<T>()[0]))>::type V;
      public:
        Builder &init(std::initializer_list<V> list)
        {
          Builder &self = static_cast<Builder &>(*this);
          *self.tracked_ = list;
          return self.init();
        }
      };

    public:
      template<class T, bool RD = true, bool WR = true, bool Prefix = false>
      class Builder : private BuilderBase<Builder<T, RD, WR, Prefix>, T>
      {
      private:
        Transaction *trans_;
        const char *key_;
        T *tracked_;
        bool init_ = false;
        static const bool rd_ = RD;
        static const bool wr_ = WR;
        static const bool prefix_ = Prefix;

        Builder(Transaction &trans, const char *key, T &val)
          : trans_(&trans), key_(key), tracked_(&val) {}

        template<bool R, bool W, bool P>
        Builder(Builder<T, R, W, P> &o)
          : trans_(o.trans_), key_(o.key_), tracked_(o.tracked_),
            init_(o.init_) {}

      public:
        Builder &init() { init_ = true; return *this; }

        template<class V>
        Builder &init(V &&val)
        {
          *tracked_ = std::forward<V>(val);
          return init();
        }

        using BuilderBase<Builder, T>::init;
        friend class BuilderBase<Builder, T>;

        Builder<T, RD, WR, true> prefix() { return *this; }
        Builder<T, false, WR, Prefix> ro() { return *this; }
        Builder<T, RD, false, Prefix> wo() { return *this; }

        void add()
        {
          trans_->add(*this);
        }

        friend class Transaction;
      };

      template<class T, bool RD, bool WR, bool Prefix> friend class Builder;

      template<class T, class S>
      Builder<T> build(S &&key, T &val)
      {
        return Builder<T>(*this, std::forward<S>(key), val);
      }

      template<class T, class S>
      Builder<T, true, false>  build_ro(S &&key, T &val)
      {
        return build(std::forward<S>(key), val).ro();
      }

      template<class T, class S>
      Builder<T, false, true> build_wo(S &&key, T &val)
      {
        return build(std::forward<S>(key), val).wo();
      }

      template<class T, bool RD, bool WR>
      void add(const Builder<T, RD, WR, true> &b)
      {
        std::unique_ptr<PrefixTracker<T, RD, WR>> p(new PrefixTracker<T, RD, WR>(b.tracked_, b.key_, kb_));
        if (b.init_) {
          p->PrefixTracker<T, RD, WR>::force_push(kb_);
        }
        trackers_.push_back(uptr_t(p.release()));
      }

      template<class T, bool RD, bool WR>
      void add(const Builder<T, RD, WR, false> &b)
      {
        knowledge::VariableReference ref = kb_.get_ref(b.key_);
        std::unique_ptr<Tracker<T, RD, WR>> p(new Tracker<T, RD, WR>(b.tracked_, ref));
        if (b.init_) {
          p->Tracker<T, RD, WR>::force_push(kb_);
        }
        trackers_.push_back(uptr_t(p.release()));
      }

      template<class T>
      void add(const char *key, T &val)
      {
        build(key, val).add();
      }

      template<class T>
      void add_init(const char *key, T &val)
      {
        build(key, val).init().add();
      }

      template<class T>
      void add_ro(const char *key, T &val)
      {
        build_ro(key, val).add();
      }
 
      template<class T>
      void add_wo(const char *key, T &val)
      {
        build_wo(key, val).add();
      }
 
      template<class T>
      void add_wo_init(const char *key, T &val)
      {
        build_wo(key, val).init().add();
      }

      template<class T>
      void add_prefix(const char *prefix, T &val)
      {
        build(prefix, val).prefix().add();
      }

      template<class T>
      void add_prefix_init(const char *prefix, T &val)
      {
        build(prefix, val).prefix().init().add();
      }

      template<class T>
      void add_prefix_ro(const char *prefix, T &val)
      {
        build_ro(prefix, val).prefix().add();
      }

      template<class T>
      void add_prefix_wo(const char *prefix, T &val)
      {
        build_wo(prefix, val).prefix().add();
      }

      template<class T>
      void add_prefix_init_wo(const char *prefix, T &val)
      {
        build_wo(prefix, val).prefix().init().add();
      }
    };
  }

  namespace threads
  {
    /**
     * Abstract base class for implementing RCW threads
     *
     * Users should override setup, compute, and finalize, but should
     * not override the init, run, and cleanup functions from BaseThread
     **/
    class RCWThread : public BaseThread
    {
    public:
      /**
       * Destructor
       **/
      virtual ~RCWThread ()
      {
      }

      /**
       * Worker threads are MADARA thread entities that manage
       * user-defined threads
       **/
      friend class WorkerThread;
      friend class Threader;

      /**
       **/
      virtual void setup (rcw::Transaction &tx)
      {
      }

      /**
       **/
      virtual void compute (rcw::Transaction &tx) = 0;

      /**
       * Cleans up any thread residue (usually instances created in setup).
       * It's important to note that the only things that would need to
       * be cleaned up are generally ports opened, memory initialized with
       * new/malloc, etc. Anything MADARA related is generally cleaned up
       * for you.
       **/
      virtual void finalize (rcw::Transaction &tx)
      {
      }

    private:
      /**
       * Initializes thread with a MADARA context
       **/
      virtual void init (knowledge::KnowledgeBase &kb)
      {
        tx_.reset(new rcw::Transaction(kb));
        setup(*tx_);
      }

      /**
       * Executes the main thread logic. In a BaseThread, the
       * execute is called once. Afterwards, cleanup is called.
       **/
      virtual void run (void) 
      {
        tx_->pull();
        compute(*tx_);
        tx_->push();
      }

      /**
       * Cleans up any thread residue (usually instances created in init).
       * It's important to note that the only things that would need to
       * be cleaned up are generally ports opened, memory initialized with
       * new/malloc, etc. Anything MADARA related is generally cleaned up
       * for you.
       **/
      virtual void cleanup (void)
      {
        finalize(*tx_);
      }

      std::unique_ptr<rcw::Transaction> tx_;
    };
  }
}

#endif  // INCL_MADARA_THREADS_RCWTHREAD_H
