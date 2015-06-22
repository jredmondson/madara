#include "madara/utility/Utility.h"
#include "madara/knowledge_engine/Knowledge_Base_Impl.h"
#include "madara/expression_tree/Interpreter.h"
#include "madara/expression_tree/Expression_Tree.h"
#include "madara/transport/udp/UDP_Transport.h"
#include "madara/transport/tcp/TCP_Transport.h"
#include "madara/transport/multicast/Multicast_Transport.h"
#include "madara/transport/broadcast/Broadcast_Transport.h"


#include <sstream>

#include "ace/OS_NS_Thread.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_sys_socket.h"

#ifdef _USE_OPEN_SPLICE_
  #include "madara/transport/splice/Splice_DDS_Transport.h"
#endif // _USE_OPEN_SPLICE_

#ifdef _USE_NDDS_
  #include "madara/transport/ndds/NDDS_Transport.h"
#endif // _USE_NDDS_

#include <iostream>

Madara::Knowledge_Engine::Knowledge_Base_Impl::Knowledge_Base_Impl ()
: settings_ (), files_ (map_)
{
  //activate_transport ();
  // no hope of transporting, so don't setup uniquehostport
}

Madara::Knowledge_Engine::Knowledge_Base_Impl::Knowledge_Base_Impl (
  const std::string & host, int transport)
: settings_ (), files_ (map_)
{
  // override default settings for the arguments
  settings_.type = transport;

  id_ = setup_unique_hostport (host);
  activate_transport ();
}

Madara::Knowledge_Engine::Knowledge_Base_Impl::Knowledge_Base_Impl (
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

Madara::Knowledge_Engine::Knowledge_Base_Impl::Knowledge_Base_Impl (
  const std::string & host, const Madara::Transport::Settings & config)
: settings_ (config), files_ (map_)
{
  id_ = setup_unique_hostport (host);
  if (!settings_.delay_launch)
    activate_transport ();
}

Madara::Knowledge_Engine::Knowledge_Base_Impl::~Knowledge_Base_Impl ()
{
  close_transport ();
  unique_bind_.close ();
}

std::string
Madara::Knowledge_Engine::Knowledge_Base_Impl::setup_unique_hostport (
  const std::string & host)
{
  // placeholder for our ip address
  std::string actual_host (host);

  if (host == "")
  {
    // start from 50k, which is just above the bottom of the user
    // definable port range (hopefully avoid conflicts with 49152-49999
    unsigned short port =  50000;

    if (Madara::Utility::bind_to_ephemeral_port (
      unique_bind_, actual_host, port)
         == -1)
    {
      map_.get_logger ().log (Logger::LOG_ERROR,
        "Knowledge_Base_Impl::setup_unique_hostport:" \
        " unable to bind to any ephemeral port\n");

      if (!settings_.never_exit)
        exit (-1);
    }

    // we were able to bind to an ephemeral port
    Madara::Utility::merge_hostport_identifier (actual_host, actual_host, port);

    map_.get_logger ().log (Logger::LOG_MAJOR,
      "Knowledge_Base_Impl::setup_unique_hostport:" \
      " unique bind to %s\n", actual_host.c_str ());
  }

  return actual_host;
}

size_t
Madara::Knowledge_Engine::Knowledge_Base_Impl::attach_transport (const std::string & id,
  Transport::Settings & settings)
{
  Madara::Transport::Base * transport (0);
  std::string originator (id);

  if (originator == "")
  {
    if (id_.size () > 0)
      originator = id_;
    else
      originator = id_ = setup_unique_hostport ();
  }

  map_.get_logger ().log (Logger::LOG_MAJOR,
    "Knowledge_Base_Impl::attach_transport:" \
    " activating transport type %d\n", settings.type);

  if (settings.type == Madara::Transport::BROADCAST)
  {
    transport = new Madara::Transport::Broadcast_Transport (originator, map_,
      settings, true);
  }
  else if (settings.type == Madara::Transport::MULTICAST)
  {
    transport = new Madara::Transport::Multicast_Transport (originator, map_,
      settings, true);
  }
  else if (settings.type == Madara::Transport::SPLICE)
  {
#ifdef _USE_OPEN_SPLICE_
    map_.get_logger ().log (Logger::LOG_MAJOR,
      "Knowledge_Base_Impl::activate_transport:" \
      " creating Open Splice DDS transport.\n");

    transport = new Madara::Transport::Splice_DDS_Transport (originator, map_,
                        settings, true);
#else
    map_.get_logger ().log (Logger::LOG_MAJOR,
      "Knowledge_Base_Impl::activate_transport:" \
      " project was not generated with opensplice=1. Transport is invalid.\n");
  #endif
  }
  else if (settings.type == Madara::Transport::NDDS)
  {
#ifdef _USE_NDDS_
    map_.get_logger ().log (Logger::LOG_MAJOR,
      "Knowledge_Base_Impl::activate_transport:" \
      " creating NDDS transport.\n");

    transport = new Madara::Transport::NDDS_Transport (originator, map_,
                        settings, true);
#else
    map_.get_logger ().log (Logger::LOG_MAJOR,
      "Knowledge_Base_Impl::activate_transport:" \
      " project was not generated with ndds=1. Transport is invalid.\n");
  #endif
  }
  else if (settings.type == Madara::Transport::UDP)
  {
    map_.get_logger ().log (Logger::LOG_MAJOR,
      "Knowledge_Base_Impl::activate_transport:" \
      " creating UDP transport.\n");

    transport = new Madara::Transport::UDP_Transport (originator, map_,
      settings, true);
  }
  else if (settings.type == Madara::Transport::TCP)
  {
    map_.get_logger ().log (Logger::LOG_MAJOR,
      "Knowledge_Base_Impl::activate_transport:" \
      " creating TCP transport.\n");

    transport = new Madara::Transport::TCP_Transport (originator, map_,
      settings, true);
  }
  else
  {
    map_.get_logger ().log (Logger::LOG_MAJOR,
      "Knowledge_Base_Impl::activate_transport:" \
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
Madara::Knowledge_Engine::Knowledge_Base_Impl::activate_transport (void)
{
  if (transports_.size () == 0)
  {
    attach_transport (id_, settings_);
  }
  else
  {
    map_.get_logger ().log (Logger::LOG_MAJOR,
      "Knowledge_Base_Impl::activate_transport:" \
      " transport already activated. If you need" \
      " a new type, close transport first\n");
  }
}

void
Madara::Knowledge_Engine::Knowledge_Base_Impl::copy (
  const Knowledge_Base_Impl & source,
        const Copy_Set & copy_set,
        bool clean_copy)
{
  map_.copy (source.map_, copy_set, clean_copy);
}

void
Madara::Knowledge_Engine::Knowledge_Base_Impl::lock (void)
{
  map_.lock ();
}

void
Madara::Knowledge_Engine::Knowledge_Base_Impl::unlock (void)
{
  map_.unlock ();
}

void
Madara::Knowledge_Engine::Knowledge_Base_Impl::close_transport (void)
{
  if (transports_.size () > 0)
  {
    for (unsigned int i = 0; i < transports_.size (); ++i)
    {
      transports_[i]->close ();
      delete transports_[i];

      map_.get_logger ().log (Logger::LOG_MAJOR,
        "Knowledge_Base_Impl::close_transport:" \
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
Madara::Knowledge_Engine::Knowledge_Base_Impl::apply_modified (
  const Eval_Settings & settings)
{
  // lock the context and apply modified flags and current clock to
  // all global variables
  map_.lock ();
  map_.apply_modified ();

  int ret = 0;
  
  send_modifieds ("Knowledge_Base_Impl:apply_modified", settings);

  map_.unlock ();

  return ret;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const std::string & key, 
  Madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
{
  int result = map_.set (key, value, settings);

  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

void
Madara::Knowledge_Engine::Knowledge_Base_Impl::mark_modified (
  const Variable_Reference & variable)
{
  map_.mark_modified (variable);
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const Variable_Reference & variable, 
  Madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
{
  int result = map_.set (variable, value, settings);

  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set_index (
  const std::string & key,
  size_t index,
  Madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
{
  int result = map_.set_index (key, index, value, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set_index", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set_index (
  const Variable_Reference & variable,
  size_t index,
  Madara::Knowledge_Record::Integer value,
  const Eval_Settings & settings)
{
  int result = map_.set_index (variable, index, value, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set_index", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const std::string & key, 
  const Knowledge_Record::Integer * value,
  uint32_t size,
  const Eval_Settings & settings)
{
  int result = map_.set (key, value, size, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const Variable_Reference & variable, 
  const Knowledge_Record::Integer * value,
  uint32_t size,
  const Eval_Settings & settings)
{
  int result = map_.set (variable, value, size, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const std::string & key, 
  const std::vector <Knowledge_Record::Integer> & value,
  const Eval_Settings & settings)
{
  int result = map_.set (key, value, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const Variable_Reference & variable, 
  const std::vector <Knowledge_Record::Integer> & value,
  const Eval_Settings & settings)
{
  int result = map_.set (variable, value, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const std::string & key, 
  double value, 
  const Eval_Settings & settings)
{
  int result = map_.set (key, value, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const Variable_Reference & variable, 
  double value, 
  const Eval_Settings & settings)
{
  int result = map_.set (variable, value, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set_index (
  const std::string & key,
  size_t index,
  double value,
  const Eval_Settings & settings)
{
  int result = map_.set_index (key, index, value, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set_index", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set_index (
  const Variable_Reference & variable,
  size_t index,
  double value,
  const Eval_Settings & settings)
{
  int result = map_.set_index (variable, index, value, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set_index", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const std::string & key, 
  const double * value,
  uint32_t size,
  const Eval_Settings & settings)
{
  int result = map_.set (key, value, size, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const Variable_Reference & variable, 
  const double * value,
  uint32_t size,
  const Eval_Settings & settings)
{
  int result = map_.set (variable, value, size, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const std::string & key, 
  const std::vector <double> & value,
  const Eval_Settings & settings)
{
  int result = map_.set (key, value, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const Variable_Reference & variable, 
  const std::vector <double> & value,
  const Eval_Settings & settings)
{
  int result = map_.set (variable, value, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const std::string & key, 
  const std::string & value, 
  const Eval_Settings & settings)
{
  int result = map_.set (key, value, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set (
  const Variable_Reference & variable, 
  const std::string & value, 
  const Eval_Settings & settings)
{
  int result = map_.set (variable, value, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set", settings);

  return result;
}

/// Read a file into the knowledge base
int
Madara::Knowledge_Engine::Knowledge_Base_Impl::read_file (
  const std::string & key, const std::string & filename, 
        const Eval_Settings & settings)
{
  if (key == "")
    return -1;

  int result = map_.read_file (key, filename, settings);
  
  send_modifieds ("Knowledge_Base_Impl:read_file", settings);

  return result;
}

/// Read a file into the knowledge base
int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set_file (
  const Variable_Reference & variable,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
{
  int result = map_.set_file (variable, value, size, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set_file", settings);

  return result;
}

/// Read a file into the knowledge base
int
Madara::Knowledge_Engine::Knowledge_Base_Impl::set_jpeg (
  const Variable_Reference & variable,
  const unsigned char * value, size_t size, 
  const Eval_Settings & settings)
{
  int result = map_.set_jpeg (variable, value, size, settings);
  
  send_modifieds ("Knowledge_Base_Impl:set_jpeg", settings);

  return result;
}

/// Read a file into the knowledge base
int
Madara::Knowledge_Engine::Knowledge_Base_Impl::read_file (
  const Variable_Reference & variable,
  const std::string & filename, 
  const Eval_Settings & settings)
{
  int result = map_.read_file (variable, filename, settings);
  
  send_modifieds ("Knowledge_Base_Impl:read_file", settings);

  return result;
}

#ifndef _MADARA_NO_KARL_

Madara::Knowledge_Engine::Compiled_Expression
Madara::Knowledge_Engine::Knowledge_Base_Impl::compile (
  const std::string & expression)
{
  map_.get_logger ().log (Logger::LOG_MAJOR,
    "Knowledge_Base_Impl::compile:" \
    " compiling %s\n", expression.c_str ());

  return map_.compile (expression);
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base_Impl::wait (const std::string & expression,
        const Wait_Settings & settings)
{
  Compiled_Expression compiled = compile (expression);
  return wait (compiled, settings);
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base_Impl::wait (
  Compiled_Expression & ce, 
  const Wait_Settings & settings)
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
    map_.print (settings.pre_print_statement, Logger::LOG_EMERGENCY);

  // lock the context
  map_.lock ();

  map_.get_logger ().log (Logger::LOG_MAJOR,
    "Knowledge_Base_Impl::wait:" \
    " waiting on %s\n", ce.logic.c_str ());

  Madara::Knowledge_Record last_value = ce.expression.evaluate (settings);

  map_.get_logger ().log (Logger::LOG_DETAILED,
    "Knowledge_Base_Impl::wait:" \
    " completed first eval to get %s\n",
    last_value.to_string ().c_str ());
  
  send_modifieds ("Knowledge_Base_Impl:wait", settings);

  map_.unlock ();
  
  current = ACE_High_Res_Timer::gettimeofday ();

  // wait for expression to be true
  while (!last_value.to_integer () &&
    (settings.max_wait_time < 0 || current < max_wait))
  {
    map_.get_logger ().log (Logger::LOG_DETAILED,
      "Knowledge_Base_Impl::wait:" \
      " current is %Q.%Q and max is %Q.%Q (poll freq is %f)\n",
      current.sec (), current.usec (), max_wait.sec (), max_wait.usec (),
      settings.poll_frequency);

    map_.get_logger ().log (Logger::LOG_DETAILED,
      "Knowledge_Base_Impl::wait:" \
      " last value didn't result in success\n");

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
      map_.wait_for_change (true);

    // relock - basically we need to evaluate the tree again, and
    // we can't have a bunch of people changing the variables as 
    // while we're evaluating the tree.
    map_.lock ();


    map_.get_logger ().log (Logger::LOG_MAJOR,
      "Knowledge_Base_Impl::wait:" \
      " waiting on %s\n", ce.logic.c_str ());

    last_value = ce.expression.evaluate (settings);

    map_.get_logger ().log (Logger::LOG_DETAILED,
      "Knowledge_Base_Impl::wait:" \
      " completed eval to get %s\n",
      last_value.to_string ().c_str ());
  
    send_modifieds ("Knowledge_Base_Impl:wait", settings);

    map_.unlock ();
    map_.signal ();

    // get current time
    current = ACE_High_Res_Timer::gettimeofday ();

  } // end while (!last)
  
  if (current >= max_wait)
  {
    map_.get_logger ().log (Logger::LOG_MAJOR,
      "Knowledge_Base_Impl::wait:" \
      " Evaluate did not succeed. Timeout occurred\n");
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    map_.print (settings.post_print_statement, Logger::LOG_ALWAYS);

  return last_value;
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base_Impl::evaluate (
  const std::string & expression,
  const Eval_Settings & settings)
{
  Compiled_Expression compiled = compile (expression);
  return evaluate (compiled, settings);
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base_Impl::evaluate (
  Compiled_Expression & ce,
  const Eval_Settings & settings)
{
  Madara::Knowledge_Record last_value;

  map_.get_logger ().log (Logger::LOG_MAJOR,
    "Knowledge_Base_Impl::evaluate:" \
    " evaluating %s.\n", ce.logic.c_str ());

  // iterators and tree for evaluation of interpreter results
  //Madara::Expression_Tree::Expression_Tree tree;

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    map_.print (settings.pre_print_statement, Logger::LOG_ALWAYS);

  // lock the context from being updated by any ongoing threads
  map_.lock ();

  // interpret the current expression and then evaluate it
  //tree = interpreter_.interpret (map_, expression);
  last_value = ce.expression.evaluate (settings);
  
  send_modifieds ("Knowledge_Base_Impl:evaluate", settings);

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    map_.print (settings.post_print_statement, Logger::LOG_ALWAYS);

  map_.unlock ();

  return last_value;
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Knowledge_Base_Impl::evaluate (
  Expression_Tree::Component_Node * root,
  const Eval_Settings & settings)
{
  Madara::Knowledge_Record last_value;

  map_.get_logger ().log (Logger::LOG_MAJOR,
    "Knowledge_Base_Impl::evaluate:" \
    " evaluating Component_Node rooted tree\n");

  // iterators and tree for evaluation of interpreter results
  //Madara::Expression_Tree::Expression_Tree tree;

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    map_.print (settings.pre_print_statement, Logger::LOG_ALWAYS);

  // lock the context from being updated by any ongoing threads
  map_.lock ();

  // interpret the current expression and then evaluate it
  //tree = interpreter_.interpret (map_, expression);
  last_value = map_.evaluate (root, settings);
  
  send_modifieds ("Knowledge_Base_Impl:evaluate", settings);

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    map_.print (settings.post_print_statement, Logger::LOG_ALWAYS);

  map_.unlock ();

  return last_value;
}

#endif // _MADARA_NO_KARL_
