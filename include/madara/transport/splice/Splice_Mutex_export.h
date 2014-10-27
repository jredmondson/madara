
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl Splice_Mutex
// ------------------------------
#ifndef SPLICE_MUTEX_EXPORT_H
#define SPLICE_MUTEX_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (SPLICE_MUTEX_HAS_DLL)
#  define SPLICE_MUTEX_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && SPLICE_MUTEX_HAS_DLL */

#if !defined (SPLICE_MUTEX_HAS_DLL)
#  define SPLICE_MUTEX_HAS_DLL 1
#endif /* ! SPLICE_MUTEX_HAS_DLL */

#if defined (SPLICE_MUTEX_HAS_DLL) && (SPLICE_MUTEX_HAS_DLL == 1)
#  if defined (SPLICE_MUTEX_BUILD_DLL)
#    define Splice_Mutex_Export ACE_Proper_Export_Flag
#    define SPLICE_MUTEX_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define SPLICE_MUTEX_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* SPLICE_MUTEX_BUILD_DLL */
#    define Splice_Mutex_Export ACE_Proper_Import_Flag
#    define SPLICE_MUTEX_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define SPLICE_MUTEX_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* SPLICE_MUTEX_BUILD_DLL */
#else /* SPLICE_MUTEX_HAS_DLL == 1 */
#  define Splice_Mutex_Export
#  define SPLICE_MUTEX_SINGLETON_DECLARATION(T)
#  define SPLICE_MUTEX_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* SPLICE_MUTEX_HAS_DLL == 1 */

// Set SPLICE_MUTEX_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (SPLICE_MUTEX_NTRACE)
#  if (ACE_NTRACE == 1)
#    define SPLICE_MUTEX_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define SPLICE_MUTEX_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !SPLICE_MUTEX_NTRACE */

#if (SPLICE_MUTEX_NTRACE == 1)
#  define SPLICE_MUTEX_TRACE(X)
#else /* (SPLICE_MUTEX_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define SPLICE_MUTEX_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (SPLICE_MUTEX_NTRACE == 1) */

#endif /* SPLICE_MUTEX_EXPORT_H */

// End of auto generated file.
