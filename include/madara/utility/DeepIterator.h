#ifndef MADARA_DEEPITERATOR_H
#define MADARA_DEEPITERATOR_H

/**
 * Helper class for type inference
 **/
template<class T>
struct TypeHelper
{
  typedef void type;
};

/**
 * Specialize this template to support various kinds of iterators.
 *
 * Must provide a typedef value_type, which is the type the DeepIterator
 * will return, and a get_deep_copy which, given a const reference to the
 * base iterator, returns value_type, with appropriate deep copying.
 *
 * See existing specializations for examples.
 *
 * There is no non-specialized version given, so that unsupported iterator
 * types will produce a compile-time error.
 **/
template<class T, class U = void, class V = void>
struct IteratorTraits; /* If error here: invalid type passed to deep_iterate()
                        */

/**
 * Specialization for plain iterators, where we can call deep_copy directly
 * on the iterator
 **/
template<class T, class V>
struct IteratorTraits<T, typename TypeHelper<typename T::value_type>::type, V>
{
  enum
  {
    is_pair = 0
  };

  typedef typename T::value_type value_type;

  static value_type get_deep_copy(const T& i)
  {
    return value_type(i->deep_copy());
  }
};

/**
 * Specialization for map-style pair iterators, where we want to deep-copy
 * the value, but not the key.
 **/
template<class T>
struct IteratorTraits<T, typename TypeHelper<typename T::value_type>::type,
    typename TypeHelper<typename T::value_type::second_type>::type>
{
  enum
  {
    is_pair = 1
  };

  typedef std::pair<const typename T::value_type::first_type&,
      typename T::value_type::second_type>
      value_type;

  static value_type get_deep_copy(const T& i)
  {
    return value_type(i->first, i->second.deep_copy());
  }
};

/**
 * The iterator type returned by deep_iterate. Use that function instead of
 * constructing this class directly.
 *
 * @tparam Iterator the type of the underlying iterator. Automatically
 * determined by deep_iterate.
 **/
template<class Iterator>
class DeepIterator : public std::iterator<std::input_iterator_tag,
                         typename IteratorTraits<Iterator>::value_type>
{
private:
  typedef IteratorTraits<Iterator> traits;

public:
  typedef typename DeepIterator::value_type value_type;
  typedef typename Iterator::value_type underlying_value_type;

  /**
   * Returns the same as the underlying iterator's operator*, except
   * any values are deep copied first
   **/
  value_type operator*() const
  {
    return traits::get_deep_copy(i_);
  }

  /**
   * Pass-through to the underlying iterator's operator->().
   * Note that methods/variables accessed through this operator
   * will reference the original, not a deep copy.
   *
   * @return pointer to the underlying iterator's value
   **/
  underlying_value_type* operator->() const
  {
    return i_.operator->();
  }

  /**
   * Pre-fix increment. Increments underlying iterator, and returns this
   * iterator afterwards. Use instead of post-fix whereever possible.
   *
   * @return *this
   **/
  DeepIterator& operator++()
  {
    ++i_;
    return *this;
  }

  /**
   * Post-fix increment. Increments underlying iterator, and returns
   * a copy of this iterator (along with a copy of the underlying iterator)
   * from prior to this increment. If you don't need the previous iterator
   * value, use the pre-fix increment instead.
   *
   * Note: calls the underlying iterator's pre-fix increment, not post-fix.
   *
   * @return a copy of *this from prior to the increment
   **/
  DeepIterator operator++(int)
  {
    DeepIterator<Iterator> ret(*this);
    ++i_;
    return *this;
  }

  /**
   * Equality operator. Passes through to underlying iterator.
   *
   * @return same as underlying iterator
   **/
  bool operator==(const DeepIterator& o) const
  {
    return i_ == o.i_;
  }

  /**
   * Equality operator. Passes through to underlying iterator.
   *
   * @return same as underlying iterator
   **/
  bool operator!=(const DeepIterator& o) const
  {
    return i_ != o.i_;
  }

private:
  DeepIterator(const Iterator& i) : i_(i) {}

  Iterator i_;

  template<class I>
  friend DeepIterator<I> deep_iterate(const I& i);
};

/**
 * Returns an input iterator from an iterator. Supports two kinds of iterators:
 *
 * Direct iterators, where given iterator i, i->deep_copy() returns a deep copy
 * of the value; OR
 *
 * Pair iterators (e.g., map::iterator), where given iterator i,
 * i->second.deep_copy() returns a deep copy of the value.
 *
 * In either case, the returned iterator behaves like the original, except
 * restricted to input semantics, and the values are deep copies of the original
 * values. Elements not deep copied (such as the key in a map's key-value pair)
 * may be const references to the original instead of a copy.
 *
 * @param i the iterator to mimic. The iterator will be copied internally.
 * @return the input iterator
 **/
template<class Iterator>
DeepIterator<Iterator> deep_iterate(const Iterator& i)
{
  return DeepIterator<Iterator>(i);
}

#endif
