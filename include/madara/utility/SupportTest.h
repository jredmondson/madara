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

namespace madara { namespace utility {

inline namespace core {

/// helper type for specifying template type parameters using a function
/// argument instead of inside explicit "<...>". This interacts more flexibly
/// with overloading and ADL.
template<class T>
struct type { using self = T; };

/// Less verbose synonym for std::enable_if
template<bool Pred, typename T = void>
using enable_if_ = typename std::enable_if<Pred, T>::type;

/// Less verbose synonym for std::decay
template<typename T>
using decay_ = typename std::decay<T>::type;

/// Less verbose synonym for std::remove_reference
template<typename T>
using remove_reference_ = typename std::remove_reference<T>::type;

/// Less verbose equivalent for std::is_same
template<typename T, typename U>
constexpr bool is_same() { return std::is_same<T, U>::value; }

/// Composition of std::is_same and std::decay
template<typename T, typename U>
constexpr bool is_same_decayed() { return is_same<decay_<T>, decay_<U>>(); }

/// Less verbose equivalent for std::is_base_of
template<typename Base, typename Derived>
constexpr bool is_base_of() { return std::is_base_of<Base, Derived>::value; }

/// Composition of std::is_base_of and std::decay
template<typename Base, typename Derived>
constexpr bool is_base_of_decayed() {
  return is_base_of<decay_<Base>, decay_<Derived>>();
}

/// Less verbose equivalent for std::is_convertible
template<typename From, typename To>
constexpr bool is_convertible() {
  return std::is_convertible<From, To>::value;
}

/// Convenience trait for accepting various reference types of the same
/// underlying type
template<typename TParam, typename Actual, typename T = void>
using enable_if_same_decayed = enable_if_<is_same_decayed<TParam, Actual>(), T>;

/// Helper function for forwarding std::initializer_list through perfect-
/// forwarding interfaces (such as emplace methods). Wrap the braced list
/// in mk_init() to force it to be treated as an initializer_list and forward
/// correctly.
template<class T>
constexpr std::initializer_list<T> mk_init(std::initializer_list<T> i) {
  return i;
}

/// Functor which ignores all arguments when called, and always returns a
/// default-constructed Ret
template<class Ret = void>
struct ignore_all
{
  template<typename... Args>
  constexpr Ret operator()(Args&&...) const { return Ret{}; }
};

/// Helper type for managing overload priority. Use overload_priority alias
template<int N>
struct overload_priority_t : overload_priority_t<N + 1> {};

/// Base case for overload_priority implementation
template<>
struct overload_priority_t<16> {};

/// Helper type for managing overload priority. Include an instantiation as
/// argument to allow otherwise ambiguous overloads to coexist. The overload
/// which has an instantiation of lowest number will be chosen, if otherwise
/// applicable.
template<int N>
using overload_priority = overload_priority_t<N>*;

/// Weakest overload_priority. Any lower value will take precedence.
using overload_priority_weakest = overload_priority<16>;

/// Strongest overload_priority. Nothing can take precedence, unless other
/// arguments are applicable to overload resolution.
using overload_priority_strongest = overload_priority<0>;

/// Call this to pass into the overload_priority argument of a function.
inline overload_priority_strongest select_overload() { return {}; }

/// Internal use. Implementation helper for is_type_tag.
template<typename T, typename = void>
struct is_type_tag_impl : std::false_type {};

/// Internal use. Implementation helper for is_type_tag.
template<typename T>
struct is_type_tag_impl<type<T>> : std::true_type {};

/// Test if T is a madara::utility::type<> instantiation
template<typename T>
constexpr bool is_type_tag() { return is_type_tag_impl<decay_<T>>::value; }

}

}

using namespace utility::core;

}

#endif
