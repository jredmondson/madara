#ifndef _MADARA_KNOWLEDGE_BASE_H
#define _MADARA_KNOWLEDGE_BASE_H

/**
 * @file KnowledgeBase.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the KnowledgeBase class
 */

#include <ostream>
#include <string>
#include <map>
#include <memory>

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "madara/knowledge/Functions.h"
#include "madara/filters/Arguments.h"
#include "madara/MadaraExport.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/transport/Transport.h"
#include "madara/expression/Interpreter.h"
#include "madara/utility/Refcounter.h"
#include "madara/knowledge/KnowledgeBaseImpl.h"
#include "madara/knowledge/CompiledExpression.h"
#include "madara/knowledge/VariableReference.h"


#ifdef _MADARA_PYTHON_CALLBACKS_
#include "madara/Boost.h"
#include "boost/python/object.hpp"
#endif

namespace madara
{

  namespace knowledge
  {

    /**
     * @class KnowledgeBase
     * @brief This class provides a distributed knowledge base to users
     */
    class MADARA_EXPORT KnowledgeBase
    {
    public:
      /**
       * Constructor
       **/
      KnowledgeBase ();

      /**
       * Constructor
       * @param   host      hostname/ip of this machine
       * @param   transport transport to use for knowledge dissemination
       **/
      KnowledgeBase (const std::string & host, int transport);

      /**
       * Constructor
       * @param   host      hostname/ip of this machine
       * @param   transport transport to use for knowledge dissemination
       * @param   domain    knowledge domain we want to join
       **/
      KnowledgeBase (const std::string & host, int transport,
        const std::string & domain);

      /**
       * Constructor
       * @param   host      hostname/ip of this machine
       * @param   config    transport settings to use for dissemination
       **/
      KnowledgeBase (const std::string & host,
        const madara::transport::TransportSettings & config);

      /**
       * Copy constructor
       * @param   original  knowledge base to copy
       **/
      KnowledgeBase (const KnowledgeBase & original) = default;

      /**
       * Refer to and use another knowledge base's context
       * @param   original  knowledge base to refer to
       **/
      void use (ThreadSafeContext & original);

      /**
       * Destructor
       **/
      ~KnowledgeBase () = default;

      /**
       * Copies variables and values from source to this context.
       * PERFORMANCE NOTES: predicates with prefixes can limit
       * copying to O(log n). predices with suffixes and no prefix
       * force O(n) copy cost since all records could fit requirements
       *
       * @param  source    the source context to copy from
       * @param  reqs  requirements that must be met
       * @param  settings  settings for appplying updates to context
       **/
      void copy (const KnowledgeBase & source,
        const KnowledgeRequirements & reqs,
        const EvalSettings & settings = EvalSettings ());

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
       * @param  settings  settings for appplying updates to context
       **/
      void copy (const KnowledgeBase & source,
        const CopySet & copy_set = CopySet (),
        bool clean_copy = false,
        const EvalSettings & settings = EvalSettings ());

      /**
       * Assigns another instance's knowledge and settings to this instance
       * @param   original  knowledge base to copy
       **/
      KnowledgeBase &operator= (const KnowledgeBase & original) = default;

      /**
       * Starts the transport mechanism for dissemination if it is closed
       **/
      void activate_transport (void);

      /**
       * Closes the transport mechanism so no dissemination is possible
       **/
      void close_transport (void);

      /**
       * Gets the number of transports
       * @return the number of transports
       **/
      size_t get_num_transports (void);

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
      madara::knowledge::KnowledgeRecord get (const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings (false));

      /**
       * Atomically returns the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  the settings for referring to variables
       * @return         the madara::knowledge::KnowledgeRecord::Integer value for the variable
       **/
      madara::knowledge::KnowledgeRecord
        get (const VariableReference & variable,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings (false));

      /**
       * Atomically returns a reference to the variable. Variable references are
       * efficient mechanisms for reference variables individually--similar to
       * speedups seen from CompiledExpression.
       * @param   key       unique identifier of the variable
       * @param settings         settings for referring to knowledge variables
       * @return            reference to the variable in the context
       **/
      VariableReference
        get_ref (const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings (false));

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not a string, returns NULL shared_ptr
       **/
      std::shared_ptr<std::string> share_string(const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const
      {
        if (impl_)
        {
          return impl_->share_string (key, settings);
        }
        else if (context_)
        {
          return context_->share_string (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not a string, returns NULL shared_ptr
       **/
      std::shared_ptr<std::string> share_string(const VariableReference & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const
      {
        if (impl_)
        {
          return impl_->share_string (key, settings);
        }
        else if (context_)
        {
          return context_->share_string (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, while resetting this record to empty.
       * If this record is not a string, returns NULL shared_ptr
       * and this record is unmodified.
       **/
      std::shared_ptr<std::string> take_string(const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ())
      {
        if (impl_)
        {
          return impl_->take_string (key, settings);
        }
        else if (context_)
        {
          return context_->take_string (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, while resetting this record to empty.
       * If this record is not a string, returns NULL shared_ptr
       * and this record is unmodified.
       **/
      std::shared_ptr<std::string> take_string(const VariableReference & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ())
      {
        if (impl_)
        {
          return impl_->take_string (key, settings);
        }
        else if (context_)
        {
          return context_->take_string (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not an integer array, returns NULL shared_ptr
       **/
      std::shared_ptr<std::vector<KnowledgeRecord::Integer>>
        share_integers(const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const
      {
        if (impl_)
        {
          return impl_->share_integers (key, settings);
        }
        else if (context_)
        {
          return context_->share_integers (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not an integer array, returns NULL shared_ptr
       **/
      std::shared_ptr<std::vector<KnowledgeRecord::Integer>>
        share_integers(const VariableReference & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const
      {
        if (impl_)
        {
          return impl_->share_integers (key, settings);
        }
        else if (context_)
        {
          return context_->share_integers (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not an integer array, returns NULL shared_ptr
       * and this record is unmodified.
       **/
      std::shared_ptr<std::vector<KnowledgeRecord::Integer>>
        take_integers(const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ())
      {
        if (impl_)
        {
          return impl_->take_integers (key, settings);
        }
        else if (context_)
        {
          return context_->take_integers (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not an integer array, returns NULL shared_ptr
       * and this record is unmodified.
       **/
      std::shared_ptr<std::vector<KnowledgeRecord::Integer>>
        take_integers(const VariableReference & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ())
      {
        if (impl_)
        {
          return impl_->take_integers (key, settings);
        }
        else if (context_)
        {
          return context_->take_integers (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not a doubles array, returns NULL shared_ptr
       **/
      std::shared_ptr<std::vector<double>>
        share_doubles(const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const
      {
        if (impl_)
        {
          return impl_->share_doubles (key, settings);
        }
        else if (context_)
        {
          return context_->share_doubles (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not a doubles array, returns NULL shared_ptr
       **/
      std::shared_ptr<std::vector<double>>
        share_doubles(const VariableReference & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const
      {
        if (impl_)
        {
          return impl_->share_doubles (key, settings);
        }
        else if (context_)
        {
          return context_->share_doubles (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not a doubles array, returns NULL shared_ptr
       * and this record is unmodified.
       **/
      std::shared_ptr<std::vector<double>>
        take_doubles(const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ())
      {
        if (impl_)
        {
          return impl_->take_doubles (key, settings);
        }
        else if (context_)
        {
          return context_->take_doubles (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not a doubles array, returns NULL shared_ptr
       * and this record is unmodified.
       **/
      std::shared_ptr<std::vector<double>>
        take_doubles(const VariableReference & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ())
      {
        if (impl_)
        {
          return impl_->take_doubles (key, settings);
        }
        else if (context_)
        {
          return context_->take_doubles (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not a binary file, returns NULL shared_ptr
       **/
      std::shared_ptr<std::vector<unsigned char>>
        share_binary(const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const
      {
        if (impl_)
        {
          return impl_->share_binary (key, settings);
        }
        else if (context_)
        {
          return context_->share_binary (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not a binary file, returns NULL shared_ptr
       **/
      std::shared_ptr<std::vector<unsigned char>>
        share_binary(const VariableReference & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const
      {
        if (impl_)
        {
          return impl_->share_binary (key, settings);
        }
        else if (context_)
        {
          return context_->share_binary (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not a binary file, returns NULL shared_ptr
       * and this record is unmodified.
       **/
      std::shared_ptr<std::vector<unsigned char>>
        take_binary(const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ())
      {
        if (impl_)
        {
          return impl_->take_binary (key, settings);
        }
        else if (context_)
        {
          return context_->take_binary (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not a binary file, returns NULL shared_ptr
       * and this record is unmodified.
       **/
      std::shared_ptr<std::vector<unsigned char>>
        take_binary(const VariableReference & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ())
      {
        if (impl_)
        {
          return impl_->take_binary (key, settings);
        }
        else if (context_)
        {
          return context_->take_binary (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not an Any, returns NULL shared_ptr
       **/
      std::shared_ptr<Any> share_any(const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const
      {
        if (impl_)
        {
          return impl_->share_any (key, settings);
        }
        else if (context_)
        {
          return context_->share_any (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, sharing with the internal one.
       * If this record is not an Any, returns NULL shared_ptr
       **/
      std::shared_ptr<Any> share_any(const VariableReference & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const
      {
        if (impl_)
        {
          return impl_->share_any (key, settings);
        }
        else if (context_)
        {
          return context_->share_any (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, while resetting this record to empty.
       * If this record is not an Any, returns NULL shared_ptr
       * and this record is unmodified.
       **/
      std::shared_ptr<Any> take_any(const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ())
      {
        if (impl_)
        {
          return impl_->take_any (key, settings);
        }
        else if (context_)
        {
          return context_->take_any (key, settings);
        }

        return nullptr;
      }

      /**
       * @return a shared_ptr, while resetting this record to empty.
       * If this record is not an Any, returns NULL shared_ptr
       * and this record is unmodified.
       **/
      std::shared_ptr<Any> take_any(const VariableReference & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ())
      {
        if (impl_)
        {
          return impl_->take_any (key, settings);
        }
        else if (context_)
        {
          return context_->take_any (key, settings);
        }

        return nullptr;
      }

      /**
       * Marks the variable reference as updated
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  settings for applying the update
       **/
      void mark_modified (const VariableReference & variable,
          const KnowledgeUpdateSettings & settings = KnowledgeUpdateSettings());

      /**
       * Marks the variable as updated
       * @param   name      name of the variable
       * @param   settings  settings for applying the update
       **/
      void mark_modified (const std::string & name,
          const KnowledgeUpdateSettings & settings = KnowledgeUpdateSettings());

      /**
       * Retrieves a value at a specified index within a knowledge array
       * @param key              knowledge location
       * @param index            index within the array
       * @param settings         settings for referring to knowledge variables
       * @return                 value at knowledge location
       **/
      madara::knowledge::KnowledgeRecord retrieve_index (const std::string & key,
             size_t index,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings (false));

      /**
       * Read a file into the knowledge base
       * @param filename           file to read
       * @param knowledge_key      key to store the file into
       * @param settings           settings to use when evaluating/updating
       */
      int read_file (const std::string & knowledge_key,
                     const std::string & filename,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically reads a file into a variable
       * @param   variable  reference to a variable (@see get_ref)
       * @param   filename  file to read
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if unsuccessful
       **/
      int
      read_file (
        const VariableReference & variable,
        const std::string & filename,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to a string
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const std::string & value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to a string
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        std::string && value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to a KnowledgeRecord
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const KnowledgeRecord & value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to a KnowledgeRecord
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        KnowledgeRecord && value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to a KnowledgeRecord
       * @param   key       the variable name
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const KnowledgeRecord & value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to a KnowledgeRecord
       * @param   key       the variable name
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        KnowledgeRecord && value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to an integer 1
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false)) {
        return set (variable, 1, settings);
      }

      /**
       * Atomically sets the value of a variable to a double array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     a STL vector of doubles
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const std::vector <double> & value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to a double array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     a STL vector of doubles
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        std::vector <double> && value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

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
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of an array index to a value.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   index     index within array
       * @param   value     new value of the array index
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      template<typename T>
      int set_index (const VariableReference & variable,
        size_t index, T&& value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to a double.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      template<typename T,
        typename std::enable_if<std::is_floating_point<T>::value,
        void*>::type = nullptr>
      int set (const VariableReference & variable,
        T value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     a STL vector of Integers
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const std::vector <KnowledgeRecord::Integer> & value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     a STL vector of Integers
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        std::vector <KnowledgeRecord::Integer> && value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

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
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to an integer.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      template<typename T,
        typename std::enable_if<std::is_integral<T>::value,
        void*>::type = nullptr>
      int set (const VariableReference & variable,
        T value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to a text file's contents.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_text (const VariableReference & variable,
        const char * value, size_t size,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

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
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

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
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to an arbitrary string.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_file (const VariableReference & variable,
        const unsigned char * value, size_t size,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to a JPEG image
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_jpeg (const VariableReference & variable,
        const unsigned char * value, size_t size,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to an XML string.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_xml (const VariableReference & variable,
        const char * value, size_t size,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the record to the value specified, as an Any value.
       *
       * Note, this does not copy meta information (e.g. quality, clock).
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      template<typename T>
      int set_any (const std::string & key,
        T && value,
        const EvalSettings & settings =
              EvalSettings ());

      /**
       * Atomically sets the record to the value specified, as an Any value.
       *
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      template<typename T>
      int set_any (const VariableReference & variable,
        T && value,
        const EvalSettings & settings =
              EvalSettings ());

      /**
       * Atomically emplaces an Any value within the given variable.
       *
       * @param   key       unique identifier of the variable
       * @param   args      arguments to emplace_any of KnowledgeRecord
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      template<typename... Args>
      int emplace_any (const std::string & key,
        const EvalSettings & settings,
        Args&&... args);

      /**
       * Atomically emplaces an Any value within the given variable.
       *
       * @param   variable  reference to a variable (@see get_ref)
       * @param   args      arguments to emplace_any of KnowledgeRecord
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      template<typename... Args>
      int emplace_any (const VariableReference & variable,
        const EvalSettings & settings,
        Args&&... args);

      /**
       * Atomically emplaces an Any value within the given variable.
       *
       * @param   key       unique identifier of the variable
       * @param   args      arguments to emplace_any of KnowledgeRecord
       * @return   0 if the value was set. -1 if null key
       **/
      template<typename Arg, typename... Args,
        enable_if_<!is_convertible<
          Arg, const KnowledgeReferenceSettings &>(), int> = 0>
      int emplace_any (const std::string & key,
        Arg&& arg, Args&&... args)
      {
        return emplace_any(key, EvalSettings{},
            std::forward<Arg>(arg), std::forward<Args>(args)...);
      }

      /**
       * Atomically emplaces an Any value within the given variable.
       *
       * @param   variable  reference to a variable (@see get_ref)
       * @param   args      arguments to emplace_any of KnowledgeRecord
       * @return   0 if the value was set. -1 if null key
       **/
      template<typename Arg, typename... Args,
        enable_if_<!is_convertible<
          Arg, const KnowledgeReferenceSettings &>(), int> = 0>
      int emplace_any (const VariableReference & variable,
        Arg&& arg, Args&&... args)
      {
        return emplace_any(variable, EvalSettings{},
            std::forward<Arg>(arg), std::forward<Args>(args)...);
      }

      /**
       * Atomically emplaces an empty Any value within the given variable.
       *
       * @param   key       unique identifier of the variable
       * @return   0 if the value was set. -1 if null key
       **/
      int emplace_any (const std::string & key)
      {
        return emplace_any(key, EvalSettings{});
      }

      /**
       * Atomically emplaces an empty Any value within the given variable.
       *
       * @param   variable  reference to a variable (@see get_ref)
       * @return   0 if the value was set. -1 if null key
       **/
      int emplace_any (const VariableReference & variable)
      {
        return emplace_any(variable, EvalSettings{});
      }

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
          EvalSettings (false, false, true, false, false));

      /**
       * Write a file from the knowledge base to a specified location
       * @param filename           file to write to
       * @param knowledge_key       key to read the file from
       */
      ssize_t write_file (const std::string & knowledge_key,
                      const std::string & filename);

      /**
       * Expands a statement using variable expansion. For example, if the
       * statement were MyKnowledge.{.id}, and .id==1, then the statement
       * would be expanded to MyKnowledge.1
       *
       * @param statement          statement to expand
       * @return                   expanded statement
       **/
      std::string expand_statement (const std::string & statement);

      /**
      * Retrieves a stringified list of all modified variables that are ready
      * to send over transport on next send_modifieds call
      * @return  stringified list of the modified knowledge records
      **/
      std::string debug_modifieds (void) const;

#ifndef _MADARA_NO_KARL_

      /**
       * Compiles a KaRL expression into an expression tree
       *
       * @param expression         expression to compile
       * @return                   compiled, optimized expression tree
       * @throw exceptions::KarlException  failure during compile/evaluate
       **/
      CompiledExpression
        compile (const std::string & expression);

      /**
       * Evaluates an expression
       *
       * @param expression      KaRL expression to evaluate
       * @param settings        Settings for evaluating and printing
       * @return                value of expression
       * @throw exceptions::KarlException  failure during compile/evaluate
       **/
      madara::knowledge::KnowledgeRecord evaluate (
        const std::string & expression,
        const EvalSettings & settings =
          EvalSettings ());

      /**
       * Evaluates an expression
       *
       * @param expression      KaRL expression to evaluate (result of compile)
       * @param settings        Settings for evaluating and printing
       * @return                value of expression
       * @throw exceptions::KarlException  failure during compile/evaluate
       **/
      madara::knowledge::KnowledgeRecord evaluate (
        CompiledExpression & expression,
        const EvalSettings & settings =
          EvalSettings ());

      /**
       * Evaluates a root-based tree (result of compile)
       *
       * @param root            root-based tree
       * @param settings        Settings for evaluating and printing
       * @return                value of expression
       * @throw exceptions::KarlException  failure during compile/evaluate
       **/
      madara::knowledge::KnowledgeRecord evaluate (
        expression::ComponentNode * root,
        const EvalSettings & settings =
          EvalSettings ());

      /**
       * Waits for an expression to be non-zero.
       * Always disseminates modifications.
       *
       * @param expression      KaRL expression to wait on
       * @param settings        Settings for the underlying expression
       *                        evaluation and printing
       * @return                value of expression
       * @throw exceptions::KarlException  failure during compile/evaluate
       **/
      madara::knowledge::KnowledgeRecord wait (
        const std::string & expression,
        const WaitSettings & settings =
          WaitSettings ());

      /**
       * Waits for an expression to be non-zero. Provides additional settings
       * for fine-tuning the time to wait and atomic print statements.
       *
       * @param expression      KaRL expression to wait on (result of compile)
       * @param settings        Settings for the underlying expression
       *                        evaluation and printing
       * @return                value of expression
       * @throw exceptions::KarlException  failure during compile/evaluate
       **/
      madara::knowledge::KnowledgeRecord wait (
        CompiledExpression & expression,
        const WaitSettings & settings =
          WaitSettings ());

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
       * Sets a knowledge variable to a specified value
       *
       * @param key             knowledge variable location
       * @param value           value to set at location
       * @param settings        settings for applying the update
       * @return                0 if successful, -1 if key is null, and
       *                        -2 if quality isn't high enough
       **/
      template<typename T,
        typename std::enable_if<std::is_integral<T>::value,
        void*>::type = nullptr>
      int set (const std::string & key,
        T value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

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
      template<typename T>
      int set_index (const std::string & key,
        size_t index, T&& value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

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
        const madara::knowledge::KnowledgeRecord::Integer * value,
        uint32_t size,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

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
        const std::vector <KnowledgeRecord::Integer> & value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

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
        std::vector <KnowledgeRecord::Integer> && value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Atomically sets the value of a variable to an integer 1
       * @param   key       the name of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false)) {
        return set (key, 1, settings);
      }

      /**
       * Sets a knowledge variable to a specified value
       *
       * @param key             knowledge variable location
       * @param value           value to set at location
       * @param settings        settings for applying the update
       * @return                0 if successful, -1 if key is null, and
       *                        -2 if quality isn't high enough
       **/
      template<typename T,
        typename std::enable_if<std::is_floating_point<T>::value,
        void*>::type = nullptr>
      int set (const std::string & key, T value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

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
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

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
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

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
        std::vector <double> && value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

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
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Sets a knowledge variable to a specified value
       *
       * @param key             knowledge variable location
       * @param value           value to set at location
       * @param settings        settings for applying the update
       * @return                0 if successful, -1 if key is null, and
       *                        -2 if quality isn't high enough
       **/
      int set (const std::string & key, std::string && value,
        const EvalSettings & settings =
          EvalSettings (false, false, true, false, false));

      /**
       * Sets the quality of writing to a certain variable from this entity
       *
       * @param key             knowledge variable location
       * @param quality         quality of writing to this location
       * @param settings        settings for referring to knowledge variables
       **/
      void set_quality (const std::string & key, uint32_t quality,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings (false));

      /**
       * Binds to an ephemeral port for unique tie breakers in global ordering.
       * Call this function if you want to generate a unique id for a
       * custom transport to use (or a built-in transport if you are creating
       * one outside of the KnowledgeBase.
       * @param  host   unique host identifier string ("" if requesting a
       *                generated id.
       * @return        unique host id (same as host param if host is not "")
       **/
      std::string setup_unique_hostport (const std::string & host = "");

      /**
       * @return a non-const reference to the Transport Settings
       * @return  settings reference
       **/
      madara::transport::TransportSettings & transport_settings (void);

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

      /**
       * Applies current time and modified to all global variables and tries
       * to send them.
       * @param settings        Settings for evaluating and printing
       * @return 0 if there was not a problem.
       **/
      int apply_modified (const EvalSettings & settings =
          EvalSettings ());

      /**
      * Alias for apply_modified. @see apply_modified
      * @param settings        Settings for evaluating and printing
      * @return 0 if there was not a problem.
      **/
      int modify (const EvalSettings & settings =
        EvalSettings ());

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
      * Clears a variable. This is safer than erasing the variable.
      * It clears the memory used in the variable and marks it as UNCREATED,
      * meaning that it is effectively deleted, will not show up in
      * @see print statements or @see save_checkpoint.
      * @param   key            unique identifier of the variable
      * @param   settings       settings for referring to variables
      * @return                 true if variable exists
      **/
      bool clear (const std::string & key,
        const KnowledgeReferenceSettings & settings =
        KnowledgeReferenceSettings ());

      /**
       * Clears the knowledge base
       * @param  erase   If true, completely erase keys.
       *                 If false, reset knowledge
       **/
      void clear (bool erase = false);

      /**
       * Resets the local changed map, which tracks checkpointing modifieds
       **/
      void reset_checkpoint (void) const;

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
       * transport should use the same ThreadSafeContext as the
       * Knowledge Engine.
       * @param  transport   a new transport to attach to the Knowledge Base
       * @return             the number of transports now attached
       **/
      size_t attach_transport (madara::transport::Base * transport);

      /**
       * Adds a built-in transport with the specified settings
       * @param  id          unique identifier for this agent (empty string
       *                     will use default generated id)
       * @param  settings    settings for the new transport
       * @return             the number of transports now attached
       **/
      size_t attach_transport (const std::string & id,
        transport::TransportSettings & settings);

      /**
       * Removes a transport
       * @param   index      index of the transport to remove. If invalid,
       *                     nothing is removed.
       * @return             the size of transports now attached
       **/
      size_t remove_transport (size_t index);

      /**
       * Returns the ThreadSafeContext associated with this Knowledge
       * Base. This is necessary for creating custom transports.
       *
       * @return             the context used by the knowledge base
       **/
      ThreadSafeContext & get_context (void);

      /**
      * Gets the log level
      * @return the maximum detail level to print
      **/
      int get_log_level (void);

      /**
      * Attaches a logger to be used for printing
      * @param logger  the logger the context should lose
      **/
      void attach_logger (logger::Logger & logger) const;

      /**
      * Gets the logger used for information printing
      * @return the context's logger
      **/
      logger::Logger & get_logger (void) const;

      /**
      * Sets the log level
      * @param  level  the maximum detail level to print
      **/
      void set_log_level (int level);

      /**
       * Returns the unique host and ephemeral binding for this Knowlede Base
       * @return             host:port identifier for this knowledge base
       **/
      std::string get_id (void);

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
      * Fills a variable map with list of keys according to a matching prefix,
      * suffix, and delimiter hierarchy. This is useful for understanding the
      * logical hierarchy of your variables (and also a key utility of
      * containers like @see containers::FlexMap).
      * @param   prefix      Text that must be present at the front of the key
      * @param   delimiter   Text that signifies a logical boundary in hierarchy If
      *                      empty, no check is performed.
      * @param   suffix      Text that must be present at the end of the key. If
      *                      empty, no check is performed.
      * @param   next_keys   The immediate keys in the hierarchy after prefix
      * @param   result      The map that will be filled with full variable names
      *                      and the Knowledge Records that correspond to
      * @param   just_keys   if true, do not fill result, only next_keys
      * @return              entries in the resulting map
      **/
      size_t  to_map (const std::string & prefix,
        const std::string & delimiter,
        const std::string & suffix,
        std::vector <std::string> & next_keys,
        std::map <std::string, knowledge::KnowledgeRecord> & result,
        bool just_keys = false);

      /**
       * Creates a variable map with Knowledge Records that begin with the given
       * prefix. Runs in O(log n + m) time, where n is the size of the
       * KnowledgeBase, and m is the number of matching records
       *
       * @param   prefix      Prefix string to match with
       * @return              A new map with just entries starting with prefix
       **/
      knowledge::KnowledgeMap to_map (const std::string & prefix) const;

      /**
       * Creates a map with Knowledge Records that begin with the given
       * prefix. Runs in O(log n + m) time, where n is the size of the
       * KnowledgeBase, and m is the number of matching records
       *
       * All key names have the "prefix" portion stripped off their front in
       * the returned map.
       *
       * @param   prefix      Prefix string to match with
       * @return              A new map with just entries starting with prefix
       **/
      knowledge::KnowledgeMap to_map_stripped (const std::string &prefix) const;

      /**
       * Saves the context to a file
       * @param   filename    name of the file to save to
       * @return  total bytes written
       * @throw exceptions::MemoryException  not enough buffer to encode
       **/
      int64_t save_context (const std::string & filename) const;

      /**
       * Saves the context to a file
       * @param   settings    the settings to save
       * @return              -1 if file open failed<br />
       *                      -2 if file write failed<br />
       *                      >0 if successful (number of bytes written)
       * @throw exceptions::MemoryException  not enough buffer to encode
       **/
      int64_t save_context (CheckpointSettings & settings) const;

      /**
      * Saves the context to a file as JSON
      * @param   filename    name of the file to save to
      * @return  total bytes written
      **/
      int64_t save_as_json (const std::string & filename) const;

      /**
       * Saves the context to a file as JSON
       * @param   settings    the settings to save
       * @return  total bytes written
       **/
      int64_t save_as_json (const CheckpointSettings & settings) const;

      /**
      * Saves the context to a file as karl assignments, rather than binary
      * @param   filename    name of the file to save to
      * @return  total bytes written
      **/
      int64_t save_as_karl (const std::string & filename) const;

      /**
       * Saves the context to a file as karl assignments, rather than binary
       * @param   settings    the settings to save
       * @return  total bytes written
       **/
      int64_t save_as_karl (const CheckpointSettings & settings) const;

      /**
       * Saves a checkpoint of a list of changes to a file
       * @param   filename    name of the file to open
       * @param   reset_modifieds  if true, resets the modified list to empty.
       * @return  total bytes written
       * @throw exceptions::MemoryException  not enough buffer to encode
       **/

      int64_t save_checkpoint (const std::string & filename,
        bool reset_modifieds = true);

      /**
       * Saves a checkpoint of a list of changes to a file
       * @param   settings    checkpoint settings to load
       * @return              -1 if file open failed<br />
       *                      -2 if file write failed<br />
       *                      >0 if successful (number of bytes written)
       * @throw exceptions::MemoryException  not enough buffer to encode
       **/

      int64_t save_checkpoint (
        CheckpointSettings & settings) const;

      /**
       * Loads the context from a file
       * @param   filename    name of the file to open
       * @param   use_id      if true, sets the unique identifier to the
       *                      one found in the saved context. If false,
       *                      keeps the default identifier.
       * @param   settings    settings to use when applying updates to context
       * @return  total bytes read
       * @throw exceptions::MemoryException  not enough buffer to encode
       **/
      int64_t load_context (const std::string & filename,
        bool use_id = true,
        const KnowledgeUpdateSettings & settings =
              KnowledgeUpdateSettings (true, true, true, false));

      /**
      * Loads the context from a file
      * @param   filename    name of the file to open
      * @param   meta        a file header that will contain metadata such
      *                      as originator, timestamp, lamport clock, etc.
      * @param   use_id      if true, sets the unique identifier to the
      *                      one found in the saved context. If false,
      *                      keeps the default identifier.
      * @param   settings    settings for applying the update
      * @return              -1 if file open failed<br />
      *                      -2 if file read failed<br />
      *                      >0 if successful (number of bytes written)
       * @throw exceptions::MemoryException  not enough buffer to encode
      **/
      int64_t load_context (const std::string & filename,
        FileHeader & meta,
        bool use_id = true,
        const KnowledgeUpdateSettings & settings =
        KnowledgeUpdateSettings (true, true, true, false));

      /**
      * Loads the context from a file
      * @param   checkpoint_settings  checkpoint settings to load
      * @param   update_settings      settings for applying the updates
      * @return              -1 if file open failed<br />
      *                      -2 if file read failed<br />
      *                      >0 if successful (number of bytes written)
       * @throw exceptions::MemoryException  not enough buffer to encode
      **/
      int64_t load_context (CheckpointSettings & checkpoint_settings,
        const KnowledgeUpdateSettings & update_settings =
        KnowledgeUpdateSettings (true, true, true, false));

      /**
      * Adds a list of VariableReferences to the current modified list.
      * @param  modifieds  a list of variables to add to modified list
      **/
      void add_modifieds (const VariableReferences & modifieds) const;

      /**
      * Saves the list of modified records to use later for resending. This
      * does not clear the modified list. This feature is useful if you
      * want to remember what has been modified and then resend later, e.g.,
      * if you believe packets may be dropped and want to resend information.
      * Use this function in conjunction with @see add_modifieds to remodify
      * @return a vector of VariableReferences to the current modified list
      **/
      VariableReferences save_modifieds (void) const;

      /**
       * Sends all modified variables through the attached transports.
       * @param   prefix      for logging purposes, the descriptor prefix for
       *                      calling method
       * @param   settings    settings for sending modifications
       * @return  number of transports the modifications were sent to
       **/
      int send_modifieds (
        const std::string & prefix = "KnowledgeBase::send_modifieds",
        const EvalSettings & settings =
          EvalSettings ());

      /**
       * Clear all modifications to the knowledge base. This action may
       * be useful if you are wanting to keep local changes but not
       * inform other agents (possibly due to a need to further process
       * and verify the information). The knowledge stays in its current
       * form (i.e., this does not roll back state or anything like that)
       */
      void clear_modifieds (void);


      /**
       * Wait for a change to happen to the context (e.g., from transports)
       **/
      void wait_for_change (void);

      /**
       * Change the knowledge base to become a facade for another context.
       * It is extremely important that the context stays within scope for
       * the duration of the life of this Knowledge Base. Otherwise, the
       * Knowledge Base will eventually point to invalid memory
       * @param  target   the target context to manipulate
       **/
      void facade_for (ThreadSafeContext & target);

    private:

      /// Pointer to actual implementation, i.e., the "bridge", which is
      /// reference counted to automate memory management.
      std::shared_ptr <KnowledgeBaseImpl> impl_;

      /// A knowledge base can also be a facade for another knowledge base
      ThreadSafeContext * context_;
    };

  }
}

// include the inline functions
#include "KnowledgeBase.inl"

#endif   // _MADARA_KNOWLEDGE_BASE_H
