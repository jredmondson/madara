
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/KnowledgeBaseImpl.h"
#include "madara/logger/Logger.h"


#include <sstream>
#include <iostream>

#include "ace/OS_NS_Thread.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_sys_socket.h"

madara::knowledge::KnowledgeBase::KnowledgeBase ()
: impl_ (new KnowledgeBaseImpl ()), context_ (0)
{
}

madara::knowledge::KnowledgeBase::KnowledgeBase (
  const std::string & host, int transport)
: impl_ (new KnowledgeBaseImpl (host, transport)), context_ (0)
{
}

madara::knowledge::KnowledgeBase::KnowledgeBase (
  const std::string & host, int transport, 
  const std::string & knowledge_domain)
: impl_ (new KnowledgeBaseImpl (host, transport, knowledge_domain)),
  context_ (0)
{
}

madara::knowledge::KnowledgeBase::KnowledgeBase (
  const std::string & host, const madara::transport::TransportSettings & settings)
: impl_ (new KnowledgeBaseImpl (host, settings)), context_ (0)
{
}

madara::knowledge::KnowledgeBase::KnowledgeBase (
  const KnowledgeBase & original)
: impl_ (original.impl_), context_ (original.context_)
{
}

madara::knowledge::KnowledgeBase::~KnowledgeBase ()
{
  // taken care of by Refcounter class
}

/// Assignment operator.
void 
madara::knowledge::KnowledgeBase::operator= (
  const KnowledgeBase &original)
{
  // Refcounter class takes care of the internal decrements and
  // increments.
  if (this != &original)
  {
    impl_ = original.impl_;
    context_ = original.context_;
  }
}

void
madara::knowledge::KnowledgeBase::use (
  ThreadSafeContext & original)
{
  impl_ = 0;
  context_ = &original;
}

void
madara::knowledge::KnowledgeBase::lock (void)
{
  if (impl_.get_ptr ())
  {
    impl_->lock ();
  }
  else if (context_)
  {
    context_->lock ();
  }
}

void
madara::knowledge::KnowledgeBase::unlock (void)
{
  if (impl_.get_ptr ())
  {
    impl_->unlock ();
  }
  else if (context_)
  {
    context_->unlock ();
  }
}

int
madara::knowledge::KnowledgeBase::get_log_level (void)
{
  int result (0);

  if (impl_.get_ptr ())
  {
    result = impl_->get_log_level ();
  }
  else if (context_)
  {
    result = context_->get_log_level ();
  }

  return result;
}

madara::logger::Logger &
madara::knowledge::KnowledgeBase::get_logger (void) const
{
  if (impl_.get_ptr ())
  {
    return impl_->get_logger ();
  }
  else
  {
    return context_->get_logger ();
  }
}

void
madara::knowledge::KnowledgeBase::attach_logger (
  logger::Logger & logger) const
{
  if (impl_.get_ptr ())
  {
    impl_->attach_logger (logger);
  }
  else
  {
    context_->attach_logger (logger);
  }
}

void
madara::knowledge::KnowledgeBase::set_log_level (int level)
{
  if (impl_.get_ptr ())
  {
    impl_->set_log_level (level);
  }
  else if (context_)
  {
    context_->set_log_level (level);
  }
}


void
madara::knowledge::KnowledgeBase::copy (
  const KnowledgeBase & source,
  const CopySet & copy_set,
  bool clean_copy)
{
  if (impl_.get_ptr () && source.impl_.get_ptr () != 0)
  {
    impl_->copy (*source.impl_.get_ptr (), copy_set, clean_copy);
  }
  else if (context_ && source.impl_.get_ptr () != 0)
  {
    KnowledgeBaseImpl * source_impl =
      (KnowledgeBaseImpl *)source.impl_.get_ptr ();
    ThreadSafeContext * source_context = &(source_impl->get_context ());

    context_->copy (*source_context, copy_set, clean_copy);
  }
}

/// Applies current time and modified to all global variables and tries
/// to send them.
int
madara::knowledge::KnowledgeBase::apply_modified (
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->apply_modified (settings);
  }
  else if (context_)
  {
    context_->apply_modified ();
  }

  return result;
}

void
madara::knowledge::KnowledgeBase::close_transport (void)
{
  if (impl_.get_ptr ())
  {
    impl_->close_transport ();
  }
}

madara::knowledge::KnowledgeRecord
madara::knowledge::KnowledgeBase::get (const std::string & key,
             const KnowledgeReferenceSettings & settings)
{
  knowledge::KnowledgeRecord result;

  if (impl_.get_ptr ())
  {
    result = impl_->get (key, settings);
  }
  else if (context_)
  {
    result = context_->get (key, settings);
  }

  return result;
}

madara::knowledge::VariableReference
madara::knowledge::KnowledgeBase::get_ref (const std::string & key,
             const KnowledgeReferenceSettings & settings)
{
  VariableReference var;
  
  if (impl_.get_ptr ())
  {
    var = impl_->get_ref (key, settings);
  }
  else if (context_)
  {
    var = context_->get_ref (key, settings);
  }

  return var;
}

madara::knowledge::KnowledgeRecord
madara::knowledge::KnowledgeBase::get (
  const VariableReference & variable,
  const KnowledgeReferenceSettings & settings)
{
  knowledge::KnowledgeRecord result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->get (variable, settings);
  }
  else if (context_)
  {
    result = context_->get (variable, settings);
  }

  return result;
}

madara::knowledge::KnowledgeRecord
madara::knowledge::KnowledgeBase::retrieve_index (
  const std::string & key, size_t index,
  const KnowledgeReferenceSettings & settings)
{
  knowledge::KnowledgeRecord result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->retrieve_index (key, index, settings);
  }
  else if (context_)
  {
    result = context_->retrieve_index (key, index, settings);
  }

  return result;
}

madara::knowledge::KnowledgeRecord
madara::knowledge::KnowledgeBase::retrieve_index (
  const VariableReference & variable, size_t index,
  const KnowledgeReferenceSettings & settings)
{
  knowledge::KnowledgeRecord result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->retrieve_index (variable, index, settings);
  }
  else if (context_)
  {
    result = context_->retrieve_index (variable, index, settings);
  }

  return result;
}


int
madara::knowledge::KnowledgeBase::read_file (
  const std::string & knowledge_key, 
  const std::string & filename, 
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->read_file (knowledge_key, filename, settings);
  }
  else if (context_)
  {
    result = context_->read_file (knowledge_key, filename, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::read_file (
  const VariableReference & variable, 
  const std::string & filename, 
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->read_file (variable, filename, settings);
  }
  else if (context_)
  {
    result = context_->read_file (variable, filename, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set_file (
  const std::string & key,
  const unsigned char * value, size_t size, 
  const EvalSettings & settings)
{
  int result = -1;

  if (key != "")
  {
    if (impl_.get_ptr ())
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

int
madara::knowledge::KnowledgeBase::set_file (
  const VariableReference & variable,
  const unsigned char * value, size_t size, 
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set_file (variable, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set_file (variable, value, size, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set_jpeg (
  const std::string & key,
  const unsigned char * value, size_t size, 
  const EvalSettings & settings)
{
  int result = -1;

  if (key != "")
  {
    if (impl_.get_ptr ())
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

int
madara::knowledge::KnowledgeBase::set_jpeg (
  const VariableReference & variable,
  const unsigned char * value, size_t size, 
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set_jpeg (variable, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set_jpeg (variable, value, size, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::read_policy (
  const std::string & knowledge_key, 
  const std::string & filename)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->read_policy (knowledge_key, filename);
  }

  return result;
}

ssize_t
madara::knowledge::KnowledgeBase::write_file (const std::string & knowledge_key, 
                     const std::string & filename)
{
  ssize_t result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->write_file (knowledge_key, filename);
  }

  return result;
}


std::string
madara::knowledge::KnowledgeBase::expand_statement (
  const std::string & statement)
{
  std::string result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->expand_statement (statement);
  }
  else if (context_)
  {
    result = context_->expand_statement (statement);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (
  const std::string & key,
  madara::knowledge::KnowledgeRecord::Integer value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

void
madara::knowledge::KnowledgeBase::mark_modified (
  const VariableReference & variable)
{
  if (impl_.get_ptr ())
  {
    impl_->mark_modified (variable);
  }
  else if (context_)
  {
    context_->mark_modified (variable);
  }
}

int
madara::knowledge::KnowledgeBase::set (
  const VariableReference & variable,
  madara::knowledge::KnowledgeRecord::Integer value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set_index (
  const std::string & key,
  size_t index,
  madara::knowledge::KnowledgeRecord::Integer value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set_index (key, index, value, settings);
  }
  else if (context_)
  {
    result = context_->set_index (key, index, value, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set_index (
  const VariableReference & variable,
  size_t index,
  madara::knowledge::KnowledgeRecord::Integer value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set_index (variable, index, value, settings);
  }
  else if (context_)
  {
    result = context_->set_index (variable, index, value, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (
  const std::string & key,
  const knowledge::KnowledgeRecord::Integer * value,
  uint32_t size,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, size, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (
  const VariableReference & variable,
  const knowledge::KnowledgeRecord::Integer * value,
  uint32_t size,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, size, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (
  const std::string & key,
  const std::vector <KnowledgeRecord::Integer> & value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (
  const VariableReference & variable,
  const std::vector <KnowledgeRecord::Integer> & value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (
  const std::string & key,
  double value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (
  const VariableReference & variable,
  double value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set_index (
  const std::string & key,
  size_t index,
  double value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set_index (key, index, value, settings);
  }
  else if (context_)
  {
    result = context_->set_index (key, index, value, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set_index (
  const VariableReference & variable,
  size_t index,
  double value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set_index (variable, index, value, settings);
  }
  else if (context_)
  {
    result = context_->set_index (variable, index, value, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (
  const std::string & key,
  const double * value,
  uint32_t size,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, size, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (
  const VariableReference & variable,
  const double * value,
  uint32_t size,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, size, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, size, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (
  const std::string & key,
  const std::vector <double> & value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (
  const VariableReference & variable,
  const std::vector <double> & value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (const std::string & key,
  const std::string & value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (key, value, settings);
  }
  else if (context_)
  {
    result = context_->set (key, value, settings);
  }

  return result;
}

int
madara::knowledge::KnowledgeBase::set (
  const VariableReference & variable,
  const std::string & value,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->set (variable, value, settings);
  }
  else if (context_)
  {
    result = context_->set (variable, value, settings);
  }

  return result;
}


/// Set quality of writing to a variable
void 
madara::knowledge::KnowledgeBase::set_quality (
  const std::string & key, uint32_t quality,
  const KnowledgeReferenceSettings & settings)
{
  if (impl_.get_ptr ())
  {
    impl_->set_quality (key, quality, settings);
  }
  else if (context_)
  {
    context_->set_quality (key, quality, true, settings);
  }
}


bool
madara::knowledge::KnowledgeBase::exists (
  const std::string & key,
  const KnowledgeReferenceSettings & settings) const
{
  bool result = false;
  
  if (impl_.get_ptr ())
  {
    result = impl_->exists (key, settings);
  }
  else if (context_)
  {
    result = context_->exists (key, settings);
  }

  return result;
}

bool
madara::knowledge::KnowledgeBase::exists (
  const VariableReference & variable,
  const KnowledgeReferenceSettings & settings) const
{
  bool result = false;
  
  if (impl_.get_ptr ())
  {
    result = impl_->exists (variable, settings);
  }
  else if (context_)
  {
    result = context_->exists (variable, settings);
  }

  return result;
}

#ifdef _USE_CID_

void
madara::knowledge::KnowledgeBase::print_all_redeployment_results (
  std::ostream & output)
{
  return impl_->print_all_redeployment_results (output);
}

void
madara::knowledge::KnowledgeBase::run_all (void)
{
  return impl_->run_all ();
}

long
madara::knowledge::KnowledgeBase::start_latency (void)
{
  return impl_->start_latency ();
}

void
madara::knowledge::KnowledgeBase::print_my_latencies (
  std::ostream & output)
{
  impl_->print_my_latencies (output);
}

void
madara::knowledge::KnowledgeBase::print_all_latencies (
  std::ostream & output)
{
  impl_->print_all_latencies (output);
}

void
madara::knowledge::KnowledgeBase::print_all_summations (
  std::ostream & output)
{
  impl_->print_all_summations (output);
}

long
madara::knowledge::KnowledgeBase::vote (void)
{
  return impl_->vote ();
}

#endif // _USE_CID_

madara::transport::TransportSettings &
madara::knowledge::KnowledgeBase::transport_settings (void)
{
  return impl_->transport_settings ();
}

void
madara::knowledge::KnowledgeBase::print (
  unsigned int level) const
{
  if (impl_.get_ptr ())
  {
    impl_->print (level);
  }
  else if (context_)
  {
    context_->print (level);
  }
}

void
madara::knowledge::KnowledgeBase::print_knowledge (
  unsigned int level) const
{
  if (impl_.get_ptr ())
  {
    impl_->print (level);
  }
  else if (context_)
  {
    context_->print (level);
  }
}

void
madara::knowledge::KnowledgeBase::to_string (
  std::string & target,
  const std::string & array_delimiter,
  const std::string & record_delimiter,
  const std::string & key_val_delimiter) const
{
  if (impl_.get_ptr ())
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


void
madara::knowledge::KnowledgeBase::print (
  const std::string & statement, unsigned int level) const
{
  if (impl_.get_ptr ())
  {
    impl_->print (statement, level);
  }
  else if (context_)
  {
    context_->print (level);
  }
}

void
madara::knowledge::KnowledgeBase::clear (bool erase)
{
  if (impl_.get_ptr ())
  {
    impl_->clear (erase);
  }
  else if (context_)
  {
    context_->clear (erase);
  }
}

void
madara::knowledge::KnowledgeBase::clear_map (void)
{
  if (impl_.get_ptr ())
  {
    impl_->clear_map ();
  }
  else if (context_)
  {
    impl_->clear_map ();
  }
}

void
madara::knowledge::KnowledgeBase::acquire (void)
{
  if (impl_.get_ptr ())
  {
    impl_->acquire ();
  }
  else if (context_)
  {
    context_->lock ();
  }
}

void
madara::knowledge::KnowledgeBase::release (void)
{
  if (impl_.get_ptr ())
  {
    impl_->release ();
  }
  else if (context_)
  {
    context_->unlock ();
  }
}

#ifndef _MADARA_NO_KARL_

madara::knowledge::CompiledExpression
madara::knowledge::KnowledgeBase::compile (
  const std::string & expression)
{
  CompiledExpression result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->compile (expression);
  }

  return result;
}

// evaluate a knowledge expression and choose to send any modifications
madara::knowledge::KnowledgeRecord
madara::knowledge::KnowledgeBase::evaluate (
  const std::string & expression,
  const EvalSettings & settings)
{
  knowledge::KnowledgeRecord result;
  
  if (impl_.get_ptr ())
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
madara::knowledge::KnowledgeRecord
madara::knowledge::KnowledgeBase::evaluate (
  CompiledExpression & expression,
  const EvalSettings & settings)
{
  knowledge::KnowledgeRecord result;
  
  if (impl_.get_ptr ())
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
madara::knowledge::KnowledgeRecord
madara::knowledge::KnowledgeBase::evaluate (
  expression::ComponentNode * root,
  const EvalSettings & settings)
{
  knowledge::KnowledgeRecord result;
  
  if (impl_.get_ptr ())
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
void
madara::knowledge::KnowledgeBase::define_function (
  const std::string & name,
    knowledge::KnowledgeRecord (*func) (const char *, FunctionArguments &, Variables &))
{
  if (impl_.get_ptr ())
  {
    impl_->define_function (name, func);
  }
  else if (context_)
  {
    context_->define_function (name, func);
  }
}

// Defines a function
void madara::knowledge::KnowledgeBase::define_function (
  const std::string & name,
    knowledge::KnowledgeRecord (*func) (FunctionArguments &, Variables &))
{
  if (impl_.get_ptr ())
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
void madara::knowledge::KnowledgeBase::define_function (
  const std::string & name, jobject func)
{
  if (impl_.get_ptr ())
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
void madara::knowledge::KnowledgeBase::define_function (
  const std::string & name, boost::python::object callable)
{
  if (impl_.get_ptr ())
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
void
madara::knowledge::KnowledgeBase::define_function (const std::string & name,
  const std::string & expression)
{
  if (impl_.get_ptr ())
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
void
madara::knowledge::KnowledgeBase::define_function (const std::string & name,
  const CompiledExpression & expression)
{
  if (impl_.get_ptr ())
  {
    impl_->define_function (name, expression);
  }
  else if (context_)
  {
    context_->define_function (name, expression);
  }
}

madara::knowledge::KnowledgeRecord
madara::knowledge::KnowledgeBase::wait (
  CompiledExpression & expression, 
  const WaitSettings & settings)
{
  knowledge::KnowledgeRecord result;

  if (context_)
  {
    /**
     * The only situation this can be useful will be if the thread safe
     * context is being used as a shared memory structure between
     * threads. This should not be used for processes communicating
     * together because the wait statement is unable to send modifieds
     * as it has zero concept of transports. The type of knowledge
     * base handled here is a facade for another knowledge base's
     * context.
     **/
    
    ACE_Time_Value current = ACE_High_Res_Timer::gettimeofday ();  
    ACE_Time_Value max_wait, sleep_time, next_epoch;
    ACE_Time_Value poll_frequency, last = current;

    if (settings.poll_frequency >= 0)
    {
      max_wait.set (settings.max_wait_time);
      max_wait = current + max_wait;
    
      poll_frequency.set (settings.poll_frequency);
      next_epoch = current + poll_frequency;
    }

    // print the post statement at highest log level (cannot be masked)
    if (settings.pre_print_statement != "")
      context_->print (settings.pre_print_statement, logger::LOG_ALWAYS);

    // lock the context
    context_->lock ();

    madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
      "KnowledgeBase::wait:" \
      " waiting on %s\n", expression.logic.c_str ());

    madara::knowledge::KnowledgeRecord last_value = expression.expression.evaluate (settings);

    madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
      "KnowledgeBase::wait:" \
      " completed first eval to get %s\n",
      last_value.to_string ().c_str ());
  
    send_modifieds ("KnowledgeBase:wait", settings);

    context_->unlock ();
  
    current = ACE_High_Res_Timer::gettimeofday ();

    // wait for expression to be true
    while (!last_value.to_integer () &&
      (settings.max_wait_time < 0 || current < max_wait))
    {
      madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
        "KnowledgeBase::wait:" \
        " current is %llu.%llu and max is %llu.%llu (poll freq is %f)\n",
        (unsigned long long)current.sec (),
        (unsigned long long)current.usec (),
        (unsigned long long)max_wait.sec (),
        (unsigned long long)max_wait.usec (),
        settings.poll_frequency);

      madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
        "KnowledgeBase::wait:" \
        " last value didn't result in success\n");

      // Unlike the other wait statements, we allow for a time based wait.
      // To do this, we allow a user to specify a 
      if (settings.poll_frequency > 0)
      {
        if (current < next_epoch)
        {
          sleep_time = next_epoch - current;
          madara::utility::sleep (sleep_time);
        }

        next_epoch = next_epoch + poll_frequency;
      }
      else
        context_->wait_for_change (true);

      // relock - basically we need to evaluate the tree again, and
      // we can't have a bunch of people changing the variables as 
      // while we're evaluating the tree.
      context_->lock ();


      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "KnowledgeBase::wait:" \
        " waiting on %s\n", expression.logic.c_str ());

      last_value = expression.expression.evaluate (settings);

      madara_logger_log (context_->get_logger (), logger::LOG_DETAILED,
        "KnowledgeBase::wait:" \
        " completed eval to get %s\n",
        last_value.to_string ().c_str ());
  
      send_modifieds ("KnowledgeBase:wait", settings);

      context_->unlock ();
      context_->signal ();

      // get current time
      current = ACE_High_Res_Timer::gettimeofday ();

    } // end while (!last)
  
    if (current >= max_wait)
    {
      madara_logger_log (context_->get_logger (), logger::LOG_MAJOR,
        "KnowledgeBase::wait:" \
        " Evaluate did not succeed. Timeout occurred\n");
    }

    // print the post statement at highest log level (cannot be masked)
    if (settings.post_print_statement != "")
      context_->print (settings.post_print_statement, logger::LOG_ALWAYS);

    return last_value;
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->wait (expression, settings);
  }

  return result;
}

madara::knowledge::KnowledgeRecord
madara::knowledge::KnowledgeBase::wait (
  const std::string & expression, 
  const WaitSettings & settings)
{
  knowledge::KnowledgeRecord result;

  if (context_)
  {
    CompiledExpression ce = context_->compile (expression);
    result = this->wait (ce, settings);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->wait (expression, settings);
  }

  return result;
}

#endif // _MADARA_NO_KARL_

void
madara::knowledge::KnowledgeBase::activate_transport (void)
{
  if (impl_.get_ptr ())
  {
    impl_->activate_transport ();
  }
}

size_t
madara::knowledge::KnowledgeBase::attach_transport (
  madara::transport::Base * transport)
{
  size_t result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->attach_transport (transport);
  }

  return result;
}

size_t
madara::knowledge::KnowledgeBase::get_num_transports (void)
{
  size_t result (0);
  
  if (impl_.get_ptr ())
  {
    result = impl_->get_num_transports ();
  }

  return result;
}

size_t
madara::knowledge::KnowledgeBase::attach_transport (const std::string & id,
        transport::TransportSettings & settings)
{
  size_t result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->attach_transport (id, settings);
  }

  return result;
}

size_t
  madara::knowledge::KnowledgeBase::remove_transport (
  size_t index)
{
  size_t result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->remove_transport (index);
  }

  return result;
}

madara::knowledge::ThreadSafeContext &
madara::knowledge::KnowledgeBase::get_context (void)
{
  ThreadSafeContext * result = 0;
  
  if (context_)
  {
    result = context_;
  }
  else if (impl_.get_ptr ())
  {
    result = &(impl_->get_context ());
  }

  return *result;
}

void
madara::knowledge::KnowledgeBase::clear_modifieds (void)
{
  if (context_)
  {
    context_->reset_modified ();
  }
  else if (impl_.get_ptr ())
  {
    impl_->clear_modifieds ();
  }
}

int
madara::knowledge::KnowledgeBase::send_modifieds (
  const std::string & prefix,
  const EvalSettings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->send_modifieds (prefix, settings);
  }

  return result;
}

std::string
madara::knowledge::KnowledgeBase::debug_modifieds (void) const
{
  std::string result = "";

  if (context_)
  {
    result = context_->debug_modifieds ();
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->debug_modifieds ();
  }

  return result;
}

/**
  * Returns the unique host and ephemeral binding for this Knowlede Base
  * @return             host:port identifier for this knowledge base
  **/
std::string
madara::knowledge::KnowledgeBase::get_id (void)
{
  std::string result;

  if (impl_.get_ptr ())
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
size_t
madara::knowledge::KnowledgeBase::to_vector (
                              const std::string & subject,
                              unsigned int start,
                              unsigned int end,
                              std::vector <KnowledgeRecord> & target)
{
  size_t result;

  if (context_)
  {
    result = context_->to_vector (subject, start, end, target);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->to_vector (subject, start, end, target);
  }

  return result;
}

void madara::knowledge::KnowledgeBase::facade_for (
  ThreadSafeContext & context)
{
  context_ = &context;
  impl_ = 0;
}

size_t
madara::knowledge::KnowledgeBase::to_map (
  const std::string & expression,
  std::map <std::string, knowledge::KnowledgeRecord> & target)
{
  size_t result;

  if (context_)
  {
    result = context_->to_map (expression, target);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->to_map (expression, target);
  }

  return result;
}

size_t
madara::knowledge::KnowledgeBase::to_map (
  const std::string & prefix,
  const std::string & delimiter,
  const std::string & suffix,
  std::vector <std::string> & next_keys,
  std::map <std::string, knowledge::KnowledgeRecord> & result,
  bool just_keys)
{
  size_t result_size;

  if (context_)
  {
    result_size = context_->to_map (
      prefix, delimiter, suffix, next_keys, result, just_keys);
  }
  else if (impl_.get_ptr ())
  {
    result_size = impl_->to_map (
      prefix, delimiter, suffix, next_keys, result, just_keys);
  }

  return result_size;
}
int64_t
madara::knowledge::KnowledgeBase::save_context (
  const std::string & filename) const
{
  int64_t result = 0;
  
  if (context_)
  {
    result = context_->save_context (filename);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->save_context (filename);
  }

  return result;
}


int64_t
madara::knowledge::KnowledgeBase::save_as_karl (
  const std::string & filename) const
{
  int64_t result = 0;

  if (context_)
  {
    result = context_->save_as_karl (filename);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->save_as_karl (filename);
  }

  return result;
}


int64_t
madara::knowledge::KnowledgeBase::save_checkpoint (
  const std::string & filename,
  bool reset_modifieds)
{
  int64_t result = 0;
  
  if (context_)
  {
    result = context_->save_checkpoint (filename);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->save_checkpoint (filename, reset_modifieds);
  }

  return result;
}

int64_t
madara::knowledge::KnowledgeBase::load_context (
  const std::string & filename,
  bool use_id,
  const KnowledgeUpdateSettings & settings)
{
  int64_t result = 0;

  if (context_)
  {
    result = impl_->load_context (filename, use_id, settings);
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->load_context (filename, use_id, settings);
  }

  return result;
}

void
madara::knowledge::KnowledgeBase::wait_for_change (void)
{
  if (context_)
  {
    context_->wait_for_change ();
  }
  else if (impl_.get_ptr ())
  {
    impl_->wait_for_change ();
  }
}

std::string
madara::knowledge::KnowledgeBase::setup_unique_hostport (
  const std::string & host)
{
  std::string result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->setup_unique_hostport (host);
  }

  return result;
}
