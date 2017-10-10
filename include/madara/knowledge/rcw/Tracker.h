

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
  template<class T, bool RD = true, bool WR = true, class dummy = void>
  class Tracker
  {
    static_assert(sizeof(T) < 0, "Type unsupported for adding to Transaction");
  };

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

    Tracker(T *tracked, VariableReference ref)
      : BaseTracker(ref), tracked_(tracked) {}

    virtual void pull()
    {
      set_value(*tracked_, knowledge_cast<V>(get()));
    }

    virtual void push(KnowledgeBase &kb) {}

    virtual void force_push(KnowledgeBase &kb) {}

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
