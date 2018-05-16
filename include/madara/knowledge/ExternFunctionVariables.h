
#ifndef _MADARA_EXTERNAL_FUNCTION_VARIABLES_H_
#define _MADARA_EXTERNAL_FUNCTION_VARIABLES_H_

#include <string>
#include "madara/MADARA_export.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/knowledge/CompiledExpression.h"
#include "madara/knowledge/VariableReference.h"
#include "madara/knowledge/FunctionArguments.h"
#include "madara/knowledge/Functions.h"

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

namespace madara
{
  namespace expression
  {
    class CompositeFunctionNode;
  }

  namespace knowledge
  {
    class ThreadSafeContext;
    class KnowledgeRecordFilters;
    
    typedef madara::knowledge::KnowledgeRecord VALUE_TYPE;
   
    /**
     * @class Variables
     * @brief Provides an interface for external functions into the MADARA
     *        KaRL variable settings
     */
    class MADARA_Export Variables
    {
    public:
      friend class madara::expression::CompositeFunctionNode;
      friend class madara::knowledge::KnowledgeRecordFilters;

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
       * Returns the ThreadSafeContext associated with this Variables facade.
       * This is necessary for creating custom transports.
       *
       * @return             the context used by the knowledge base
       **/
      ThreadSafeContext * get_context (void);

      /**
       * Retrieves the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   settings  settings when referring to variables
       * @return   the value of the keyed variable
       **/
      knowledge::KnowledgeRecord get (const std::string & key,
        const KnowledgeReferenceSettings & settings =
        knowledge::KnowledgeReferenceSettings (false));
      
      /**
       * Retrieves the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  settings when referring to variables
       * @return   the value of the keyed variable
       **/
      knowledge::KnowledgeRecord get (const VariableReference & variable,
        const KnowledgeReferenceSettings & settings =
        knowledge::KnowledgeReferenceSettings (false));
      
      /**
       * Retrieves the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   settings  settings when referring to variables
       * @return   a variable reference to the key (efficient accessor)
       **/
      VariableReference get_ref (const std::string & key,
        const KnowledgeReferenceSettings & settings =
        knowledge::KnowledgeReferenceSettings (false));
      
      /**
       * Sets the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const std::string & key,
        madara::knowledge::KnowledgeRecord::Integer value,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const VariableReference & variable,
        madara::knowledge::KnowledgeRecord::Integer value,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
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
        const madara::knowledge::KnowledgeRecord::Integer * value,
        uint32_t size,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     an array of Integers
       * @param   size      number of elements in the array
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const madara::knowledge::KnowledgeRecord::Integer * value,
        uint32_t size,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   key       unique identifier of the variable
       * @param   value     a STL vector of Integers
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const std::vector <KnowledgeRecord::Integer> & value,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     a STL vector of Integers
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const std::vector <KnowledgeRecord::Integer> & value,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const std::string & key, double value,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const VariableReference & variable, double value,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
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
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to a double array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     an array of doubles
       * @param   size      number of elements in the array
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const double * value,
        uint32_t size,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
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
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to a double array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     a STL vector of doubles
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const std::vector <double> & value,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const std::string & key, const std::string & value,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const VariableReference & variable, const std::string & value,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const std::string & key,
        const madara::knowledge::KnowledgeRecord & value,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Sets the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  Settings for updating knowledge
       * @return   the new value of the keyed variable
       **/
      int set (const VariableReference & variable,
        const madara::knowledge::KnowledgeRecord & value,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Atomically increments the value of the variable
       * @param   key       unique identifier of the variable
       * @param   settings  Settings for updating knowledge
       * @return                 new value of variable
       **/
      knowledge::KnowledgeRecord inc (const std::string & key,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Atomically increments the value of the variable
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  Settings for updating knowledge
       * @return            new value of variable
       **/
      knowledge::KnowledgeRecord inc (const VariableReference & variable,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));

      /**
       * Decrements the value of the variable
       * @param   key       unique identifier of the variable
       * @param   settings  Settings for updating knowledge
       * @return            new value of variable
       **/
      knowledge::KnowledgeRecord dec (const std::string & key,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Decrements the value of the variable
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  Settings for updating knowledge
       * @return            new value of variable
       **/
      knowledge::KnowledgeRecord dec (const VariableReference & variable,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings (
            false, true, false, false));
      
      /**
       * Saves all keys and values into a string, using the underlying
       * knowledge::KnowledgeRecord::to_string function. This is an optimized
       * version that allows the specification of a target string to
       * avoid string copying (which can be expensive with longer
       * strings across multiple function boundaries). This function differs
       * from knowledge::KnowledgeRecord to_string in that it is intended to save the
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
       * @param   level          log level. @see LogMacros.h
       **/
      void print (unsigned int level = 0) const;

      /**
       * Print a statement, similar to printf (variable expansions
       * allowed) e.g., input = "MyVar{.id} = {MyVar{.id}}\n";
       * @param   statement      templated statement to print from
       * @param   level          log level. @see LogMacros.h
       **/
      void print (const std::string & statement, unsigned int level = 0) const;
      
      /**
       * Retrieves a value at a specified index within a knowledge array
       * @param   key        variable name of the array
       * @param   index      index within the array
       * @param   settings   settings for referring to knowledge variables
       * @return             value at knowledge location
       **/
      madara::knowledge::KnowledgeRecord retrieve_index (
             const std::string & key,
             size_t index,
             const KnowledgeReferenceSettings & settings =
               knowledge::KnowledgeReferenceSettings (false));

      /**
       * Retrieves a value at a specified index within a knowledge array
       * @param   variable   reference to a variable (@see get_ref)
       * @param   index      index within the array
       * @param   settings   settings for referring to knowledge variables
       * @return             value at knowledge location
       **/
      madara::knowledge::KnowledgeRecord retrieve_index (
             const VariableReference & variable,
             size_t index,
             const KnowledgeReferenceSettings & settings =
               knowledge::KnowledgeReferenceSettings (false));

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
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings ());

      /**
       * Checks if a knowledge location exists in the context
       *
       * @param key             knowledge variable location
       * @param settings        settings for referring to knowledge variables
       * @return                true if location has been set
       **/
      bool exists (const std::string & key,
        const KnowledgeReferenceSettings & settings =
          KnowledgeReferenceSettings (false)) const;
      
      /**
       * Checks if a knowledge variable exists in the context
       *
       * @param variable        knowledge variable reference
       * @param settings        settings for referring to knowledge variables
       * @return                true if location has been set
       **/
      bool exists (const VariableReference & variable,
        const KnowledgeReferenceSettings & settings =
          KnowledgeReferenceSettings (false)) const;
      
#ifndef _MADARA_NO_KARL_
      
      /**
       * Compiles a KaRL expression into an expression tree. Always do this
       * before calling evaluate because it puts the expression into an
       * optimized format. Best practice is to save the CompiledExpression
       * in a global variable or in some kind of persistent store. Pair with
       * expand_statement if you know that variable expansion is used but
       * the variable values that are expanded never change (e.g. an id that
       * is set through the command line and thus stays the same after it is
       * initially set).
       *
       * @param expression         expression to compile
       * @return                   compiled, optimized expression tree
       * @throw expression::KarlException  failure during compile/evaluate
       **/
      CompiledExpression
        compile (const std::string & expression);
      
      /**
       * Evaluates an expression (USE ONLY FOR PROTOTYPING; DO NOT USE IN
       * PRODUCTION SYSTEMS). Consider compiling the expression first with
       * a one-time compile call during an initialization phase for your
       * program, and then using the evaluate CompiledExpression call
       * in any function that must be called frequently or periodically.
       * The difference in overhead between this function and the compiled
       * version is orders of magnitude (generally nanoseconds versus
       * microseconds every call).
       *
       * @param expression      KaRL expression to evaluate
       * @param settings        Settings for evaluating and printing
       * @return                value of expression
       * @throw expression::KarlException  failure during compile/evaluate
       **/
      madara::knowledge::KnowledgeRecord evaluate (
        const std::string & expression,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings ());
      
      /**
       * Evaluates an expression. Recommended best practices are to compile the
       * expression into a global variable or persistent store outside of the
       * function call and use a reference to this CompiledExpression from
       * within the external function that you create.
       *
       * @param expression      KaRL expression to evaluate (result of compile)
       * @param settings        Settings for updating knowledge
       * @return                value of expression
       * @throw expression::KarlException  failure during compile/evaluate
       **/
      madara::knowledge::KnowledgeRecord evaluate (
        CompiledExpression & expression,
        const KnowledgeUpdateSettings & settings =
          knowledge::KnowledgeUpdateSettings ());
      
      /**
       * Defines a function
       * @param  name       name of the function
       * @param  func       external function to call with this name
       **/
      void define_function (const std::string & name,
        knowledge::KnowledgeRecord (*func) (FunctionArguments &, Variables &));
      
      /**
       * Defines a named function that can distinguish the name it was called
       * with in MADARA
       * @param  name       name of the function
       * @param  func       external function to call with this name
       **/
      void define_function (const std::string & name,
        knowledge::KnowledgeRecord (*func) (const char *, FunctionArguments &, Variables &));
      
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
        const CompiledExpression & expression);
      
#endif // _MADARA_NO_KARL_

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
                              std::vector <KnowledgeRecord> & target);

      /**
      * Creates an iteration of VariableReferences to all keys matching
      * the prefix and suffix
      * @param  prefix   string that the key must start with
      * @param  suffix   string that the key must end with
      * @param  matches  all VariableReferences matching the prefix and suffix
      **/
      void get_matches (
        const std::string & prefix,
        const std::string & suffix,
        VariableReferences & matches);

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
                       std::map <std::string, knowledge::KnowledgeRecord> & target);
      
      /**
      * Saves the context to a file as karl assignments, rather than binary
      * @param   filename    name of the file to save to
      * @return  total bytes written
      **/
      int64_t save_as_karl (const std::string & filename) const;

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
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings (true, true, true, false));
      
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
      ThreadSafeContext * context_;
    };
  }
}

#endif
