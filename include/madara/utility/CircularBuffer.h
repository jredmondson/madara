

#ifndef MADARA_KNOWLEDGE_CIRCULAR_BUFFER_H_
#define MADARA_KNOWLEDGE_CIRCULAR_BUFFER_H_

/**
 * @file CircularBuffer.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the CircularBuffer class
 **/

#include <sstream>
#include <memory>
#include <type_traits>
#include "madara/MadaraExport.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/IntTypes.h"
#include "madara/utility/SupportTest.h"

namespace madara { namespace utility {

/**
 * General purpose circular buffer container. Differs from some implementations
 * in that it tracks an always-growing index, enabling multiple independent
 * consumers. Overwritten elements are safely seen as missing, and will not
 * silently alias the newer elements overwriting them.
 **/
template<typename T>
class CircularBuffer
{
public:
  /**
   * Construct with zero capacity. Not useful in this state. Does not allocate
   * to the heap.
   **/
  CircularBuffer() = default;

  /**
   * Initialize with given capacity. Up to that many contiguous elements will
   * be held at once. If more than that many elements are added to the back of
   * the buffer, elements from the front will silently be removed.
   **/
  explicit CircularBuffer(size_t capacity, size_t initial_index = 0)
    : front_(initial_index),
      back_(initial_index),
      data_((T *)operator new(sizeof(T) * capacity)),
      cap_(capacity) {}

  /**
   * Copy constructor. Abides by typical semantics.
   **/
  CircularBuffer(const CircularBuffer &other)
    : CircularBuffer(other.size(), other.front_)
  {
    for (const T &cur : other) {
      emplace_back(cur);
    }
  }

  /**
   * Move constructor. Abides by typical semantics.
   **/
  CircularBuffer(CircularBuffer &&other) noexcept
  {
    swap(other);
  }

  /**
   * Copy assignment operator. Abides by typical semantics.
   **/
  CircularBuffer &operator=(const CircularBuffer &other)
  {
    CircularBuffer tmp(other);
    *this = std::move(tmp);
    return *this;
  }

  /**
   * Move assignment operator. Abides by typical semantics.
   **/
  CircularBuffer &operator=(CircularBuffer &&other) noexcept
  {
    swap(other);
    return *this;
  }

  /**
   * Destructor. Abides by typical semantics.
   **/
  ~CircularBuffer() noexcept
  {
    clear();
    operator delete(data_);
  }

  /**
   * Swaps contents with another CircularBuffer, without copying elements.
   **/
  void swap(CircularBuffer &other) noexcept
  {
    using std::swap;
    swap(front_, other.front_);
    swap(back_, other.back_);
    swap(data_, other.data_);
    swap(cap_, other.cap_);
  }

  /**
   * Change the capacity of this CircularBuffer. Unlike vectors, this operation
   * will always set capacity to exactly the size given. This operation always
   * moves each element individually.
   **/
  void reserve(size_t size)
  {
    CircularBuffer tmp(size, front_);
    while (!empty()) {
      tmp.emplace_back(std::move(pop_front()));
    }
    swap(tmp);
  }

  /// Is this CircularBuffer empty?
  bool empty() const { return front_ == back_; }

  /// Number of elements this buffer can hold without dropping any
  size_t capacity() const { return cap_; }

  /// Number of elements this buffer currently holds, up to capacity
  size_t size() const { return back_ - front_; }

  /// Index of front element
  size_t front_index() const { return front_; }

  /// Index of back element
  size_t back_index() const { return back_ - 1; }

  /// Convert an actual index, to the modulused actual index within buffer
  size_t actual(size_t i) const { return i % capacity(); }

  /// Actual index of front within the buffer
  size_t front_actual() const { return actual(front_index()); }

  /// Actual index of back within the buffer
  size_t back_actual() const { return actual(back_index()); }

  /// Empty the buffer, destructing all elements
  void clear() {
    while (!empty()) {
      discard_front();
    }
  }

  /// Const input iterator for elements of a CircularBuffer. Usual semantics.
  class const_iterator
  {
  public:
    using value_type = const T;
    using difference_type = size_t;
    using reference = const T &;
    using pointer = const T *;
    using iterator_category = std::random_access_iterator_tag;

    reference operator*() const { return (*buf_)[idx_]; }
    reference at() const { return buf_->at(idx_); }
    pointer operator->() const { return &(*buf_)[idx_]; }
    reference operator[](size_t i) const { return (*buf_)[idx_ + i]; }
    reference at(size_t i) const { return buf_->at(idx_ + i); }
    const_iterator &operator++() { ++idx_; return *this; }
    const_iterator &operator--() { --idx_; return *this; }
    const_iterator operator++(int) { auto ret = *this; ++idx_; return ret; }
    const_iterator operator--(int) { auto ret = *this; --idx_; return ret; }

    /// Index this iterator refers to
    size_t index() const { return idx_; }

    /// Actual index within buffer this iterator refers to
    size_t index_actual() const { return buf_->actual(idx_); }

    bool operator==(const const_iterator &other) const
    {
      return buf_ == other.buf_ && idx_ == other.idx_;
    }

    bool operator!=(const const_iterator &other) const
    {
      return !operator==(other);
    }

    bool operator<(const const_iterator &other) const
    {
      return buf_ < other.buf_ && idx_ < other.idx_;
    }

    bool operator>(const const_iterator &other) const
    {
      return buf_ > other.buf_ && idx_ > other.idx_;
    }

    bool operator<=(const const_iterator &other) const
    {
      return !operator>(other);
    }

    bool operator>=(const const_iterator &other) const
    {
      return !operator<(other);
    }

    const_iterator &operator+=(size_t n)
    {
      idx_ += n;
      return *this;
    }

    const_iterator &operator-=(size_t n)
    {
      idx_ -= n;
      return *this;
    }

    const_iterator operator+(size_t n) const
    {
      const_iterator ret = *this;
      ret += n;
      return ret;
    }

    friend const_iterator operator+(size_t n, const const_iterator &me)
    {
      const_iterator ret = me;
      ret += n;
      return ret;
    }

    const_iterator operator-(size_t n) const
    {
      const_iterator ret = *this;
      ret -= n;
      return ret;
    }

    size_t operator-(const const_iterator &other) const
    {
      if (buf_ != other.buf_) {
        return (size_t) -1;
      }

      return idx_ - other.idx_;
    }

  private:
    const_iterator(const CircularBuffer &buf, size_t idx)
      : buf_(&buf), idx_(idx) {}

    const CircularBuffer *buf_;
    size_t idx_;

    friend class CircularBuffer<T>;
  };

  /// Mutable input iterator for elements of a CircularBuffer. Usual semantics.
  class iterator
  {
  public:
    using value_type = T;
    using difference_type = size_t;
    using reference = T &;
    using pointer = T *;
    using iterator_category = std::random_access_iterator_tag;

    reference operator *() const { return (*buf_)[idx_]; }
    reference at() const { return buf_->at(idx_); }
    pointer operator ->() const { return &(*buf_)[idx_]; }
    reference operator [](size_t i) const { return (*buf_)[idx_ + i]; }
    reference at(size_t i) const { return buf_->at(idx_ + i); }
    iterator &operator++() { ++idx_; return *this; }
    iterator &operator--() { --idx_; return *this; }
    iterator operator++(int) { auto ret = *this; ++idx_; return ret; }
    iterator operator--(int) { auto ret = *this; --idx_; return ret; }

    /// Index this iterator refers to
    size_t index() const { return idx_; }
    /// Actual index within buffer this iterator refers to
    size_t index_actual() const { return buf_->actual(idx_); }

    bool operator==(const iterator &other) const
    {
      return buf_ == other.buf_ && idx_ == other.idx_;
    }

    bool operator!=(const iterator &other) const
    {
      return !operator==(other);
    }

    bool operator<(const iterator &other) const
    {
      return buf_ < other.buf_ && idx_ < other.idx_;
    }

    bool operator>(const iterator &other) const
    {
      return buf_ > other.buf_ && idx_ > other.idx_;
    }

    bool operator<=(const iterator &other) const
    {
      return !operator>(other);
    }

    bool operator>=(const iterator &other) const
    {
      return !operator<(other);
    }

    iterator &operator+=(size_t n)
    {
      idx_ += n;
      return *this;
    }

    iterator &operator-=(size_t n)
    {
      idx_ -= n;
      return *this;
    }

    iterator operator+(size_t n) const
    {
      iterator ret = *this;
      ret += n;
      return ret;
    }

    friend iterator operator+(size_t n, const iterator &me)
    {
      iterator ret = me;
      ret += n;
      return ret;
    }

    iterator operator-(size_t n) const
    {
      iterator ret = *this;
      ret -= n;
      return ret;
    }

    size_t operator-(const iterator &other) const
    {
      if (buf_ != other.buf_) {
        return (size_t) -1;
      }

      return idx_ - other.idx_;
    }

  private:
    iterator(CircularBuffer &buf, size_t idx)
      : buf_(&buf), idx_(idx) {}

    CircularBuffer *buf_;
    size_t idx_;

    friend class CircularBuffer<T>;
  };

  /// Create a mutable iterator to the front of the buffer
  iterator begin() { return {*this, front_}; }

  /// Create a mutable iterator to the back of the buffer
  iterator end() { return {*this, back_}; }

  /// Create a const iterator to the front of the buffer
  const_iterator begin() const { return {*this, front_}; }

  /// Create a const iterator to the back of the buffer
  const_iterator end() const { return {*this, back_}; }

  /// Create a const iterator to the back of the buffer
  const_iterator cbegin() const { return {*this, front_}; }

  /// Create a const iterator to the back of the buffer
  const_iterator cend() const { return {*this, back_}; }

  /// Get a reference to the ith element. Undefined behavior if empty, or if
  /// index is outside front_index() and back_index().
  T &operator[](size_t i) { return data_[actual(i)]; }

  /// Get a const reference to the ith element. Undefined behavior if empty,
  /// or if index is outside front_index() and back_index().
  const T &operator[](size_t i) const { return data_[actual(i)]; }

  /// Get a reference to the front element. Undefined behavior if empty.
  T &front() { return (*this)[front_index()]; }

  /// Get a const reference to the front element. Undefined behavior if empty.
  const T &front() const { return (*this)[front_index()]; }

  /// Get a reference to the back element. Undefined behavior if empty.
  T &back() { return (*this)[back_index()]; }

  /// Get a const reference to the back element. Undefined behavior if empty.
  const T &back() const { return (*this)[back_index()]; }

  /// Get a copy of the front element, or a default constructed value if
  /// the buffer is empty.
  T get_front() { return empty() ? T() : front(); }

  /// Get a copy of the front element, or a default constructed value if
  /// the buffer is empty.
  T get_back() { return empty() ? T() : back(); }

  /**
   * Compares given index @a i relative to existing range of elements
   * (front_index() to back_index())
   *
   * @return 0 if index is within range of existing elements. Otherwise, if
   *   @a i < front_index(), return @i - front_index() (negative). Else, if
   *   @a i > back_index(), return @i - back_index() (positive).
   **/
  ssize_t check_range(size_t i) const
  {
    ssize_t ret = i - front_;
    if (ret < 0) {
      return ret;
    }
    ret = i - back_;
    if (ret > 0) {
      return ret;
    }
    return 0;
  }

private:
  void throw_out_of_range(const char *func, size_t i) const
  {
    if (i < front_)
    {
      std::stringstream ss;
      ss << "CircularBuffer::" << func << ": index " << i <<
            " is before oldest index " << front_;
      throw std::out_of_range(ss.str());
    }

    if (i >= back_)
    {
      std::stringstream ss;
      ss << "CircularBuffer::" << func << ": index " << i <<
            " passes newest index " << back_;
      throw std::out_of_range(ss.str());
    }
  }

public:
  /**
   * Get a reference to ith element. Throws std::out_of_range if this index
   * does not currently exist.
   **/
  T &at(size_t i)
  {
    throw_out_of_range(__func__, i);
    return (*this)[i];
  }

  /**
   * Get a const reference to ith element. Throws std::out_of_range if this
   * index does not currently exist.
   **/
  const T &at(size_t i) const
  {
    throw_out_of_range(__func__, i);
    return (*this)[i];
  }

  /**
   * Remove the front element and discard it.
   **/
  void discard_front()
  {
    front().~T();
    ++front_;
  }

  /**
   * Remove the front element and return it.
   **/
  T pop_front()
  {
    if (empty()) {
      return T{};
    }
    T ret = std::move(front());
    discard_front();
    return ret;
  }

  /**
   * Remove the back element and discard it.
   **/
  void discard_back()
  {
    back().~T();
    --back_;
  }

  /**
   * Remove the back element and return it.
   **/
  T pop_back()
  {
    if (empty()) {
      return T{};
    }
    T ret = std::move(back());
    discard_back();
    return ret;
  }

  /**
   * Construct a new element in-place in the back of the buffer. Will discard
   * the front element if size() already equals capacity().
   *
   * @return a reference to the new element
   **/
  template<typename... Args>
  T &emplace_back(Args&&... args)
  {
    if (size() >= capacity()) {
      discard_front();
    }
    ++back_;
    T &ret = *new(&(back())) T(std::forward<Args>(args)...);
    return ret;
  }

  /**
   * Copy the value @a val to the back of the buffer. Will discard
   * the front element if size() already equals capacity().
   *
   * @return a reference to the new element
   **/
  T &push_back(const T &val)
  {
    return emplace_back(val);
  }

  /**
   * Move the value @a val to the back of the buffer. Will discard
   * the front element if size() already equals capacity().
   *
   * @return a reference to the new element
   **/
  T &push_back(T &&val)
  {
    return emplace_back(std::move(val));
  }

private:
  size_t front_ = 0;
  size_t back_ = 0;

  T *data_ = nullptr;

  size_t cap_ = 0;

  friend class iterator;
  friend class const_iterator;
};

} }

#endif  // _MADARA_KNOWLEDGE_CIRCULAR_BUFFER_H_
