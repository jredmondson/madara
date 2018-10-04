
#ifndef _MADARA_INTTYPES_H_
#define _MADARA_INTTYPES_H_

// Because Microsoft decided to not support inttypes.h
// from VS 2003 until VS 2012, we simply typedef the
// symbols that are supported in all MS products when
// we know we have a MS compiler

#ifdef _MSC_VER

#if (_MSC_VER <= 1600)

#ifndef PRIu64

#define PRId32 "I32d"
#define PRIi32 "I32i"
#define PRIo32 "I32o"
#define PRIu32 "I32u"
#define PRIx32 "I32x"
#define PRIX32 "I32X"

#define PRId64 "I64d"
#define PRIi64 "I64i"
#define PRIo64 "I64o"
#define PRIu64 "I64u"
#define PRIx64 "I64x"
#define PRIX64 "I64X"
#include <BaseTsd.h>
#if !defined(ssize_t)
typedef SSIZE_T ssize_t;
#endif

#endif  // ! ifndef PRIu64

#else  // (MSC_VER > 1600)

// otherwise, we do something more logical
#include <inttypes.h>
#include <stddef.h>
#include <BaseTsd.h>
#if !defined(ssize_t)
typedef SSIZE_T ssize_t;
#endif
#endif  // (MSC_VER <= 1600)

#else  // !_MSC_VER

// otherwise, we do something more logical
#include <inttypes.h>
#include <stddef.h>

#endif  // _MSC_VER

#endif  // _MADARA_INTTYPES_H_
