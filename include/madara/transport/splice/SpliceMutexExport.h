
#ifndef SPLICE_MUTEX_EXPORT_H
#define SPLICE_MUTEX_EXPORT_H

#if defined(MADARA_AS_STATIC_LIBS) && !defined(SPLICE_MUTEX_HAS_DLL)
#define SPLICE_MUTEX_HAS_DLL 0
#endif /* SPLICE_MUTEX_AS_STATIC_LIBS && SPLICE_MUTEX_HAS_DLL */

#if !defined(SPLICE_MUTEX_HAS_DLL)
#define SPLICE_MUTEX_HAS_DLL 1
#endif /* ! SPLICE_MUTEX_HAS_DLL */

#if defined(_WIN32)
#if !defined(SPLICE_MUTEX_Proper_Export_Flag)
#define SPLICE_MUTEX_Proper_Export_Flag __declspec(dllexport)
#define SPLICE_MUTEX_Proper_Import_Flag __declspec(dllimport)
#endif
#else
#if !defined(SPLICE_MUTEX_Proper_Export_Flag)
#define SPLICE_MUTEX_Proper_Export_Flag
#define SPLICE_MUTEX_Proper_Import_Flag
#endif
#endif

#if defined(SPLICE_MUTEX_HAS_DLL) && (SPLICE_MUTEX_HAS_DLL == 1)
#if defined(SPLICE_MUTEX_BUILD_DLL)
#define SPLICE_MUTEX_Export SPLICE_MUTEX_Proper_Export_Flag
#else /* SPLICE_MUTEX_BUILD_DLL */
#define SPLICE_MUTEX_Export SPLICE_MUTEX_Proper_Import_Flag
#endif /* SPLICE_MUTEX_BUILD_DLL */
#else  /* SPLICE_MUTEX_HAS_DLL == 1 */
#define SPLICE_MUTEX_Export
#endif /* SPLICE_MUTEX_HAS_DLL == 1 */

#endif /* SPLICE_MUTEX_EXPORT_H */

// End of auto generated file.
