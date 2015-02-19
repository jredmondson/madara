#ifndef _MADARA_KNOWLEDGE_BASE_H
#define _MADARA_KNOWLEDGE_BASE_H

/**
 * @file Knowledge_Base.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Knowledge_Base class
 */

#include <ostream>
#include <string>
#include <map>

#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"
#include "madara/knowledge_engine/Functions.h"
#include "madara/filters/Arguments.h"
#include "madara/MADARA_export.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/transport/Transport.h"
#include "madara/expression_tree/Interpreter.h"
#include "madara/utility/Thread_Safe_Refcounter.h"
#include "madara/knowledge_engine/Knowledge_Base_Impl.h"
#include "madara/knowledge_engine/Compiled_Expression.h"
#include "madara/knowledge_engine/Variable_Reference.h"


#ifdef _MADARA_PYTHON_CALLBACKS_
#include "boost/python/object.hpp"
#endif

namespace Madara
{

  namespace Knowledge_Engine
  {

    /**
     * @class Knowledge_Base
     * @brief This class provides a distributed knowledge base to users
     */
    class MADARA_Export Knowledge_Base
    {
    public:
      /**
       * Constructor
       **/
      Knowledge_Base ();

      /**
       * Constructor
       * @param   host      hostname/ip of this machine
       * @param   transport transport to use for knowledge dissemination
       **/
      Knowledge_Base (const std::string & host, int transport);

      /**
       * Constructor
       * @param   host      hostname/ip of this machine
       * @param   transport transport to use for knowledge dissemination
       * @param   domain    knowledge domain we want to join
       **/
      Knowledge_Base (const std::string & host, int transport,
        const std::string & domain);

      /**
       * Constructor
       * @param   host      hostname/ip of this machine
       * @param   config    transport settings to use for dissemination
       **/
      Knowledge_Base (const std::string & host, 
        const Madara::Transport::Settings & config);

      /**
       * Copy constructor
       * @param   original  knowledge base to copy
       **/
      Knowledge_Base (const Knowledge_Base & original);
      
      /**
       * Refer to and use another knowledge base's context
       * @param   original  knowledge base to refer to
       **/
      void use (Thread_Safe_Context & original);

      /**
       * Destructor
       **/
      ~Knowledge_Base ();
      
      /**
       * Copies variables and values from source to this context. PERFORMANCE
       * NOTES: worst case depends on size of copy_set. If empty, performance
       * is always O (n), where n is number of variables in the source context.
       * If copy_set is not empty, then performance is O (m log n) where m is
       * the number of variables in copy_set and n is number of variables in
       * the source context.
       * <br>&nbsp;<br>
       * Note that this is a deep copy due to the fact
       * that source and destination are expected to have their own thread
       * management (ref-counted variables can be problematic if shallow copy).
       *
       * @param  source    the source knowledge base to copy from
       * @param  copy_set  a map of variables that should be copied. If
       *                   empty, then everything is copied. If not empty,
       *                   only the supplied variables will be copied.
       * @param  clean_copy  if true, clear the destination context (this)
       *                     before copying.
       **/
      void copy (const Knowledge_Base & source,
        const Copy_Set & copy_set = Copy_Set (),
        bool clean_copy = false);

      /**
       * Assigns another instance's knowledge and settings to this instance
       * @param   original  knowledge base to copy
       **/
      void operator= (const Knowledge_Base & original);

      /**
       * Starts the transport mechanism for dissemination if it is closed
       **/
      void activate_transport (void);

      /**
       * Closes the transport mechanism so no dissemination is possible
       **/
      void close_transport (void);

      /**
       * Locks the context to prevent updates over the network
       **/
      void lock (void);
      
      /**
       * Unlocks the context to allow updates over the network (is only
       * necessary if the context has been explicitly locked)
       **/
      void unlock (void);

      /**
       * Retrieves a knowledge value
       * @param key              knowledge location
       * @param settings         settings for referring to knowledge variables
       * @return                 value at knowledge location
       **/
      Madara::Knowledge_Record get (const std::string & key,
             const Knowledge_Reference_Settings & settings =
                     Knowledge_Reference_Settings (false));
      
      /**
       * Atomically returns the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  the settings for referring to variables
       * @return         the Madara::Knowledge_Record::Integer value for the variable
       **/
      Madara::Knowledge_Record
        get (const Variable_Reference & variable,
             const Knowledge_Reference_Settings & settings =
                     Knowledge_Reference_Settings (false));
      
      /**
       * Atomically returns a reference to the variable. Variable references are
       * efficient mechanisms for reference variables individually--similar to
       * speedups seen from Compiled_Expression.
       * @param   key       unique identifier of the variable
       * @param settings         settings for referring to knowledge variables
       * @return            reference to the variable in the context
       **/
      Variable_Reference
        get_ref (const std::string & key,
             const Knowledge_Reference_Settings & settings =
                     Knowledge_Reference_Settings (false));
      
      /**
       * Marks the variable reference as updated
       * @param   variable  reference to a variable (@see get_ref)
       **/
      void mark_modified (const Variable_Reference & variable);
      
      /**
       * Retrieves a value at a specified index within a knowledge array
       * @param key              knowledge location
       * @param index            index within the array
       * @param settings         settings for referring to knowledge variables
       * @return                 value at knowledge location
       **/
      Madara::Knowledge_Record retrieve_index (const std::string & key,
             size_t index,
             const Knowledge_Reference_Settings & settings =
                     Knowledge_Reference_Settings (false));

      /**
       * Read a file into the knowledge base
       * @param filename           file to read
       * @param knowledge_key      key to store the file into
       * @param settings           settings to use when evaluating/updating
       */
      int read_file (const std::string & knowledge_key, 
                     const std::string & filename, 
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
      /**
       * Atomically reads a file into a variable
       * @param   variable  reference to a variable (@see get_ref)
       * @param   filename  file to read
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if unsuccessful
       **/
      int
      read_file (
        const Variable_Reference & variable,
        const std::string & filename,
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to a string
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const Variable_Reference & variable,
        const std::string & value,
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to a double array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     a STL vector of doubles
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const Variable_Reference & variable,
        const std::vector <double> & value,
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
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
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
      /**
       * Atomically sets the value of an array index to a double.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   index     index within array
       * @param   value     new value of the array index
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_index (const Variable_Reference & variable,
        size_t index, double value,
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to a double.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const Variable_Reference & variable,
        double value,
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
 
      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     a STL vector of Integers
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const Variable_Reference & variable,
        const std::vector <Knowledge_Record::Integer> & value,
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
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
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));

      /**
       * Atomically sets the value of an array index to a double.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   index     index within array
       * @param   value     new value of the array index
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_index (const Variable_Reference & variable,
        size_t index,
        Knowledge_Record::Integer value = Knowledge_Record::Integer (1),
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to an integer.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const Variable_Reference & variable,
        Madara::Knowledge_Record::Integer value = Knowledge_Record::Integer (1), 
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to a text file's contents.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_text (const Variable_Reference & variable,
        const char * value, size_t size, 
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to an arbitrary string.
       * @param   key       name of a variable
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_file (const std::string & key,
        const unsigned char * value, size_t size, 
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to a JPEG image
       * @param   key       name of a variable
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_jpeg (const std::string & key,
        const unsigned char * value, size_t size, 
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to an arbitrary string.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_file (const Variable_Reference & variable,
        const unsigned char * value, size_t size, 
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to a JPEG image
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_jpeg (const Variable_Reference & variable,
        const unsigned char * value, size_t size, 
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
      /**
       * Atomically sets the value of a variable to an XML string.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_xml (const Variable_Reference & variable,
        const char * value, size_t size, 
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
      
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
          Eval_Settings (false, false, true, false, false));

      /**
       * Write a file from the knowledge base to a specified location
       * @param filename           file to write to
       * @param knowledge_key       key to read the file from
       */
      ssize_t write_file (const std::string & knowledge_key, 
                      const std::string & filename);

      /**
       * Read a policy into the knowledge base
       * @param policy_key          key to read the policy into
       * @param policy_file         file to read the policy from
       */
      int read_policy (const std::string & policy_key, 
                       const std::string & policy_file);

      /**
       * Expands a statement using variable expansion. For example, if the
       * statement were MyKnowledge.{.id}, and .id==1, then the statement
       * would be expanded to MyKnowledge.1
       *
       * @param statement          statement to expand
       * @return                   expanded statement
       **/
      std::string expand_statement (const std::string & statement);

#ifndef _MADARA_NO_KARL_

      /**
       * Compiles a KaRL expression into an expression tree
       *
       * @param expression         expression to compile
       * @return                   compiled, optimized expression tree
       **/
      Compiled_Expression
        compile (const std::string & expression);
      
      /**
       * Evaluates an expression
       *
       * @param expression      KaRL expression to evaluate
       * @param settings        Settings for evaluating and printing
       * @return                value of expression
       **/
      Madara::Knowledge_Record evaluate (
        const std::string & expression,
        const Eval_Settings & settings =
          Eval_Settings ());

      /**
       * Evaluates an expression
       *
       * @param expression      KaRL expression to evaluate (result of compile)
       * @param settings        Settings for evaluating and printing
       * @return                value of expression
       **/
      Madara::Knowledge_Record evaluate (
        Compiled_Expression & expression,
        const Eval_Settings & settings =
          Eval_Settings ());
      
      /**
       * Evaluates a root-based tree (result of compile)
       *
       * @param root            root-based tree
       * @param settings        Settings for evaluating and printing
       * @return                value of expression
       **/
      Madara::Knowledge_Record evaluate (
        Expression_Tree::Component_Node * root,
        const Eval_Settings & settings =
          Eval_Settings ());

      /**
       * Waits for an expression to be non-zero.
       * Always disseminates modifications.
       *
       * @param expression      KaRL expression to wait on
       * @param settings        Settings for the underlying expression
       *                        evaluation and printing
       * @return                value of expression
       **/
      Madara::Knowledge_Record wait (
        const std::string & expression,
        const Wait_Settings & settings =
          Wait_Settings ());

      /**
       * Waits for an expression to be non-zero. Provides additional settings
       * for fine-tuning the time to wait and atomic print statements.
       *
       * @param expression      KaRL expression to wait on (result of compile)
       * @param settings        Settings for the underlying expression
       *                        evaluation and printing
       * @return                value of expression
       **/
      Madara::Knowledge_Record wait (
        Compiled_Expression & expression,
        const Wait_Settings & settings =
          Wait_Settings ());
      
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
      
#endif // _MADARA_NO_KARL_

      /**
       * Sets a knowledge variable to a specified value
       *
       * @param key             knowledge variable location
       * @param value           value to set at location
       * @param settings        settings for applying the update
       * @return                0 if successful, -1 if key is null, and
       *                        -2 if quality isn't high enough
       **/
      int set (const std::string & key,
        Madara::Knowledge_Record::Integer value = 
          Madara::Knowledge_Record::MODIFIED, 
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));

      /**
       * Sets an index within an array to a specified value
       *
       * @param key             knowledge variable location
       * @param index           index of the location in the array
       * @param value           value to set at location
       * @param settings        settings for applying the update
       * @return                0 if successful, -1 if key is null, and
       *                        -2 if quality isn't high enough
       **/
      int set_index (const std::string & key,
        size_t index,
        Madara::Knowledge_Record::Integer value,
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));

      /**
       * Sets a knowledge variable to a specified value
       *
       * @param key             knowledge variable location
       * @param value           array of integers to set at the location
       * @param size            number of elements in the array
       * @param settings        settings for applying the update
       * @return                0 if successful, -1 if key is null, and
       *                        -2 if quality isn't high enough
       **/
      int set (const std::string & key,
        const Madara::Knowledge_Record::Integer * value,
        uint32_t size,
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
       
      /**
       * Sets a knowledge variable to a specified value
       *
       * @param key             knowledge variable location
       * @param value           array of integers to set at the location
       * @param settings        settings for applying the update
       * @return                0 if successful, -1 if key is null, and
       *                        -2 if quality isn't high enough
       **/
      int set (const std::string & key,
        const std::vector <Knowledge_Record::Integer> & value,
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
       
      /**
       * Sets a knowledge variable to a specified value
       *
       * @param key             knowledge variable location
       * @param value           value to set at location
       * @param settings        settings for applying the update
       * @return                0 if successful, -1 if key is null, and
       *                        -2 if quality isn't high enough
       **/
      int set (const std::string & key, double value, 
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));

      /**
       * Sets an index within an array to a specified value
       *
       * @param key             knowledge variable location
       * @param index           index of the location in the array
       * @param value           value to set at location
       * @param settings        settings for applying the update
       * @return                0 if successful, -1 if key is null, and
       *                        -2 if quality isn't high enough
       **/
      int set_index (const std::string & key,
        size_t index,
        double value,
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));

      /**
       * Sets a knowledge variable to a specified value
       *
       * @param key             knowledge variable location
       * @param value           array of doubles to set at the location
       * @param size            number of elements in the array
       * @param settings        settings for applying the update
       * @return                0 if successful, -1 if key is null, and
       *                        -2 if quality isn't high enough
       **/
      int set (const std::string & key,
        const double * value,
        uint32_t size,
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
       
      /**
       * Sets a knowledge variable to a specified value
       *
       * @param key             knowledge variable location
       * @param value           array of doubles to set at the location
       * @param settings        settings for applying the update
       * @return                0 if successful, -1 if key is null, and
       *                        -2 if quality isn't high enough
       **/
      int set (const std::string & key,
        const std::vector <double> & value,
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));
        
      /**
       * Sets a knowledge variable to a specified value
       *
       * @param key             knowledge variable location
       * @param value           value to set at location
       * @param settings        settings for applying the update
       * @return                0 if successful, -1 if key is null, and
       *                        -2 if quality isn't high enough
       **/
      int set (const std::string & key, const std::string & value, 
        const Eval_Settings & settings =
          Eval_Settings (false, false, true, false, false));

      /**
       * Sets the quality of writing to a certain variable from this entity
       *
       * @param key             knowledge variable location
       * @param quality         quality of writing to this location
       * @param settings        settings for referring to knowledge variables
       **/
      void set_quality (const std::string & key, uint32_t quality,
             const Knowledge_Reference_Settings & settings =
                     Knowledge_Reference_Settings (false));
      
      /**
       * Binds to an ephemeral port for unique tie breakers in global ordering.
       * Call this function if you want to generate a unique id for a
       * custom transport to use (or a built-in transport if you are creating
       * one outside of the Knowledge_Base.
       * @param  host   unique host identifier string ("" if requesting a
       *                generated id.
       * @return        unique host id (same as host param if host is not "")
       **/
      std::string setup_unique_hostport (const std::string & host = "");

#ifdef _USE_CID_
      
      /**
       * Print all redeployment algorithm results
       **/
      void print_all_redeployment_results (std::ostream & output);

      /**
       * Run all redeployment algorithms specified in Transport Settings,
       * within the latencies.algorithm_configs variable.
       **/
      void run_all (void);

      /**
       * Starts a latency round
       * @return  result of operation or -1 if we are shutting down
       **/
      long start_latency (void);

      /**
       * Prints latencies associated with this process id
       * @param   output    stream to print latencies to
       **/
      void print_my_latencies (std::ostream & output);

      /**
       * Prints all latencies in the context for all processes
       * @param   output    stream to print latencies to
       **/
      void print_all_latencies (std::ostream & output);

      /**
       * Prints all summations in the context for all processes
       * @param   output    stream to print summations to
       **/
      void print_all_summations (std::ostream & output);
      
      /**
       * Votes with the lowest algorithm results current in the Transport settings.
       * This is only useful if latency collection is enabled and run_all has been
       * populated with useful redeployment algorithms before being ran. See the
       * Madara::Transport::Settings class for all voting parameters.
       **/
      long vote (void);

#endif // _USE_CID_

      /**
       * Returns a non-const reference to the Transport Settings
       * @return  settings reference
       **/
      Madara::Transport::Settings & transport_settings (void);

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
       * Applies current time and modified to all global variables and tries
       * to send them.
       * @param settings        Settings for evaluating and printing
       * @return 0 if there was not a problem.
       **/
      int apply_modified (const Eval_Settings & settings =
          Eval_Settings ());

      /**
       * Sets the log level to dictate the detail of MADARA logging. A higher
       * log level will print out all lower levels of logging. 10 is currently
       * the highest log level. Anything higher than this will be the
       * equivalent of setting the log level to 10.
       * level      
       * 0          Emergency printing. Generally major, critical errors
       * 1          Impending critical errors. Usually lead to a level 0 error
       * 2          Non-fatal errors. These indicate lowest level of errors.
       * 3          Error details. These describe errors above this level.
       * 4          Warnings. These indicate potential problems.
       * 5          Major events. Normal, important events.
       * 6          Minor events. Normal, minor events.
       * 7          Event trace. Normal components of minor events.
       * 8          Debug info. Configuration information.
       * 9          Debug trace. Trace information for debugging info.
       * 10         Debug detailed trace. Very detailed trace information.
       * @param   level    severity of log messages to log
       * @return    the resulting log level
       **/
      static int log_level (int level);
      
      /**
       * Gets the log level to dictate the detail of MADARA logging. A higher
       * log level will print out all lower levels of logging. 10 is currently
       * the highest log level. Anything higher than this will be the
       * equivalent of setting the log level to 10.
       * level      
       * 0          Emergency printing. Generally major, critical errors
       * 1          Impending critical errors. Usually lead to a level 0 error
       * 2          Non-fatal errors. These indicate lowest level of errors.
       * 3          Error details. These describe errors above this level.
       * 4          Warnings. These indicate potential problems.
       * 5          Major events. Normal, important events.
       * 6          Minor events. Normal, minor events.
       * 7          Event trace. Normal components of minor events.
       * 8          Debug info. Configuration information.
       * 9          Debug trace. Trace information for debugging info.
       * 10         Debug detailed trace. Very detailed trace information.
       * @return    the log level currently used to log information
       **/
      static int log_level (void);

      /**
       * Prints all knowledge variables and values in the context
       * @param   level    level to log the knowledge at
       **/
      void print (unsigned int level = 0) const;
      
      /**
       * Deprecated alias for @see print with only level. This class
       * member will print all knowledge in the context.
       * @param   level    level to log the knowledge at
       **/
      void print_knowledge (unsigned int level = 0) const;

      /**
       * Expands and prints a user provided statement at a logging level.
       * For example, if the statement were MyKnowledge.{.id}, and .id==1,
       * then the statement would be expanded to MyKnowledge.1
       * 
       * @param   statement  statement to expand with variable values
       * @param   level      level to log the knowledge at
       **/
      void print (const std::string & statement, 
        unsigned int level = 0) const;
      
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
       * Clears the knowledge base
       **/
      void clear (void);

      /**
       * Clears the knowledge base
       **/
      void clear_map (void);

      /**
       * Acquires the recursive lock on the knowledge base. This will
       * block any other thread from updating or using the knowledge
       * base until you call @ release.
       **/
      void acquire (void);

      /**
       * Releases a recursive lock on the knowledge base. This will
       * allow other thread to access the knowledge base if you had
       * previously called @ acquire.
       **/
      void release (void);
      
      /**
       * Attaches a transport to the Knowledge Engine. Note that the
       * transport should use the same Thread_Safe_Context as the
       * Knowledge Engine.
       * @param  transport   a new transport to attach to the Knowledge Base
       * @return             the number of transports now attached
       **/
      size_t attach_transport (Madara::Transport::Base * transport);
      
      /**
       * Adds a built-in transport with the specified settings
       * @param  id          unique identifier for this agent (empty string
       *                     will use default generated id)
       * @param  settings    settings for the new transport
       * @return             the number of transports now attached
       **/
      size_t attach_transport (const std::string & id,
        Transport::Settings & settings);
      
      /**
       * Removes a transport
       * @param   index      index of the transport to remove. If invalid,
       *                     nothing is removed.
       * @return             the size of transports now attached
       **/
      size_t remove_transport (size_t index);

      /**
       * Returns the Thread_Safe_Context associated with this Knowledge
       * Base. This is necessary for creating custom transports.
       *
       * @return             the context used by the knowledge base
       **/
      Thread_Safe_Context & get_context (void);
      
      /**
       * Returns the unique host and ephemeral binding for this Knowlede Base
       * @return             host:port identifier for this knowledge base
       **/
      std::string get_id (void);
      
      /**
       * Sets logging to STDERR (terminal). This is the default.
       * @param   clear_flags if true, clears all other logging options. If
       *                      false, retains previous logging options.
       **/
      static void log_to_stderr (bool clear_flags = true);

      /**
       * Sets logging to a file
       *
       * @param   filename    path to the file that should contain log messages
       * @param   clear_flags if true, clears all other logging options. If
       *                      false, retains previous logging options.
       **/
      static void log_to_file (const char * filename, bool clear_flags = true);

      /**
       * Sets logging to the system log
       * @param   prog_name   system logs require a program name. Use argv[0]
       *                      if unsure what to do with this.
       * @param   clear_flags if true, clears all other logging options. If
       *                      false, retains previous logging options
       **/
      static void log_to_system_log (const char * prog_name, bool clear_flags = true);

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
       * @param   expression  An expression that matches the variable names
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
       * @param   use_id      if true, sets the unique identifier to the
       *                      one found in the saved context. If false,
       *                      keeps the default identifier.
       * @param   settings    settings to use when applying updates to context
       * @return  total bytes read
       **/
      int64_t load_context (const std::string & filename,
        bool use_id = true,
        const Knowledge_Update_Settings & settings = 
              Knowledge_Update_Settings (true, true, true, false));

      /**
       * Sends all modified variables through the attached transports.
       * @param   prefix      for logging purposes, the descriptor prefix for 
       *                      calling method
       * @param   settings    settings for sending modifications
       * @return  number of transports the modifications were sent to
       **/
      int send_modifieds (
        const std::string & prefix = "Knowledge_Base::send_modifieds",
        const Eval_Settings & settings =
          Eval_Settings ());

      
      /**
       * Wait for a change to happen to the context (e.g., from transports)
       **/
      void wait_for_change (void);


    private:

      /// Pointer to actual implementation, i.e., the "bridge", which is
      /// reference counted to automate memory management. 
      Madara::Utility::Thread_Safe_Refcounter <Knowledge_Base_Impl> impl_;

      /// A knowledge base can also be a facade for another knowledge base
      Thread_Safe_Context * context_;
    };

  }
}

// include the inline functions
#include "Knowledge_Base.inl"

#endif   // _MADARA_KNOWLEDGE_BASE_H
