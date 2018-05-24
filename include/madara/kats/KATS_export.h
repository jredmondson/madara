
#ifndef KATS_EXPORT_H
#define KATS_EXPORT_H

#if defined (KATS_AS_STATIC_LIBS) && !defined (KATS_HAS_DLL)
#  define KATS_HAS_DLL 0
#endif /* KATS_AS_STATIC_LIBS && KATS_HAS_DLL */

#if !defined (KATS_HAS_DLL)
#  define KATS_HAS_DLL 1
#endif /* ! KATS_HAS_DLL */

#if defined  (_WIN32)
#  if !defined (KATS_Proper_Export_Flag)
#    define KATS_Proper_Export_Flag __declspec (dllexport)
#    define KATS_Proper_Import_Flag __declspec (dllimport)
#  endif
#else
#  if !defined (KATS_Proper_Export_Flag)
#    define KATS_Proper_Export_Flag
#    define KATS_Proper_Import_Flag
#  endif
#endif

#if defined (KATS_HAS_DLL) && (KATS_HAS_DLL == 1)
#  if defined (KATS_BUILD_DLL)
#    define KATS_Export KATS_Proper_Export_Flag
#  else /* KATS_BUILD_DLL */
#    define KATS_Export KATS_Proper_Import_Flag
#  endif /* KATS_BUILD_DLL */
#else /* KATS_HAS_DLL == 1 */
#  define KATS_Export
#endif /* KATS_HAS_DLL == 1 */

#endif /* KATS_EXPORT_H */

// End of auto generated file.
