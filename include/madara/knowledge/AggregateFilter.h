
#ifndef _MADARA_AGGREGATE_FILTERS_H_
#define _MADARA_AGGREGATE_FILTERS_H_

#include <list>
#include <string>
#include "madara/MadaraExport.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/ExternFunctionVariables.h"
#include "madara/transport/TransportContext.h"
#include "madara/filters/AggregateFilter.h"

#ifdef _MADARA_JAVA_
  #include <jni.h>
  #include "madara_jni.h"
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
  #include <boost/python.hpp>
#endif

/**
 * @file AggregateFilter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains declarations for declaring external functions that
 * are accessible by the transport layer to do aggregate update filtering
 **/

namespace madara
{
  namespace knowledge
  {
    /**
     * @class AggregateFilter
     * @brief This class stores a function definition
     */
    class MADARA_EXPORT AggregateFilter
    {
    public:
      /**
       * Types of functions supported
       **/
      enum Types
      {
        UNINITIALIZED = 0,
        EXTERN_UNNAMED = 1,
        EXTERN_NAMED = 2,
        PYTHON_CALLABLE = 3,
        JAVA_CALLABLE = 4,
        FUNCTOR = 5
      };

      /**
       * Default constructor
       **/
      AggregateFilter ()
        : unnamed_filter (0), functor (0), type (UNINITIALIZED)
      {
      }

      /**
       * Constructor for function pointer
       **/
      AggregateFilter (void (*extern_func) (
        KnowledgeMap &, const transport::TransportContext &,
        Variables &))
        : unnamed_filter (extern_func), functor (0), type (EXTERN_UNNAMED)
      {
      }
      
      /**
       * Constructor for functor
       **/
      AggregateFilter (filters::AggregateFilter * filter)
        : unnamed_filter (0), functor (filter), type (FUNCTOR)
      {
      }
      
#ifdef _MADARA_JAVA_
      jobject java_object;
      
      /**
       * Constructor for java
       **/
      AggregateFilter (jobject& object)
      : type (JAVA_CALLABLE)
      {
        //We have to create a globla ref to the object or we cant call it
        JNIEnv* env = madara_jni_get_env();
        java_object = (jobject) env->NewGlobalRef(object);
      }
      
      bool is_java_callable (void) const
      {
        return type == JAVA_CALLABLE;
      }
      
      //KnowledgeRecord call_java(KnowledgeMap & recordsMap, const transport::TransportContext & context, Variables & vars) const
      //{
      //  return knowledge::KnowledgeRecord ();
      //}
#endif
      
#ifdef _MADARA_PYTHON_CALLBACKS_
      /**
       * Constructor for function pointer
       **/
      AggregateFilter (boost::python::object & func)
        : python_function (func), type (PYTHON_CALLABLE)
      {
        // Check to make sure its a callable object
        if (0 == PyObject_HasAttrString (func.ptr (), "__call__"))
        {
          // If not, lets throw an exception to warn the user
          PyErr_SetString (
            PyExc_TypeError,
            "Handler must be a callable object");

          boost::python::throw_error_already_set(); 
        } 
      }
      
      bool is_python_callable (void) const
      {
        return type == PYTHON_CALLABLE && !python_function.is_none ();
      }

      boost::python::object python_function;
#endif
      
      inline bool is_extern_unnamed (void) const
      {
        return type == EXTERN_UNNAMED && unnamed_filter;
      }
      
      inline bool is_extern_named (void) const
      {
        return type == EXTERN_NAMED && named_filter;
      }
  
      inline bool is_functor (void) const
      {
        return type == FUNCTOR;
      }

      bool is_uninitialized (void) const
      {
        return type == UNINITIALIZED;
      }

      /// mapped function call for aggregate filter
      void (*unnamed_filter) (
        KnowledgeMap &, const transport::TransportContext &,
        Variables &);
      
      /// mapped function call for aggregate filter
      void (*named_filter) (
        const char *, KnowledgeMap &, 
        const transport::TransportContext &,
        Variables &);

      /// mapped functor for aggregate filtering
      filters::AggregateFilter * functor;

      // type of function definition
      int type;
    };

    /**
     * Typedef for a list of aggregate filters
     **/
    typedef   std::list <AggregateFilter>    AggregateFilters;
  }
}




#endif // _MADARA_AGGREGATE_FILTERS_H_