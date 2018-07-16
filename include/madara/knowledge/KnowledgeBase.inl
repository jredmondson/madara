#ifndef _MADARA_KNOWLEDGE_BASE_INL_
#define _MADARA_KNOWLEDGE_BASE_INL_

#include <fstream>

namespace madara { namespace knowledge {

/**
 * @file KnowledgeBase.inl
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the inline functions for the KnowledgeBase class
 */

inline KnowledgeBase::KnowledgeBase ()
: impl_ (new KnowledgeBaseImpl ()), context_ (0)
{
}

inline KnowledgeBase::KnowledgeBase (
  const std::string & host, int transport)
: impl_ (new KnowledgeBaseImpl (host, transport)), context_ (0)
{
}

inline KnowledgeBase::KnowledgeBase (
  const std::string & host, int transport,
  const std::string & knowledge_domain)
: impl_ (new KnowledgeBaseImpl (host, transport, knowledge_domain)),
  context_ (0)
{
}

inline KnowledgeBase::KnowledgeBase (
  const std::string & host, const madara::transport::TransportSettings & settings)
: impl_ (new KnowledgeBaseImpl (host, settings)), context_ (0)
{
}

template<typename T,
  typename std::enable_if<std::is_integral<T>::value,
  void*>::type>
inline int
KnowledgeBase::set (
  const std::string & key,
  T value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (key, (KnowledgeRecord::Integer)value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, (KnowledgeRecord::Integer)value, settings);
  }

  return result;
}

template<typename T,
  typename std::enable_if<std::is_integral<T>::value,
  void*>::type>
inline int
KnowledgeBase::set (
  const VariableReference & variable,
  T value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (variable, (KnowledgeRecord::Integer)value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, (KnowledgeRecord::Integer)value, settings);
  }

  return result;
}

template<typename T,
  typename std::enable_if<std::is_floating_point<T>::value,
  void*>::type>
inline int
KnowledgeBase::set (
  const std::string & key,
  T value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (key, (double)value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, (double)value, settings);
  }

  return result;
}

template<typename T,
  typename std::enable_if<std::is_floating_point<T>::value,
  void*>::type>
inline int
KnowledgeBase::set (
  const VariableReference & variable,
  T value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (variable, (double)value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, (double)value, settings);
  }

  return result;
}

template<typename T>
inline int
KnowledgeBase::set_any (const std::string & key,
  T && value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set_any (key, std::forward<T>(value), settings);
  }
  else if (context_)
  {
    result = context_->set_any (key, std::forward<T>(value), settings);
  }

  return result;
}

template<typename T>
inline int
KnowledgeBase::set_any (const VariableReference & variable,
  T && value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set_any (variable, std::forward<T>(value), settings);
  }
  else if (context_)
  {
    result = context_->set_any (variable, std::forward<T>(value), settings);
  }

  return result;
}

template<typename... Args>
inline int
KnowledgeBase::emplace_any (const std::string & key,
  const EvalSettings & settings,
  Args&&... args)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->emplace_any (key, settings,
        std::forward<Args>(args)...);
  }
  else if (context_)
  {
    result = context_->emplace_any (key, settings,
        std::forward<Args>(args)...);
  }

  return result;
}

template<typename... Args>
inline int
KnowledgeBase::emplace_any (const VariableReference & variable,
  const EvalSettings & settings,
  Args&&... args)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->emplace_any (variable, settings,
        std::forward<Args>(args)...);
  }
  else if (context_)
  {
    result = context_->emplace_any (variable, settings,
        std::forward<Args>(args)...);
  }

  return result;
}


inline void
KnowledgeBase::use (
  ThreadSafeContext & original)
{
  impl_ = 0;
  context_ = &original;
}

inline void
KnowledgeBase::lock (void)
{
  if (impl_.get ())
  {
    impl_->lock ();
  }
  else if (context_)
  {
    context_->lock ();
  }
}

inline void
KnowledgeBase::unlock (void)
{
  if (impl_.get ())
  {
    impl_->unlock ();
  }
  else if (context_)
  {
    context_->unlock ();
  }
}

inline int
KnowledgeBase::get_log_level (void)
{
  int result (0);

  if (impl_.get ())
  {
    result = impl_->get_log_level ();
  }
  else if (context_)
  {
    result = context_->get_log_level ();
  }

  return result;
}

inline madara::logger::Logger &
KnowledgeBase::get_logger (void) const
{
  if (impl_.get ())
  {
    return impl_->get_logger ();
  }
  else
  {
    return context_->get_logger ();
  }
}

inline void
KnowledgeBase::attach_logger (
  logger::Logger & logger) const
{
  if (impl_.get ())
  {
    impl_->attach_logger (logger);
  }
  else
  {
    context_->attach_logger (logger);
  }
}

inline void
KnowledgeBase::set_log_level (int level)
{
  if (impl_.get ())
  {
    impl_->set_log_level (level);
  }
  else if (context_)
  {
    context_->set_log_level (level);
  }
}


inline void
KnowledgeBase::copy (
  const KnowledgeBase & source,
  const KnowledgeRequirements & reqs,
  const EvalSettings & settings)
{
  if (impl_.get () && source.impl_.get () != 0)
  {
    impl_->copy (*source.impl_.get (), reqs, settings);
  }
  else if (context_ && source.impl_.get () != 0)
  {
    KnowledgeBaseImpl * source_impl =
      (KnowledgeBaseImpl *)source.impl_.get ();
    ThreadSafeContext * source_context = &(source_impl->get_context ());

    context_->copy (*source_context, reqs, settings);
  }
}

inline void
KnowledgeBase::copy (
  const KnowledgeBase & source,
  const CopySet & copy_set,
  bool clean_copy,
  const EvalSettings & settings)
{
  if (impl_.get () && source.impl_.get () != 0)
  {
    impl_->copy (*source.impl_.get (), copy_set, clean_copy, settings);
  }
  else if (context_ && source.impl_.get () != 0)
  {
    KnowledgeBaseImpl * source_impl =
      (KnowledgeBaseImpl *)source.impl_.get ();
    ThreadSafeContext * source_context = &(source_impl->get_context ());

    context_->copy (*source_context, copy_set, clean_copy, settings);
  }
}

/// Applies current time and modified to all global variables and tries
/// to send them.
inline int
KnowledgeBase::apply_modified (
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->apply_modified (settings);
  }
  else if (context_)
  {
    context_->apply_modified ();
  }

  return result;
}

inline void
KnowledgeBase::close_transport (void)
{
  if (impl_.get ())
  {
    impl_->close_transport ();
  }
}

inline KnowledgeRecord
KnowledgeBase::get (const std::string & key,
             const KnowledgeReferenceSettings & settings)
{
  KnowledgeRecord result;

  if (impl_.get ())
  {
    result = impl_->get (key, settings);
  }
  else if (context_)
  {
    result = context_->get (key, settings);
  }

  return result;
}

inline VariableReference
KnowledgeBase::get_ref (const std::string & key,
             const KnowledgeReferenceSettings & settings)
{
  VariableReference var;

  if (impl_.get ())
  {
    var = impl_->get_ref (key, settings);
  }
  else if (context_)
  {
    var = context_->get_ref (key, settings);
  }

  return var;
}

inline KnowledgeRecord
KnowledgeBase::get (
  const VariableReference & variable,
  const KnowledgeReferenceSettings & settings)
{
  KnowledgeRecord result;

  if (impl_.get ())
  {
    result = impl_->get (variable, settings);
  }
  else if (context_)
  {
    result = context_->get (variable, settings);
  }

  return result;
}

inline KnowledgeRecord
KnowledgeBase::retrieve_index (
  const std::string & key, size_t index,
  const KnowledgeReferenceSettings & settings)
{
  KnowledgeRecord result;

  if (impl_.get ())
  {
    result = impl_->retrieve_index (key, index, settings);
  }
  else if (context_)
  {
    result = context_->retrieve_index (key, index, settings);
  }

  return result;
}

inline KnowledgeRecord
KnowledgeBase::retrieve_index (
  const VariableReference & variable, size_t index,
  const KnowledgeReferenceSettings & settings)
{
  KnowledgeRecord result;

  if (impl_.get ())
  {
    result = impl_->retrieve_index (variable, index, settings);
  }
  else if (context_)
  {
    result = context_->retrieve_index (variable, index, settings);
  }

  return result;
}


inline int
KnowledgeBase::read_file (
  const std::string & knowledge_key,
  const std::string & filename,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->read_file (knowledge_key, filename, settings);
  }
  else if (context_)
  {
    result = context_->read_file (knowledge_key, filename, settings);
  }

  return result;
}

inline int
KnowledgeBase::read_file (
  const VariableReference & variable,
  const std::string & filename,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->read_file (variable, filename, settings);
  }
  else if (context_)
  {
    result = context_->read_file (variable, filename, settings);
  }

  return result;
}

inline int
KnowledgeBase::set_file (
  const std::string & key,
  const unsigned char * value, size_t size,
  const EvalSettings & settings)
{
  int result = -1;

  if (key != "")
  {
    if (impl_.get ())
    {
      result = impl_->set_file (
        impl_->get_ref (key, settings), value, size, settings);
    }
    else if (context_)
    {
      result = context_->set_file (
        context_->get_ref (key, settings), value, size, settings);
    }
  }

  return result;
}

inline int
KnowledgeBase::set_file (
  const VariableReference & variable,
  const unsigned char * value, size_t size,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set_file (variable, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set_file (variable, value, size, settings);
  }

  return result;
}

inline int
KnowledgeBase::set_jpeg (
  const std::string & key,
  const unsigned char * value, size_t size,
  const EvalSettings & settings)
{
  int result = -1;

  if (key != "")
  {
    if (impl_.get ())
    {
      result = impl_->set_jpeg (
        impl_->get_ref (key, settings), value, size, settings);
    }
    else if (context_)
    {
      result = context_->set_jpeg (
        impl_->get_ref (key, settings), value, size, settings);
    }
  }

  return result;
}

inline int
KnowledgeBase::set_jpeg (
  const VariableReference & variable,
  const unsigned char * value, size_t size,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set_jpeg (variable, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set_jpeg (variable, value, size, settings);
  }

  return result;
}

inline ssize_t
KnowledgeBase::write_file (const std::string & knowledge_key,
                     const std::string & filename)
{
  ssize_t result = 0;

  if (impl_.get ())
  {
    result = impl_->write_file (knowledge_key, filename);
  }

  return result;
}


inline std::string
KnowledgeBase::expand_statement (
  const std::string & statement)
{
  std::string result;

  if (impl_.get ())
  {
    result = impl_->expand_statement (statement);
  }
  else if (context_)
  {
    result = context_->expand_statement (statement);
  }

  return result;
}

inline void
KnowledgeBase::mark_modified (
  const VariableReference & variable,
  const KnowledgeUpdateSettings & /*settings*/)
{
  if (impl_.get ())
  {
    impl_->mark_modified (variable);
  }
  else if (context_)
  {
    context_->mark_modified (variable);
  }
}

inline void
KnowledgeBase::mark_modified (
  const std::string & name,
  const KnowledgeUpdateSettings & settings)
{
  if (impl_.get ())
  {
    impl_->mark_modified (name, settings);
  }
  else if (context_)
  {
    context_->mark_modified (name, settings);
  }
}

inline int
KnowledgeBase::set (
  const VariableReference & variable,
  KnowledgeRecord && value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (variable, std::move(value), settings);
  }
  else if (context_)
  {
    result = context_->set (variable, std::move(value), settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const std::string & key,
  const KnowledgeRecord & value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const std::string & key,
  KnowledgeRecord && value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (key, std::move(value), settings);
  }
  else if (context_)
  {
    result = context_->set (key, std::move(value), settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const VariableReference & variable,
  const KnowledgeRecord & value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}

template<typename T>
inline int
KnowledgeBase::set_index (
  const std::string & key,
  size_t index, T&& value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set_index (key, index, std::forward<T>(value), settings);
  }
  else if (context_)
  {
    result = context_->set_index (key, index, std::forward<T>(value), settings);
  }

  return result;
}

template<typename T>
inline int
KnowledgeBase::set_index (
  const VariableReference & variable,
  size_t index, T&& value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set_index (variable, index, std::forward<T>(value), settings);
  }
  else if (context_)
  {
    result = context_->set_index (variable, index, std::forward<T>(value), settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const std::string & key,
  const KnowledgeRecord::Integer * value,
  uint32_t size,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (key, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, size, settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const VariableReference & variable,
  const KnowledgeRecord::Integer * value,
  uint32_t size,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (variable, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, size, settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const std::string & key,
  const std::vector <KnowledgeRecord::Integer> & value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const VariableReference & variable,
  const std::vector <KnowledgeRecord::Integer> & value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const std::string & key,
  std::vector <KnowledgeRecord::Integer> && value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (key, std::move(value), settings);
  }
  else if (context_)
  {
    result = context_->set (key, std::move(value), settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const VariableReference & variable,
  std::vector <KnowledgeRecord::Integer> && value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (variable, std::move(value), settings);
  }
  else if (context_)
  {
    result = context_->set (variable, std::move(value), settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const std::string & key,
  const double * value,
  uint32_t size,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (key, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, size, settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const VariableReference & variable,
  const double * value,
  uint32_t size,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (variable, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, size, settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const std::string & key,
  const std::vector <double> & value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const std::string & key,
  std::vector <double> && value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (key, std::move(value), settings);
  }
  else if (context_)
  {
    result = context_->set (key, std::move(value), settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const VariableReference & variable,
  const std::vector <double> & value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const VariableReference & variable,
  std::vector <double> && value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (variable, std::move(value), settings);
  }
  else if (context_)
  {
    result = context_->set (variable, std::move(value), settings);
  }

  return result;
}

inline int
KnowledgeBase::set (const std::string & key,
  const std::string & value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const VariableReference & variable,
  const std::string & value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}

inline int
KnowledgeBase::set (const std::string & key,
  std::string && value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (key, std::move(value), settings);
  }
  else if (context_)
  {
    result = context_->set (key, std::move(value), settings);
  }

  return result;
}

inline int
KnowledgeBase::set (
  const VariableReference & variable,
  std::string && value,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->set (variable, std::move(value), settings);
  }
  else if (context_)
  {
    result = context_->set (variable, std::move(value), settings);
  }

  return result;
}


/// Set quality of writing to a variable
inline void
KnowledgeBase::set_quality (
  const std::string & key, uint32_t quality,
  const KnowledgeReferenceSettings & settings)
{
  if (impl_.get ())
  {
    impl_->set_quality (key, quality, settings);
  }
  else if (context_)
  {
    context_->set_quality (key, quality, true, settings);
  }
}


inline bool
KnowledgeBase::exists (
  const std::string & key,
  const KnowledgeReferenceSettings & settings) const
{
  bool result = false;

  if (impl_.get ())
  {
    result = impl_->exists (key, settings);
  }
  else if (context_)
  {
    result = context_->exists (key, settings);
  }

  return result;
}

inline bool
KnowledgeBase::exists (
  const VariableReference & variable,
  const KnowledgeReferenceSettings & settings) const
{
  bool result = false;

  if (impl_.get ())
  {
    result = impl_->exists (variable, settings);
  }
  else if (context_)
  {
    result = context_->exists (variable, settings);
  }

  return result;
}

inline madara::transport::TransportSettings &
KnowledgeBase::transport_settings (void)
{
  return impl_->transport_settings ();
}

inline void
KnowledgeBase::print (
  unsigned int level) const
{
  if (impl_.get ())
  {
    impl_->print (level);
  }
  else if (context_)
  {
    context_->print (level);
  }
}

inline void
KnowledgeBase::print_knowledge (
  unsigned int level) const
{
  if (impl_.get ())
  {
    impl_->print (level);
  }
  else if (context_)
  {
    context_->print (level);
  }
}

inline void
KnowledgeBase::to_string (
  std::string & target,
  const std::string & array_delimiter,
  const std::string & record_delimiter,
  const std::string & key_val_delimiter) const
{
  if (impl_.get ())
  {
    impl_->to_string (target,
      array_delimiter, record_delimiter, key_val_delimiter);
  }
  else if (context_)
  {
    context_->to_string (target,
      array_delimiter, record_delimiter, key_val_delimiter);
  }
}


inline void
KnowledgeBase::print (
  const std::string & statement, unsigned int level) const
{
  if (impl_.get ())
  {
    impl_->print (statement, level);
  }
  else if (context_)
  {
    context_->print (level);
  }
}

inline bool
KnowledgeBase::clear (const std::string & key,
  const KnowledgeReferenceSettings & settings)
{
  bool result (false);

  if (impl_.get ())
  {
    result = impl_->clear (key, settings);
  }
  else if (context_)
  {
    result = context_->clear (key, settings);
  }

  return result;
}

inline void
KnowledgeBase::clear (bool erase)
{
  if (impl_.get ())
  {
    impl_->clear (erase);
  }
  else if (context_)
  {
    context_->clear (erase);
  }
}

inline void
KnowledgeBase::clear_map (void)
{
  if (impl_.get ())
  {
    impl_->clear_map ();
  }
  else if (context_)
  {
    context_->clear ();
  }
}

inline void
KnowledgeBase::acquire (void)
{
  if (impl_.get ())
  {
    impl_->acquire ();
  }
  else if (context_)
  {
    context_->lock ();
  }
}

inline void
KnowledgeBase::release (void)
{
  if (impl_.get ())
  {
    impl_->release ();
  }
  else if (context_)
  {
    context_->unlock ();
  }
}

#ifndef _MADARA_NO_KARL_

inline CompiledExpression
KnowledgeBase::compile (
  const std::string & expression)
{
  CompiledExpression result;

  if (impl_.get ())
  {
    result = impl_->compile (expression);
  }

  return result;
}

// evaluate a knowledge expression and choose to send any modifications
inline KnowledgeRecord
KnowledgeBase::evaluate (
  const std::string & expression,
  const EvalSettings & settings)
{
  KnowledgeRecord result;

  if (impl_.get ())
  {
    result = impl_->evaluate (expression, settings);
  }
  else if (context_)
  {
    CompiledExpression ce = context_->compile (expression);
    result = context_->evaluate (ce, settings);
  }

  return result;
}

// evaluate a knowledge expression and choose to send any modifications
inline KnowledgeRecord
KnowledgeBase::evaluate (
  CompiledExpression & expression,
  const EvalSettings & settings)
{
  KnowledgeRecord result;

  if (impl_.get ())
  {
    result = impl_->evaluate (expression, settings);
  }
  else if (context_)
  {
    result = context_->evaluate (expression, settings);
  }

  return result;
}

// evaluate a knowledge expression and choose to send any modifications
inline KnowledgeRecord
KnowledgeBase::evaluate (
  expression::ComponentNode * root,
  const EvalSettings & settings)
{
  KnowledgeRecord result;

  if (impl_.get ())
  {
    result = impl_->evaluate (root, settings);
  }
  else if (context_)
  {
    result = context_->evaluate (root, settings);
  }

  return result;
}


// Defines a function
inline void
KnowledgeBase::define_function (
  const std::string & name,
    KnowledgeRecord (*func) (const char *, FunctionArguments &, Variables &))
{
  if (impl_.get ())
  {
    impl_->define_function (name, func);
  }
  else if (context_)
  {
    context_->define_function (name, func);
  }
}

// Defines a function
inline void
KnowledgeBase::define_function (
  const std::string & name,
    KnowledgeRecord (*func) (FunctionArguments &, Variables &))
{
  if (impl_.get ())
  {
    impl_->define_function (name, func);
  }
  else if (context_)
  {
    context_->define_function (name, func);
  }
}

#ifdef _MADARA_JAVA_
// Defines a function
inline void
KnowledgeBase::define_function (
  const std::string & name, jobject func)
{
  if (impl_.get ())
  {
    impl_->define_function (name, func);
  }
  else if (context_)
  {
    context_->define_function (name, func);
  }
}
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_

// Defines a function
inline void
KnowledgeBase::define_function (
  const std::string & name, boost::python::object callable)
{
  if (impl_.get ())
  {
    impl_->define_function (name, callable);
  }
  else if (context_)
  {
    context_->define_function (name, callable);
  }
}

#endif

/**
  * Defines a MADARA KaRL function
  * @param  name       name of the function
  * @param  expression KaRL function body
  **/
inline void
KnowledgeBase::define_function (const std::string & name,
  const std::string & expression)
{
  if (impl_.get ())
  {
    impl_->define_function (name, expression);
  }
  else if (context_)
  {
    context_->define_function (name, expression);
  }
}

/**
  * Defines a MADARA KaRL function
  * @param  name       name of the function
  * @param  expression KaRL function body
  **/
inline void
KnowledgeBase::define_function (const std::string & name,
  const CompiledExpression & expression)
{
  if (impl_.get ())
  {
    impl_->define_function (name, expression);
  }
  else if (context_)
  {
    context_->define_function (name, expression);
  }
}

inline KnowledgeRecord
KnowledgeBase::wait (
  const std::string & expression,
  const WaitSettings & settings)
{
  KnowledgeRecord result;

  if (context_)
  {
    CompiledExpression ce = context_->compile (expression);
    result = this->wait (ce, settings);
  }
  else if (impl_.get ())
  {
    result = impl_->wait (expression, settings);
  }

  return result;
}

#endif // _MADARA_NO_KARL_
inline void
KnowledgeBase::activate_transport (void)
{
  if (impl_.get ())
  {
    impl_->activate_transport ();
  }
}

inline size_t
KnowledgeBase::attach_transport (
  madara::transport::Base * transport)
{
  size_t result = 0;

  if (impl_.get ())
  {
    result = impl_->attach_transport (transport);
  }

  return result;
}

inline size_t
KnowledgeBase::get_num_transports (void)
{
  size_t result (0);

  if (impl_.get ())
  {
    result = impl_->get_num_transports ();
  }

  return result;
}

inline size_t
KnowledgeBase::attach_transport (const std::string & id,
        transport::TransportSettings & settings)
{
  size_t result = 0;

  if (impl_.get ())
  {
    result = impl_->attach_transport (id, settings);
  }

  return result;
}

inline size_t
  KnowledgeBase::remove_transport (
  size_t index)
{
  size_t result = 0;

  if (impl_.get ())
  {
    result = impl_->remove_transport (index);
  }

  return result;
}

inline ThreadSafeContext &
KnowledgeBase::get_context (void)
{
  ThreadSafeContext * result = 0;

  if (context_)
  {
    result = context_;
  }
  else if (impl_.get ())
  {
    result = &(impl_->get_context ());
  }

  return *result;
}

inline void
KnowledgeBase::clear_modifieds (void)
{
  if (context_)
  {
    context_->reset_modified ();
  }
  else if (impl_.get ())
  {
    impl_->clear_modifieds ();
  }
}

inline void
KnowledgeBase::add_modifieds (const VariableReferences & modifieds) const
{
  if (context_)
  {
    context_->add_modifieds (modifieds);
  }
  else if (impl_.get ())
  {
    impl_->add_modifieds (modifieds);
  }
}

inline VariableReferences
KnowledgeBase::save_modifieds (void) const
{
  VariableReferences default_result;

  if (context_)
  {
    return context_->save_modifieds ();
  }
  else if (impl_.get ())
  {
    return impl_->save_modifieds ();
  }

  return default_result;
}

inline int
KnowledgeBase::send_modifieds (
  const std::string & prefix,
  const EvalSettings & settings)
{
  int result = 0;

  if (impl_.get ())
  {
    result = impl_->send_modifieds (prefix, settings);
  }

  return result;
}

inline std::string
KnowledgeBase::debug_modifieds (void) const
{
  std::string result = "";

  if (context_)
  {
    result = context_->debug_modifieds ();
  }
  else if (impl_.get ())
  {
    result = impl_->debug_modifieds ();
  }

  return result;
}

/**
  * Returns the unique host and ephemeral binding for this Knowlede Base
  * @return             host:port identifier for this knowledge base
  **/
inline std::string
KnowledgeBase::get_id (void)
{
  std::string result;

  if (impl_.get ())
  {
    result = impl_->get_id ();
  }

  return result;
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
KnowledgeBase::to_vector (
                              const std::string & subject,
                              unsigned int start,
                              unsigned int end,
                              std::vector <KnowledgeRecord> & target)
{
  size_t result = 0;

  if (context_)
  {
    result = context_->to_vector (subject, start, end, target);
  }
  else if (impl_.get ())
  {
    result = impl_->to_vector (subject, start, end, target);
  }

  return result;
}

inline void
KnowledgeBase::get_matches ( const std::string & prefix,
  const std::string & suffix, VariableReferences & matches)
{
  if (context_)
  {
    context_->get_matches (prefix, suffix, matches);
  }
  else if (impl_.get ())
  {
    impl_->get_matches (prefix, suffix, matches);
  }
}

inline void KnowledgeBase::facade_for (
  ThreadSafeContext & context)
{
  context_ = &context;
  impl_ = 0;
}

inline size_t
KnowledgeBase::to_map (
  const std::string & expression,
  std::map <std::string, KnowledgeRecord> & target)
{
  size_t result = 0;

  if (context_)
  {
    result = context_->to_map (expression, target);
  }
  else if (impl_.get ())
  {
    result = impl_->to_map (expression, target);
  }

  return result;
}

inline size_t
KnowledgeBase::to_map (
  const std::string & prefix,
  const std::string & delimiter,
  const std::string & suffix,
  std::vector <std::string> & next_keys,
  std::map <std::string, KnowledgeRecord> & result,
  bool just_keys)
{
  size_t result_size = 0;

  if (context_)
  {
    result_size = context_->to_map (
      prefix, delimiter, suffix, next_keys, result, just_keys);
  }
  else if (impl_.get ())
  {
    result_size = impl_->to_map (
      prefix, delimiter, suffix, next_keys, result, just_keys);
  }

  return result_size;
}

inline KnowledgeMap
KnowledgeBase::to_map (
  const std::string & prefix) const
{
  if (context_)
  {
    return context_->to_map (prefix);
  }
  else if (impl_.get ())
  {
    return impl_->to_map (prefix);
  }

  return KnowledgeMap();
}

inline KnowledgeMap
KnowledgeBase::to_map_stripped (
  const std::string & prefix) const
{
  if (context_)
  {
    return context_->to_map_stripped (prefix);
  }
  else if (impl_.get ())
  {
    return impl_->to_map_stripped (prefix);
  }

  return KnowledgeMap();
}

inline int64_t
KnowledgeBase::save_context (
  const std::string & filename) const
{
  int64_t result = 0;

  if (context_)
  {
    result = context_->save_context (filename);
  }
  else if (impl_.get ())
  {
    result = impl_->save_context (filename);
  }

  return result;
}

inline int64_t
KnowledgeBase::save_context (
  CheckpointSettings & settings) const
{
  int64_t result = 0;

  if (context_)
  {
    result = context_->save_context (settings);
  }
  else if (impl_.get ())
  {
    result = impl_->save_context (settings);
  }

  return result;
}


inline int64_t
KnowledgeBase::save_as_json (
  const std::string & filename) const
{
  int64_t result = 0;

  if (context_)
  {
    result = context_->save_as_json (filename);
  }
  else if (impl_.get ())
  {
    result = impl_->save_as_json (filename);
  }

  return result;
}

inline int64_t
KnowledgeBase::save_as_json (
  const CheckpointSettings & settings) const
{
  int64_t result = 0;

  if (context_)
  {
    result = context_->save_as_json (settings);
  }
  else if (impl_.get ())
  {
    result = impl_->save_as_json (settings);
  }

  return result;
}

inline int64_t
KnowledgeBase::save_as_karl (
  const std::string & filename) const
{
  int64_t result = 0;

  if (context_)
  {
    result = context_->save_as_karl (filename);
  }
  else if (impl_.get ())
  {
    result = impl_->save_as_karl (filename);
  }

  return result;
}

inline int64_t
KnowledgeBase::save_as_karl (
  const CheckpointSettings & settings) const
{
  int64_t result = 0;

  if (context_)
  {
    result = context_->save_as_karl (settings);
  }
  else if (impl_.get ())
  {
    result = impl_->save_as_karl (settings);
  }

  return result;
}


inline int64_t
KnowledgeBase::save_checkpoint (
  const std::string & filename,
  bool reset_modifieds)
{
  int64_t result = 0;

  if (context_)
  {
    result = context_->save_checkpoint (filename);
  }
  else if (impl_.get ())
  {
    result = impl_->save_checkpoint (filename, reset_modifieds);
  }

  return result;
}

inline int64_t
KnowledgeBase::save_checkpoint (
  CheckpointSettings & settings) const
{
  int64_t result = 0;

  if (context_)
  {
    result = context_->save_checkpoint (settings);
  }
  else if (impl_.get ())
  {
    result = impl_->save_checkpoint (settings);
  }

  return result;
}

inline int64_t
KnowledgeBase::load_context (
  const std::string & filename,
  bool use_id,
  const KnowledgeUpdateSettings & settings)
{
  int64_t result = 0;

  if (context_)
  {
    std::string id;
    result = context_->load_context (filename, id, settings);
  }
  else if (impl_.get ())
  {
    result = impl_->load_context (filename, use_id, settings);
  }

  return result;
}

inline int64_t
KnowledgeBase::load_context (
const std::string & filename,
FileHeader & meta,
bool use_id,
const KnowledgeUpdateSettings & settings)
{
  int64_t result = 0;

  if (context_)
  {
    result = context_->load_context (filename, meta, settings);
  }
  else if (impl_.get ())
  {
    result = impl_->load_context (filename, meta, use_id, settings);
  }

  return result;
}

inline int64_t
KnowledgeBase::load_context (
CheckpointSettings & checkpoint_settings,
const KnowledgeUpdateSettings & update_settings)
{
  int64_t result = 0;

  if (context_)
  {
    result = context_->load_context (checkpoint_settings, update_settings);
  }
  else if (impl_.get ())
  {
    result = impl_->load_context (checkpoint_settings, update_settings);
  }

  return result;
}

inline void
KnowledgeBase::wait_for_change (void)
{
  if (context_)
  {
    context_->wait_for_change ();
  }
  else if (impl_.get ())
  {
    impl_->wait_for_change ();
  }
}

inline std::string
KnowledgeBase::setup_unique_hostport (
  const std::string & host)
{
  std::string result;

  if (impl_.get ())
  {
    result = impl_->setup_unique_hostport (host);
  }

  return result;
}

inline int
KnowledgeBase::modify (const EvalSettings & settings)
{
  return apply_modified (settings);
}

} }

#endif   // _MADARA_KNOWLEDGE_BASE_INL_
