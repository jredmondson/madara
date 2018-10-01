

#ifndef INCL_MADARA_RCW_PREFIX_TRACKER_H
#define INCL_MADARA_RCW_PREFIX_TRACKER_H

/**
 * @file PrefixTracker.h
 * @author David Kyle <dskyle@sei.cmu.edu>
 *
 * Implements PrefixTracker template used by Transaction
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
#include "BaseTracker.h"

namespace madara
{
namespace knowledge
{
namespace rcw
{
/// Tracker that puts values into a multiple prefixed KnowledgeRecord.
/// Used internally by Transaction. Not visible to normal users.
/// Default implementation gives error if type isn't std::vector<Tracked<...>
template<class T, class R, bool RD = true, bool WR = true, class dummy = void>
class PrefixTracker
{
  static_assert(
      sizeof(T) < 0, "Type unsupported for adding to Transaction with prefix");
};

/// If trying to create a tracker that is read-only, and write-only, give error
template<class T, class R>
class PrefixTracker<T, R, false, false, void> : public BaseTracker
{
private:
  static_assert(sizeof(T) < 0,
      "Cannot create prefix tracker that can neither read nor write");
};

/// Tracker that puts values into a multiple prefixed KnowledgeRecord.
/// Used internally by Transaction. Not visible to normal users.
/// Specialization of PrefixTracker for std::vector<Tracked<...>>
template<class T, bool RD, bool WR>
class PrefixTracker<T, VariableReference, RD, WR,
    typename std::enable_if<
        supports_get_value<T>::value && supports_indexed_get_value<T>::value &&
        supports_size<T>::value && supports_is_dirty<T>::value &&
        supports_is_all_dirty<T>::value && supports_is_size_dirty<T>::value &&
        supports_indexed_is_dirty<T>::value>::type> : public BaseTracker
{
private:
  T* tracked_;          /// Tracked std::vector
  std::string prefix_;  /// prefix string to use
  KnowledgeBase kb_;    /// The knowledge base to reference

  /// vector of references to elements of vector in knowledge base
  std::vector<VariableReference> elems_;

  typedef
      typename std::decay<decltype(get_value(std::declval<T>()[0]))>::type V;

  static const bool can_read = RD;
  static const bool can_write = WR;

  PrefixTracker(T* tracked, const std::string& prefix, KnowledgeBase& kb)
    : BaseTracker(kb.get_ref(prefix + ".size")),
      tracked_(tracked),
      prefix_(prefix),
      kb_(kb),
      elems_()
  {
    update_elems();
  }

  /// Update knowledge base array so it matches dimensions of the tracked object
  void update_elems()
  {
    const size_t n = tracked_->size();
    if (elems_.size() == n)
      return;
    if (elems_.size() > n) {
      for (size_t i = n; i < elems_.size(); ++i) {
        get_mut(elems_[i]).clear_value();
      }
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
        V val = knowledge_cast<V>(get(elems_[i]));
        set_value(*tracked_, i, val);
      } else {
        set_value(*tracked_, i, V());
      }
    }
    clear_dirty(*tracked_);
  }

  virtual void push(KnowledgeBase& kb)
  {
    if (can_write) {
      const size_t n = tracked_->size();
      if (is_all_dirty(*tracked_)) {
        return force_push(kb);
      }
      if (is_size_dirty(*tracked_)) {
        set(kb, n);
        post_set(kb);
      }

      update_elems();
      for (size_t i = 0; i < n; ++i) {
        if (is_dirty(*tracked_, i)) {
          set(kb, elems_[i], knowledge_cast(get_value(*tracked_, i)));
          post_set(kb, elems_[i]);
        }
      }
    }
  }

  virtual void force_push(KnowledgeBase& kb)
  {
    if (can_write) {
      const size_t n = tracked_->size();
      set(kb, n);
      post_set(kb);

      update_elems();
      for (size_t i = 0; i < n; ++i) {
        set(kb, elems_[i], knowledge_cast(tracked_->at(i)));
        post_set(kb, elems_[i]);
      }
    }
  }

  virtual const char* get_name() const
  {
    return prefix_.c_str();
  }

  virtual const void* get_tracked() const
  {
    return (void*)tracked_;
  }

  friend class Transaction;
};
}
}
}  // end namespace madara::knowledge::rcw

#endif  // INCL_MADARA_RCW_PREFIX_TRACKER_H
