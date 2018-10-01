
#ifndef MADARA_EXPORT_H
#define MADARA_EXPORT_H

#if defined(_WIN32) && !defined(MADARA_BUILD_STATIC)
#if !defined(MADARA_PROPER_EXPORT_FLAG)
#define MADARA_PROPER_EXPORT_FLAG __declspec(dllexport)
#define MADARA_PROPER_IMPORT_FLAG __declspec(dllimport)
#endif
#else
#if !defined(MADARA_PROPER_EXPORT_FLAG)
#define MADARA_PROPER_EXPORT_FLAG
#define MADARA_PROPER_IMPORT_FLAG
#endif
#endif

#if defined(MADARA_BUILD_DLL)
#define MADARA_EXPORT MADARA_PROPER_EXPORT_FLAG
#else
#define MADARA_EXPORT MADARA_PROPER_IMPORT_FLAG
#endif /* MADARA_BUILD_DLL */

#endif /* MADARA_EXPORT_H */

// End of auto generated file.
