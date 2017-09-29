

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
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/Functions.h"
#include "madara/utility/stdint.h"
#include "madara/MADARA_export.h"
#include "madara/knowledge/VariableReference.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/knowledge_cast.h"
#include "madara/knowledge/containers/Collection.h"
#include "madara/threads/BaseThread.h"

namespace madara
{
  namespace rcw
  {
    template<class T>
    class Tracked
    {
    private:
      T val_;
      char dirty_;
    public:
      Tracked() : val_(), dirty_(false) {}
      explicit Tracked(T val) : val_(val), dirty_(false) {}

      const T& get() const
      {
        return val_;
      }

      T& get_mut()
      {
        modify();
        return val_;
      }

      const T& operator*() const
      {
        return get();
      }

      const T* operator->() const
      {
        return &get();
      }

      void set(T val)
      {
        modify();
        val_ = val;
      }

      Tracked &operator=(T val)
      {
        set(val);
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
    };

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
    const T &get_value(const Tracked<T> &t)
    {
      return t.get();
    }

    template<typename T>
    void set_value(Tracked<T> &t, T &v)
    {
      return t.set(v);
    }

    /*
    template<typename T>
    class Tracked<std::vector<T>>
    {
    private:
      std::vector<T> val_;
      std::vector<char> dirty_;
    public:
      Tracked() : val_(), dirty_() {}
      explicit Tracked(std::vector<T> val) : val_(val), dirty_(val.size()) {}

      const std::vector<T>& get() const
      {
        return val_;
      }

      const T &at(size_t i) const
      {
        return val_.at(i);
      }

      const T &get(size_t i) const
      {
        return at(i);
      }

      const T &operator[](size_t i) const
      {
        return val_[i];
      }

      const std::vector<T>& operator*() const
      {
        return get();
      }

      const std::vector<T>* operator->() const
      {
        return &get();
      }

      void set(const &std::vector<T> val)
      {
        val_ = val;
        dirty_.resize(val_.size());
        modify();
      }

      void set(T val, size_t i)
      {
        *dirty_.at(i) = true;
        *val_.at(i) = val;
      }

      Tracked &operator=(const &std::vector<T> val)
      {
        set(val);
      }

      void modify()
      {
        for (char &cur : dirty_)
          *cur = true;
      }

      bool is_dirty()
      {
        for (const char &cur : dirty_)
          if (cur)
            return true;
        return false;
      }

      void clear_dirty()
      {
        for (char &cur : dirty_)
          *cur = true;
      }
    }
    */

    class BaseTracker
    {
    public:
      virtual ~BaseTracker() {}

    private:
      virtual void pull() = 0;
      virtual void push() = 0;

      friend class Transaction;
    };

    template <typename T>
    struct supports_is_dirty_impl
    {
        template<typename U>
        static auto test(U *p) -> decltype(is_dirty(*p), clear_dirty(*p), set_value(*p, get_value(*p)), std::true_type());
        template<typename U>
        static auto test(...) -> std::false_type;

        using type = decltype(test<T>(0));
    };

    template <typename T>
    struct supports_is_dirty : supports_is_dirty_impl<T>::type {};

    template<class T, class dummy = void>
    class Tracker : public BaseTracker
    {
    private:
      T *tracked_;
      knowledge::VariableReference ref_;
      T orig_;

      Tracker(T *tracked, knowledge::VariableReference ref)
        : tracked_(tracked), ref_(ref), orig_() {}

      virtual void pull()
      {
        orig_ = knowledge::knowledge_cast<T>(*ref_.record_);
        *tracked_ = orig_;
      }

      virtual void push()
      {
        if (*tracked_ != orig_) {
          fprintf(stderr, "pushing %s\n", ref_.get_name());
          *ref_.record_ = knowledge::knowledge_cast(*tracked_);
        }
      }

      friend class Transaction;
    };

    template<class T>
    class Tracker<T, typename std::enable_if<supports_is_dirty<T>::value>::type> : public BaseTracker
    {
    private:
      T *tracked_;
      knowledge::VariableReference ref_;

      typedef typename std::decay<decltype(get_value(*tracked_))>::type V;

      Tracker(T *tracked, knowledge::VariableReference ref)
        : tracked_(tracked), ref_(ref) {}

      virtual void pull()
      {
        V val = knowledge::knowledge_cast<V>(*ref_.record_);
        set_value(*tracked_, val);
        clear_dirty(*tracked_);
      }

      virtual void push()
      {
        if (is_dirty(*tracked_)) {
          *ref_.record_ = knowledge::knowledge_cast(get_value(*tracked_));
        }
      }

      friend class Transaction;
    };

    template<class T>
    struct ValGetter
    {
      static const T& get(const T& v) { return v; }
    };

    template<class T>
    struct ValGetter<Tracked<T>>
    {
      static const T& get(const Tracked<T>& v) { return *v; }
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
        for (auto &t : trackers_) {
          t->push();
        }
      }

      void pull(void)
      {
        for (auto &t : trackers_) {
          t->pull();
        }
      }

      template<class T>
      void add(const std::string &key, T &val)
      {
        knowledge::VariableReference ref = kb_.get_ref(key);
        trackers_.push_back(uptr_t(new Tracker<T>(&val, ref)));
      }

      template<class T>
      void add_init(const std::string &key, T &val)
      {
        knowledge::VariableReference ref = kb_.get_ref(key);
        kb_.set(ref, ValGetter<T>::get(val));
        trackers_.push_back(uptr_t(new Tracker<T>(&val, ref)));
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
