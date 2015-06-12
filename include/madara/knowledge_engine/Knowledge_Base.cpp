
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Knowledge_Base_Impl.h"
#include "madara/utility/Log_Macros.h"

#include <sstream>
#include <iostream>

#include "ace/OS_NS_Thread.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_sys_socket.h"

void Madara::Knowledge_Engine::Knowledge_Base::log_to_stderr (
  bool clear_flags)
{
  ACE_LOG_MSG->set_flags (ACE_Log_Msg::STDERR);
  if (clear_flags)
  {
    ACE_LOG_MSG->clr_flags (ACE_Log_Msg::OSTREAM);
    ACE_LOG_MSG->clr_flags (ACE_Log_Msg::SYSLOG);
  }
}

void Madara::Knowledge_Engine::Knowledge_Base::log_to_file (
  const char * filename, bool clear_flags)
{
  // get the old message output stream and delete it
  ACE_OSTREAM_TYPE * output = new std::ofstream (filename);
  ACE_LOG_MSG->msg_ostream (output, true);
  ACE_LOG_MSG->set_flags (ACE_Log_Msg::OSTREAM);

  if (clear_flags)
  {
    ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);
    ACE_LOG_MSG->clr_flags (ACE_Log_Msg::SYSLOG);
  }
}

void Madara::Knowledge_Engine::Knowledge_Base::log_to_system_log (
  const char * prog_name, bool clear_flags)
{
  // open a new socket to the SYSLOG with madara set as logging agent
  ACE_LOG_MSG->open (prog_name, ACE_Log_Msg::SYSLOG, prog_name);

  if (clear_flags)
  {
    ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);
    ACE_LOG_MSG->clr_flags (ACE_Log_Msg::OSTREAM);
  }
}

int
Madara::Knowledge_Engine::Knowledge_Base::log_level (int level)
{
  if (level >= 0)
    MADARA_debug_level = level;

  return MADARA_debug_level;
}

int
Madara::Knowledge_Engine::Knowledge_Base::log_level (void)
{
  return MADARA_debug_level;
}


Madara::Knowledge_Engine::Knowledge_Base::Knowledge_Base ()
: impl_ (new Knowledge_Base_Impl ()), context_ (0)
{
}

Madara::Knowledge_Engine::Knowledge_Base::Knowledge_Base (
  const std::string & host, int transport)
: impl_ (new Knowledge_Base_Impl (host, transport)), context_ (0)
{
}

Madara::Knowledge_Engine::Knowledge_Base::Knowledge_Base (
  const std::string & host, int transport, 
  const std::string & knowledge_domain)
: impl_ (new Knowledge_Base_Impl (host, transport, knowledge_domain)),
  context_ (0)
{
}

Madara::Knowledge_Engine::Knowledge_Base::Knowledge_Base (
  const std::string & host, const Madara::Transport::Settings & settings)
: impl_ (new Knowledge_Base_Impl (host, settings)), context_ (0)
{
}

Madara::Knowledge_Engine::Knowledge_Base::Knowledge_Base (
  const Knowledge_Base & original)
: impl_ (original.impl_), context_ (original.context_)
{
}

Madara::Knowledge_Engine::Knowledge_Base::~Knowledge_Base ()
{
  // taken care of by Refcounter class
}

/// Assignment operator.
void 
Madara::Knowledge_Engine::Knowledge_Base::operator= (
  const Knowledge_Base &original)
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
Madara::Knowledge_Engine::Knowledge_Base::use (
  Thread_Safe_Context & original)
{
  impl_ = 0;
  context_ = &original;
}

void
Madara::Knowledge_Engine::Knowledge_Base::lock (void)
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
Madara::Knowledge_Engine::Knowledge_Base::unlock (void)
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


void
Madara::Knowledge_Engine::Knowledge_Base::copy (
  const Knowledge_Base & source,
  const Copy_Set & copy_set,
  bool clean_copy)
{
  if (impl_.get_ptr () && source.impl_.get_ptr () != 0)
  {
    impl_->copy (*source.impl_.get_ptr (), copy_set, clean_copy);
  }
  else if (context_ && source.impl_.get_ptr () != 0)
  {
    Knowledge_Base_Impl * source_impl =
      (Knowledge_Base_Impl *)source.impl_.get_ptr ();
    Thread_Safe_Context * source_context = &(source_impl->get_context ());

    context_->copy (*source_context, copy_set, clean_copy);
  }
}

/// Applies current time and modified to all global variables and tries
/// to send them.
int
Madara::Knowledge_Engine::Knowledge_Base::apply_modified (
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::close_transport (void)
{
  if (impl_.get_ptr ())
  {
    impl_->close_transport ();
  }
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::get (const std::string & key,
             const Knowledge_Reference_Settings & settings)
{
  Knowledge_Record result;

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

Madara::Knowledge_Engine::Variable_Reference
Madara::Knowledge_Engine::Knowledge_Base::get_ref (const std::string & key,
             const Knowledge_Reference_Settings & settings)
{
  Variable_Reference var;
  
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

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::get (
  const Variable_Reference & variable,
  const Knowledge_Reference_Settings & settings)
{
  Knowledge_Record result;
  
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

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::retrieve_index (
  const std::string & key, size_t index,
  const Knowledge_Reference_Settings & settings)
{
  Knowledge_Record result;
  
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

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::retrieve_index (
  const Variable_Reference & variable, size_t index,
  const Knowledge_Reference_Settings & settings)
{
  Knowledge_Record result;
  
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
Madara::Knowledge_Engine::Knowledge_Base::read_file (
  const std::string & knowledge_key, 
  const std::string & filename, 
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::read_file (
  const Variable_Reference & variable, 
  const std::string & filename, 
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set_file (
  const std::string & key,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set_file (
  const Variable_Reference & variable,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set_jpeg (
  const std::string & key,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set_jpeg (
  const Variable_Reference & variable,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::read_policy (
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
Madara::Knowledge_Engine::Knowledge_Base::write_file (const std::string & knowledge_key, 
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
Madara::Knowledge_Engine::Knowledge_Base::expand_statement (
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const std::string & key,
  Madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::mark_modified (
  const Variable_Reference & variable)
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  Madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set_index (
  const std::string & key,
  size_t index,
  Madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set_index (
  const Variable_Reference & variable,
  size_t index,
  Madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const std::string & key,
  const Knowledge_Record::Integer * value,
  uint32_t size,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  const Knowledge_Record::Integer * value,
  uint32_t size,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const std::string & key,
  const std::vector <Knowledge_Record::Integer> & value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  const std::vector <Knowledge_Record::Integer> & value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const std::string & key,
  double value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  double value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set_index (
  const std::string & key,
  size_t index,
  double value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set_index (
  const Variable_Reference & variable,
  size_t index,
  double value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const std::string & key,
  const double * value,
  uint32_t size,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  const double * value,
  uint32_t size,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const std::string & key,
  const std::vector <double> & value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  const std::vector <double> & value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set (const std::string & key,
  const std::string & value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  const std::string & value,
  const Eval_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::set_quality (
  const std::string & key, uint32_t quality,
  const Knowledge_Reference_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::exists (
  const std::string & key,
  const Knowledge_Reference_Settings & settings) const
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
Madara::Knowledge_Engine::Knowledge_Base::exists (
  const Variable_Reference & variable,
  const Knowledge_Reference_Settings & settings) const
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
Madara::Knowledge_Engine::Knowledge_Base::print_all_redeployment_results (
  std::ostream & output)
{
  return impl_->print_all_redeployment_results (output);
}

void
Madara::Knowledge_Engine::Knowledge_Base::run_all (void)
{
  return impl_->run_all ();
}

long
Madara::Knowledge_Engine::Knowledge_Base::start_latency (void)
{
  return impl_->start_latency ();
}

void
Madara::Knowledge_Engine::Knowledge_Base::print_my_latencies (
  std::ostream & output)
{
  impl_->print_my_latencies (output);
}

void
Madara::Knowledge_Engine::Knowledge_Base::print_all_latencies (
  std::ostream & output)
{
  impl_->print_all_latencies (output);
}

void
Madara::Knowledge_Engine::Knowledge_Base::print_all_summations (
  std::ostream & output)
{
  impl_->print_all_summations (output);
}

long
Madara::Knowledge_Engine::Knowledge_Base::vote (void)
{
  return impl_->vote ();
}

#endif // _USE_CID_

Madara::Transport::Settings &
Madara::Knowledge_Engine::Knowledge_Base::transport_settings (void)
{
  return impl_->transport_settings ();
}

void
Madara::Knowledge_Engine::Knowledge_Base::print (
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
Madara::Knowledge_Engine::Knowledge_Base::print_knowledge (
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
Madara::Knowledge_Engine::Knowledge_Base::to_string (
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
Madara::Knowledge_Engine::Knowledge_Base::print (
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
Madara::Knowledge_Engine::Knowledge_Base::clear (bool erase)
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
Madara::Knowledge_Engine::Knowledge_Base::clear_map (void)
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
Madara::Knowledge_Engine::Knowledge_Base::acquire (void)
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
Madara::Knowledge_Engine::Knowledge_Base::release (void)
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

Madara::Knowledge_Engine::Compiled_Expression
Madara::Knowledge_Engine::Knowledge_Base::compile (
  const std::string & expression)
{
  Compiled_Expression result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->compile (expression);
  }

  return result;
}

// evaluate a knowledge expression and choose to send any modifications
Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::evaluate (
  const std::string & expression,
  const Eval_Settings & settings)
{
  Knowledge_Record result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->evaluate (expression, settings);
  }
  else if (context_)
  {
    Compiled_Expression ce = context_->compile (expression);
    result = context_->evaluate (ce, settings);
  }

  return result;
}

// evaluate a knowledge expression and choose to send any modifications
Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::evaluate (
  Compiled_Expression & expression,
  const Eval_Settings & settings)
{
  Knowledge_Record result;
  
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
Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::evaluate (
  Expression_Tree::Component_Node * root,
  const Eval_Settings & settings)
{
  Knowledge_Record result;
  
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
Madara::Knowledge_Engine::Knowledge_Base::define_function (
  const std::string & name,
    Knowledge_Record (*func) (const char *, Function_Arguments &, Variables &))
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
void Madara::Knowledge_Engine::Knowledge_Base::define_function (
  const std::string & name,
    Knowledge_Record (*func) (Function_Arguments &, Variables &))
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
void Madara::Knowledge_Engine::Knowledge_Base::define_function (
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
void Madara::Knowledge_Engine::Knowledge_Base::define_function (
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
Madara::Knowledge_Engine::Knowledge_Base::define_function (const std::string & name,
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
Madara::Knowledge_Engine::Knowledge_Base::define_function (const std::string & name,
  const Compiled_Expression & expression)
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

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::wait (
  Compiled_Expression & expression, 
  const Wait_Settings & settings)
{
  Knowledge_Record result;

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
      context_->print (settings.pre_print_statement, MADARA_LOG_EMERGENCY);

    // lock the context
    context_->lock ();

    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Knowledge_Base::wait:" \
        " waiting on %s\n", expression.logic.c_str ()));

    Madara::Knowledge_Record last_value = expression.expression.evaluate (settings);

    MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
        DLINFO "Knowledge_Base::wait:" \
        " completed first eval to get %s\n",
      last_value.to_string ().c_str ()));
  
    send_modifieds ("Knowledge_Base:wait", settings);

    context_->unlock ();
  
    current = ACE_High_Res_Timer::gettimeofday ();

    // wait for expression to be true
    while (!last_value.to_integer () &&
      (settings.max_wait_time < 0 || current < max_wait))
    {
      MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
          DLINFO "Knowledge_Base::wait:" \
          " current is %Q.%Q and max is %Q.%Q (poll freq is %f)\n",
          current.sec (), current.usec (), max_wait.sec (), max_wait.usec (),
          settings.poll_frequency));

      MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
          DLINFO "Knowledge_Base::wait:" \
          " last value didn't result in success\n"));

      // Unlike the other wait statements, we allow for a time based wait.
      // To do this, we allow a user to specify a 
      if (settings.poll_frequency > 0)
      {
        if (current < next_epoch)
        {
          sleep_time = next_epoch - current;
          Madara::Utility::sleep (sleep_time);
        }

        next_epoch = next_epoch + poll_frequency;
      }
      else
        context_->wait_for_change (true);

      // relock - basically we need to evaluate the tree again, and
      // we can't have a bunch of people changing the variables as 
      // while we're evaluating the tree.
      context_->lock ();

    
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "Knowledge_Base::wait:" \
          " waiting on %s\n", expression.logic.c_str ()));

      last_value = expression.expression.evaluate (settings);
    
      MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
          DLINFO "Knowledge_Base::wait:" \
          " completed eval to get %s\n",
        last_value.to_string ().c_str ()));
  
      send_modifieds ("Knowledge_Base:wait", settings);

      context_->unlock ();
      context_->signal ();

      // get current time
      current = ACE_High_Res_Timer::gettimeofday ();

    } // end while (!last)
  
    if (current >= max_wait)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "Knowledge_Base::wait:" \
          " Evaluate did not succeed. Timeout occurred.\n"));
    }

    // print the post statement at highest log level (cannot be masked)
    if (settings.post_print_statement != "")
      context_->print (settings.post_print_statement, MADARA_LOG_EMERGENCY);

    return last_value;
  }
  else if (impl_.get_ptr ())
  {
    result = impl_->wait (expression, settings);
  }

  return result;
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::wait (
  const std::string & expression, 
  const Wait_Settings & settings)
{
  Knowledge_Record result;

  if (context_)
  {
    Compiled_Expression ce = context_->compile (expression);
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
Madara::Knowledge_Engine::Knowledge_Base::activate_transport (void)
{
  if (impl_.get_ptr ())
  {
    impl_->activate_transport ();
  }
}

size_t
Madara::Knowledge_Engine::Knowledge_Base::attach_transport (
  Madara::Transport::Base * transport)
{
  size_t result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->attach_transport (transport);
  }

  return result;
}

size_t
Madara::Knowledge_Engine::Knowledge_Base::get_num_transports (void)
{
  size_t result (0);
  
  if (impl_.get_ptr ())
  {
    result = impl_->get_num_transports ();
  }

  return result;
}

size_t
Madara::Knowledge_Engine::Knowledge_Base::attach_transport (const std::string & id,
        Transport::Settings & settings)
{
  size_t result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->attach_transport (id, settings);
  }

  return result;
}

size_t
  Madara::Knowledge_Engine::Knowledge_Base::remove_transport (
  size_t index)
{
  size_t result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->remove_transport (index);
  }

  return result;
}

Madara::Knowledge_Engine::Thread_Safe_Context &
Madara::Knowledge_Engine::Knowledge_Base::get_context (void)
{
  Thread_Safe_Context * result = 0;
  
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
Madara::Knowledge_Engine::Knowledge_Base::clear_modifieds (void)
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
Madara::Knowledge_Engine::Knowledge_Base::send_modifieds (
  const std::string & prefix,
  const Eval_Settings & settings)
{
  int result = 0;
  
  if (impl_.get_ptr ())
  {
    result = impl_->send_modifieds (prefix, settings);
  }

  return result;
}

/**
  * Returns the unique host and ephemeral binding for this Knowlede Base
  * @return             host:port identifier for this knowledge base
  **/
std::string
Madara::Knowledge_Engine::Knowledge_Base::get_id (void)
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
Madara::Knowledge_Engine::Knowledge_Base::to_vector (
                              const std::string & subject,
                              unsigned int start,
                              unsigned int end,
                              std::vector <Knowledge_Record> & target)
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

void Madara::Knowledge_Engine::Knowledge_Base::facade_for (
  Thread_Safe_Context & context)
{
  context_ = &context;
  impl_ = 0;
}

size_t
Madara::Knowledge_Engine::Knowledge_Base::to_map (
  const std::string & expression,
  std::map <std::string, Knowledge_Record> & target)
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
Madara::Knowledge_Engine::Knowledge_Base::to_map (
  const std::string & prefix,
  const std::string & delimiter,
  const std::string & suffix,
  std::vector <std::string> & next_keys,
  std::map <std::string, Knowledge_Record> & result,
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
Madara::Knowledge_Engine::Knowledge_Base::save_context (
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
Madara::Knowledge_Engine::Knowledge_Base::save_as_karl (
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
Madara::Knowledge_Engine::Knowledge_Base::save_checkpoint (
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
Madara::Knowledge_Engine::Knowledge_Base::load_context (
  const std::string & filename,
  bool use_id,
  const Knowledge_Update_Settings & settings)
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
Madara::Knowledge_Engine::Knowledge_Base::wait_for_change (void)
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
Madara::Knowledge_Engine::Knowledge_Base::setup_unique_hostport (
  const std::string & host)
{
  std::string result;
  
  if (impl_.get_ptr ())
  {
    result = impl_->setup_unique_hostport (host);
  }

  return result;
}
