
#ifndef OSPL_EXPORT_H
#define OSPL_EXPORT_H

#if defined(MADARA_AS_STATIC_LIBS) && !defined(OSPL_HAS_DLL)
#define OSPL_HAS_DLL 0
#endif /* OSPL_AS_STATIC_LIBS && OSPL_HAS_DLL */

#if !defined(OSPL_HAS_DLL)
#define OSPL_HAS_DLL 1
#endif /* ! OSPL_HAS_DLL */

#if defined(_WIN32)
#if !defined(OSPL_Proper_Export_Flag)
#define OSPL_Proper_Export_Flag __declspec(dllexport)
#define OSPL_Proper_Import_Flag __declspec(dllimport)
#endif
#else
#if !defined(OSPL_Proper_Export_Flag)
#define OSPL_Proper_Export_Flag
#define OSPL_Proper_Import_Flag
#endif
#endif

#if defined(OSPL_HAS_DLL) && (OSPL_HAS_DLL == 1)
#if defined(OSPL_BUILD_DLL)
#define OSPL_Export OSPL_Proper_Export_Flag
#else /* OSPL_BUILD_DLL */
#define OSPL_Export OSPL_Proper_Import_Flag
#endif /* OSPL_BUILD_DLL */
#else  /* OSPL_HAS_DLL == 1 */
#define OSPL_Export
#endif /* OSPL_HAS_DLL == 1 */

#endif /* OSPL_EXPORT_H */

// End of auto generated file.
