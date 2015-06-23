
#ifndef _MADARA_STDINT_H_
#define _MADARA_STDINT_H_

// Because Microsoft decided to not support stdint.h
// from VS 2003 until VS 2010, we simply typedef the
// symbols that are supported in all MS products when
// we know we have a MS compiler

#ifdef _MSC_VER

  #if (MSC_VER < 1600)

    #ifndef int32_t

    typedef __int32             int32_t;
    typedef unsigned __int32    uint32_t;
    typedef __int64             int64_t;
    typedef unsigned __int64    uint64_t;

    #endif

  #else // (MSC_VER >= 1600)

      // otherwise, we do something more logical
      #include <stdint.h>

  #endif // (MSC_VER < 1600)

#else   // !_MSC_VER

  // otherwise, we do something more logical
  #include <stdint.h>

#endif  // _MSC_VER

#endif // _MADARA_STDINT_H_