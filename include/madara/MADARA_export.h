
#ifndef MADARA_EXPORT_H
#define MADARA_EXPORT_H

#if defined (MADARA_AS_STATIC_LIBS) && !defined (MADARA_HAS_DLL)
#  define MADARA_HAS_DLL 0
#endif /* MADARA_AS_STATIC_LIBS && MADARA_HAS_DLL */

#if !defined (MADARA_HAS_DLL)
#  define MADARA_HAS_DLL 1
#endif /* ! MADARA_HAS_DLL */

#if defined  (_WIN32)
#  if !defined (MADARA_Proper_Export_Flag)
#    define MADARA_Proper_Export_Flag __declspec (dllexport)
#    define MADARA_Proper_Import_Flag __declspec (dllimport)
#  endif
#else
#  if !defined (MADARA_Proper_Export_Flag)
#    define MADARA_Proper_Export_Flag
#    define MADARA_Proper_Import_Flag
#  endif
#endif

#if defined (MADARA_HAS_DLL) && (MADARA_HAS_DLL == 1)
#  if defined (MADARA_BUILD_DLL)
#    define MADARA_Export MADARA_Proper_Export_Flag
#  else /* MADARA_BUILD_DLL */
#    define MADARA_Export MADARA_Proper_Import_Flag
#  endif /* MADARA_BUILD_DLL */
#else /* MADARA_HAS_DLL == 1 */
#  define MADARA_Export
#endif /* MADARA_HAS_DLL == 1 */

#endif /* MADARA_EXPORT_H */

// End of auto generated file.
