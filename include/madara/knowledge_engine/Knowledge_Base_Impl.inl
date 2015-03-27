#ifndef _MADARA_KNOWLEDGE_BASE_IMPL_INL_
#define _MADARA_KNOWLEDGE_BASE_IMPL_INL_

#include "madara/utility/Utility.h"
#include "madara/knowledge_engine/Knowledge_Base_Impl.h"
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/expression_tree/Interpreter.h"
#include "madara/expression_tree/Expression_Tree.h"
#include "madara/utility/Log_Macros.h"

/**
 * @file Knowledge_Base_Impl.inl
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the inline functions for Knowledge_Base_Impl class
 */

inline Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base_Impl::get (
  const std::string & t_key,
  const Knowledge_Reference_Settings & settings)
{
  Madara::Knowledge_Record result (map_.get (t_key, settings));
  
  // if the result is ref counted, then do a deep copy to prevent cache issues
  if (result.is_ref_counted ())
  {
    result.deep_copy (result);
  }

  return result;
}

inline Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base_Impl::get (
  const Variable_Reference & variable,
  const Knowledge_Reference_Settings & settings)
{
  Madara::Knowledge_Record result (map_.get (variable, settings));
  
  // if the result is ref counted, then do a deep copy to prevent cache issues
  if (result.is_ref_counted ())
  {
    result.deep_copy (result);
  }

  return result;
}

inline Madara::Knowledge_Engine::Variable_Reference
Madara::Knowledge_Engine::Knowledge_Base_Impl::get_ref (
  const std::string & t_key,
  const Knowledge_Reference_Settings & settings)
{
  return map_.get_ref (t_key, settings);
}

inline Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base_Impl::retrieve_index (
  const std::string & t_key,
  size_t index,
  const Knowledge_Reference_Settings & settings)
{
  return map_.retrieve_index (t_key, index, settings);
}

inline Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base_Impl::retrieve_index (
  const Variable_Reference & variable,
  size_t index,
  const Knowledge_Reference_Settings & settings)
{
  return map_.retrieve_index (variable, index, settings);
}

inline bool
Madara::Knowledge_Engine::Knowledge_Base_Impl::exists (
  const std::string & key,
  const Knowledge_Reference_Settings & settings) const
{
  return map_.exists (key, settings);
}

inline bool
Madara::Knowledge_Engine::Knowledge_Base_Impl::exists (
  const Variable_Reference & variable,
  const Knowledge_Reference_Settings & settings) const
{
  return map_.exists (variable, settings);
}

inline std::string
Madara::Knowledge_Engine::Knowledge_Base_Impl::expand_statement (
  const std::string & statement) const
{
  return map_.expand_statement (statement);
}

/// Read a policy into the knowledge base
inline int
Madara::Knowledge_Engine::Knowledge_Base_Impl::read_policy (
  const std::string & knowledge_key, const std::string & filename)
{
  return files_.read_policy (knowledge_key, filename);
}

/// Write file from the knowledge base to a specified file
inline ssize_t
Madara::Knowledge_Engine::Knowledge_Base_Impl::write_file (
  const std::string & knowledge_key, const std::string & filename)
{
  return map_.get_record (knowledge_key)->to_file (filename);
}

/// Set quality of writing to a variable
inline void 
Madara::Knowledge_Engine::Knowledge_Base_Impl::set_quality (
  const std::string & t_key, uint32_t quality,
  const Knowledge_Reference_Settings & settings)
{
  map_.set_write_quality (t_key, quality, settings);
}

#ifdef _USE_CID_

inline void
Madara::Knowledge_Engine::Knowledge_Base_Impl::print_all_redeployment_results (
  std::ostream & output)
{
  settings_.print_all_results (output);
}

inline void
Madara::Knowledge_Engine::Knowledge_Base_Impl::run_all (void)
{
  settings_.run_all ();
}

inline void
Madara::Knowledge_Engine::Knowledge_Base_Impl::print_my_latencies (
  std::ostream & output)
{
  settings_.print_my_latencies (output);
}

inline void
Madara::Knowledge_Engine::Knowledge_Base_Impl::print_all_latencies (
  std::ostream & output)
{
  settings_.print_all_latencies (output);
}

inline void
Madara::Knowledge_Engine::Knowledge_Base_Impl::print_all_summations (
  std::ostream & output)
{
  settings_.print_all_summations (output);
}

inline long
Madara::Knowledge_Engine::Knowledge_Base_Impl::start_latency (void)
{
  if (transport_)
    return transport_->start_latency ();
  else
    return -1;
}

inline long
Madara::Knowledge_Engine::Knowledge_Base_Impl::vote (void)
{
  if (transport_)
    return transport_->vote ();
  else
    return -1;
}

#endif // _USE_CID_

inline Madara::Transport::Settings &
Madara::Knowledge_Engine::Knowledge_Base_Impl::transport_settings (void)
{
  return settings_;
}

inline void
Madara::Knowledge_Engine::Knowledge_Base_Impl::print (
  unsigned int level) const
{
  MADARA_DEBUG ((int)level, (LM_INFO, 
    "\nKnowledge in Knowledge Base:\n"));
  map_.print (level);
}

inline void
Madara::Knowledge_Engine::Knowledge_Base_Impl::to_string (
  std::string & target,
  const std::string & array_delimiter,
  const std::string & record_delimiter,
  const std::string & key_val_delimiter) const
{
  map_.to_string (target,
    array_delimiter, record_delimiter, key_val_delimiter);
}

inline void
Madara::Knowledge_Engine::Knowledge_Base_Impl::print (
  const std::string & statement, unsigned int level) const
{
  map_.print (statement, level);
}

inline void
Madara::Knowledge_Engine::Knowledge_Base_Impl::clear (void)
{
  map_.clear ();
}

inline void
Madara::Knowledge_Engine::Knowledge_Base_Impl::clear_map (void)
{
  map_.clear ();
}

/// lock the underlying knowledge base against any updates
/// until we release
inline void Madara::Knowledge_Engine::Knowledge_Base_Impl::acquire (void)
{
  map_.lock ();
}

/// release the lock on the underlying knowledge base
inline void Madara::Knowledge_Engine::Knowledge_Base_Impl::release (void)
{
  map_.unlock ();
}

#ifndef _MADARA_NO_KARL_

// Defines a function
inline
void Madara::Knowledge_Engine::Knowledge_Base_Impl::define_function (
  const std::string & name, Knowledge_Record (*func) (Function_Arguments &, Variables &))
{
  map_.define_function (name, func);
}

// Defines a function
inline
void Madara::Knowledge_Engine::Knowledge_Base_Impl::define_function (
  const std::string & name,
    Knowledge_Record (*func) (const char *, Function_Arguments &, Variables &))
{
  map_.define_function (name, func);
}

#ifdef _MADARA_JAVA_
inline
void
Madara::Knowledge_Engine::Knowledge_Base_Impl::define_function (
  const std::string & name, jobject callable)
{
  map_.define_function (name, callable);
}
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_

inline
void
Madara::Knowledge_Engine::Knowledge_Base_Impl::define_function (
  const std::string & name, boost::python::object callable)
{
  map_.define_function (name, callable);
}

#endif  

/**
  * Defines a MADARA KaRL function      
  **/
inline
void
Madara::Knowledge_Engine::Knowledge_Base_Impl::define_function (const std::string & name,
  const std::string & expression)
{
  map_.define_function (name, expression);
}
     

inline
void
Madara::Knowledge_Engine::Knowledge_Base_Impl::define_function (
  const std::string & name,
  const Compiled_Expression & expression)
{
  map_.define_function (name, expression);
}
     
inline Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base_Impl::wait (
  const std::string & expression)
{
  Compiled_Expression compiled = compile (expression);
  Wait_Settings settings;
  return wait (compiled, settings);
}

inline Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base_Impl::evaluate (
  const std::string & expression)
{
  Compiled_Expression compiled = compile (expression);
  Eval_Settings settings;
  return evaluate (compiled, settings);
}

#endif // _MADARA_NO_KARL_

inline
size_t
Madara::Knowledge_Engine::Knowledge_Base_Impl::attach_transport (
  Madara::Transport::Base * transport)
{
  transports_.push_back (transport);
  return transports_.size ();
}
 
inline
size_t
Madara::Knowledge_Engine::Knowledge_Base_Impl::remove_transport (size_t index)
{
  if (index < transports_.size ())
    transports_.erase (transports_.begin () + index);

  return transports_.size ();
}

/**
  * Returns the Thread_Safe_Context associated with this Knowledge
  * Base. This is necessary for creating custom transports.
  *
  * @return             the context used by the knowledge base
  **/
inline 
Madara::Knowledge_Engine::Thread_Safe_Context & 
Madara::Knowledge_Engine::Knowledge_Base_Impl::get_context (void)
{
  return map_;
}


/**
  * Returns the unique host and ephemeral binding for this Knowlede Base
  * @return             host:port identifier for this knowledge base
  **/
inline
std::string 
Madara::Knowledge_Engine::Knowledge_Base_Impl::get_id (void)
{
  if (id_ == "")
  {
    id_ = setup_unique_hostport ();
  }

  return id_;
}

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
inline
size_t
Madara::Knowledge_Engine::Knowledge_Base_Impl::to_vector (
                              const std::string & subject,
                              unsigned int start,
                              unsigned int end,
                              std::vector <Knowledge_Record> & target)
{
  return map_.to_vector (subject, start, end, target);
}

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
inline
size_t
Madara::Knowledge_Engine::Knowledge_Base_Impl::to_map (
  const std::string & expression,
  std::map <std::string, Knowledge_Record> & target)
{
  return map_.to_map (expression, target);
}


inline int64_t
Madara::Knowledge_Engine::Knowledge_Base_Impl::save_context (
  const std::string & filename)
{
  return map_.save_context (filename, id_);
}

inline void
Madara::Knowledge_Engine::Knowledge_Base_Impl::clear_modifieds (
  void)
{
  map_.reset_modified ();
}

inline int64_t
Madara::Knowledge_Engine::Knowledge_Base_Impl::save_checkpoint (
  const std::string & filename,
  bool reset_modifieds)
{
  int64_t total_written = map_.save_checkpoint (filename, id_);

  if (reset_modifieds)
    map_.reset_modified ();

  return total_written;
}

inline int64_t
  Madara::Knowledge_Engine::Knowledge_Base_Impl::load_context (
  const std::string & filename,
  bool  use_id,
  const Knowledge_Update_Settings & settings)
{
  if (use_id)
    return map_.load_context (filename, id_, settings);
  else
  {
    std::string id;
    return map_.load_context (filename, id, settings);
  }
}

inline int
Madara::Knowledge_Engine::Knowledge_Base_Impl::send_modifieds (
  const std::string & prefix,
  const Eval_Settings & settings)
{
  int result = 0;

  if (transports_.size () > 0 && !settings.delay_sending_modifieds)
  {
    const Knowledge_Records & modified = map_.get_modified ();

    if (modified.size () > 0)
    {
      // if there is not an allowed send_list list
      if (settings.send_list.size () == 0)
      {
        // send across each transport
        for (unsigned int i = 0; i < transports_.size (); ++i, ++result)
          transports_[i]->send_data (modified);

        // reset the modified map
        map_.reset_modified ();
      }
      else
      {
        Knowledge_Records allowed_modifieds;
        // otherwise, we are only allowed to send a subset of modifieds
        for (Knowledge_Records::const_iterator i = modified.begin ();
             i != modified.end (); ++i)
        {
          if (settings.send_list.find (i->first) != settings.send_list.end ())
          {
            allowed_modifieds[i->first] = i->second;
          }
        }

        // if the subset was greater than zero, we send the subset
        if (allowed_modifieds.size () > 0)
        {
          // send across each transport
          for (unsigned int i = 0; i < transports_.size (); ++i, ++result)
            transports_[i]->send_data (allowed_modifieds);

          // reset modified list for the allowed modifications
          for (Knowledge_Records::const_iterator i = allowed_modifieds.begin ();
               i != allowed_modifieds.end (); ++i)
          {
            map_.reset_modified (i->first);
          }
        }
      }
     
      map_.inc_clock (settings);
 
      if (settings.signal_changes)
        map_.signal (false);
    }
    else
    {
      MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
          DLINFO "%s:" \
          " no modifications to send\n", prefix.c_str ()));
      result = -1;
    }
  }
  else
  {
    if (transports_.size () == 0)
    {
      MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
          DLINFO "%s:" \
          "  No transport configured.\n", prefix.c_str ()));

      result = -2;
    }
    else if (settings.delay_sending_modifieds)
    {
      MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
          DLINFO "%s:" \
          "  User requested to not send modifieds.\n", prefix.c_str ()));

      result = -3;
    }
  }

  return result;
}


inline void
Madara::Knowledge_Engine::Knowledge_Base_Impl::wait_for_change (void)
{
  map_.wait_for_change ();
}


#endif  // _MADARA_KNOWLEDGE_BASE_IMPL_INL_
