
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl NDDS_Transport
// ------------------------------
#ifndef NDDS_TRANSPORT_EXPORT_H
#define NDDS_TRANSPORT_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (NDDS_TRANSPORT_HAS_DLL)
#  define NDDS_TRANSPORT_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && NDDS_TRANSPORT_HAS_DLL */

#if !defined (NDDS_TRANSPORT_HAS_DLL)
#  define NDDS_TRANSPORT_HAS_DLL 1
#endif /* ! NDDS_TRANSPORT_HAS_DLL */

#if defined (NDDS_TRANSPORT_HAS_DLL) && (NDDS_TRANSPORT_HAS_DLL == 1)
#  if defined (NDDS_TRANSPORT_BUILD_DLL)
#    define NDDS_Transport_Export ACE_Proper_Export_Flag
#    define NDDS_TRANSPORT_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define NDDS_TRANSPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* NDDS_TRANSPORT_BUILD_DLL */
#    define NDDS_Transport_Export ACE_Proper_Import_Flag
#    define NDDS_TRANSPORT_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define NDDS_TRANSPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* NDDS_TRANSPORT_BUILD_DLL */
#else /* NDDS_TRANSPORT_HAS_DLL == 1 */
#  define NDDS_Transport_Export
#  define NDDS_TRANSPORT_SINGLETON_DECLARATION(T)
#  define NDDS_TRANSPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* NDDS_TRANSPORT_HAS_DLL == 1 */

// Set NDDS_TRANSPORT_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (NDDS_TRANSPORT_NTRACE)
#  if (ACE_NTRACE == 1)
#    define NDDS_TRANSPORT_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define NDDS_TRANSPORT_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !NDDS_TRANSPORT_NTRACE */

#if (NDDS_TRANSPORT_NTRACE == 1)
#  define NDDS_TRANSPORT_TRACE(X)
#else /* (NDDS_TRANSPORT_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define NDDS_TRANSPORT_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (NDDS_TRANSPORT_NTRACE == 1) */

#endif /* NDDS_TRANSPORT_EXPORT_H */

// End of auto generated file.
