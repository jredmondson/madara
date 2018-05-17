#ifndef INCL_MADARA_TEST_H
#define INCL_MADARA_TEST_H

#include <iostream>
#include <string>
#include <vector>

static int fail_count = 0;
static int test_count = 0;

inline bool print_all(std::ostream &) { return false; }

template<typename T, typename... Args>
inline bool print_all(std::ostream &o, T&& v, Args&& ...args)
{
  o << v;
  print_all(o, std::forward<Args>(args)...);
  return true;
}

template<typename... Args>
inline bool print_all_err(Args&& ...args)
{
  return print_all(std::cerr, std::forward<Args>(args)...);
}

static const char *fail_str = "[ FAIL! ]";
static const char *success_str = "[SUCCESS]";

#define test(cond, ...) \
  do { \
    ++test_count; \
    if (!(cond)) { \
      std::cerr << fail_str << " "; \
      ++fail_count; \
    } else { \
      std::cerr << success_str << " "; \
    } \
    if (print_all_err(__VA_ARGS__)) std::cerr << ": "; \
    std::cerr << #cond << std::endl; \
  } while(0);

#define test_eq(lhs, rhs, ...) \
  do { \
    auto l = (lhs); \
    auto r = (rhs); \
    ++test_count; \
    if (!((l) == (r))) { \
      std::cerr << fail_str << " "; \
      ++fail_count; \
    } else { \
      std::cerr << success_str << " "; \
    } \
    if (print_all_err(__VA_ARGS__)) std::cerr << ": "; \
    std::cerr << #lhs << " [" << l << "] == " << #rhs << " [" << r << "]" << std::endl; \
  } while(0);

#define test_neq(lhs, rhs, ...) \
  do { \
    auto l = (lhs); \
    auto r = (rhs); \
    ++test_count; \
    if ((l) == (r)) { \
      std::cerr << fail_str << " "; \
      ++fail_count; \
    } else { \
      std::cerr << success_str << " "; \
    } \
    if (print_all_err(__VA_ARGS__)) std::cerr << " "; \
    std::cerr << #lhs << " [" << l << "] != " << #rhs << " [" << r << "]" << std::endl; \
  } while(0);

#define tests_finalize() \
  do { \
    int pass_count = test_count - fail_count; \
    std::cout << (fail_count == 0 ? "TESTS SUCCEEDED" : "TESTS FAILED"); \
    std::cout << ": " << pass_count << " out of " << test_count << " passed" << std::endl; \
    return fail_count == 0 ? 0 : 1; \
  } while(0)

template<typename T>
inline ostream &operator<<(ostream &o, const std::vector<T> &v)
{
  int count = 0;
  o << "{";
  for (const auto &cur : v) {
    if (count++ > 0) o << ", ";
    o << cur;
  }
  o << "}";
  return o;
}

#endif // INCL_MADARA_TEST_H
