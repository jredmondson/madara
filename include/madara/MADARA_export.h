
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl -d MADARA
// ------------------------------
#ifndef MADARA_EXPORT_H
#define MADARA_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (MADARA_HAS_DLL)
#  define MADARA_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && MADARA_HAS_DLL */

#if !defined (MADARA_HAS_DLL)
#  define MADARA_HAS_DLL 1
#endif /* ! MADARA_HAS_DLL */

#if defined (MADARA_HAS_DLL) && (MADARA_HAS_DLL == 1)
#  if defined (MADARA_BUILD_DLL)
#    define MADARA_Export ACE_Proper_Export_Flag
#    define MADARA_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define MADARA_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* MADARA_BUILD_DLL */
#    define MADARA_Export ACE_Proper_Import_Flag
#    define MADARA_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define MADARA_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* MADARA_BUILD_DLL */
#else /* MADARA_HAS_DLL == 1 */
#  define MADARA_Export
#  define MADARA_SINGLETON_DECLARATION(T)
#  define MADARA_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* MADARA_HAS_DLL == 1 */

// Set MADARA_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
//#if !defined (MADARA_NTRACE)
//#  if (ACE_NTRACE == 1)
//#    define MADARA_NTRACE 1
//#  else /* (ACE_NTRACE == 1) */
//#    define MADARA_NTRACE 0
//#  endif /* (ACE_NTRACE == 1) */
//#endif /* !MADARA_NTRACE */

//#if (MADARA_NTRACE == 1)
//#  define MADARA_TRACE(X)
//#else /* (MADARA_NTRACE == 1) */
//#  if !defined (ACE_HAS_TRACE)
//#    define ACE_HAS_TRACE
//#  endif /* ACE_HAS_TRACE */
//#  define MADARA_TRACE(X) ACE_TRACE_IMPL(X)
//#  include "ace/Trace.h"
//#endif /* (MADARA_NTRACE == 1) */

#endif /* MADARA_EXPORT_H */

// End of auto generated file.
