

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

template<typename T>
class CircularBuffer
{
public:
  CircularBuffer() = default;

  explicit CircularBuffer(size_t size)
    : data_((T *)operator new(sizeof(T) * size)),
      cap_(size) {}

  CircularBuffer(const CircularBuffer &other)
    : CircularBuffer(other.size())
  {
    for (const T &cur : other) {
      emplace_back(cur);
    }
  }

  CircularBuffer(CircularBuffer &&other) noexcept
  {
    swap(other);
  }

  CircularBuffer &operator=(const CircularBuffer &other)
  {
    CircularBuffer tmp(other);
    *this = std::move(tmp);
    return *this;
  }

  CircularBuffer &operator=(CircularBuffer &&other) noexcept
  {
    swap(other);
    return *this;
  }

  ~CircularBuffer() noexcept
  {
    clear();
    delete data_;
  }

  void swap(CircularBuffer &other) noexcept
  {
    using std::swap;
    swap(front_, other.front_);
    swap(back_, other.back_);
    swap(data_, other.data_);
    swap(cap_, other.cap_);
  }

  void reserve(size_t size)
  {
    CircularBuffer tmp(size);
    while (!empty()) {
      tmp.emplace_back(std::move(pop_front()));
    }
    swap(tmp);
  }

  bool empty() const { return front_ == back_; }
  size_t capacity() const { return cap_; }
  size_t size() const { return back_ - front_; }
  size_t front_index() const { return front_; }
  size_t back_index() const { return back_ - 1; }
  size_t actual(size_t i) const { return i % capacity(); }
  size_t front_actual() const { return actual(front_index()); }
  size_t back_actual() const { return actual(back_index()); }

  void clear() {
    while (!empty()) {
      discard_front();
    }
  }

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
    size_t index() const { return idx_; }
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
    size_t index() const { return idx_; }
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

  iterator begin() { return {*this, front_}; }
  iterator end() { return {*this, back_}; }

  const_iterator begin() const { return {*this, front_}; }
  const_iterator end() const { return {*this, back_}; }
  const_iterator cbegin() const { return {*this, front_}; }
  const_iterator cend() const { return {*this, back_}; }

  T &operator[](size_t i) { return data_[actual(i)]; }
  const T &operator[](size_t i) const { return data_[actual(i)]; }

  T &front() { return (*this)[front_index()]; }
  const T &front() const { return (*this)[front_index()]; }
  T &back() { return (*this)[back_index()]; }
  const T &back() const { return (*this)[back_index()]; }

  T get_front() { return empty() ? T() : front(); }
  T get_back() { return empty() ? T() : back(); }

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
  T &at(size_t i)
  {
    throw_out_of_range(__func__, i);
    return (*this)[i];
  }

  const T &at(size_t i) const
  {
    throw_out_of_range(__func__, i);
    return (*this)[i];
  }

  void discard_front()
  {
    //std::cerr << "Discarding front: " << front_ << std::endl;
    front().~T();
    ++front_;
  }

  T pop_front()
  {
    if (empty()) {
      return T{};
    }
    T ret = std::move(front());
    discard_front();
    return ret;
  }

  void discard_back()
  {
    //std::cerr << "Discarding back: " << back_ << std::endl;
    back().~T();
    --back_;
  }

  T pop_back()
  {
    if (empty()) {
      return T{};
    }
    T ret = std::move(back());
    discard_back();
    return ret;
  }

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

  T &push_back(const T &val)
  {
    return emplace_back(val);
  }

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
