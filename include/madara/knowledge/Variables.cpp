#ifndef _MADARA_EXTERNAL_FUNCTION_VARIABLES_CPP_
#define _MADARA_EXTERNAL_FUNCTION_VARIABLES_CPP_

#include "madara/logger/GlobalLogger.h"
#include "madara/knowledge/Variables.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeRecordFilters.h"

void madara::knowledge::Variables::operator=(Variables& rhs)
{
  if(this != &rhs || context_ != rhs.context_)
  {
    context_ = rhs.context_;
  }
}

madara::knowledge::ThreadSafeContext* madara::knowledge::Variables::get_context(
    void)
{
  return context_;
}

madara::knowledge::KnowledgeRecord madara::knowledge::Variables::get(
    const std::string& key, const KnowledgeReferenceSettings& settings)
{
  madara::knowledge::KnowledgeRecord result;

  if(context_)
  {
    VariableReference variable = context_->get_ref(key, settings);
    result = context_->get(variable, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::get. Context not set correctly\n");
  }

  return result;
}

madara::knowledge::KnowledgeRecord madara::knowledge::Variables::get(
    const VariableReference& variable,
    const KnowledgeReferenceSettings& settings)
{
  madara::knowledge::KnowledgeRecord result;

  if(context_)
  {
    result = context_->get(variable, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::get. Context not set correctly\n");
  }

  return result;
}

bool madara::knowledge::Variables::exists(
    const std::string& key, const KnowledgeReferenceSettings& settings) const
{
  bool result(false);

  if(context_)
  {
    result = context_->exists(key, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::exists. Context not set correctly\n");
  }

  return result;
}

bool madara::knowledge::Variables::exists(const VariableReference& variable,
    const KnowledgeReferenceSettings& settings) const
{
  bool result(false);

  if(context_)
  {
    result = context_->exists(variable, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::exists. Context not set correctly\n");
  }

  return result;
}

void madara::knowledge::Variables::apply_modified(
    const KnowledgeUpdateSettings& /*settings*/)
{
  if(context_)
  {
    context_->apply_modified();
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::apply_modified. Context not set correctly\n");
  }
}

madara::knowledge::KnowledgeRecord madara::knowledge::Variables::retrieve_index(
    const std::string& key, size_t index,
    const KnowledgeReferenceSettings& settings)
{
  if(context_)
  {
    VariableReference variable = context_->get_ref(key, settings);
    return context_->retrieve_index(variable, index, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::retrieve_index. Context not set correctly\n");

    return madara::knowledge::KnowledgeRecord();
  }
}

madara::knowledge::KnowledgeRecord madara::knowledge::Variables::retrieve_index(
    const VariableReference& variable, size_t index,
    const KnowledgeReferenceSettings& settings)
{
  if(context_)
  {
    return context_->retrieve_index(variable, index, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::retrieve_index. Context not set correctly\n");

    return madara::knowledge::KnowledgeRecord();
  }
}

madara::knowledge::VariableReference madara::knowledge::Variables::get_ref(
    const std::string& key, const KnowledgeReferenceSettings& settings)
{
  if(context_)
  {
    return context_->get_ref(key, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::get_ref. Context not set correctly\n");

    return madara::knowledge::VariableReference();
  }
}

int madara::knowledge::Variables::set(const std::string& key,
    madara::knowledge::KnowledgeRecord::Integer value,
    const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    VariableReference variable = context_->get_ref(key, settings);
    return context_->set(variable, value, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const VariableReference& variable,
    madara::knowledge::KnowledgeRecord::Integer value,
    const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    return context_->set(variable, value, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const std::string& key,
    const knowledge::KnowledgeRecord::Integer* value, uint32_t size,
    const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    VariableReference variable = context_->get_ref(key, settings);
    return context_->set(variable, value, size, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const VariableReference& variable,
    const knowledge::KnowledgeRecord::Integer* value, uint32_t size,
    const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    return context_->set(variable, value, size, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const std::string& key,
    const std::vector<KnowledgeRecord::Integer>& value,
    const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    VariableReference variable = context_->get_ref(key, settings);
    return context_->set(variable, value, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const VariableReference& variable,
    const std::vector<KnowledgeRecord::Integer>& value,
    const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    return context_->set(variable, value, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const std::string& key,
    const madara::knowledge::KnowledgeRecord& value,
    const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    VariableReference variable = context_->get_ref(key, settings);
    return context_->set(variable, value, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const VariableReference& variable,
    const madara::knowledge::KnowledgeRecord& value,
    const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    return context_->set(variable, value, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const std::string& key, double value,
    const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    VariableReference variable = context_->get_ref(key, settings);
    return context_->set(variable, value, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const VariableReference& variable,
    double value, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    return context_->set(variable, value, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const std::string& key,
    const double* value, uint32_t size, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    VariableReference variable = context_->get_ref(key, settings);
    return context_->set(variable, value, size, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const VariableReference& variable,
    const double* value, uint32_t size, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    return context_->set(variable, value, size, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const std::string& key,
    const std::vector<double>& value, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    VariableReference variable = context_->get_ref(key, settings);
    return context_->set(variable, value, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const VariableReference& variable,
    const std::vector<double>& value, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    return context_->set(variable, value, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const std::string& key,
    const std::string& value, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    VariableReference variable = context_->get_ref(key, settings);
    return context_->set(variable, value, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

int madara::knowledge::Variables::set(const VariableReference& variable,
    const std::string& value, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    return context_->set(variable, value, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::set. Context not set correctly\n");

    return -4;
  }
}

madara::knowledge::KnowledgeRecord madara::knowledge::Variables::inc(
    const std::string& key, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    VariableReference variable = context_->get_ref(key, settings);
    return context_->inc(variable, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::inc. Context not set correctly\n");

    return madara::knowledge::KnowledgeRecord();
  }
}

madara::knowledge::KnowledgeRecord madara::knowledge::Variables::inc(
    const VariableReference& variable, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    return context_->inc(variable, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::inc. Context not set correctly\n");

    return madara::knowledge::KnowledgeRecord();
  }
}

madara::knowledge::KnowledgeRecord madara::knowledge::Variables::dec(
    const std::string& key, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    VariableReference variable = context_->get_ref(key, settings);
    return context_->dec(variable, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::dec. Context not set correctly\n");

    return madara::knowledge::KnowledgeRecord();
  }
}

madara::knowledge::KnowledgeRecord madara::knowledge::Variables::dec(
    const VariableReference& variable, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    return context_->dec(variable, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::dec. Context not set correctly\n");

    return madara::knowledge::KnowledgeRecord();
  }
}

void madara::knowledge::Variables::to_string(std::string& target,
    const std::string& array_delimiter, const std::string& record_delimiter,
    const std::string& key_val_delimiter) const
{
  if(context_)
  {
    context_->to_string(
        target, array_delimiter, record_delimiter, key_val_delimiter);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::to_string. Context not set correctly\n");
  }
}

void madara::knowledge::Variables::print(unsigned int level) const
{
  if(context_)
  {
    context_->print(level);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::print. Context not set correctly\n");
  }
}

void madara::knowledge::Variables::print(
    const std::string& statement, unsigned int level) const
{
  if(context_)
  {
    context_->print(statement, level);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::print. Context not set correctly\n");
  }
}

std::string madara::knowledge::Variables::expand_statement(
    const std::string& statement) const
{
  if(context_)
  {
    return context_->expand_statement(statement);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::expand_statement. Context not set correctly\n");

    return "";
  }
}

#ifndef _MADARA_NO_KARL_

// Defines a function
void madara::knowledge::Variables::define_function(const std::string& name,
    knowledge::KnowledgeRecord (*func)(FunctionArguments&, Variables&))
{
  if(context_)
  {
    return context_->define_function(name, func);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::define_function. Context not set correctly\n");
  }
}

#ifdef _MADARA_JAVA_
// Defines a function
void madara::knowledge::Variables::define_function(
    const std::string& name, jobject func)
{
  if(context_)
  {
    return context_->define_function(name, func);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::define_function. Context not set correctly\n");
  }
}
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_

// Defines a function
void madara::knowledge::Variables::define_function(
    const std::string& name, boost::python::object callable)
{
  if(context_)
  {
    return context_->define_function(name, callable);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::define_function. Context not set correctly\n");
  }
}

#endif

/**
 * Defines a MADARA KaRL function
 * @param  name       name of the function
 * @param  expression KaRL function body
 **/
void madara::knowledge::Variables::define_function(
    const std::string& name, const std::string& expression)
{
  if(context_)
  {
    return context_->define_function(name, expression);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::define_function. Context not set correctly\n");
  }
}

/**
 * Defines a MADARA KaRL function
 * @param  name       name of the function
 * @param  expression KaRL function body
 **/
void madara::knowledge::Variables::define_function(
    const std::string& name, const CompiledExpression& expression)
{
  if(context_)
  {
    return context_->define_function(name, expression);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::define_function. Context not set correctly\n");
  }
}

madara::knowledge::CompiledExpression madara::knowledge::Variables::compile(
    const std::string& expression)
{
  if(context_)
  {
    return context_->compile(expression);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::compile. Context not set correctly\n");

    return CompiledExpression();
  }
}

madara::knowledge::KnowledgeRecord madara::knowledge::Variables::evaluate(
    const std::string& expression, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    CompiledExpression compiled = context_->compile(expression);
    return compiled.expression.evaluate(settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::evaluate. Context not set correctly\n");

    return madara::knowledge::KnowledgeRecord(0);
  }
}

madara::knowledge::KnowledgeRecord madara::knowledge::Variables::evaluate(
    CompiledExpression& expression, const KnowledgeUpdateSettings& settings)
{
  if(context_)
  {
    return expression.expression.evaluate(settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::evaluate. Context not set correctly\n");

    return madara::knowledge::KnowledgeRecord(0);
  }
}

#endif  // _MADARA_NO_KARL_

size_t madara::knowledge::Variables::to_vector(const std::string& subject,
    unsigned int start, unsigned int end, std::vector<KnowledgeRecord>& target)
{
  size_t result(0);

  if(context_)
  {
    result = context_->to_vector(subject, start, end, target);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::to_vector. Context not set correctly\n");
  }

  return result;
}

void madara::knowledge::Variables::get_matches(const std::string& prefix,
    const std::string& suffix, VariableReferences& matches)
{
  if(context_)
  {
    context_->get_matches(prefix, suffix, matches);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::get_matches: Context not set correctly\n");
  }
}

size_t madara::knowledge::Variables::to_map(const std::string& expression,
    std::map<std::string, knowledge::KnowledgeRecord>& target)
{
  size_t result(0);

  if(context_)
  {
    result = context_->to_map(expression, target);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::to_map. Context not set correctly\n");
  }

  return result;
}

int64_t madara::knowledge::Variables::save_context(const std::string& filename)
{
  int64_t result(0);

  if(context_)
  {
    result = context_->save_context(filename);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::save_context. Context not set correctly\n");
  }

  return result;
}

int64_t madara::knowledge::Variables::save_as_karl(
    const std::string& filename) const
{
  int64_t result(0);

  if(context_)
  {
    result = context_->save_as_karl(filename);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::save_as_karl. Context not set correctly\n");
  }

  return result;
}

int64_t madara::knowledge::Variables::save_checkpoint(
    const std::string& filename, bool reset_modifieds)
{
  int64_t result(0);

  if(context_)
  {
    result = context_->save_context(filename);

    if(reset_modifieds)
      context_->reset_modified();
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::save_checkpoint. Context not set correctly\n");
  }

  return result;
}

int64_t madara::knowledge::Variables::load_context(
    const std::string& filename, const KnowledgeUpdateSettings& settings)
{
  int64_t result(0);

  if(context_)
  {
    std::string id;
    result = context_->load_context(filename, id, settings);
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::load_context. Context not set correctly\n");
  }

  return result;
}

ssize_t madara::knowledge::Variables::write_file(
    const std::string& knowledge_key, const std::string& filename)
{
  if(context_)
  {
    return context_->get_record(knowledge_key)->to_file(filename);
    ;
  }
  else
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_EMERGENCY,
        "Variables::write_file. Context not set correctly\n");

    return 0;
  }
}

#endif
