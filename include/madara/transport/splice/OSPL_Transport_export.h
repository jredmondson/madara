
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl OSPL_Transport
// ------------------------------
#ifndef OSPL_TRANSPORT_EXPORT_H
#define OSPL_TRANSPORT_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (OSPL_TRANSPORT_HAS_DLL)
#  define OSPL_TRANSPORT_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && OSPL_TRANSPORT_HAS_DLL */

#if !defined (OSPL_TRANSPORT_HAS_DLL)
#  define OSPL_TRANSPORT_HAS_DLL 1
#endif /* ! OSPL_TRANSPORT_HAS_DLL */

#if defined (OSPL_TRANSPORT_HAS_DLL) && (OSPL_TRANSPORT_HAS_DLL == 1)
#  if defined (OSPL_TRANSPORT_BUILD_DLL)
#    define OSPL_Transport_Export ACE_Proper_Export_Flag
#    define OSPL_TRANSPORT_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define OSPL_TRANSPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* OSPL_TRANSPORT_BUILD_DLL */
#    define OSPL_Transport_Export ACE_Proper_Import_Flag
#    define OSPL_TRANSPORT_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define OSPL_TRANSPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* OSPL_TRANSPORT_BUILD_DLL */
#else /* OSPL_TRANSPORT_HAS_DLL == 1 */
#  define OSPL_Transport_Export
#  define OSPL_TRANSPORT_SINGLETON_DECLARATION(T)
#  define OSPL_TRANSPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* OSPL_TRANSPORT_HAS_DLL == 1 */

// Set OSPL_TRANSPORT_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (OSPL_TRANSPORT_NTRACE)
#  if (ACE_NTRACE == 1)
#    define OSPL_TRANSPORT_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define OSPL_TRANSPORT_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !OSPL_TRANSPORT_NTRACE */

#if (OSPL_TRANSPORT_NTRACE == 1)
#  define OSPL_TRANSPORT_TRACE(X)
#else /* (OSPL_TRANSPORT_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define OSPL_TRANSPORT_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (OSPL_TRANSPORT_NTRACE == 1) */

#endif /* OSPL_TRANSPORT_EXPORT_H */

// End of auto generated file.
