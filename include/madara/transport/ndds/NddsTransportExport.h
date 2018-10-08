
#ifndef NDDS_EXPORT_H
#define NDDS_EXPORT_H

#if defined(MADARA_AS_STATIC_LIBS) && !defined(NDDS_HAS_DLL)
#define NDDS_HAS_DLL 0
#endif /* NDDS_AS_STATIC_LIBS && NDDS_HAS_DLL */

#if !defined(NDDS_HAS_DLL)
#define NDDS_HAS_DLL 1
#endif /* ! NDDS_HAS_DLL */

#if defined(_WIN32)
#if !defined(NDDS_Proper_Export_Flag)
#define NDDS_Proper_Export_Flag __declspec(dllexport)
#define NDDS_Proper_Import_Flag __declspec(dllimport)
#endif
#else
#if !defined(NDDS_Proper_Export_Flag)
#define NDDS_Proper_Export_Flag
#define NDDS_Proper_Import_Flag
#endif
#endif

#if defined(NDDS_HAS_DLL) && (NDDS_HAS_DLL == 1)
#if defined(NDDS_BUILD_DLL)
#define NDDS_Export NDDS_Proper_Export_Flag
#else /* NDDS_BUILD_DLL */
#define NDDS_Export NDDS_Proper_Import_Flag
#endif /* NDDS_BUILD_DLL */
#else  /* NDDS_HAS_DLL == 1 */
#define NDDS_Export
#endif /* NDDS_HAS_DLL == 1 */

#endif /* NDDS_EXPORT_H */

// End of auto generated file.
