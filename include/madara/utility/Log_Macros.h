/**
 * @file Log_Macros.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * Macros used for logging in MADARA, based on the DAnCE Log Macros
 * maintained by Will Otte <wotte@dre.vanderbilt.edu>.
 * 
 * Note that though we include MADARA_Logger_export for future expansion
 * possibilities, we are simply including the Log_Macros.h and Log_Macros.cpp
 * files into each project that might need these. Otherwise, KATS users would
 * have to explicitly include Madara.dll or Madara.lib and Kats.dll or Kats.lib instead of just 
 * Kats.dll or Kats.lib.
 */


#ifndef _MADARA_LOG_MACROS_H_
#define _MADARA_LOG_MACROS_H_

#include "MADARA_Logger_export.h"
#include "ace/Log_Msg.h"

#if !defined (DLINFO)
# define DLINFO ACE_TEXT("(%P|%t) [%M] - %T - ")
#endif

extern MADARA_Logger_Export unsigned int MADARA_debug_level;

// Defines for logging levels

/// Used for errors that cause the fatal shutdown of any portion
/// of the infrastructure.
#define MADARA_LOG_EMERGENCY            0
/// Used for MADARA errors at the point the error
/// exits the process in question, or when a decision is made to
/// cause the MADARA suite to fail.
#define MADARA_LOG_TERMINAL_ERROR       1
/// Used for non-fatal deployment errors that do not cause deployment
/// failure.
#define MADARA_LOG_NONFATAL_ERROR       2
/// Used to log detailed error information at the point of failure
#define MADARA_LOG_ERROR                3
/// used to indicate that a questionable situation that doesn't cause
/// deployment failure, but can cause undefined conditions.
#define MADARA_LOG_WARNING              4
/// Used to indicate that a ``significant'' MADARA event has completed.
#define MADARA_LOG_MAJOR_EVENT          5
/// Used to inficate a ``minor'' MADARA event has completed.
#define MADARA_LOG_MINOR_EVENT          6
/// Used to trace significant actions within major/minor events.  This
/// will usually include starts for major/minor events.
#define MADARA_LOG_EVENT_TRACE          7
/// Used to display important configuration information that impacts
/// major MADARA events.
#define MADARA_LOG_MAJOR_DEBUG_INFO     8
/// The following two statements are used to closely trace the
/// execution of MADARA code.  If in doubt, a message should go into
/// these categories.  The difference between a TRACE and a
/// DETAILED_TRACE will be largely a judgement call.  These messages
/// should be emitted using MADARA_LOG_TRACE (X, Y).
#define MADARA_LOG_TRACE                9
#define MADARA_LOG_DETAILED_TRACE     10

// By default tracing is turned off.
#if !defined (MADARA_NTRACE)
#  define MADARA_NTRACE 1
#endif /* MADARA_NTRACE */

#if (MADARA_NTRACE == 1)
#  if !defined (ACE_NTRACE)
#    define MADARA_TRACE(X) do {} while (0)
#    define MADARA_ENABLE_TRACE() do {} while (0)
#    define MADARA_DISABLE_TRACE() do {} while (0)
#  else
#    if (ACE_NTRACE == 0)
#      error MADARA_TRACE cannot be disabled if ACE_TRACE is enabled
#    else
#      define MADARA_TRACE(X) do {} while (0)
#      define MADARA_ENABLE_TRACE() do {} while (0)
#      define MADARA_DISABLE_TRACE() do {} while (0)
#    endif
#  endif
#else
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define MADARA_TRACE(X) ACE_TRACE_IMPL (X)
#  define MADARA_ENABLE_TRACE() ACE_Trace::start_tracing ()
#  define MADARA_DISABLE_TRACE() ACE_Trace::stop_tracing ()
#  undef DLINFO // Make log messages indent with tracing.
#  define DLINFO ACE_TEXT("%I(%P|%t) [%M] - %T - ")
#  include "ace/Trace.h"
#endif /* MADARA_NTRACE */

#if defined (MADARA_NLOGGING)
# define MADARA_ERROR(L, X) do {} while (0)
# define MADARA_DEBUG(L, X) do {} while (0)
#define MADARA_ERROR_RETURN(L, X, Y) return (Y)
#define MADARA_ERROR_BREAK(L, X) { break; }
#else
# if !defined (MADARA_ERROR)
#  define MADARA_ERROR(L, X) \
  do { \
    if (MADARA_debug_level >= L) \
      { \
        int const __ace_error = ACE_Log_Msg::last_error_adapter (); \
        ACE_Log_Msg *ace___ = ACE_Log_Msg::instance ();               \
        ace___->conditional_set (__FILE__, __LINE__, -1, __ace_error); \
        ace___->log X; \
      } \
  } while (0)
#  endif
# if !defined (MADARA_DEBUG)
#  define MADARA_DEBUG(L, X) \
  do { \
    if (MADARA_debug_level >= L) \
      { \
        int const __ace_error = ACE_Log_Msg::last_error_adapter (); \
        ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
        ace___->conditional_set (__FILE__, __LINE__, 0, __ace_error); \
        ace___->log X; \
      } \
  } while (0)
# endif
# if !defined (MADARA_LOG_TRACE)
#  define MADARA_LOG_TRACE(L, X) \
  do { \
    if (MADARA_debug_level >= L) \
      { \
        int const __ace_error = ACE_Log_Msg::last_error_adapter (); \
        ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
        ace___->conditional_set (__FILE__, __LINE__, 0, __ace_error); \
        ace___->log X; \
      } \
  } while (0)
# endif
# if !defined (MADARA_ERROR_RETURN)
#  define MADARA_ERROR_RETURN(L, X, Y) \
  do { \
    if (MADARA_debug_level >= L) \
      { \
        int const __ace_error = ACE_Log_Msg::last_error_adapter (); \
        ACE_Log_Msg *ace___ = ACE_Log_Msg::instance (); \
        ace___->conditional_set (__FILE__, __LINE__, Y, __ace_error); \
        ace___->log X; \
        return Y; \
      } \
  } while (0)
# endif
# if !defined (MADARA_ERROR_BREAK)
#  define MADARA_ERROR_BREAK(L, X) { MADARA_ERROR (L, X); break; }
# endif
#endif

#endif // _MADARA_LOG_MACROS_H_
