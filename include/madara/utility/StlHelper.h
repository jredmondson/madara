#pragma once

/**
 * @file Comparators.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * General helpers for STL conversions and usage
 **/

#include <type_traits>

/**
 * Macro which generates feature testing traits, to allow enabling features
 * based on what a given type supports. The tests provide ::value member
 * which is true if the given expr can compile correctly with the given
 * type; false otherwise
 *
 * var is a pointer to the type being tested
 */
#define MADARA_MAKE_SUPPORT_TEST(name, var, expr)                   \
  template<typename T>                                              \
  struct supports_##name##_impl                                     \
  {                                                                 \
    template<typename U>                                            \
    static auto test(U* var) -> decltype((expr), std::true_type()); \
    template<typename U>                                            \
    static auto test(...) -> std::false_type;                       \
    using type = decltype(test<T>(0));                              \
  };                                                                \
  template<typename T>                                              \
  struct supports_##name : supports_##name##_impl<T>::type          \
  {                                                                 \
  }

/**
 * Macro which generates feature testing traits, to allow enabling features
 * based on what a given type supports. The tests provide ::value member
 * which is true if the given expr can compile correctly with the given
 * type; false otherwise
 *
 * var is a value of the type being tested
 */
#define MADARA_MAKE_VAL_SUPPORT_TEST(name, var, expr)              \
  template<typename T>                                             \
  struct supports_##name##_impl                                    \
  {                                                                \
    template<typename U>                                           \
    static auto test(U var) -> decltype((expr), std::true_type()); \
    template<typename U>                                           \
    static auto test(...) -> std::false_type;                      \
    using type = decltype(test<T>(std::declval<T>()));             \
  };                                                               \
  template<typename T>                                             \
  struct supports_##name : supports_##name##_impl<T>::type         \
  {                                                                \
  }

namespace madara
{
namespace utility
{
/// helper type for specifying template type parameters using a function
/// argument instead of inside explicit "<...>". This interacts more flexibly
/// with overloading and ADL.
template<class T>
struct type
{
  type() = default;
  using self = T;
};

/// Less verbose synonym for std::enable_if
template<bool Pred, typename T = void>
using enable_if_ = typename std::enable_if<Pred, T>::type;

/// Less verbose synonym for std::decay
template<typename T>
using decay_ = typename std::decay<T>::type;

/**
 * Creates a unique_ptr for the templated type
 **/
template<typename T, typename... Args>
std::unique_ptr<T> mk_unique(Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 * Converts a typed value into a unique_ptr of a decayed type
 **/
template<typename T>
std::unique_ptr<decay_<T>> into_unique(T&& val)
{
  using V = decay_<T>;
  return mk_unique<V>(std::forward<T>(val));
}


/// Less verbose equivalent for std::is_same
template<typename T, typename U>
constexpr bool is_same()
{
  return std::is_same<T, U>::value;
}


/// Composition of std::is_same and std::decay
template<typename T, typename U>
constexpr bool is_same_decayed()
{
  return is_same<decay_<T>, decay_<U>>();
}

/// Less verbose equivalent for std::is_base_of
template<typename Base, typename Derived>
constexpr bool is_base_of()
{
  return std::is_base_of<Base, Derived>::value;
}

/// Composition of std::is_base_of and std::decay
template<typename Base, typename Derived>
constexpr bool is_base_of_decayed()
{
  return is_base_of<decay_<Base>, decay_<Derived>>();
}

/// Less verbose equivalent for std::is_convertible
template<typename From, typename To>
constexpr bool is_convertible()
{
  return std::is_convertible<From, To>::value;
}

/// Less verbose equivalent for std::is_integral
template<typename T>
constexpr bool is_integral()
{
  return std::is_integral<T>::value;
}

/// Less verbose equivalent for std::is_floating_point
template<typename T>
constexpr bool is_floating_point()
{
  return std::is_floating_point<T>::value;
}

/// Less verbose equivalent for std::is_arithmetic
template<typename T>
constexpr bool is_arithmetic()
{
  return std::is_arithmetic<T>::value;
}

/// Less verbose equivalent for std::is_enum
template<typename T>
constexpr bool is_enum()
{
  return std::is_enum<T>::value;
}

/// Is T arithmetic or an enum?
template<typename T>
constexpr bool is_numeric()
{
  return is_arithmetic<T>() || is_enum<T>();
}

/// Is T arithmetic or an enum?
template<typename T>
constexpr bool is_int_numeric()
{
  return is_integral<T>() || is_enum<T>();
}

#define MADARA_AUTORET_FUNC(NAME, ARGS, ...)                       \
  inline auto NAME                                                 \
      ARGS->::madara::utility::core::decay_<decltype(__VA_ARGS__)> \
  {                                                                \
    return (__VA_ARGS__);                                          \
  }

#define MADARA_AUTORET_REF_FUNC(NAME, ARGS, ...) \
  inline auto NAME ARGS->decltype(__VA_ARGS__)   \
  {                                              \
    return (__VA_ARGS__);                        \
  }

template<typename Func, typename Arg0>
MADARA_AUTORET_REF_FUNC(
    invoke_, (Func func, Arg0&& arg0), std::forward<Arg0>(arg0).*func)

template<typename Func, typename Arg0, typename... Args>
MADARA_AUTORET_REF_FUNC(invoke_, (Func func, Arg0&& arg0, Args&&... args),
    (std::forward<Arg0>(arg0).*func)(std::forward<Args>(args)...))

template<typename Func, typename... Args>
MADARA_AUTORET_REF_FUNC(
    invoke_, (Func func, Args&&... args), func(std::forward<Args>(args)...))

}  // namespace utility
}  // namespace madara