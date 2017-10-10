

#ifndef INCL_MADARA_RCW_TRANSACTION_H
#define INCL_MADARA_RCW_TRANSACTION_H

/**
 * @file Transaction.h
 * @author David Kyle <dskyle@sei.cmu.edu>
 *
 * Implements Transaction class
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
#include "Tracker.h"
#include "PrefixTracker.h"

namespace madara { namespace knowledge { namespace rcw
{
  class Transaction
  {
  public:
    typedef std::unique_ptr<BaseTracker> tracker_ptr;

    template<class T, bool Const = false, bool RD = true, bool WR = true,
             bool Prefix = false, bool Init = false>
    class Builder;

  private:
    typedef std::vector<tracker_ptr> trackers_vec;
    mutable KnowledgeBase kb_;
    mutable trackers_vec trackers_;

    template<class B, class I, class T, class dummy = void>
    class InitHandler
    {
    public:
      I init() const;
    };

    template<class B, class I, class T>
    class InitHandler<B, I, T, 
        typename std::enable_if<
                     supports_get_value<T>::value &&
                     supports_indexed_get_value<T>::value &&
                     supports_size<T>::value>::type>
    {
      typedef typename std::decay<decltype(get_value(std::declval<T>()[0]))>::type V;
    public:
      I init(std::initializer_list<V> list) const
      {
        const B &self = static_cast<const B &>(*this);
        *self.tracked_ = list;
        return self.init();
      }
    };

  public:
    Transaction(KnowledgeBase kb) : kb_(kb), trackers_() {}

    void push(void)
    {
      ContextGuard guard(kb_);
      for (auto &t : trackers_) {
        t->push(kb_);
      }
    }

    void pull(void)
    {
      ContextGuard guard(kb_);
      for (auto &t : trackers_) {
        t->pull();
      }
    }

    template<class T, bool Const, bool RD, bool WR, bool Prefix, bool Init>
    class Builder : private InitHandler<
                               Builder<T, Const, RD, WR, Prefix, Init>,
                               Builder<T, Const, RD, WR, Prefix, true>, T>
    {
    private:
      typedef typename std::conditional<Const, const Transaction *,
                                        Transaction *>::type trans_type;
      typedef decltype(*(trans_type)nullptr) trans_ref_type;
      const trans_type trans_;
      const char * const key_;
      T * const tracked_;
      static const bool rd_ = RD;
      static const bool wr_ = WR;
      static const bool prefix_ = Prefix;
      static const bool init_ = Init;

      Builder(trans_ref_type trans, const char *key, T &val)
        : trans_(&trans), key_(key), tracked_(&val) {}

      template<bool R, bool W, bool P, bool I>
      Builder(const Builder<T, Const, R, W, P, I> &o)
        : trans_(o.trans_), key_(o.key_), tracked_(o.tracked_) {}

      typedef Builder<T, Const, RD, WR, Prefix, true> init_type;
      typedef Builder<T, Const, RD, WR, true, Init> prefix_type;
      typedef Builder<T, Const, true, false, Prefix, Init> ro_type;
      typedef Builder<T, Const, false, true, Prefix, Init> wo_type;

    public:
      init_type init() const
      {
        static_assert(!Const, "Can't use init() on build() of const Transaction");
        return *this;
      }

      prefix_type prefix() const { return *this; }
      ro_type ro() const { return *this; }
      wo_type wo() const { return *this; }

      template<class V>
      init_type init(V &&val) const
      {
        *tracked_ = std::forward<V>(val);
        return init();
      }

      using InitHandler<Builder, init_type, T>::init;
      friend class InitHandler<Builder, init_type, T>;

      void add() const
      {
        trans_->add(*this);
      }

      friend class Transaction;
    };

    template<class T, bool Const, bool RD, bool WR, bool Prefix, bool Init> friend class Builder;

    template<class T, class S>
    Builder<T> build(S &&key, T &val)
    {
      return Builder<T>(*this, std::forward<S>(key), val);
    }

    template<class T, class S>
    Builder<T, true> build(S &&key, T &val) const
    {
      return Builder<T, true>(*this, std::forward<S>(key), val);
    }

    void add_tracker(tracker_ptr t) const
    {
      trackers_.emplace_back(std::move(t));
    }

    template<class T, bool RD, bool WR, bool Init>
    void add(const Builder<T, false, RD, WR, true, Init> &b)
    {
      std::unique_ptr<PrefixTracker<T, RD, WR>> p(new PrefixTracker<T, RD, WR>(b.tracked_, b.key_, kb_));
      if (b.init_) {
        p->PrefixTracker<T, RD, WR>::force_push(kb_);
      }
      add_tracker(tracker_ptr(p.release()));
    }

    template<class T, bool RD, bool WR, bool Init>
    void add(const Builder<T, false, RD, WR, false, Init> &b)
    {
      VariableReference ref = kb_.get_ref(b.key_);
      std::unique_ptr<Tracker<T, RD, WR>> p(new Tracker<T, RD, WR>(b.tracked_, ref));
      if (b.init_) {
        p->Tracker<T, RD, WR>::force_push(kb_);
      }
      add_tracker(tracker_ptr(p.release()));
    }

    template<class T, bool RD, bool WR>
    void add(const Builder<T, true, RD, WR, true, false> &b) const
    {
      std::unique_ptr<PrefixTracker<T, RD, WR>> p(new PrefixTracker<T, RD, WR>(b.tracked_, b.key_, kb_));
      add_tracker(tracker_ptr(p.release()));
    }

    template<class T, bool RD, bool WR>
    void add(const Builder<T, true, RD, WR, false, false> &b) const
    {
      VariableReference ref = kb_.get_ref(b.key_);
      std::unique_ptr<Tracker<T, RD, WR>> p(new Tracker<T, RD, WR>(b.tracked_, ref));
      add_tracker(tracker_ptr(p.release()));
    }

    template<class T>
    void add(const char *key, T &val) const
    {
      build(key, val).add();
    }

    template<class T>
    void add_init(const char *key, T &val)
    {
      build(key, val).init().add();
    }

    template<class T>
    void add_ro(const char *key, T &val) const
    {
      build(key, val).ro().add();
    }

    template<class T>
    void add_wo(const char *key, T &val) const
    {
      build(key, val).wo().add();
    }

    template<class T>
    void add_wo_init(const char *key, T &val)
    {
      build(key, val).wo().init().add();
    }

    template<class T>
    void add_prefix(const char *prefix, T &val) const
    {
      build(prefix, val).prefix().add();
    }

    template<class T>
    void add_prefix_init(const char *prefix, T &val)
    {
      build(prefix, val).prefix().init().add();
    }

    template<class T>
    void add_prefix_ro(const char *prefix, T &val) const
    {
      build(prefix, val).prefix().ro().add();
    }

    template<class T>
    void add_prefix_wo(const char *prefix, T &val) const
    {
      build(prefix, val).prefix().wo().add();
    }

    template<class T>
    void add_prefix_init_wo(const char *prefix, T &val)
    {
      build(prefix, val).prefix().init().wo().add();
    }

    bool remove(const char *name) const
    {
      iter_type i = std::find_if(trackers_.begin(), trackers_.end(), [name](const tracker_ptr &cur) {
          return strcmp(name, cur->get_name()) == 0;
        });
      if (i != trackers_.end()) {
        swap_remove(i);
        return 1;
      } else {
        return 0;
      }
    }

    bool remove(const std::string &name) const
    {
      return remove(name.c_str());
    }

    template<class T>
    bool remove(const T *var) const
    {
      iter_type i = std::find_if(trackers_.begin(), trackers_.end(), [&var](const tracker_ptr &cur) {
          return (void *)var == cur->get_tracked();
        });
      if (i != trackers_.end()) {
        swap_remove(i);
        return 1;
      } else {
        return 0;
      }
    }

  private:
    typedef trackers_vec::iterator iter_type;
    void swap_remove(iter_type i) const
    {
      using namespace std;
      swap(*i, trackers_.back());
      trackers_.pop_back();
    }
  };
} } } // end namespace knowledge::rcw

#endif  // INCL_MADARA_RCW_TRANSACTION_H
