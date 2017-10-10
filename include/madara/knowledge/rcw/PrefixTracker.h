

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
#include "madara/utility/stdint.h"
#include "madara/MADARA_export.h"
#include "madara/knowledge/VariableReference.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/knowledge_cast.h"
#include "madara/knowledge/ContextGuard.h"
#include "BaseTracker.h"

namespace madara { namespace knowledge { namespace rcw
{
  /// Tracker that puts values into a multiple prefixed KnowledgeRecords.
  /// Used internally by Transaction. Not visible to normal users.
  template<class T, bool RD = true, bool WR = true, class dummy = void>
  class PrefixTracker
  {
    static_assert(sizeof(T) < 0, "Type unsupported for adding to Transaction with prefix");
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
    KnowledgeBase kb_;
    std::vector<VariableReference> elems_;

    typedef typename std::decay<decltype(get_value(std::declval<T>()[0]))>::type V;

    static const bool can_read = RD;
    static const bool can_write = WR;

    PrefixTracker(T *tracked, const std::string &prefix, KnowledgeBase &kb)
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
          V val = knowledge_cast<V>(get(elems_[i]));
          set_value(*tracked_, i, val);
        } else {
          set_value(*tracked_, i, V());
        }
      }
      clear_dirty(*tracked_);
    }

    virtual void push(KnowledgeBase &kb)
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
            set(kb, elems_[i], knowledge_cast(get_value(*tracked_, i)));
            post_set(kb, elems_[i]);
          }
        }
      }
    }

    virtual void force_push(KnowledgeBase &kb)
    {
      if (can_write) {
        const size_t n = tracked_->size();
        set(kb, (int64_t)n);
        post_set(kb);

        update_elems();
        for (size_t i = 0; i < n; ++i) {
          set(kb, elems_[i], knowledge_cast(tracked_->at(i)));
          post_set(kb, elems_[i]);
        }
      }
    }

    virtual const char *get_name() const
    {
      return prefix_.c_str();
    }

    virtual const void *get_tracked() const
    {
      return (void*)tracked_;
    }

    friend class Transaction;
  };
} } } // end namespace madara::knowledge::rcw

#endif  // INCL_MADARA_RCW_PREFIX_TRACKER_H
