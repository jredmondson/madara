#ifndef _MADARA_KNOWLEDGE_BASE_IMPL_INL_
#define _MADARA_KNOWLEDGE_BASE_IMPL_INL_

#include "madara/utility/Utility.h"
#include "madara/knowledge/KnowledgeBaseImpl.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/expression/Interpreter.h"
#include "madara/expression/ExpressionTree.h"


/**
 * @file KnowledgeBaseImpl.inl
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the inline functions for KnowledgeBaseImpl class
 */

namespace madara { namespace knowledge {

inline KnowledgeBaseImpl::KnowledgeBaseImpl ()
  : settings_ ()
{
  //activate_transport ();
}

inline KnowledgeBaseImpl::KnowledgeBaseImpl (
  const std::string & host, int transport)
  : settings_ ()
{
  // override default settings for the arguments
  settings_.type = transport;

  id_ = setup_unique_hostport (host);
  activate_transport ();
}

inline KnowledgeBaseImpl::KnowledgeBaseImpl (
  const std::string & host, int transport,
  const std::string & knowledge_domain)
  : settings_ ()
{
  // override default settings for the arguments
  settings_.type = transport;
  settings_.write_domain = knowledge_domain;

  id_ = setup_unique_hostport (host);
  activate_transport ();
}

inline KnowledgeBaseImpl::KnowledgeBaseImpl (
  const std::string & host, const madara::transport::TransportSettings & config)
  : settings_ (config)
{
  id_ = setup_unique_hostport (host);
  if (!settings_.delay_launch)
    activate_transport ();
}

inline KnowledgeBaseImpl::~KnowledgeBaseImpl ()
{
  close_transport ();
}
inline KnowledgeRecord
KnowledgeBaseImpl::get (
  const std::string & t_key,
  const KnowledgeReferenceSettings & settings)
{
  return map_.get (t_key, settings);
}

inline KnowledgeRecord
KnowledgeBaseImpl::get (
  const VariableReference & variable,
  const KnowledgeReferenceSettings & settings)
{
  return map_.get (variable, settings);
}

inline VariableReference
KnowledgeBaseImpl::get_ref (
  const std::string & t_key,
  const KnowledgeReferenceSettings & settings)
{
  return map_.get_ref (t_key, settings);
}

inline int
KnowledgeBaseImpl::get_log_level (void)
{
  return map_.get_log_level ();
}

inline std::string
KnowledgeBaseImpl::debug_modifieds (void) const
{
  return map_.debug_modifieds ();
}

inline madara::logger::Logger &
KnowledgeBaseImpl::get_logger (void) const
{
  return map_.get_logger ();
}

inline void
KnowledgeBaseImpl::attach_logger (
  logger::Logger & logger) const
{
  map_.attach_logger (logger);
}

inline void
KnowledgeBaseImpl::set_log_level (int level)
{
  map_.set_log_level (level);
}

inline KnowledgeRecord
KnowledgeBaseImpl::retrieve_index (
  const std::string & t_key,
  size_t index,
  const KnowledgeReferenceSettings & settings)
{
  return map_.retrieve_index (t_key, index, settings);
}

inline KnowledgeRecord
KnowledgeBaseImpl::retrieve_index (
  const VariableReference & variable,
  size_t index,
  const KnowledgeReferenceSettings & settings)
{
  return map_.retrieve_index (variable, index, settings);
}

inline bool
KnowledgeBaseImpl::exists (
  const std::string & key,
  const KnowledgeReferenceSettings & settings) const
{
  return map_.exists (key, settings);
}

inline bool
KnowledgeBaseImpl::exists (
  const VariableReference & variable,
  const KnowledgeReferenceSettings & settings) const
{
  return map_.exists (variable, settings);
}

/**
 * Updates all global variables to current clock and then
 * sends them if a transport is available. This is useful
 * when trying to synchronize to late joiners (this process
 * will resend all global variables.
 **/
inline int
KnowledgeBaseImpl::apply_modified (
const EvalSettings & settings)
{
  // lock the context and apply modified flags and current clock to
  // all global variables
  MADARA_GUARD_TYPE guard (map_.mutex_);

  map_.apply_modified ();

  int ret = 0;

  send_modifieds ("KnowledgeBaseImpl:apply_modified", settings);

  return ret;
}

inline void
KnowledgeBaseImpl::mark_modified (
  const VariableReference & variable,
  const KnowledgeUpdateSettings & settings)
{
  map_.mark_modified (variable, settings);
}

inline void
KnowledgeBaseImpl::mark_modified (
  const std::string & name,
  const KnowledgeUpdateSettings & settings)
{
  map_.mark_modified (name, settings);
}

/// Read a file into the knowledge base
inline int
KnowledgeBaseImpl::read_file (
const std::string & key, const std::string & filename,
const EvalSettings & settings)
{
  if (key == "")
    return -1;

  int result = map_.read_file (key, filename, settings);

  send_modifieds ("KnowledgeBaseImpl:read_file", settings);

  return result;
}

/// Read a file into the knowledge base
inline int
KnowledgeBaseImpl::set_file (
const VariableReference & variable,
const unsigned char * value, size_t size,
const EvalSettings & settings)
{
  int result = map_.set_file (variable, value, size, settings);

  send_modifieds ("KnowledgeBaseImpl:set_file", settings);

  return result;
}

/// Read a file into the knowledge base
inline int
KnowledgeBaseImpl::set_jpeg (
const VariableReference & variable,
const unsigned char * value, size_t size,
const EvalSettings & settings)
{
  int result = map_.set_jpeg (variable, value, size, settings);

  send_modifieds ("KnowledgeBaseImpl:set_jpeg", settings);

  return result;
}

/// Read a file into the knowledge base
inline int
KnowledgeBaseImpl::read_file (
const VariableReference & variable,
const std::string & filename,
const EvalSettings & settings)
{
  int result = map_.read_file (variable, filename, settings);

  send_modifieds ("KnowledgeBaseImpl:read_file", settings);

  return result;
}

inline void
KnowledgeBaseImpl::activate_transport (void)
{
  MADARA_GUARD_TYPE guard (map_.mutex_);

  if (transports_.size () == 0)
  {
    attach_transport (id_, settings_);
  }
  else
  {
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " transport already activated. If you need" \
      " a new type, close transport first\n");
  }
}

inline void
KnowledgeBaseImpl::copy (
const KnowledgeBaseImpl & source,
const KnowledgeRequirements & reqs)
{
  map_.copy (source.map_, reqs);
}

inline void
KnowledgeBaseImpl::copy (
const KnowledgeBaseImpl & source,
const CopySet & copy_set,
bool clean_copy)
{
  map_.copy (source.map_, copy_set, clean_copy);
}

inline void
KnowledgeBaseImpl::lock (void)
{
  map_.lock ();
}

inline void
KnowledgeBaseImpl::unlock (void)
{
  map_.unlock ();
}


inline std::string
KnowledgeBaseImpl::expand_statement (
  const std::string & statement) const
{
  return map_.expand_statement (statement);
}

/// Write file from the knowledge base to a specified file
inline ssize_t
KnowledgeBaseImpl::write_file (
  const std::string & knowledge_key, const std::string & filename)
{
  return map_.get_record (knowledge_key)->to_file (filename);
}

/// Set quality of writing to a variable
inline void
KnowledgeBaseImpl::set_quality (
  const std::string & t_key, uint32_t quality,
  const KnowledgeReferenceSettings & settings)
{
  map_.set_write_quality (t_key, quality, settings);
}

inline madara::transport::TransportSettings &
KnowledgeBaseImpl::transport_settings (void)
{
  return settings_;
}

inline void
KnowledgeBaseImpl::print (
  unsigned int level) const
{
  map_.print (
    "\nKnowledge in Knowledge Base:\n", logger::LOG_ALWAYS);

  map_.print (level);
}

inline void
KnowledgeBaseImpl::to_string (
  std::string & target,
  const std::string & array_delimiter,
  const std::string & record_delimiter,
  const std::string & key_val_delimiter) const
{
  map_.to_string (target,
    array_delimiter, record_delimiter, key_val_delimiter);
}

inline void
KnowledgeBaseImpl::print (
  const std::string & statement, unsigned int level) const
{
  map_.print (statement, level);
}

inline bool
KnowledgeBaseImpl::clear (const std::string & key,
  const KnowledgeReferenceSettings & settings)
{
  return map_.clear (key, settings);
}

inline void
KnowledgeBaseImpl::clear (bool erase)
{
  map_.clear (erase);
}

inline void
KnowledgeBaseImpl::clear_map (void)
{
  map_.clear ();
}

/// lock the underlying knowledge base against any updates
/// until we release
inline void KnowledgeBaseImpl::acquire (void)
{
  map_.lock ();
}

/// release the lock on the underlying knowledge base
inline void KnowledgeBaseImpl::release (void)
{
  map_.unlock ();
}

#ifndef _MADARA_NO_KARL_

// Defines a function
inline void
KnowledgeBaseImpl::define_function (
  const std::string & name, KnowledgeRecord (*func) (FunctionArguments &, Variables &))
{
  map_.define_function (name, func);
}

// Defines a function
inline void
KnowledgeBaseImpl::define_function (
  const std::string & name,
    KnowledgeRecord (*func) (const char *, FunctionArguments &, Variables &))
{
  map_.define_function (name, func);
}

#ifdef _MADARA_JAVA_
inline void
KnowledgeBaseImpl::define_function (
  const std::string & name, jobject callable)
{
  map_.define_function (name, callable);
}
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_

inline void
KnowledgeBaseImpl::define_function (
  const std::string & name, boost::python::object callable)
{
  map_.define_function (name, callable);
}

#endif

/**
  * Defines a MADARA KaRL function
  **/
inline void
KnowledgeBaseImpl::define_function (const std::string & name,
  const std::string & expression)
{
  map_.define_function (name, expression);
}


inline void
KnowledgeBaseImpl::define_function (
  const std::string & name,
  const CompiledExpression & expression)
{
  map_.define_function (name, expression);
}

inline KnowledgeRecord
KnowledgeBaseImpl::wait (
  const std::string & expression)
{
  CompiledExpression compiled = compile (expression);
  WaitSettings settings;
  return wait (compiled, settings);
}

inline KnowledgeRecord
KnowledgeBaseImpl::evaluate (
  const std::string & expression)
{
  CompiledExpression compiled = compile (expression);
  EvalSettings settings;
  return evaluate (compiled, settings);
}

#endif // _MADARA_NO_KARL_

inline size_t
KnowledgeBaseImpl::attach_transport (
  transport::Base * transport)
{
  MADARA_GUARD_TYPE guard (map_.mutex_);

  transports_.emplace_back (transport);
  return transports_.size ();
}

inline size_t
KnowledgeBaseImpl::get_num_transports (void)
{
  MADARA_GUARD_TYPE guard (map_.mutex_);

  return transports_.size ();
}

inline size_t
KnowledgeBaseImpl::remove_transport (size_t index)
{
  std::unique_ptr<transport::Base> transport;
  size_t size = 0;
  {
    MADARA_GUARD_TYPE guard (map_.mutex_);
    size = transports_.size ();
    if (index < size) {
      using std::swap;
      swap (transport, transports_[index]);
      transports_.erase (transports_.begin() + index);
      size = transports_.size ();
    }
  }

  if (transport) {
    transport->close ();
  }

  return size;
}

/**
  * Returns the ThreadSafeContext associated with this Knowledge
  * Base. This is necessary for creating custom transports.
  *
  * @return             the context used by the knowledge base
  **/
inline ThreadSafeContext &
KnowledgeBaseImpl::get_context (void)
{
  return map_;
}


/**
  * Returns the unique host and ephemeral binding for this Knowlede Base
  * @return             host:port identifier for this knowledge base
  **/
inline std::string
KnowledgeBaseImpl::get_id (void)
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
inline size_t
KnowledgeBaseImpl::to_vector (
                              const std::string & subject,
                              unsigned int start,
                              unsigned int end,
                              std::vector <KnowledgeRecord> & target)
{
  return map_.to_vector (subject, start, end, target);
}

inline void
KnowledgeBaseImpl::get_matches (
      const std::string & prefix,
      const std::string & suffix,
      VariableReferences & matches)
{
  map_.get_matches (prefix, suffix, matches);
}

inline size_t
KnowledgeBaseImpl::to_map (
  const std::string & expression,
  std::map <std::string, KnowledgeRecord> & target)
{
  return map_.to_map (expression, target);
}

inline size_t
KnowledgeBaseImpl::to_map (
  const std::string & prefix,
  const std::string & delimiter,
  const std::string & suffix,
  std::vector <std::string> & next_keys,
  std::map <std::string, KnowledgeRecord> & result,
  bool just_keys)
{
  return map_.to_map (
    prefix, delimiter, suffix, next_keys, result, just_keys);
}

inline KnowledgeMap
KnowledgeBaseImpl::to_map (
  const std::string & prefix) const
{
  return map_.to_map (prefix);
}

inline KnowledgeMap
KnowledgeBaseImpl::to_map_stripped (
  const std::string & prefix) const
{
  return map_.to_map_stripped (prefix);
}


inline int64_t
KnowledgeBaseImpl::save_context (
  const std::string & filename) const
{
  return map_.save_context (filename, id_);
}

inline int64_t
KnowledgeBaseImpl::save_context (
  CheckpointSettings & settings) const
{
  settings.originator = id_;
  return map_.save_context (settings);
}

inline int64_t
KnowledgeBaseImpl::save_as_json (
const std::string & filename) const
{
  return map_.save_as_json (filename);
}

inline int64_t
KnowledgeBaseImpl::save_as_json (
const CheckpointSettings & settings) const
{
  return map_.save_as_json (settings);
}

inline int64_t
KnowledgeBaseImpl::save_as_karl (
const std::string & filename) const
{
  return map_.save_as_karl (filename);
}

inline int64_t
KnowledgeBaseImpl::save_as_karl (
const CheckpointSettings & settings) const
{
  return map_.save_as_karl (settings);
}

inline void
KnowledgeBaseImpl::clear_modifieds (
  void)
{
  map_.reset_modified ();
}

inline void
madara::knowledge::KnowledgeBaseImpl::reset_checkpoint (
  void) const
{
  map_.reset_checkpoint ();
}

inline int64_t
KnowledgeBaseImpl::save_checkpoint (
  const std::string & filename,
  bool reset_modifieds)
{
  int64_t total_written = map_.save_checkpoint (filename, id_);

  if (reset_modifieds)
    map_.reset_checkpoint ();

  return total_written;
}

inline int64_t
KnowledgeBaseImpl::save_checkpoint (
  CheckpointSettings & settings) const
{
  settings.originator = id_;
  return map_.save_checkpoint (settings);
}

inline int64_t
  KnowledgeBaseImpl::load_context (
  const std::string & filename,
  bool  use_id,
  const KnowledgeUpdateSettings & settings)
{
  int64_t result = 0;

  if (use_id)
    result = map_.load_context (filename, id_, settings);
  else
  {
    std::string id;
    result = map_.load_context (filename, id, settings);
  }

  return result;
}

inline int64_t
KnowledgeBaseImpl::load_context (
const std::string & filename,
FileHeader & meta,
bool  use_id,
const KnowledgeUpdateSettings & settings)
{
  int64_t result = 0;

  if (use_id)
  {
    result = map_.load_context (filename, meta, settings);
    id_ = meta.originator;
  }
  else
  {
    result = map_.load_context (filename, meta, settings);
  }

  return result;
}

inline int64_t
KnowledgeBaseImpl::load_context (
CheckpointSettings & checkpoint_settings,
const KnowledgeUpdateSettings & update_settings)
{
  int64_t result = 0;

  if (checkpoint_settings.originator == "")
  {
    result = map_.load_context (checkpoint_settings, update_settings);

    if (checkpoint_settings.originator != "")
      id_ = checkpoint_settings.originator;
  }
  else
  {
    result = map_.load_context (checkpoint_settings, update_settings);
  }

  return result;
}

inline void
KnowledgeBaseImpl::add_modifieds (
  const VariableReferences & modifieds) const
{
  map_.add_modifieds (modifieds);
}

inline VariableReferences
KnowledgeBaseImpl::save_modifieds (void) const
{
  return map_.save_modifieds ();
}

#ifndef _MADARA_NO_KARL_

inline void
KnowledgeBaseImpl::wait_for_change (void)
{
  map_.wait_for_change ();
}

inline KnowledgeRecord
KnowledgeBaseImpl::evaluate (
const std::string & expression,
const EvalSettings & settings)
{
  CompiledExpression compiled = compile (expression);
  return evaluate (compiled, settings);
}

#endif // _MADARA_NO_KARL_

} }


#endif  // _MADARA_KNOWLEDGE_BASE_IMPL_INL_
