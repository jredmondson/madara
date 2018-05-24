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

#endif
