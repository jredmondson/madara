

#ifndef INCL_MADARA_RCW_TRACKER_H
#define INCL_MADARA_RCW_TRACKER_H

/**
 * @file Tracker.h
 * @author David Kyle <dskyle@sei.cmu.edu>
 *
 * Implements Tracker template used by Transaction
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
  /// Tracker which puts variable values into a single KnowledgeRecord. Used
  /// internally by Transaction. Not visible to normal users.
  template<class T, class R, bool RD = true, bool WR = true, class dummy = void>
  class Tracker
  {
    static_assert(sizeof(T) < 0, "Type unsupported for adding to Transaction");
  };

  /// If trying to create a tracker that is read-only, and write-only, give error
  template<class T, class R>
  class Tracker<T, R, false, false, void>
    : public BaseTracker
  {
  private:
    static_assert(sizeof(T) < 0, "Cannot create tracker that can neither read nor write");
  };

  /// Tracker specialization for types that don't track modification status,
  /// and that can both read and write
  template<class T>
  class Tracker<T, VariableReference, true, true, typename std::enable_if<
                     supports_get_value<T>::value &&
                     supports_knowledge_cast<T>::value &&
                     supports_self_eq<T>::value &&
                     !supports_is_dirty<T>::value>::type>
    : public BaseTracker
  {
  private:
    typedef typename std::decay<decltype(get_value(std::declval<T>()))>::type V;

    T *tracked_; /// Pointer to tracked object
    V orig_; /// Original pulled value to compare to on push

    static const bool can_read = true;
    static const bool can_write = true;

    Tracker(T *tracked, VariableReference ref)
      : BaseTracker(ref), tracked_(tracked), orig_() {}

    virtual void pull()
    {
      orig_ = knowledge_cast<V>(get());
      set_value(*tracked_, orig_);
    }

    virtual void push(KnowledgeBase &kb)
    {
      if (get_value(*tracked_) != get_value(orig_)) {
        Tracker::force_push(kb);
      }
    }

    virtual void force_push(KnowledgeBase &kb)
    {
      set(kb, knowledge_cast(get_value(*tracked_)));
      post_set(kb);
    }

    virtual const char *get_name() const
    {
      return ref_.get_name();
    }

    virtual const void *get_tracked() const
    {
      return (void*)tracked_;
    }

    friend class Transaction;
  };

  /// Tracker specialization for types that can only read
  template<class T>
  class Tracker<T, VariableReference, true, false, typename std::enable_if<
                     supports_get_value<T>::value &&
                     supports_knowledge_cast<T>::value>::type>
    : public BaseTracker
  {
  private:
    typedef typename std::decay<decltype(get_value(std::declval<T>()))>::type V;

    T *tracked_; /// Pointer to tracked object

    static const bool can_read = true;
    static const bool can_write = false;

    Tracker(T *tracked, VariableReference ref)
      : BaseTracker(ref), tracked_(tracked) {}

    virtual void pull()
    {
      set_value(*tracked_, knowledge_cast<V>(get()));
    }

    virtual void push(KnowledgeBase &) {}

    virtual void force_push(KnowledgeBase &) {}

    virtual const char *get_name() const
    {
      return ref_.get_name();
    }

    virtual const void *get_tracked() const
    {
      return (void*)tracked_;
    }

    friend class Transaction;
  };

  /// Tracker specialization for types that don't track modification status,
  /// and that can only write
  template<class T>
  class Tracker<T, VariableReference, false, true, typename std::enable_if<
                     supports_get_value<T>::value &&
                     supports_knowledge_cast<T>::value &&
                     supports_self_eq<T>::value &&
                     !supports_is_dirty<T>::value>::type>
    : public BaseTracker
  {
  private:
    typedef typename std::decay<decltype(get_value(std::declval<T>()))>::type V;

    T *tracked_; /// Pointer to tracked object
    // Note that we don't store original value, since we set to default value
    // on a pull, and can compare to that on push to see if it has changed.

    static const bool can_read = false;
    static const bool can_write = true;

    Tracker(T *tracked, VariableReference ref)
      : BaseTracker(ref), tracked_(tracked) {}

    virtual void pull()
    {
      set_value(*tracked_, V());
    }

    virtual void push(KnowledgeBase &kb)
    {
      if (get_value(*tracked_) != V()) {
        Tracker::force_push(kb);
      }
    }

    virtual void force_push(KnowledgeBase &kb)
    {
      set(kb, knowledge_cast(get_value(*tracked_)));
      post_set(kb);
    }

    virtual const char *get_name() const
    {
      return ref_.get_name();
    }

    virtual const void *get_tracked() const
    {
      return (void*)tracked_;
    }

    friend class Transaction;
  };

  /// Tracker specialization for types that track their own modification status
  /// and that can both read and write
  template<class T>
  class Tracker<T, VariableReference, true, true, typename std::enable_if<
                     supports_get_value<T>::value &&
                     supports_knowledge_cast<T>::value &&
                     supports_is_dirty<T>::value &&
                     !supports_indexed_is_dirty<T>::value>::type >
    : public BaseTracker
  {
  private:
    T *tracked_; /// Pointer to tracked object

    static const bool can_read = true;
    static const bool can_write = true;

    typedef typename std::decay<decltype(get_value(std::declval<T>()))>::type V;

    Tracker(T *tracked, VariableReference ref)
      : BaseTracker(ref), tracked_(tracked) {}

    virtual void pull()
    {
      V val = knowledge_cast<V>(get());
      set_value(*tracked_, val);
      clear_dirty(*tracked_);
    }

    virtual void push(KnowledgeBase &kb)
    {
      if (is_dirty(*tracked_)) {
        Tracker::force_push(kb);
      }
    }

    virtual void force_push(KnowledgeBase &kb)
    {
      set(kb, knowledge_cast(get_value(*tracked_)));
      post_set(kb);
    }

    virtual const char *get_name() const
    {
      return ref_.get_name();
    }

    virtual const void *get_tracked() const
    {
      return (void*)tracked_;
    }

    friend class Transaction;
  };

  /// Tracker specialization for types that track their own modification status
  /// and that can only write
  template<class T>
  class Tracker<T, VariableReference, false, true, typename std::enable_if<
                     supports_get_value<T>::value &&
                     supports_knowledge_cast<T>::value &&
                     supports_is_dirty<T>::value &&
                     !supports_indexed_is_dirty<T>::value>::type >
    : public BaseTracker
  {
  private:
    T *tracked_; /// Pointer to tracked object

    typedef typename std::decay<decltype(get_value(std::declval<T>()))>::type V;

    static const bool can_read = false;
    static const bool can_write = true;

    Tracker(T *tracked, VariableReference ref)
      : BaseTracker(ref), tracked_(tracked) {}

    virtual void pull()
    {
      set_value(*tracked_, V());
      clear_dirty(*tracked_);
    }

    virtual void push(KnowledgeBase &kb)
    {
      if (is_dirty(*tracked_)) {
        Tracker::force_push(kb);
      }
    }

    virtual void force_push(KnowledgeBase &kb)
    {
      set(kb, knowledge_cast(get_value(*tracked_)));
      post_set(kb);
    }

    virtual const char *get_name() const
    {
      return ref_.get_name();
    }

    virtual const void *get_tracked() const
    {
      return (void*)tracked_;
    }

    friend class Transaction;
  };

  /// Tracker specialization for types with individual elements that each track
  /// their own modification status
  template<class T, bool RD, bool WR>
  class Tracker<T, VariableReference, RD, WR, typename std::enable_if<
                     supports_indexed_get_value<T>::value &&
                     supports_size<T>::value &&
                     !supports_is_all_dirty<T>::value &&
                     !supports_is_size_dirty<T>::value &&
                     supports_indexed_is_dirty<T>::value>::type >
    : public BaseTracker
  {
  private:
    static_assert(RD == true, "Write-only not supported for containers of tracked items");

    T *tracked_; /// Pointer to tracked
    size_t orig_size_; /// Size as pulled to compare to on push

    typedef typename std::decay<decltype(get_value(std::declval<T>(), 0))>::type V;

    static const bool can_read = RD;
    static const bool can_write = WR;

    Tracker(T *tracked, VariableReference ref)
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

    virtual void push(KnowledgeBase &kb)
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

    virtual void force_push(KnowledgeBase &kb)
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

    virtual const char *get_name() const
    {
      return ref_.get_name();
    }

    virtual const void *get_tracked() const
    {
      return (void*)tracked_;
    }

    friend class Transaction;
  };

  /// Tracker specialization for types that have individual indexed elements,
  /// and track their modification status individually.
  template<class T, bool RD, bool WR>
  class Tracker<T, VariableReference, RD, WR, typename std::enable_if<
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
    T *tracked_; /// Pointer to tracked object

    typedef typename std::decay<decltype(get_value(std::declval<T>()))>::type V;

    static const bool can_read = RD;
    static const bool can_write = WR;

    Tracker(T *tracked, VariableReference ref)
      : BaseTracker(ref), tracked_(tracked) {}

    virtual void pull()
    {
      if (can_read) {
        V val = knowledge_cast<V>(get());
        set_value(*tracked_, val);
      } else {
        set_value(*tracked_, V());
      }
      clear_dirty(*tracked_);
    }

    virtual void push(KnowledgeBase &kb)
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

    virtual void force_push(KnowledgeBase &kb)
    {
      if (can_write) {
        set(kb, knowledge_cast(get_value(*tracked_)));
        post_set(kb);
      }
    }

    virtual const char *get_name() const
    {
      return ref_.get_name();
    }

    virtual const void *get_tracked() const
    {
      return (void*)tracked_;
    }

    friend class Transaction;
  };
} } } // end namespace madara::knowledge::rcw

#endif  // INCL_MADARA_RCW_TRACKER_H
