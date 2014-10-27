
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/knowledge_engine/Knowledge_Base_Impl.h"
#include "madara/utility/Log_Macros.h"

#include <sstream>
#include <iostream>

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
: impl_ (new Knowledge_Base_Impl ())
{
}

Madara::Knowledge_Engine::Knowledge_Base::Knowledge_Base (
  const std::string & host, int transport)
: impl_ (new Knowledge_Base_Impl (host, transport))
{
}

Madara::Knowledge_Engine::Knowledge_Base::Knowledge_Base (
  const std::string & host, int transport, 
  const std::string & knowledge_domain)
: impl_ (new Knowledge_Base_Impl (host, transport, knowledge_domain))
{
}

Madara::Knowledge_Engine::Knowledge_Base::Knowledge_Base (
  const std::string & host, const Madara::Transport::Settings & settings)
: impl_ (new Knowledge_Base_Impl (host, settings))
{
}

Madara::Knowledge_Engine::Knowledge_Base::Knowledge_Base (
  const Knowledge_Base & original)
: impl_ (original.impl_)
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
    impl_ = original.impl_;   
}

void
Madara::Knowledge_Engine::Knowledge_Base::lock (void)
{
  impl_->lock ();
}

void
Madara::Knowledge_Engine::Knowledge_Base::unlock (void)
{
  impl_->unlock ();
}


void
Madara::Knowledge_Engine::Knowledge_Base::copy (
  const Knowledge_Base & source,
  const Copy_Set & copy_set,
  bool clean_copy)
{
  if (impl_.get_ptr () && source.impl_.get_ptr () != 0)
    impl_->copy (*source.impl_.get_ptr (), copy_set, clean_copy);
}

void
Madara::Knowledge_Engine::Knowledge_Base::activate_transport (void)
{
  impl_->activate_transport ();
}

/// Applies current time and modified to all global variables and tries
/// to send them.
int
Madara::Knowledge_Engine::Knowledge_Base::apply_modified (
  const Eval_Settings & settings)
{
  return impl_->apply_modified (settings);
}

void
Madara::Knowledge_Engine::Knowledge_Base::close_transport (void)
{
  impl_->close_transport ();
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::get (const std::string & key,
             const Knowledge_Reference_Settings & settings)
{
  return impl_->get (key, settings);
}

Madara::Knowledge_Engine::Variable_Reference
Madara::Knowledge_Engine::Knowledge_Base::get_ref (const std::string & key,
             const Knowledge_Reference_Settings & settings)
{
  return impl_->get_ref (key, settings);
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::get (
  const Variable_Reference & variable,
  const Knowledge_Reference_Settings & settings)
{
  return impl_->get (variable, settings);
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::retrieve_index (
  const std::string & key, size_t index,
  const Knowledge_Reference_Settings & settings)
{
  return impl_->retrieve_index (key, index, settings);
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::retrieve_index (
  const Variable_Reference & variable, size_t index,
  const Knowledge_Reference_Settings & settings)
{
  return impl_->retrieve_index (variable, index, settings);
}


int
Madara::Knowledge_Engine::Knowledge_Base::read_file (
  const std::string & knowledge_key, 
  const std::string & filename, 
  const Eval_Settings & settings)
{
  return impl_->read_file (knowledge_key, filename, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::read_file (
  const Variable_Reference & variable, 
  const std::string & filename, 
  const Eval_Settings & settings)
{
  return impl_->read_file (variable, filename, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set_file (
  const std::string & key,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
{
  if (key != "")
  {
    return impl_->set_file (
      impl_->get_ref (key, settings), value, size, settings);
  }
  else
    return -1;
}

int
Madara::Knowledge_Engine::Knowledge_Base::set_file (
  const Variable_Reference & variable,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
{
  return impl_->set_file (variable, value, size, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set_jpeg (
  const std::string & key,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
{
  if (key != "")
  {
    return impl_->set_jpeg (
      impl_->get_ref (key, settings), value, size, settings);
  }
  else
    return -1;
}

int
Madara::Knowledge_Engine::Knowledge_Base::set_jpeg (
  const Variable_Reference & variable,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
{
  return impl_->set_jpeg (variable, value, size, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::read_policy (
  const std::string & knowledge_key, 
  const std::string & filename)
{
  return impl_->read_policy (knowledge_key, filename);
}

ssize_t
Madara::Knowledge_Engine::Knowledge_Base::write_file (const std::string & knowledge_key, 
                     const std::string & filename)
{
  return impl_->write_file (knowledge_key, filename);
}


std::string
Madara::Knowledge_Engine::Knowledge_Base::expand_statement (
  const std::string & statement)
{
  return impl_->expand_statement (statement);
}

Madara::Knowledge_Engine::Compiled_Expression
Madara::Knowledge_Engine::Knowledge_Base::compile (
  const std::string & expression)
{
  return impl_->compile (expression);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const std::string & key,
  Madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
{
  return impl_->set (key, value, settings);
}

void
Madara::Knowledge_Engine::Knowledge_Base::mark_modified (
  const Variable_Reference & variable)
{
  return impl_->mark_modified (variable);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  Madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
{
  return impl_->set (variable, value, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set_index (
  const std::string & key,
  size_t index,
  Madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
{
  return impl_->set_index (key, index, value, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set_index (
  const Variable_Reference & variable,
  size_t index,
  Madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
{
  return impl_->set_index (variable, index, value, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const std::string & key,
  const Knowledge_Record::Integer * value,
  uint32_t size,
  const Eval_Settings & settings)
{
  return impl_->set (key, value, size, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  const Knowledge_Record::Integer * value,
  uint32_t size,
  const Eval_Settings & settings)
{
  return impl_->set (variable, value, size, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const std::string & key,
  const std::vector <Knowledge_Record::Integer> & value,
  const Eval_Settings & settings)
{
  return impl_->set (key, value, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  const std::vector <Knowledge_Record::Integer> & value,
  const Eval_Settings & settings)
{
  return impl_->set (variable, value, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const std::string & key,
  double value,
  const Eval_Settings & settings)
{
  return impl_->set (key, value, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  double value,
  const Eval_Settings & settings)
{
  return impl_->set (variable, value, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set_index (
  const std::string & key,
  size_t index,
  double value,
  const Eval_Settings & settings)
{
  return impl_->set_index (key, index, value, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set_index (
  const Variable_Reference & variable,
  size_t index,
  double value,
  const Eval_Settings & settings)
{
  return impl_->set_index (variable, index, value, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const std::string & key,
  const double * value,
  uint32_t size,
  const Eval_Settings & settings)
{
  return impl_->set (key, value, size, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  const double * value,
  uint32_t size,
  const Eval_Settings & settings)
{
  return impl_->set (variable, value, size, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const std::string & key,
  const std::vector <double> & value,
  const Eval_Settings & settings)
{
  return impl_->set (key, value, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  const std::vector <double> & value,
  const Eval_Settings & settings)
{
  return impl_->set (variable, value, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (const std::string & key,
  const std::string & value,
  const Eval_Settings & settings)
{
  return impl_->set (key, value, settings);
}

int
Madara::Knowledge_Engine::Knowledge_Base::set (
  const Variable_Reference & variable,
  const std::string & value,
  const Eval_Settings & settings)
{
  return impl_->set (variable, value, settings);
}


/// Set quality of writing to a variable
void 
Madara::Knowledge_Engine::Knowledge_Base::set_quality (
  const std::string & key, uint32_t quality,
  const Knowledge_Reference_Settings & settings)
{
  impl_->set_quality (key, quality, settings);
}


bool
Madara::Knowledge_Engine::Knowledge_Base::exists (
  const std::string & key,
  const Knowledge_Reference_Settings & settings) const
{
  return impl_->exists (key, settings);
}

bool
Madara::Knowledge_Engine::Knowledge_Base::exists (
  const Variable_Reference & variable,
  const Knowledge_Reference_Settings & settings) const
{
  return impl_->exists (variable, settings);
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

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::wait (
  Compiled_Expression & expression, 
  const Wait_Settings & settings)
{
  return impl_->wait (expression, settings);
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::wait (
  const std::string & expression, 
  const Wait_Settings & settings)
{
  return impl_->wait (expression, settings);
}

void
Madara::Knowledge_Engine::Knowledge_Base::print (
  unsigned int level) const
{
  impl_->print (level);
}

void
Madara::Knowledge_Engine::Knowledge_Base::print_knowledge (
  unsigned int level) const
{
  impl_->print (level);
}

void
Madara::Knowledge_Engine::Knowledge_Base::print (
  const std::string & statement, unsigned int level) const
{
  impl_->print (statement, level);
}

void
Madara::Knowledge_Engine::Knowledge_Base::clear (void)
{
  impl_->clear ();
}

void
Madara::Knowledge_Engine::Knowledge_Base::clear_map (void)
{
  impl_->clear_map ();
}

void
Madara::Knowledge_Engine::Knowledge_Base::acquire (void)
{
  impl_->acquire ();
}

void
Madara::Knowledge_Engine::Knowledge_Base::release (void)
{
  impl_->release ();
}

// evaluate a knowledge expression and choose to send any modifications
Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::evaluate (
  const std::string & expression,
  const Eval_Settings & settings)
{
  return impl_->evaluate (expression, settings);
}

// evaluate a knowledge expression and choose to send any modifications
Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::evaluate (
  Compiled_Expression & expression,
  const Eval_Settings & settings)
{
  return impl_->evaluate (expression, settings);
}

// evaluate a knowledge expression and choose to send any modifications
Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base::evaluate (
  Expression_Tree::Component_Node * root,
  const Eval_Settings & settings)
{
  return impl_->evaluate (root, settings);
}


// Defines a function
void Madara::Knowledge_Engine::Knowledge_Base::define_function (
  const std::string & name,
    Knowledge_Record (*func) (const char *, Function_Arguments &, Variables &))
{
  impl_->define_function (name, func);
}

// Defines a function
void Madara::Knowledge_Engine::Knowledge_Base::define_function (
  const std::string & name,
    Knowledge_Record (*func) (Function_Arguments &, Variables &))
{
  impl_->define_function (name, func);
}

#ifdef _MADARA_JAVA_
// Defines a function
void Madara::Knowledge_Engine::Knowledge_Base::define_function (
  const std::string & name, jobject func)
{
  impl_->define_function (name, func);
}
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_

// Defines a function
void Madara::Knowledge_Engine::Knowledge_Base::define_function (
  const std::string & name, boost::python::object callable)
{
  impl_->define_function (name, callable);
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
  impl_->define_function (name, expression);
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
  impl_->define_function (name, expression);
}
    


size_t
Madara::Knowledge_Engine::Knowledge_Base::attach_transport (
  Madara::Transport::Base * transport)
{
  return impl_->attach_transport (transport);
}

size_t
Madara::Knowledge_Engine::Knowledge_Base::attach_transport (const std::string & id,
        Transport::Settings & settings)
{
  return impl_->attach_transport (id, settings);
}

size_t
  Madara::Knowledge_Engine::Knowledge_Base::remove_transport (
  size_t index)
{
  return impl_->remove_transport (index);
}

Madara::Knowledge_Engine::Thread_Safe_Context &
Madara::Knowledge_Engine::Knowledge_Base::get_context (void)
{
  return impl_->get_context ();
}

int
Madara::Knowledge_Engine::Knowledge_Base::send_modifieds (
  const std::string & prefix,
  const Eval_Settings & settings)
{
  return impl_->send_modifieds (prefix, settings);
}

/**
  * Returns the unique host and ephemeral binding for this Knowlede Base
  * @return             host:port identifier for this knowledge base
  **/
std::string
Madara::Knowledge_Engine::Knowledge_Base::get_id (void)
{
  return impl_->get_id ();
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
  return impl_->to_vector (subject, start, end, target);
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
size_t
Madara::Knowledge_Engine::Knowledge_Base::to_map (
  const std::string & expression,
  std::map <std::string, Knowledge_Record> & target)
{
  return impl_->to_map (expression, target);
}

int64_t
Madara::Knowledge_Engine::Knowledge_Base::save_context (
  const std::string & filename)
{
  return impl_->save_context (filename);
}

int64_t
Madara::Knowledge_Engine::Knowledge_Base::save_checkpoint (
  const std::string & filename,
  bool reset_modifieds)
{
  return impl_->save_checkpoint (filename, reset_modifieds);
}

int64_t
Madara::Knowledge_Engine::Knowledge_Base::load_context (
  const std::string & filename,
  bool use_id,
  const Knowledge_Update_Settings & settings)
{
  return impl_->load_context (filename, use_id, settings);
}

void
Madara::Knowledge_Engine::Knowledge_Base::wait_for_change (void)
{
  impl_->wait_for_change ();
}

std::string
Madara::Knowledge_Engine::Knowledge_Base::setup_unique_hostport (
  const std::string & host)
{
  return impl_->setup_unique_hostport (host);
}
