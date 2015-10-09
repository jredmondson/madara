#include "madara/utility/Utility.h"
#include "madara/knowledge/KnowledgeBaseImpl.h"
#include "madara/expression/Interpreter.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/transport/udp/UDPTransport.h"
#include "madara/transport/tcp/TCPTransport.h"
#include "madara/transport/multicast/MulticastTransport.h"
#include "madara/transport/broadcast/BroadcastTransport.h"


#include <sstream>

#include "ace/OS_NS_Thread.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_sys_socket.h"

#ifdef _USE_OPEN_SPLICE_
#include "madara/transport/splice/SpliceDDSTransport.h"
#endif // _USE_OPEN_SPLICE_

#ifdef _USE_NDDS_
#include "madara/transport/ndds/NDDSTransport.h"
#endif // _USE_NDDS_

#include <iostream>

madara::knowledge::KnowledgeBaseImpl::KnowledgeBaseImpl ()
  : settings_ (), files_ (map_)
{
  //activate_transport ();
  // no hope of transporting, so don't setup uniquehostport
}

madara::knowledge::KnowledgeBaseImpl::KnowledgeBaseImpl (
  const std::string & host, int transport)
  : settings_ (), files_ (map_)
{
  // override default settings for the arguments
  settings_.type = transport;

  id_ = setup_unique_hostport (host);
  activate_transport ();
}

madara::knowledge::KnowledgeBaseImpl::KnowledgeBaseImpl (
  const std::string & host, int transport,
  const std::string & knowledge_domain)
  : settings_ (), files_ (map_)
{
  // override default settings for the arguments
  settings_.type = transport;
  settings_.domains = knowledge_domain;

  id_ = setup_unique_hostport (host);
  activate_transport ();
}

madara::knowledge::KnowledgeBaseImpl::KnowledgeBaseImpl (
  const std::string & host, const madara::transport::Settings & config)
  : settings_ (config), files_ (map_)
{
  id_ = setup_unique_hostport (host);
  if (!settings_.delay_launch)
    activate_transport ();
}

madara::knowledge::KnowledgeBaseImpl::~KnowledgeBaseImpl ()
{
  close_transport ();
  unique_bind_.close ();
}

std::string
madara::knowledge::KnowledgeBaseImpl::setup_unique_hostport (
const std::string & host)
{
  // placeholder for our ip address
  std::string actual_host (host);

  if (host == "")
  {
    // start from 50k, which is just above the bottom of the user
    // definable port range (hopefully avoid conflicts with 49152-49999
    unsigned short port = 50000;

    if (madara::utility::bind_to_ephemeral_port (
      unique_bind_, actual_host, port)
      == -1)
    {
      madara_logger_log (map_.get_logger (), logger::LOG_ERROR,
        "KnowledgeBaseImpl::setup_unique_hostport:" \
        " unable to bind to any ephemeral port\n");

      if (!settings_.never_exit)
        exit (-1);
    }

    // we were able to bind to an ephemeral port
    madara::utility::merge_hostport_identifier (actual_host, actual_host, port);

    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::setup_unique_hostport:" \
      " unique bind to %s\n", actual_host.c_str ());
  }

  return actual_host;
}

size_t
madara::knowledge::KnowledgeBaseImpl::attach_transport (const std::string & id,
transport::Settings & settings)
{
  madara::transport::Base * transport (0);
  std::string originator (id);

  if (originator == "")
  {
    if (id_.size () > 0)
      originator = id_;
    else
      originator = id_ = setup_unique_hostport ();
  }

  madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
    "KnowledgeBaseImpl::attach_transport:" \
    " activating transport type %d\n", settings.type);

  if (settings.type == madara::transport::BROADCAST)
  {
    transport = new madara::transport::BroadcastTransport (originator, map_,
      settings, true);
  }
  else if (settings.type == madara::transport::MULTICAST)
  {
    transport = new madara::transport::MulticastTransport (originator, map_,
      settings, true);
  }
  else if (settings.type == madara::transport::SPLICE)
  {
#ifdef _USE_OPEN_SPLICE_
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " creating Open Splice DDS transport.\n");

    transport = new madara::transport::SpliceDDSTransport (originator, map_,
      settings, true);
#else
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " project was not generated with opensplice=1. Transport is invalid.\n");
#endif
  }
  else if (settings.type == madara::transport::NDDS)
  {
#ifdef _USE_NDDS_
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " creating NDDS transport.\n");

    transport = new madara::transport::NDDSTransport (originator, map_,
      settings, true);
#else
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " project was not generated with ndds=1. Transport is invalid.\n");
#endif
  }
  else if (settings.type == madara::transport::UDP)
  {
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " creating UDP transport.\n");

    transport = new madara::transport::UDPTransport (originator, map_,
      settings, true);
  }
  else if (settings.type == madara::transport::TCP)
  {
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " creating TCP transport.\n");

    transport = new madara::transport::TCPTransport (originator, map_,
      settings, true);
  }
  else
  {
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " no transport was specified. Setting transport to null.\n");
  }

  // if we have a valid transport, add it to the transports vector
  if (transport != 0)
  {
    transports_.push_back (transport);
  }

  return transports_.size ();
}

void
madara::knowledge::KnowledgeBaseImpl::activate_transport (void)
{
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

void
madara::knowledge::KnowledgeBaseImpl::copy (
const KnowledgeBaseImpl & source,
const CopySet & copy_set,
bool clean_copy)
{
  map_.copy (source.map_, copy_set, clean_copy);
}

void
madara::knowledge::KnowledgeBaseImpl::lock (void)
{
  map_.lock ();
}

void
madara::knowledge::KnowledgeBaseImpl::unlock (void)
{
  map_.unlock ();
}

void
madara::knowledge::KnowledgeBaseImpl::close_transport (void)
{
  if (transports_.size () > 0)
  {
    for (unsigned int i = 0; i < transports_.size (); ++i)
    {
      transports_[i]->close ();
      delete transports_[i];

      madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::close_transport:" \
        " transport has been closed\n");
    }

    transports_.clear ();
  }
}

/**
 * Updates all global variables to current clock and then
 * sends them if a transport is available. This is useful
 * when trying to synchronize to late joiners (this process
 * will resend all global variables.
 **/
int
madara::knowledge::KnowledgeBaseImpl::apply_modified (
const EvalSettings & settings)
{
  // lock the context and apply modified flags and current clock to
  // all global variables
  map_.lock ();
  map_.apply_modified ();

  int ret = 0;

  send_modifieds ("KnowledgeBaseImpl:apply_modified", settings);

  map_.unlock ();

  return ret;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const std::string & key,
madara::KnowledgeRecord::Integer value,
const EvalSettings & settings)
{
  int result = map_.set (key, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

void
madara::knowledge::KnowledgeBaseImpl::mark_modified (
const VariableReference & variable)
{
  map_.mark_modified (variable);
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const VariableReference & variable,
madara::KnowledgeRecord::Integer value,
const EvalSettings & settings)
{
  int result = map_.set (variable, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set_index (
const std::string & key,
size_t index,
madara::KnowledgeRecord::Integer value,
const EvalSettings & settings)
{
  int result = map_.set_index (key, index, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set_index", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set_index (
const VariableReference & variable,
size_t index,
madara::KnowledgeRecord::Integer value,
const EvalSettings & settings)
{
  int result = map_.set_index (variable, index, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set_index", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const std::string & key,
const KnowledgeRecord::Integer * value,
uint32_t size,
const EvalSettings & settings)
{
  int result = map_.set (key, value, size, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const VariableReference & variable,
const KnowledgeRecord::Integer * value,
uint32_t size,
const EvalSettings & settings)
{
  int result = map_.set (variable, value, size, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const std::string & key,
const std::vector <KnowledgeRecord::Integer> & value,
const EvalSettings & settings)
{
  int result = map_.set (key, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const VariableReference & variable,
const std::vector <KnowledgeRecord::Integer> & value,
const EvalSettings & settings)
{
  int result = map_.set (variable, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const std::string & key,
double value,
const EvalSettings & settings)
{
  int result = map_.set (key, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const VariableReference & variable,
double value,
const EvalSettings & settings)
{
  int result = map_.set (variable, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set_index (
const std::string & key,
size_t index,
double value,
const EvalSettings & settings)
{
  int result = map_.set_index (key, index, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set_index", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set_index (
const VariableReference & variable,
size_t index,
double value,
const EvalSettings & settings)
{
  int result = map_.set_index (variable, index, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set_index", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const std::string & key,
const double * value,
uint32_t size,
const EvalSettings & settings)
{
  int result = map_.set (key, value, size, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const VariableReference & variable,
const double * value,
uint32_t size,
const EvalSettings & settings)
{
  int result = map_.set (variable, value, size, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const std::string & key,
const std::vector <double> & value,
const EvalSettings & settings)
{
  int result = map_.set (key, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const VariableReference & variable,
const std::vector <double> & value,
const EvalSettings & settings)
{
  int result = map_.set (variable, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const std::string & key,
const std::string & value,
const EvalSettings & settings)
{
  int result = map_.set (key, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

int
madara::knowledge::KnowledgeBaseImpl::set (
const VariableReference & variable,
const std::string & value,
const EvalSettings & settings)
{
  int result = map_.set (variable, value, settings);

  send_modifieds ("KnowledgeBaseImpl:set", settings);

  return result;
}

/// Read a file into the knowledge base
int
madara::knowledge::KnowledgeBaseImpl::read_file (
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
int
madara::knowledge::KnowledgeBaseImpl::set_file (
const VariableReference & variable,
const unsigned char * value, size_t size,
const EvalSettings & settings)
{
  int result = map_.set_file (variable, value, size, settings);

  send_modifieds ("KnowledgeBaseImpl:set_file", settings);

  return result;
}

/// Read a file into the knowledge base
int
madara::knowledge::KnowledgeBaseImpl::set_jpeg (
const VariableReference & variable,
const unsigned char * value, size_t size,
const EvalSettings & settings)
{
  int result = map_.set_jpeg (variable, value, size, settings);

  send_modifieds ("KnowledgeBaseImpl:set_jpeg", settings);

  return result;
}

/// Read a file into the knowledge base
int
madara::knowledge::KnowledgeBaseImpl::read_file (
const VariableReference & variable,
const std::string & filename,
const EvalSettings & settings)
{
  int result = map_.read_file (variable, filename, settings);

  send_modifieds ("KnowledgeBaseImpl:read_file", settings);

  return result;
}

#ifndef _MADARA_NO_KARL_

madara::knowledge::CompiledExpression
madara::knowledge::KnowledgeBaseImpl::compile (
const std::string & expression)
{
  madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
    "KnowledgeBaseImpl::compile:" \
    " compiling %s\n", expression.c_str ());

  return map_.compile (expression);
}

madara::KnowledgeRecord
madara::knowledge::KnowledgeBaseImpl::wait (const std::string & expression,
const WaitSettings & settings)
{
  CompiledExpression compiled = compile (expression);
  return wait (compiled, settings);
}

madara::KnowledgeRecord
madara::knowledge::KnowledgeBaseImpl::wait (
CompiledExpression & ce,
const WaitSettings & settings)
{
  // get current time of day
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
    map_.print (settings.pre_print_statement, logger::LOG_EMERGENCY);

  // lock the context
  map_.lock ();

  madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
    "KnowledgeBaseImpl::wait:" \
    " waiting on %s\n", ce.logic.c_str ());

  madara::KnowledgeRecord last_value = ce.expression.evaluate (settings);

  madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
    "KnowledgeBaseImpl::wait:" \
    " completed first eval to get %s\n",
    last_value.to_string ().c_str ());

  send_modifieds ("KnowledgeBaseImpl:wait", settings);

  map_.unlock ();

  current = ACE_High_Res_Timer::gettimeofday ();

  // wait for expression to be true
  while (!last_value.to_integer () &&
    (settings.max_wait_time < 0 || current < max_wait))
  {
    madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
      "KnowledgeBaseImpl::wait:" \
      " current is %" PRIu64 ".%" PRIu64 " and max is %" PRIu64 ".%" PRIu64 " (poll freq is %f)\n",
      current.sec (), current.usec (), max_wait.sec (), max_wait.usec (),
      settings.poll_frequency);

    madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
      "KnowledgeBaseImpl::wait:" \
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
      map_.wait_for_change (true);

    // relock - basically we need to evaluate the tree again, and
    // we can't have a bunch of people changing the variables as 
    // while we're evaluating the tree.
    map_.lock ();


    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::wait:" \
      " waiting on %s\n", ce.logic.c_str ());

    last_value = ce.expression.evaluate (settings);

    madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
      "KnowledgeBaseImpl::wait:" \
      " completed eval to get %s\n",
      last_value.to_string ().c_str ());

    send_modifieds ("KnowledgeBaseImpl:wait", settings);

    map_.unlock ();
    map_.signal ();

    // get current time
    current = ACE_High_Res_Timer::gettimeofday ();

  } // end while (!last)

  if (current >= max_wait)
  {
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::wait:" \
      " Evaluate did not succeed. Timeout occurred\n");
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    map_.print (settings.post_print_statement, logger::LOG_ALWAYS);

  return last_value;
}

madara::KnowledgeRecord
madara::knowledge::KnowledgeBaseImpl::evaluate (
const std::string & expression,
const EvalSettings & settings)
{
  CompiledExpression compiled = compile (expression);
  return evaluate (compiled, settings);
}

madara::KnowledgeRecord
madara::knowledge::KnowledgeBaseImpl::evaluate (
CompiledExpression & ce,
const EvalSettings & settings)
{
  madara::KnowledgeRecord last_value;

  madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
    "KnowledgeBaseImpl::evaluate:" \
    " evaluating %s.\n", ce.logic.c_str ());

  // iterators and tree for evaluation of interpreter results
  //madara::expression::ExpressionTree tree;

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    map_.print (settings.pre_print_statement, logger::LOG_ALWAYS);

  // lock the context from being updated by any ongoing threads
  map_.lock ();

  // interpret the current expression and then evaluate it
  //tree = interpreter_.interpret (map_, expression);
  last_value = ce.expression.evaluate (settings);

  send_modifieds ("KnowledgeBaseImpl:evaluate", settings);

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    map_.print (settings.post_print_statement, logger::LOG_ALWAYS);

  map_.unlock ();

  return last_value;
}

madara::KnowledgeRecord
madara::knowledge::KnowledgeBaseImpl::evaluate (
expression::ComponentNode * root,
const EvalSettings & settings)
{
  madara::KnowledgeRecord last_value;

  madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
    "KnowledgeBaseImpl::evaluate:" \
    " evaluating ComponentNode rooted tree\n");

  // iterators and tree for evaluation of interpreter results
  //madara::expression::ExpressionTree tree;

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    map_.print (settings.pre_print_statement, logger::LOG_ALWAYS);

  // lock the context from being updated by any ongoing threads
  map_.lock ();

  // interpret the current expression and then evaluate it
  //tree = interpreter_.interpret (map_, expression);
  last_value = map_.evaluate (root, settings);

  send_modifieds ("KnowledgeBaseImpl:evaluate", settings);

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    map_.print (settings.post_print_statement, logger::LOG_ALWAYS);

  map_.unlock ();

  return last_value;
}

#endif // _MADARA_NO_KARL_
