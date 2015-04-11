
#ifndef _MADARA_EXTERNAL_FUNCTIONS_H_
#define _MADARA_EXTERNAL_FUNCTIONS_H_

#include <string>
#include "madara/MADARA_export.h"
#include "madara/knowledge_engine/Function_Arguments.h"
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"
#include "madara/expression_tree/Expression_Tree.h"
#include "madara/filters/Record_Filter.h"

#ifdef _MADARA_JAVA_
#include <jni.h>
#include "madara_jni.h"
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
  #include <boost/python.hpp>
#endif

/**
 * @file Functions.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains declarations for declaring external functions that
 * are accessible by the MADARA KARL interpreter
 **/

namespace Madara
{
  namespace Knowledge_Engine
  {
    class Variables;
    class Thread_Safe_Context;
    
    typedef Madara::Knowledge_Record VALUE_TYPE;
     
    /**
     * @class Function
     * @brief This class stores a function definition
     */
    class MADARA_Export Function
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
        KARL_EXPRESSION = 3,
        PYTHON_CALLABLE = 4,
        JAVA_CALLABLE = 5,
        FUNCTOR = 6
      };

      /**
       * Default constructor
       **/
      Function ()
        : extern_named (0), extern_unnamed (0),

#ifndef _MADARA_NO_KARL_
          function_contents (),
#endif // _MADARA_NO_KARL_
        
          functor (0), type (UNINITIALIZED)
      {
      }

      /**
       * Constructor for function pointer
       **/
      Function (Knowledge_Record (*func) (Function_Arguments &, Variables &))
        : extern_named (0), extern_unnamed (func),

#ifndef _MADARA_NO_KARL_
          function_contents (),
#endif // _MADARA_NO_KARL_
        
          functor (0), type (EXTERN_UNNAMED)
      {
      }
      
      /**
       * Constructor for function pointer
       **/
      Function (Knowledge_Record (*func) (const char *, 
        Function_Arguments &, Variables &))
        : extern_named (func), extern_unnamed (0),

#ifndef _MADARA_NO_KARL_
          function_contents (),
#endif // _MADARA_NO_KARL_
        
          functor (0), type (EXTERN_NAMED)
      {
      }
      
#ifndef _MADARA_NO_KARL_
      /**
       * Constructor for KaRL expression
       **/
      Function (const Madara::Expression_Tree::Expression_Tree & func)
        : extern_named (0), extern_unnamed (0), function_contents (func),
          functor (0), type (KARL_EXPRESSION)
      {
      }
      
#endif // _MADARA_NO_KARL_

      /**
       * Constructor for KaRL expression
       **/
      Function (Filters::Record_Filter * filter)
        : extern_named (0), extern_unnamed (0),

#ifndef _MADARA_NO_KARL_
          function_contents (),
#endif // _MADARA_NO_KARL_
        
          functor (filter), type (FUNCTOR)
      {
      }
       
      inline bool is_extern_unnamed (void) const
      {
        return type == EXTERN_UNNAMED && extern_unnamed;
      }
      
      inline bool is_extern_named (void) const
      {
        return type == EXTERN_NAMED && extern_named;
      }
  
      inline bool is_karl_expression (void) const
      {
        return type == KARL_EXPRESSION;
      }
      
      inline bool is_functor (void) const
      {
        return functor != 0;
      }

      inline bool is_uninitialized (void) const
      {
        return type == UNINITIALIZED;
      }

      // internal function pointer
      Knowledge_Record (*extern_named) (
        const char *, Function_Arguments &, Variables &);

      // internal function pointer
      Knowledge_Record (*extern_unnamed) (Function_Arguments &, Variables &);
       
#ifndef _MADARA_NO_KARL_
      // expression tree
      Madara::Expression_Tree::Expression_Tree function_contents;
#endif // _MADARA_NO_KARL_

      Filters::Record_Filter * functor;

      // type of function definition
      int type;

  
#ifdef _MADARA_JAVA_
      jobject java_object;
      
      /**
       * Constructor for java
       **/
      Function (jobject& object)
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
      
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
      /**
       * Constructor for function pointer
       **/
      Function (boost::python::object & func)
        : python_function (func), type (PYTHON_CALLABLE)
      {
        bool invalid_callable = false;
   
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
   

    };
    
  }
}

#endif
