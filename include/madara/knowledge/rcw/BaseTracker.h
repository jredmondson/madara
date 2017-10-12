

#ifndef INCL_MADARA_RCW_BASE_TRACKER_H
#define INCL_MADARA_RCW_BASE_TRACKER_H

/**
 * @file Tracker.h
 * @author David Kyle <dskyle@sei.cmu.edu>
 *
 * Implements BaseTracker abstract class used by Transaction, and
 * implemented by Tracker and PrefixTracker
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

  /// Base type for Trackers. Used internally by Transaction. Not visible
  /// for typical users.
  class BaseTracker
  {
  public:
    virtual ~BaseTracker() {}

  private:
    VariableReference ref_; /// Reference to tracked variable

    /// Constructor from a VariableReference
    BaseTracker(VariableReference ref) : ref_(ref) {}

    /// Override to implement pulling logic (from ref_)
    virtual void pull() = 0;
    /// Override to implement pushing logic (into ref_)
    virtual void push(KnowledgeBase &kb) = 0;
    /// As push, but ignore any modification status tracking
    virtual void force_push(KnowledgeBase &kb) = 0;
    /// Get name of MADARA variable
    virtual const char *get_name() const = 0;
    /// Get pointer to tracked object
    virtual const void *get_tracked() const = 0;

    /// Get record @ref_ points to. No locking, so be careful!
    const KnowledgeRecord &get() const
    {
      return get(ref_);
    }

    /// Get record @ref_ points to. No locking, so be careful!
    KnowledgeRecord &get_mut()
    {
      return get_mut(ref_);
    }

    /// Get record @ref points to. No locking, so be careful!
    static const KnowledgeRecord &get(const VariableReference &ref)
    {
      return *ref.record_;
    }

    /// Get record @ref points to. No locking, so be careful!
    static KnowledgeRecord &get_mut(const VariableReference &ref)
    {
      return *ref.record_;
    }

    /// Set record @ref_ points to. No locking, so be careful!
    void set(KnowledgeBase &kb, KnowledgeRecord rec)
    {
      return set(kb, ref_, rec);
    }

    /// Set record @ref points to. No locking, so be careful!
    static void set(KnowledgeBase &kb, const VariableReference &ref, KnowledgeRecord rec)
    {
      kb.get_context().set_unsafe_impl(ref, rec, EvalSettings());
    }

    /// Set index in record @ref_ points to. No locking, so be careful!
    template<typename I>
    auto set_index(KnowledgeBase &kb, size_t idx, I val) ->
      typename std::enable_if<std::is_integral<I>::value>::type
    {
      kb.get_context().set_index_unsafe_impl(ref_, idx, val, EvalSettings());
    }

    /// Set index in record @ref points to. No locking, so be careful!
    template<typename I>
    auto set_index(KnowledgeBase &kb, size_t idx, I val) ->
      typename std::enable_if<std::is_floating_point<I>::value>::type
    {
      kb.get_context().set_index_unsafe_impl(ref_, idx, val, EvalSettings());
    }

    /// Call after setting to perform any necessary after-setting logic
    void post_set(KnowledgeBase &kb)
    {
      post_set(kb, ref_);
    }

    /// Call after setting to perform any necessary after-setting logic
    void post_set(KnowledgeBase &kb, const VariableReference &ref)
    {
      kb.get_context().mark_and_signal(ref.name_.get_ptr (), ref.record_, EvalSettings());
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
        knowledge_cast(get_value(*p)),
        knowledge_cast<decltype(get_value(*p))>(
          std::declval<KnowledgeRecord>())));

  MADARA_MAKE_SUPPORT_TEST(self_eq, p, (
        get_value(*p) == get_value(*p),
        get_value(*p) != get_value(*p)));
} } } // end namespace madara::knowledge::rcw

#endif  // INCL_MADARA_RCW_BASE_TRACKER_H
