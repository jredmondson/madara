#ifndef _MADARA_VARIABLE_REFERENCE_H_
#define _MADARA_VARIABLE_REFERENCE_H_

/**
 * @file CompiledExpression.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the CompiledExpression class which is used by
 * the madara::knowledge::KnowledgeBase class
 */

#include <string>
#include <cstring>
#include "madara/MadaraExport.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/utility/ScopedArray.h"

namespace madara
{
namespace knowledge
{
class ThreadSafeContext;
class KnolwedgeBaseImpl;
class Variables;

namespace rcw
{
class BaseTracker;
}

/**
 * @class VariableReference
 * @brief Optimized reference to a variable within the knowledge base
 **/
class MADARA_EXPORT VariableReference
{
public:
  friend class ThreadSafeContext;
  friend class KnowledgeBaseImpl;
  friend class Variables;
  friend class rcw::BaseTracker;

  using pair_ptr = KnowledgeMap::value_type*;

  /**
   * Default constructor
   **/
  VariableReference() = default;

  /**
   * Construct from a pointer to entry in a KnowledgeMap
   **/
  VariableReference(pair_ptr entry);

  /**
   * Checks to see if the variable reference has been initialized
   * @return  true if valid, false otherwise
   **/
  bool is_valid(void) const;

  /**
   * Equality operator
   * @param   rhs      variable reference to compare to
   **/
  bool operator==(const VariableReference& rhs) const;

  /**
   * Inequality operator
   * @param   rhs      variable reference to compare to
   **/
  bool operator!=(const VariableReference& rhs) const;

  /**
   * Returns the name of the variable.
   * Returns nullptr if is_valid() is false.
   *
   * @return    the name of the referenced variable
   **/
  const char* get_name(void) const;

  /**
   * Returns a pointer to the variable's KnowledgeRecord
   * Do not use this pointer unless you've locked the ThreadSafeContext
   * it belongs to. Returns nullptr if is_valid() is false.
   *
   * @return    the pointer to the KnowledgeRecord
   **/
  KnowledgeRecord* get_record_unsafe(void) const;

  /**
   * Refer to a different variable
   **/
  void assign(pair_ptr entry);

private:
  pair_ptr entry_ = nullptr;
};

/// a vector of variable references
typedef std::vector<VariableReference> VariableReferences;

/// a map of variable references
typedef std::map<const char*, VariableReference, compare_const_char_ptr>
    VariableReferenceMap;

/**
 * Helper class for iterating over VariableReferenceMaps, optionally
 * including history of changed records, since a given TOI
 *
 * This is NOT thread safe. Only use this reader if the KnowledgeBase
 * the map points into is locked by the current thread. Do not modify the
 * map, or the KnowledgeBase, while using this reader.
 *
 * @code{.cpp}
 * // Example to iterate over full recorded history of records in map
 * VariableReferenceMapReader reader(map, true);
 * for(auto cur : reader) {
 *   const char *name = cur.first;
 *   const KnowledgeRecord *rec = cur.second;
 *   ...
 * }
 * @endcode
 **/
class VariableReferenceMapReader
{
public:
  /**
   * Primary constructor. Keeps a reference to @a map, so ensure this
   * reader doesn't outlive it.
   *
   * @param map the map to iterate over
   * @param history if true, include historical values. Otherwise, only
   *    iterate over latest value of any records.
   * @param min_toi only iterate over historical values of TOI > this.
   *    If a record has no history capacity, it will always be included.
   **/
  VariableReferenceMapReader(const VariableReferenceMap& map,
      bool history = false, uint64_t min_toi = 0)
    : map_(map), history_(history), min_toi_(min_toi)
  {
  }

  /**
   * Constant Input Iterator over values in the map.
   **/
  class const_iterator
  {
  public:
    using value_type = std::pair<const char*, const KnowledgeRecord*>;
    using difference_type = size_t;
    using reference = value_type;
    using pointer = value_type*;
    using iterator_category = std::input_iterator_tag;

    /**
     * Gives a pair value (note, not a reference). First part is the
     * name of the record, second part is the value.
     **/
    value_type operator*() const
    {
      if(biter_)
      {
        return {miter_->first, &**biter_};
      }
      return {miter_->first, miter_->second.get_record_unsafe()};
    }

    /**
     * Increments iterator, to move onto the next record in the map.
     **/
    const_iterator& operator++()
    {
      if(biter_)
      {
        ++*biter_;
        auto rec = miter_->second.get_record_unsafe();
        auto cbuf = rec->share_circular_buffer();
        if(cbuf && *biter_ != cbuf->cend())
        {
          return *this;
        }
      }

      ++miter_;
      biter_ = begin_biter();
      return *this;
    }

    /**
     * Post-Increments iterator, to move onto the next record in the map.
     * Use pre-increment instead when possible.
     **/
    const_iterator operator++(int)
    {
      const_iterator ret(*this);
      ++*this;
      return ret;
    }

    bool operator==(const const_iterator& o) const
    {
      return reader_ == o.reader_ && miter_ == o.miter_ &&
             ((biter_ == nullptr && o.biter_ == nullptr) ||
                 (biter_ && o.biter_ && *biter_ == *o.biter_));
    }

    bool operator!=(const const_iterator& o) const
    {
      return !(*this == o);
    }

  private:
    using miter_type = VariableReferenceMap::const_iterator;
    using biter_type = utility::CircularBuffer<KnowledgeRecord>::const_iterator;
    using biter_uptr = std::unique_ptr<biter_type>;

    biter_uptr begin_biter()
    {
      for(;;)
      {
        if(!reader_->history_ || miter_ == reader_->map_.end())
        {
          return nullptr;
        }

        const KnowledgeRecord* rec = miter_->second.get_record_unsafe();

        if(!rec->has_history())
        {
          return nullptr;
        }

        auto cbuf = rec->share_circular_buffer();
        auto ret = cbuf->cbegin();

        if(reader_->min_toi_ > 0)
        {
          ret = std::upper_bound(ret, cbuf->cend(), reader_->min_toi_,
              [](const uint64_t& lhs, const KnowledgeRecord& rhs) {
                return lhs < rhs.toi();
              });
        }

        if(ret == cbuf->cend())
        {
          ++miter_;
          continue;
        }

        return into_unique(std::move(ret));
      }
    }

    const_iterator(const VariableReferenceMapReader& f)
      : reader_(&f), miter_(reader_->map_.begin()), biter_(begin_biter())
    {
    }

    const_iterator(const VariableReferenceMapReader& f, miter_type miter)
      : reader_(&f), miter_(miter), biter_(begin_biter())
    {
    }

  public:
    const_iterator(const const_iterator& o)
      : reader_(o.reader_),
        miter_(o.miter_),
        biter_(o.biter_ ? into_unique(*o.biter_) : nullptr)
    {
    }

    const_iterator& operator=(const const_iterator& o)
    {
      biter_ = o.biter_ ? into_unique(*o.biter_) : nullptr;
      reader_ = o.reader_;
      miter_ = o.miter_;
      return *this;
    }

    const_iterator(const_iterator&& o) = default;
    const_iterator& operator=(const_iterator&& o) = default;

  private:
    /// Reader the iterator belongs to.
    const VariableReferenceMapReader* reader_;

    /// Iterator over map members
    miter_type miter_;

    /// Iterator over history of current map member, if any exists
    biter_uptr biter_{nullptr};

    friend class VariableReferenceMapReader;
  };

  const_iterator begin() const
  {
    return {*this};
  }

  const_iterator end() const
  {
    return {*this, map_.end()};
  }

private:
  /// Map to iterate over
  const VariableReferenceMap& map_;

  /// True if iteration should include history
  bool history_ = false;

  /// Only iterate updates with TOI > this value
  uint64_t min_toi_ = 0;
};
}
}

#include "VariableReference.inl"

#endif  // _MADARA_VARIABLE_REFERENCE_H_
