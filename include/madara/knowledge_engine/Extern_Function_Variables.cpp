#ifndef _MADARA_EXTERNAL_FUNCTION_VARIABLES_CPP_
#define _MADARA_EXTERNAL_FUNCTION_VARIABLES_CPP_

#include "madara/knowledge_engine/Extern_Function_Variables.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Record_Filters.h"

#include "madara/utility/Log_Macros.h"

void
Madara::Knowledge_Engine::Variables::operator= (Variables & rhs)
{
  if (this != &rhs || context_ != rhs.context_)
    context_ = rhs.context_;
}

Madara::Knowledge_Engine::Thread_Safe_Context *
Madara::Knowledge_Engine::Variables::get_context (void)
{
  return context_;
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Variables::get (const std::string & key,
             const Knowledge_Reference_Settings & settings)
{
  Madara::Knowledge_Record result;

  if (context_)
  {
    Variable_Reference variable = context_->get_ref (key, settings);
    result.deep_copy (context_->get (variable, settings));
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::get. Context not set correctly.\n"));
  }

  return result;
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Variables::get (const Variable_Reference & variable,
             const Knowledge_Reference_Settings & settings)
{
  Madara::Knowledge_Record result;

  if (context_)
  {
    result.deep_copy (context_->get (variable, settings));
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::get. Context not set correctly.\n"));
  }

  return result;
}

bool
Madara::Knowledge_Engine::Variables::exists (const std::string & key,
             const Knowledge_Reference_Settings & settings) const
{
  bool result (false);

  if (context_)
  {
    result = context_->exists (key, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::exists. Context not set correctly.\n"));
  }

  return result;
}

bool
Madara::Knowledge_Engine::Variables::exists (const Variable_Reference & variable,
             const Knowledge_Reference_Settings & settings) const
{
  bool result (false);

  if (context_)
  {
    result = context_->exists (variable, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::exists. Context not set correctly.\n"));
  }

  return result;
}

void
Madara::Knowledge_Engine::Variables::apply_modified (
  const Knowledge_Update_Settings & settings)
{
  if (context_)
    context_->apply_modified ();
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::apply_modified. Context not set correctly.\n"));
  }
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Variables::retrieve_index (
  const std::string & key,
  size_t index,
  const Knowledge_Reference_Settings & settings)
{
  if (context_)
  {
    Variable_Reference variable = context_->get_ref (key, settings);
    return context_->retrieve_index (variable, index, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::retrieve_index. Context not set correctly.\n"));

    return Madara::Knowledge_Record ();
  }
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Variables::retrieve_index (
  const Variable_Reference & variable,
  size_t index,
  const Knowledge_Reference_Settings & settings)
{
  if (context_)
    return context_->retrieve_index (variable, index, settings);
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::retrieve_index. Context not set correctly.\n"));

    return Madara::Knowledge_Record ();
  }
}

Madara::Knowledge_Engine::Variable_Reference
Madara::Knowledge_Engine::Variables::get_ref (const std::string & key,
             const Knowledge_Reference_Settings & settings)
{
  if (context_)
    return context_->get_ref (key, settings);
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::get_ref. Context not set correctly.\n"));

    return Madara::Knowledge_Engine::Variable_Reference ();
  }
}

int
Madara::Knowledge_Engine::Variables::set (const std::string & key,
                          Madara::Knowledge_Record::Integer value,
        const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    Variable_Reference variable = context_->get_ref (key, settings);
    return context_->set (variable, value, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

int
Madara::Knowledge_Engine::Variables::set (const Variable_Reference & variable,
                          Madara::Knowledge_Record::Integer value,
        const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    return context_->set (variable, value, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}


int
Madara::Knowledge_Engine::Variables::set (const std::string & key,
                          const Knowledge_Record::Integer * value,
                          uint32_t size,
        const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    Variable_Reference variable = context_->get_ref (key, settings);
    return context_->set (variable, value, size, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}
 
int
Madara::Knowledge_Engine::Variables::set (
  const Variable_Reference & variable,
  const Knowledge_Record::Integer * value,
  uint32_t size,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    return context_->set (variable, value, size, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}
 
int
Madara::Knowledge_Engine::Variables::set (
  const std::string & key,
  const std::vector <Knowledge_Record::Integer> & value,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    Variable_Reference variable = context_->get_ref (key, settings);
    return context_->set (variable, value, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

int
Madara::Knowledge_Engine::Variables::set (
  const Variable_Reference & variable,
  const std::vector <Knowledge_Record::Integer> & value,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    return context_->set (variable, value, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

int
Madara::Knowledge_Engine::Variables::set (
  const std::string & key,
  const Madara::Knowledge_Record & value,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    Variable_Reference variable = context_->get_ref (key, settings);
    return context_->set (variable, value, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

int
Madara::Knowledge_Engine::Variables::set (
  const Variable_Reference & variable,
  const Madara::Knowledge_Record & value,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    return context_->set (variable, value, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

int
Madara::Knowledge_Engine::Variables::set (
  const std::string & key,
  double value,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    Variable_Reference variable = context_->get_ref (key, settings);
    return context_->set (variable, value, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

int
Madara::Knowledge_Engine::Variables::set (
  const Variable_Reference & variable,
  double value,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    return context_->set (variable, value, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

int
Madara::Knowledge_Engine::Variables::set (
  const std::string & key,
  const double * value,
  uint32_t size,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    Variable_Reference variable = context_->get_ref (key, settings);
    return context_->set (variable, value, size, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

int
Madara::Knowledge_Engine::Variables::set (
  const Variable_Reference & variable,
  const double * value,
  uint32_t size,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    return context_->set (variable, value, size, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

int
Madara::Knowledge_Engine::Variables::set (
  const std::string & key,
  const std::vector <double> & value,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    Variable_Reference variable = context_->get_ref (key, settings);
    return context_->set (variable, value, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

int
Madara::Knowledge_Engine::Variables::set (
  const Variable_Reference & variable,
  const std::vector <double> & value,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    return context_->set (variable, value, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

int
Madara::Knowledge_Engine::Variables::set (const std::string & key,
                                          const std::string & value,
        const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    Variable_Reference variable = context_->get_ref (key, settings);
    return context_->set (variable, value, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

int
Madara::Knowledge_Engine::Variables::set (
  const Variable_Reference & variable,
  const std::string & value,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    return context_->set (variable, value, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::set. Context not set correctly.\n"));

    return -4;
  }
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Variables::inc (const std::string & key,
        const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    Variable_Reference variable = context_->get_ref (key, settings);
    return context_->inc (variable, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::inc. Context not set correctly.\n"));

    return Madara::Knowledge_Record ();
  }
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Variables::inc (
  const Variable_Reference & variable,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    return context_->inc (variable, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::inc. Context not set correctly.\n"));

    return Madara::Knowledge_Record ();
  }
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Variables::dec (
  const std::string & key,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    Variable_Reference variable = context_->get_ref (key, settings);
    return context_->dec (variable, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::dec. Context not set correctly.\n"));

    return Madara::Knowledge_Record ();
  }
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Variables::dec (
  const Variable_Reference & variable,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    return context_->dec (variable, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::dec. Context not set correctly.\n"));

    return Madara::Knowledge_Record ();
  }
}

void
Madara::Knowledge_Engine::Variables::to_string (
  std::string & target,
  const std::string & array_delimiter,
  const std::string & record_delimiter,
  const std::string & key_val_delimiter) const
{
  if (context_)
    context_->to_string (target,
      array_delimiter, record_delimiter, key_val_delimiter);
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::to_string. Context not set correctly.\n"));
}

void
Madara::Knowledge_Engine::Variables::print (unsigned int level) const
{
  if (context_)
    context_->print (level);
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::print. Context not set correctly.\n"));
}


void
Madara::Knowledge_Engine::Variables::print (const std::string & statement,
                                   unsigned int level) const
{
  if (context_)
    context_->print (statement, level);
  else
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::print. Context not set correctly.\n"));
}

std::string
Madara::Knowledge_Engine::Variables::expand_statement (
                                  const std::string & statement) const
{
  if (context_)
    return context_->expand_statement (statement);
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::expand_statement. Context not set correctly.\n"));

    return "";
  }
}


// Defines a function
void Madara::Knowledge_Engine::Variables::define_function (
  const std::string & name,
  Knowledge_Record (*func) (Function_Arguments &, Variables &))
{
  if (context_)
    return context_->define_function (name, func);
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::define_function. Context not set correctly.\n"));
  }
}

#ifdef _MADARA_JAVA_
// Defines a function
void Madara::Knowledge_Engine::Variables::define_function (
  const std::string & name, jobject func)
{
  if (context_)
    return context_->define_function (name, func);
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::define_function. Context not set correctly.\n"));
  }
}
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_

// Defines a function
void Madara::Knowledge_Engine::Variables::define_function (
  const std::string & name, boost::python::object callable)
{
  if (context_)
    return context_->define_function (name, callable);
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::define_function. Context not set correctly.\n"));
  }
}

#endif

/**
  * Defines a MADARA KaRL function
  * @param  name       name of the function
  * @param  expression KaRL function body       
  **/
void
Madara::Knowledge_Engine::Variables::define_function (const std::string & name,
  const std::string & expression)
{
  if (context_)
    return context_->define_function (name, expression);
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::define_function. Context not set correctly.\n"));
  }
}
      
/**
  * Defines a MADARA KaRL function
  * @param  name       name of the function
  * @param  expression KaRL function body       
  **/
void
Madara::Knowledge_Engine::Variables::define_function (const std::string & name,
  const Compiled_Expression & expression)
{
  if (context_)
    return context_->define_function (name, expression);
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::define_function. Context not set correctly.\n"));
  }
}
    

Madara::Knowledge_Engine::Compiled_Expression
Madara::Knowledge_Engine::Variables::compile (const std::string & expression)
{
  if (context_)
    return context_->compile (expression);
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::compile. Context not set correctly.\n"));

    return Compiled_Expression ();
  }
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Variables::evaluate (
  const std::string & expression,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    Compiled_Expression compiled = context_->compile (expression);
    return compiled.expression.evaluate (settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::evaluate. Context not set correctly.\n"));

    return Madara::Knowledge_Record::Integer (0);
  }
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Variables::evaluate (
  Compiled_Expression & expression,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    return expression.expression.evaluate (settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::evaluate. Context not set correctly.\n"));

    return Madara::Knowledge_Record::Integer (0);
  }
}

size_t
Madara::Knowledge_Engine::Variables::to_vector (
                              const std::string & subject,
                              unsigned int start,
                              unsigned int end,
                              std::vector <Knowledge_Record> & target)
{
  if (context_)
  {
    return context_->to_vector (subject, start, end, target);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::to_vector. Context not set correctly.\n"));

    return Madara::Knowledge_Record::Integer (0);
  }
}

size_t
Madara::Knowledge_Engine::Variables::to_map (
  const std::string & expression,
  std::map <std::string, Knowledge_Record> & target)
{
  if (context_)
  {
    return context_->to_map (expression, target);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::to_map. Context not set correctly.\n"));

    return Madara::Knowledge_Record::Integer (0);
  }
}



int64_t
Madara::Knowledge_Engine::Variables::save_context (
  const std::string & filename)
{
  if (context_)
  {
    return context_->save_context (filename);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::save_context. Context not set correctly.\n"));

    return Madara::Knowledge_Record::Integer (0);
  }
}

int64_t
Madara::Knowledge_Engine::Variables::save_checkpoint (
  const std::string & filename,
  bool reset_modifieds)
{
  if (context_)
  {
    int64_t result = context_->save_context (filename);

    if (reset_modifieds)
      context_->reset_modified ();

    return result;
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::save_checkpoint. Context not set correctly.\n"));

    return Madara::Knowledge_Record::Integer (0);
  }
}
      
int64_t
Madara::Knowledge_Engine::Variables::load_context (
  const std::string & filename,
  const Knowledge_Update_Settings & settings)
{
  if (context_)
  {
    std::string id;
    return context_->load_context (filename, id, settings);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::load_context. Context not set correctly.\n"));

    return Madara::Knowledge_Record::Integer (0);
  }
}
     
ssize_t
Madara::Knowledge_Engine::Variables::write_file (
  const std::string & knowledge_key, 
  const std::string & filename)
{
  if (context_)
  {
    return context_->get_record (knowledge_key)->to_file (filename);;
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
      "Variables::write_file. Context not set correctly.\n"));

    return 0;
  }
}


#endif