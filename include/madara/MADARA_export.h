
#ifndef MADARA_EXPORT_H
#define MADARA_EXPORT_H

#if defined  (_WIN32) && !defined (MADARA_BUILD_STATIC)
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

#if defined (MADARA_BUILD_DLL)
#  define MADARA_Export MADARA_Proper_Export_Flag
#else
#  define MADARA_Export MADARA_Proper_Import_Flag
#endif /* MADARA_BUILD_DLL */

#endif /* MADARA_EXPORT_H */

// End of auto generated file.
