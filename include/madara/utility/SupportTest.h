#ifndef INCL_MADARA_UTILITY_SUPPORT_TEST_H
#define INCL_MADARA_UTILITY_SUPPORT_TEST_H

/**
 * @file SupportTest.h
 * @author David Kyle <dskyle@sei.cmu.edu>
 *
 * Provides MADARA_MAKE_SUPPORT_TEST macro
 **/

#include <type_traits>
#include <initializer_list>

/**
 * Macro which generates feature testing traits, to allow enabling features
 * based on what a given type supports. The tests provide ::value member
 * which is true if the given expr can compile correctly with the given
 * type; false otherwise
 *
 * var is a pointer to the type being tested
 */
#define MADARA_MAKE_SUPPORT_TEST(name, var, expr) template <typename T> \
struct supports_##name##_impl { \
    template<typename U> static auto test(U *var) -> decltype((expr), std::true_type()); \
    template<typename U> static auto test(...) -> std::false_type; \
    using type = decltype(test<T>(0)); \
}; \
template <typename T> struct supports_##name : supports_##name##_impl<T>::type {}

/**
 * Macro which generates feature testing traits, to allow enabling features
 * based on what a given type supports. The tests provide ::value member
 * which is true if the given expr can compile correctly with the given
 * type; false otherwise
 *
 * var is a value of the type being tested
 */
#define MADARA_MAKE_VAL_SUPPORT_TEST(name, var, expr) template <typename T> \
struct supports_##name##_impl { \
    template<typename U> static auto test(U var) -> decltype((expr), std::true_type()); \
    template<typename U> static auto test(...) -> std::false_type; \
    using type = decltype(test<T>(std::declval<T>())); \
}; \
template <typename T> struct supports_##name : supports_##name##_impl<T>::type {}

namespace madara {

/// helper type for specifying template type parameters using a function
/// argument instead of inside explicit "<...>". This interacts more flexibly
/// with overloading and ADL.
template<class T>
class type {};

template<bool Pred, typename T = void>
using enable_if_ = typename std::enable_if<Pred, T>::type;

template<typename T>
using decay_ = typename std::decay<T>::type;

template<typename T>
using remove_reference_ = typename std::remove_reference<T>::type;

template<typename T, typename U>
constexpr bool is_same() { return std::is_same<T, U>::value; }

template<typename T, typename U>
constexpr bool is_same_decayed() { return is_same<decay_<T>, decay_<U>>(); }

template<typename Base, typename Derived>
constexpr bool is_base_of() { return std::is_base_of<Base, Derived>::value; }

template<typename Base, typename Derived>
constexpr bool is_base_of_decayed() {
  return is_base_of<decay_<Base>, decay_<Derived>>();
}

template<typename From, typename To>
constexpr bool is_convertible() {
  return std::is_convertible<From, To>::value;
}

template<class T>
constexpr std::initializer_list<T> mk_init(std::initializer_list<T> i) {
  return i;
}

}

#endif
