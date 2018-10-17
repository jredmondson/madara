

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
#include "madara/utility/StdInt.h"
#include "madara/MadaraExport.h"
#include "madara/knowledge/VariableReference.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/KnowledgeCast.h"
#include "madara/knowledge/ContextGuard.h"
#include "Tracker.h"
#include "PrefixTracker.h"

namespace madara
{
namespace knowledge
{
/// Namespace holding Read-Compute-Write based abstractions for knowledge base
/// access. For normal use, refer to @see Transaction and @see Tracked.
///
/// See https://sourceforge.net/p/madara/wiki/ReadComputeWrite/ for details.
namespace rcw
{
/**
 * Manages a Read-Compute-Write cycle for registered variables.
 *
 * See https://sourceforge.net/p/madara/wiki/ReadComputeWrite/ for details.
 **/
class Transaction
{
public:
  typedef std::unique_ptr<BaseTracker> tracker_ptr;

  /// Builder object returned by build()
  template<class T, class K, bool Const = false, bool RD = true, bool WR = true,
      bool Prefix = false, bool Init = false>
  class Builder;

private:
  /// Type of a vector which stores trackers
  typedef std::vector<tracker_ptr> trackers_vec;

  /// The knowledge base to push to and pull from
  mutable KnowledgeBase kb_;

  /// Stores all registered trackers
  mutable trackers_vec trackers_;

  /// Helper type to enable initialization using initializer_list
  /// Fallback implementation for types which do not support initializer_list
  template<class B, class I, class T, class dummy = void>
  class InitHandler
  {
  public:
    I init() const;
  };

  /// Helper type to enable initialization using initializer_list
  /// Specialization for types which do support initializer_list
  template<class B, class I, class T>
  class InitHandler<B, I, T,
      typename std::enable_if<supports_get_value<T>::value &&
                              supports_indexed_get_value<T>::value &&
                              supports_size<T>::value>::type>
  {
    typedef
        typename std::decay<decltype(get_value(std::declval<T>()[0]))>::type V;

  public:
    I init(std::initializer_list<V> list) const
    {
      const B& self = static_cast<const B&>(*this);
      *self.tracked_ = list;
      return self.init();
    }
  };

public:
  /// The constructor. Note that this keeps a copy of the KnowledgeBase
  /// given, thus extending the lifetime of the underlying knowledge base.
  /// @param kb the knowledge base to read and modify
  Transaction(KnowledgeBase kb) : kb_(kb), trackers_() {}

  /// Call to push all values of registered variables into the knowledge base
  void push(void)
  {
    ContextGuard guard(kb_);
    for(auto& t : trackers_)
    {
      t->push(kb_);
    }
  }

  /// Call to update values of registered variables from the knowledge base
  void pull(void)
  {
    ContextGuard guard(kb_);
    for(auto& t : trackers_)
    {
      t->pull();
    }
  }

  /// Builder type which will be returned from the build*() methods
  /// No need to construct directly; just use those methods
  template<class T, class K, bool Const, bool RD, bool WR, bool Prefix,
      bool Init>
  class Builder
    : private InitHandler<Builder<T, K, Const, RD, WR, Prefix, Init>,
          Builder<T, K, Const, RD, WR, Prefix, true>, T>
  {
  private:
    typedef
        typename std::conditional<Const, const Transaction*, Transaction*>::type
            trans_type;
    typedef decltype(*(trans_type) nullptr) trans_ref_type;
    const trans_type trans_;
    const K key_;
    T* const tracked_;
    static const bool rd_ = RD;
    static const bool wr_ = WR;
    static const bool prefix_ = Prefix;
    static const bool init_ = Init;

    Builder(trans_ref_type trans, K key, T& val)
      : trans_(&trans), key_(key), tracked_(&val)
    {
    }

    template<bool R, bool W, bool P, bool I>
    Builder(const Builder<T, K, Const, R, W, P, I>& o)
      : trans_(o.trans_), key_(o.key_), tracked_(o.tracked_)
    {
    }

    typedef Builder<T, K, Const, RD, WR, Prefix, true> init_type;
    typedef Builder<T, K, Const, RD, WR, true, Init> prefix_type;
    typedef Builder<T, K, Const, true, false, Prefix, Init> ro_type;
    typedef Builder<T, K, Const, false, true, Prefix, Init> wo_type;

  public:
    /// If used, the mapping will push the current value of the variable into
    /// the knowledge base, immediately
    init_type init() const
    {
      static_assert(!Const, "Can't use init() on build() of const Transaction");
      return *this;
    }

    /// If used, the name given in build() will be used as a prefix, as
    /// apropriate for the variable type.
    prefix_type prefix() const
    {
      return *this;
    }

    /// The mapping will only read from the knowledge base, not write back
    ro_type ro() const
    {
      return *this;
    }

    /// The mapping will only read from the knowledge base, not write back
    /// Synonym for @see ro
    ro_type readonly() const
    {
      return *this;
    }

    /// The mapping will only write to the knowledge base. On pull(), the
    /// variable will be reset to its default value.
    wo_type wo() const
    {
      return *this;
    }

    /// The mapping will only write to the knowledge base. On pull(), the
    /// variable will be reset to its default value.
    /// Synonym for @see wo
    wo_type writeonly() const
    {
      return *this;
    }

    /// As init(), but first set the variable to @val
    /// @param val the value to set
    template<class V>
    init_type init(V&& val) const
    {
      *tracked_ = std::forward<V>(val);
      return init();
    }

    using InitHandler<Builder, init_type, T>::init;
    friend class InitHandler<Builder, init_type, T>;

    /// Finish configuring this Builder, and add the new mapping
    void add() const
    {
      trans_->add(*this);
    }

    friend class Transaction;
  };

  template<class T, class K, bool Const, bool RD, bool WR, bool Prefix,
      bool Init>
  friend class Builder;

  /**
   * Create a new mapping, using the Builder interface.
   * IMPORTANT: the variable given must not outlive the Transaction.
   *
   * @param key the name of the underlying MADARA variable
   * @param val a reference to the variable to manage
   */
  template<class T, class K>
  Builder<T, const char*> build(K&& key, T& val)
  {
    return Builder<T, const char*>(*this, std::forward<K>(key), val);
  }

#ifndef DOXYGEN
  template<class T, class S>
  Builder<T, const char*, true> build(S&& key, T& val) const
  {
    return Builder<T, const char*, true>(*this, std::forward<S>(key), val);
  }
#endif  // DOXYGEN

  /// Add a tracker object directly. For advanced users only.
  /// @param t the tracker to add
  void add_tracker(tracker_ptr t) const
  {
    trackers_.emplace_back(std::move(t));
  }

#ifdef DOXYGEN
  /// Add a mapping from a Builder object
  /// @param b the Builder object to use
  template<class T>
  void add(const Builder<T, const char*>& b);
#else
  template<class T, bool RD, bool WR, bool Init>
  void add(const Builder<T, const char*, false, RD, WR, true, Init>& b)
  {
    std::unique_ptr<PrefixTracker<T, VariableReference, RD, WR> > p(
        new PrefixTracker<T, VariableReference, RD, WR>(
            b.tracked_, b.key_, kb_));
    if(b.init_)
    {
      p->PrefixTracker<T, VariableReference, RD, WR>::force_push(kb_);
    }
    add_tracker(tracker_ptr(p.release()));
  }

  template<class T, bool RD, bool WR, bool Init>
  void add(const Builder<T, const char*, false, RD, WR, false, Init>& b)
  {
    VariableReference ref = kb_.get_ref(b.key_);
    std::unique_ptr<Tracker<T, VariableReference, RD, WR> > p(
        new Tracker<T, VariableReference, RD, WR>(b.tracked_, ref));
    if(b.init_)
    {
      p->Tracker<T, VariableReference, RD, WR>::force_push(kb_);
    }
    add_tracker(tracker_ptr(p.release()));
  }

  template<class T, bool RD, bool WR>
  void add(const Builder<T, const char*, true, RD, WR, true, false>& b) const
  {
    std::unique_ptr<PrefixTracker<T, VariableReference, RD, WR> > p(
        new PrefixTracker<T, VariableReference, RD, WR>(
            b.tracked_, b.key_, kb_));
    add_tracker(tracker_ptr(p.release()));
  }

  template<class T, bool RD, bool WR>
  void add(const Builder<T, const char*, true, RD, WR, false, false>& b) const
  {
    VariableReference ref = kb_.get_ref(b.key_);
    std::unique_ptr<Tracker<T, VariableReference, RD, WR> > p(
        new Tracker<T, VariableReference, RD, WR>(b.tracked_, ref));
    add_tracker(tracker_ptr(p.release()));
  }
#endif  // DOXYGEN

  /// Add a new default mapping for MADARA variable @key and reference @val
  /// @param key the MADARA variable name to use. If it doesn't exist, it
  ///            will be created.
  /// @param val reference to the object to push/pull to/from
  template<class T, class K>
  void add(K&& key, T& val) const
  {
    build(std::forward<K>(key), val).add();
  }

  /// Add a new mapping for MADARA variable @key and reference @val.
  /// This will immediately update @key with the value of @val
  /// @param key the MADARA variable name to use. If it doesn't exist, it
  ///            will be created.
  /// @param val reference to the object to push/pull to/from
  template<class T, class K>
  void add_init(K&& key, T& val)
  {
    build(std::forward<K>(key), val).init().add();
  }

  /// Add a new prefix-based mapping for MADARA variable @key and ref @val.
  /// @param key the MADARA variable name to use. If it doesn't exist, it
  ///            will be created.
  /// @param val reference to the object to push/pull to/from
  template<class T, class K>
  void add_prefix(K&& prefix, T& val) const
  {
    build(std::forward<K>(prefix), val).prefix().add();
  }

  /// Add a new prefix-based mapping for MADARA variable @key and ref @val.
  /// This will immediately update @key with the value of @val
  /// @param key the MADARA variable name to use. If it doesn't exist, it
  ///            will be created.
  /// @param val reference to the object to push/pull to/from
  template<class T, class K>
  void add_prefix_init(K&& prefix, T& val)
  {
    build(std::forward<K>(prefix), val).prefix().init().add();
  }

  /// Remove a mapping by name.
  /// If multiple exist for the given name, only one will be removed. Which
  /// one is arbitrary.
  /// @param name name of the MADARA variable in the mapping
  /// @returns true if the mapping was found; false otherwise
  bool remove(const char* name) const
  {
    iter_type i = std::find_if(
        trackers_.begin(), trackers_.end(), [name](const tracker_ptr& cur) {
          return strcmp(name, cur->get_name()) == 0;
        });
    if(i != trackers_.end())
    {
      swap_remove(i);
      return 1;
    }
    else
    {
      return 0;
    }
  }

  /// Remove a mapping by name.
  /// If multiple exist for the given name, only one will be removed. Which
  /// one is arbitrary.
  /// @param name name of the MADARA variable in the mapping
  /// @returns true if the mapping was found; false otherwise
  bool remove(const std::string& name) const
  {
    return remove(name.c_str());
  }

  /// Remove a mapping by pointer to the mapped object.
  /// If multiple exist for the given variable, only one will be removed.
  /// Which one is arbitrary.
  /// @param var pointer to the variable mapped
  /// @returns true if the mapping was found; false otherwise
  template<class T>
  bool remove(const T* var) const
  {
    iter_type i = std::find_if(
        trackers_.begin(), trackers_.end(), [&var](const tracker_ptr& cur) {
          return (void*)var == cur->get_tracked();
        });
    if(i != trackers_.end())
    {
      swap_remove(i);
      return 1;
    }
    else
    {
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
}
}
}  // end namespace knowledge::rcw

#endif  // INCL_MADARA_RCW_TRANSACTION_H
