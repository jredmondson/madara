#ifndef MADARA_TEST_TEST_H_
#define MADARA_TEST_TEST_H_

#include <utility>
#include <string>
#include <sstream>

#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"

template<typename... Args>
inline void log(Args&&... args)
{
  using namespace madara::logger;
  madara_logger_ptr_log(
      global_logger.get(), LOG_ALWAYS, std::forward<Args>(args)...);
}

#define LOG(expr) (log("INFO    : " #expr "\n"), (expr))

#define VAL(expr)                                          \
  ([&]() mutable -> decltype(expr) {                       \
    decltype(expr) e = (expr);                             \
    std::ostringstream msg;                                \
    msg << e;                                              \
    log("INFO    : " #expr " == %s\n", msg.str().c_str()); \
    return e;                                              \
  }())

/// Test failure count, local to each translation unit (i.e., .cpp file)
static int madara_tests_fail_count = 0;

/// Reset this translation unit's failure count
static inline void madara_tests_reset_count()
{
  madara_tests_fail_count = 0;
}

#define TEST_OP(lhs, op, rhs)                                     \
  do                                                              \
  {                                                               \
    decltype(lhs) l = (lhs);                                      \
    decltype(rhs) r = (rhs);                                      \
    std::ostringstream msg;                                       \
    msg << #lhs " [" << l << "] " #op " " #rhs " [" << r << "] "; \
    std::string smsg = msg.str();                                 \
    const char* cmsg = smsg.c_str();                              \
    if (l op r)                                                   \
    {                                                             \
      log("SUCCESS : %s\n", cmsg);                                \
    }                                                             \
    else                                                          \
    {                                                             \
      log("FAIL    : %s\n", cmsg);                                \
      ++madara_tests_fail_count;                                  \
    }                                                             \
  } while (0)

#define TEST_EQ(lhs, rhs) TEST_OP(lhs, ==, rhs)
#define TEST_NE(lhs, rhs) TEST_OP(lhs, !=, rhs)
#define TEST_LT(lhs, rhs) TEST_OP(lhs, <, rhs)
#define TEST_LE(lhs, rhs) TEST_OP(lhs, <=, rhs)
#define TEST_GT(lhs, rhs) TEST_OP(lhs, >, rhs)
#define TEST_GE(lhs, rhs) TEST_OP(lhs, >=, rhs)

#endif
