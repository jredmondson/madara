
#ifndef _MADARA_EXTERNAL_FUNCTION_VARIABLES_H_
#define _MADARA_EXTERNAL_FUNCTION_VARIABLES_H_

#include <string>
#include "madara/MADARA_export.h"
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"
#include "madara/expression_tree/Expression_Tree.h"
#include "madara/knowledge_engine/Compiled_Expression.h"
#include "madara/knowledge_engine/Variable_Reference.h"
#include "madara/knowledge_engine/Function_Arguments.h"
#include "madara/knowledge_engine/Functions.h"

#ifdef _MADARA_PYTHON_CALLBACKS_
#include "boost/python/object.hpp"
#endif

#ifdef _MADARA_JAVA_
  #include <jni.h>
  #include "madara_jni.h"
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
  namespace Expression_Tree
  {
    class Composite_Function_Node;
  }

  namespace Knowledge_Engine
  {
    class Thread_Safe_Context;
    class Knowledge_Record_Filters;
    
    typedef Madara::Knowledge_Record VALUE_TYPE;
   
    /**
     * @class Variables
     * @brief Provides an interface for external functions into the MADARA
     *        KaRL variable settings
     */
    class MADARA_Export Variables
    {
    public:
      friend class Madara::Expression_Tree::Composite_Function_Node;
      friend class Madara::Knowledge_Engine::Knowledge_Record_Filters;

      /**
       * Constructor
       **/
      Variables ()
        : context_ (0)
      {
      }
      
      /**
       * Constructor
       **/
      Variables (const Variables & rhs)
        : context_ (rhs.context_)
      {
      }
      
      /**
       * Assignment operator
       **/
      void operator= (Variables & rhs);
      
      /**
       * Returns the Thread_Safe_Context associated with this Variables facade.
       * This is necessary for creating custom transports.
       *
       * @return             the context used by the knowledge base
       **/
      Thread_Safe_Context * get_context (void);

      /**
       * Retrieves the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   settings  settings when referring to variables
       * @return   the value of the keyed variable
       **/
      Knowledge_Record get (const std::string & key,
        const Knowledge_Reference_Settings & settings =
        Knowledge_Engine::Knowledge_Reference_Settings (false));
      
      /**
       * Retrieves the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  settings when referring to variables
       * @return   the value of the keyed variable
       **/
      Knowledge_Record get (const Variable_Reference & variable,
        const Knowledge_Reference_Settings & settings =
        Knowledge_Engine::Knowledge_Reference_Settings (false));
      
      /**
       * Retrieves the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   settings  settings when referring to variables
       * @return   a variable reference to the key (efficient accessor)
       **/
      Variable_Reference get_ref (const std::string & key,
        const Knowledge_Reference_Settings & settings =
        Knowledge_Engine::Knowledge_Reference_Settings (false));
      
      /**
       * Sets the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const std::string & key,
        Madara::Knowledge_Record::Integer value,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const Variable_Reference & variable,
        Madara::Knowledge_Record::Integer value,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   key       unique identifier of the variable
       * @param   value     an array of Integers
       * @param   size      number of elements in the array
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const Madara::Knowledge_Record::Integer * value,
        uint32_t size,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     an array of Integers
       * @param   size      number of elements in the array
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const Variable_Reference & variable,
        const Madara::Knowledge_Record::Integer * value,
        uint32_t size,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   key       unique identifier of the variable
       * @param   value     a STL vector of Integers
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const std::vector <Knowledge_Record::Integer> & value,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     a STL vector of Integers
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const Variable_Reference & variable,
        const std::vector <Knowledge_Record::Integer> & value,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const std::string & key, double value,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const Variable_Reference & variable, double value,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to a double array.
       * @param   key       unique identifier of the variable
       * @param   value     an array of doubles
       * @param   size      number of elements in the array
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const double * value,
        uint32_t size,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to a double array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     an array of doubles
       * @param   size      number of elements in the array
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const Variable_Reference & variable,
        const double * value,
        uint32_t size,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to a double array.
       * @param   key       unique identifier of the variable
       * @param   value     a STL vector of doubles
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const std::vector <double> & value,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to a double array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     a STL vector of doubles
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const Variable_Reference & variable,
        const std::vector <double> & value,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const std::string & key, const std::string & value,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const Variable_Reference & variable, const std::string & value,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const std::string & key,
        const Madara::Knowledge_Record & value,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const Variable_Reference & variable,
        const Madara::Knowledge_Record & value,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Atomically increments the value of the variable
       * @param   key       unique identifier of the variable
       * @param   settings  Settings for updating knowledge
       * @return                 new value of variable
       **/
      Knowledge_Record inc (const std::string & key,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Atomically increments the value of the variable
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  Settings for updating knowledge
       * @return            new value of variable
       **/
      Knowledge_Record inc (const Variable_Reference & variable,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));

      /**
       * Decrements the value of the variable
       * @param   key       unique identifier of the variable
       * @param   settings  Settings for updating knowledge
       * @return            new value of variable
       **/
      Knowledge_Record dec (const std::string & key,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Decrements the value of the variable
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  Settings for updating knowledge
       * @return            new value of variable
       **/
      Knowledge_Record dec (const Variable_Reference & variable,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings (
            false, true, false, false));
      
      /**
       * Saves all keys and values into a string, using the underlying
       * Knowledge_Record::to_string function. This is an optimized
       * version that allows the specification of a target string to
       * avoid string copying (which can be expensive with longer
       * strings across multiple function boundaries). This function differs
       * from Knowledge_Record to_string in that it is intended to save the
       * database in a format that can be easily parseable. Consequently,
       * strings are delineated in this function by being included in single
       * quotes. Arrays are delineated with array indices [].
       * 
       * This is not appropriate
       * for saving the context if it has binary data inside of it, as
       * only the size of the data entry would be saved.
       * @param target  where to store the resulting string operation.
       * @param array_delimiter  for any arrays, the characters in between
       *                         data elements.
       * @param record_delimiter the characters to place between individual
       *                         key/value pairs
       * @param key_val_delimiter the characters to place between keys and
       *                          values.
       **/
      void to_string (std::string & target,
        const std::string & array_delimiter = ",",
        const std::string & record_delimiter = ";\n",
        const std::string & key_val_delimiter = "=") const;

      /**
       * Prints all variables and values in the context
       * @param   level          log level. @see Log_Macros.h
       **/
      void print (unsigned int level = 0) const;

      /**
       * Print a statement, similar to printf (variable expansions
       * allowed) e.g., input = "MyVar{.id} = {MyVar{.id}}\n";
       * @param   statement      templated statement to print from
       * @param   level          log level. @see Log_Macros.h
       **/
      void print (const std::string & statement, unsigned int level = 0) const;
      
      /**
       * Retrieves a value at a specified index within a knowledge array
       * @param   key        variable name of the array
       * @param   index      index within the array
       * @param   settings   settings for referring to knowledge variables
       * @return             value at knowledge location
       **/
      Madara::Knowledge_Record retrieve_index (
             const std::string & key,
             size_t index,
             const Knowledge_Reference_Settings & settings =
               Knowledge_Engine::Knowledge_Reference_Settings (false));

      /**
       * Retrieves a value at a specified index within a knowledge array
       * @param   variable   reference to a variable (@see get_ref)
       * @param   index      index within the array
       * @param   settings   settings for referring to knowledge variables
       * @return             value at knowledge location
       **/
      Madara::Knowledge_Record retrieve_index (
             const Variable_Reference & variable,
             size_t index,
             const Knowledge_Reference_Settings & settings =
               Knowledge_Engine::Knowledge_Reference_Settings (false));

      /**
       * Expands a string with variable expansion. For instance, if
       * .id == 5, and a statement of "MyVar{.id} = {.id} * 30" then
       * then expanded statement would be "MyVar5 = 5 * 30".
       * @param  statement  statement to expand. Useful for printing.
       * @return            variable expanded statement
       **/
      std::string expand_statement (const std::string & statement) const;
      
      /**
       * Applies modified to all global variables
       * @param settings        Settings for evaluating and printing
       **/
      void apply_modified (
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings ());

      /**
       * Compiles a KaRL expression into an expression tree. Always do this
       * before calling evaluate because it puts the expression into an
       * optimized format. Best practice is to save the Compiled_Expression
       * in a global variable or in some kind of persistent store. Pair with
       * expand_statement if you know that variable expansion is used but
       * the variable values that are expanded never change (e.g. an id that
       * is set through the command line and thus stays the same after it is
       * initially set).
       *
       * @param expression         expression to compile
       * @return                   compiled, optimized expression tree
       **/
      Compiled_Expression
        compile (const std::string & expression);
      
      /**
       * Evaluates an expression (USE ONLY FOR PROTOTYPING; DO NOT USE IN
       * PRODUCTION SYSTEMS). Consider compiling the expression first with
       * a one-time compile call during an initialization phase for your
       * program, and then using the evaluate Compiled_Expression call
       * in any function that must be called frequently or periodically.
       * The difference in overhead between this function and the compiled
       * version is orders of magnitude (generally nanoseconds versus
       * microseconds every call).
       *
       * @param expression      KaRL expression to evaluate
       * @param settings        Settings for evaluating and printing
       * @return                value of expression
       **/
      Madara::Knowledge_Record evaluate (
        const std::string & expression,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings ());
      
      /**
       * Checks if a knowledge location exists in the context
       *
       * @param key             knowledge variable location
       * @param settings        settings for referring to knowledge variables
       * @return                true if location has been set
       **/
      bool exists (const std::string & key,
        const Knowledge_Reference_Settings & settings =
          Knowledge_Reference_Settings (false)) const;
      
      /**
       * Checks if a knowledge variable exists in the context
       *
       * @param variable        knowledge variable reference
       * @param settings        settings for referring to knowledge variables
       * @return                true if location has been set
       **/
      bool exists (const Variable_Reference & variable,
        const Knowledge_Reference_Settings & settings =
          Knowledge_Reference_Settings (false)) const;

      /**
       * Evaluates an expression. Recommended best practices are to compile the
       * expression into a global variable or persistent store outside of the
       * function call and use a reference to this Compiled_Expression from
       * within the external function that you create.
       *
       * @param expression      KaRL expression to wait on (result of compile)
       * @param settings        Settings for updating knowledge
       * @return                value of expression
       **/
      Madara::Knowledge_Record evaluate (
        Compiled_Expression & expression,
        const Knowledge_Update_Settings & settings =
          Knowledge_Engine::Knowledge_Update_Settings ());
      
      /**
       * Defines a function
       * @param  name       name of the function
       * @param  func       external function to call with this name
       **/
      void define_function (const std::string & name,
        Knowledge_Record (*func) (Function_Arguments &, Variables &));
      
      /**
       * Defines a named function that can distinguish the name it was called
       * with in MADARA
       * @param  name       name of the function
       * @param  func       external function to call with this name
       **/
      void define_function (const std::string & name,
        Knowledge_Record (*func) (const char *, Function_Arguments &, Variables &));
      
#ifdef _MADARA_JAVA_
      /**
       * Defines a named java function
       * @param  name       name of the function
       * @param  callable   external java object to call with this name
       **/
      void define_function (const std::string & name, jobject callable);
#endif
      
#ifdef _MADARA_PYTHON_CALLBACKS_
      /**
       * Defines a named python function
       * @param  name       name of the function
       * @param  callable   external python function to call with this name
       **/
      void define_function (const std::string & name, boost::python::object callable);
#endif

      /**
       * Defines a MADARA KaRL function
       * @param  name       name of the function
       * @param  expression KaRL function body       
       **/
      void define_function (const std::string & name,
        const std::string & expression);
      
      /**
       * Defines a MADARA KaRL function
       * @param  name       name of the function
       * @param  expression KaRL function body       
       **/
      void define_function (const std::string & name,
        const Compiled_Expression & expression);
      
      /**
       * Fills a vector with Knowledge Records that begin with a common subject
       * and have a finite range of integer values.
       * @param   subject     The common subject of the variable names. For
       *                      instance, if we are looking for a range of vars
       *                      like "var0", "var1", "var2", then the common
       *                      subject would be "var".
       * @param   start       An inclusive start index
       * @param   end         An inclusive end index
       * @param   target      The vector that will be filled with
       *                      Knowledge Record instances within the subject
       *                      range.
       * @return              entries in the resulting vector
       **/
      size_t to_vector (const std::string & subject,
                              unsigned int start,
                              unsigned int end,
                              std::vector <Knowledge_Record> & target);
      
      /**
       * Fills a variable map with Knowledge Records that match an expression.
       * At the moment, this expression must be of the form "subject*"
       * @param   subject     An expression that matches the variable names
       *                      that are of interest. Wildcards may only be
       *                      at the end.
       * @param   target      The map that will be filled with variable names
       *                      and the Knowledge Records they correspond to
       * @return              entries in the resulting map
       **/
      size_t to_map    (const std::string & subject,
                       std::map <std::string, Knowledge_Record> & target);
      
      /**
       * Saves the context to a file
       * @param   filename    name of the file to open
       * @return  total bytes written
       **/
      int64_t save_context (const std::string & filename);
      
      /**
       * Saves a checkpoint of a list of changes to a file
       * @param   filename    name of the file to open
       * @param   reset_modifieds  if true, resets the modified list to empty.
       * @return  total bytes written
       **/

      int64_t save_checkpoint (const std::string & filename,
        bool reset_modifieds = true);
      
      /**
       * Loads the context from a file
       * @param   filename    name of the file to open
       * @param   settings    settings to use when applying updates to context
       * @return  total bytes read
       **/
      int64_t load_context (const std::string & filename,
        const Knowledge_Update_Settings & settings = 
              Knowledge_Update_Settings (true, true, true, false));
      
      /**
       * Write a file from the context to a specified location
       * @param filename           file to write to
       * @param knowledge_key       key to read the file from
       * @return   -1 if unsuccessful, 0 otherwise
       */
      ssize_t write_file (const std::string & knowledge_key, 
                          const std::string & filename);

    private:
      /**
       * Variables context that is directly used by the KaRL engine
       **/
      Thread_Safe_Context * context_;
    };
  }
}




#endif