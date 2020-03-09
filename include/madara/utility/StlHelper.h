#pragma once

/**
 * @file Comparators.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * General helpers for STL conversions and usage
 **/

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

    // template<typename Func, typename Arg0>
    // MADARA_AUTORET_REF_FUNC(
    //     invoke_, (Func func, Arg0&& arg0), std::forward<Arg0>(arg0).*func)

    // template<typename Func, typename Arg0, typename... Args>
    // MADARA_AUTORET_REF_FUNC(invoke_, (Func func, Arg0&& arg0, Args&&... args),
    //     (std::forward<Arg0>(arg0).*func)(std::forward<Args>(args)...))

    // template<typename Func, typename... Args>
    // MADARA_AUTORET_REF_FUNC(
    //     invoke_, (Func func, Args&&... args), func(std::forward<Args>(args)...))

  }
}