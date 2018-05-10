#include "madara/utility/Utility.h"
#include "madara/knowledge/KnowledgeBaseImpl.h"
#include "madara/expression/Interpreter.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/transport/udp/UdpTransport.h"
#include "madara/transport/udp/UdpRegistryServer.h"
#include "madara/transport/udp/UdpRegistryClient.h"
#include "madara/transport/tcp/TcpTransport.h"
#include "madara/transport/multicast/MulticastTransport.h"
#include "madara/transport/broadcast/BroadcastTransport.h"


#include <sstream>

#include "ace/OS_NS_Thread.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_sys_socket.h"

#ifdef _MADARA_USING_ZMQ_
#include "madara/transport/zmq/ZMQTransport.h"
#endif

#ifdef _USE_OPEN_SPLICE_
#include "madara/transport/splice/SpliceDDSTransport.h"
#endif // _USE_OPEN_SPLICE_

#ifdef _USE_NDDS_
#include "madara/transport/ndds/NddsTransport.h"
#endif // _USE_NDDS_

#include <iostream>

namespace madara { namespace knowledge {

std::string
KnowledgeBaseImpl::setup_unique_hostport (
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
KnowledgeBaseImpl::attach_transport (const std::string & id,
transport::TransportSettings & settings)
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

    transport = new madara::transport::NddsTransport (originator, map_,
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

    transport = new madara::transport::UdpTransport (originator, map_,
      settings, true);
  }
  else if (settings.type == madara::transport::ZMQ)
  {
#ifdef _MADARA_USING_ZMQ_
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " creating ZMQ transport.\n");

    transport = new madara::transport::ZMQTransport (originator, map_,
      settings, true);
#else
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " project was not generated with zmq=1. Transport is invalid.\n");
#endif
  }
  else if (settings.type == madara::transport::REGISTRY_SERVER)
  {
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " creating UDP Registry Server transport.\n");

    transport = new madara::transport::UdpRegistryServer (originator, map_,
      settings, true);
  }
  else if (settings.type == madara::transport::REGISTRY_CLIENT)
  {
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " creating UDP Registry Client transport.\n");

    transport = new madara::transport::UdpRegistryClient (originator, map_,
      settings, true);
  }
  else if (settings.type == madara::transport::TCP)
  {
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " creating TCP transport.\n");

    transport = new madara::transport::TcpTransport (originator, map_,
      settings, true);
  }
  else
  {
    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::activate_transport:" \
      " no transport was specified. Setting transport to null.\n");
  }

  MADARA_GUARD_TYPE guard (map_.mutex_);

  // if we have a valid transport, add it to the transports vector
  if (transport != 0)
  {
    transports_.emplace_back (transport);
  }

  return transports_.size ();
}

void
KnowledgeBaseImpl::close_transport (void)
{
  decltype(transports_) old_transports;
  {
    MADARA_GUARD_TYPE guard (map_.mutex_);
    using std::swap;
    swap(old_transports, transports_);
  }

  for (auto &transport : old_transports)
  {
    transport->close ();

    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::close_transport:" \
      " transport has been closed\n");
  }
}


#ifndef _MADARA_NO_KARL_

CompiledExpression
KnowledgeBaseImpl::compile (
const std::string & expression)
{
  madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
    "KnowledgeBaseImpl::compile:" \
    " compiling %s\n", expression.c_str ());

  return map_.compile (expression);
}

KnowledgeRecord
KnowledgeBaseImpl::wait (const std::string & expression,
const WaitSettings & settings)
{
  CompiledExpression compiled = compile (expression);
  return wait (compiled, settings);
}

KnowledgeRecord
KnowledgeBaseImpl::wait (
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

  KnowledgeRecord last_value;
  {
    MADARA_GUARD_TYPE guard (map_.mutex_);

    madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::wait:" \
      " waiting on %s\n", ce.logic.c_str ());

    last_value = ce.expression.evaluate (settings);

    madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
      "KnowledgeBaseImpl::wait:" \
      " completed first eval to get %s\n",
      last_value.to_string ().c_str ());

    send_modifieds ("KnowledgeBaseImpl:wait", settings);
  }

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
    {
      MADARA_GUARD_TYPE guard (map_.mutex_);

      madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::wait:" \
        " waiting on %s\n", ce.logic.c_str ());

      last_value = ce.expression.evaluate (settings);

      madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
        "KnowledgeBaseImpl::wait:" \
        " completed eval to get %s\n",
        last_value.to_string ().c_str ());

      send_modifieds ("KnowledgeBaseImpl:wait", settings);

    }
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

KnowledgeRecord
KnowledgeBaseImpl::evaluate (
CompiledExpression & ce,
const EvalSettings & settings)
{
  KnowledgeRecord last_value;

  madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
    "KnowledgeBaseImpl::evaluate:" \
    " evaluating %s.\n", ce.logic.c_str ());

  // iterators and tree for evaluation of interpreter results
  //madara::expression::ExpressionTree tree;

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    map_.print (settings.pre_print_statement, logger::LOG_ALWAYS);

  // lock the context from being updated by any ongoing threads
  {
    MADARA_GUARD_TYPE guard (map_.mutex_);

    // interpret the current expression and then evaluate it
    //tree = interpreter_.interpret (map_, expression);
    last_value = ce.expression.evaluate (settings);

    send_modifieds ("KnowledgeBaseImpl:evaluate", settings);

    // print the post statement at highest log level (cannot be masked)
    if (settings.post_print_statement != "")
      map_.print (settings.post_print_statement, logger::LOG_ALWAYS);
  }

  return last_value;
}

KnowledgeRecord
KnowledgeBaseImpl::evaluate (
expression::ComponentNode * root,
const EvalSettings & settings)
{
  KnowledgeRecord last_value;

  madara_logger_log (map_.get_logger (), logger::LOG_MAJOR,
    "KnowledgeBaseImpl::evaluate:" \
    " evaluating ComponentNode rooted tree\n");

  // iterators and tree for evaluation of interpreter results
  //madara::expression::ExpressionTree tree;

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    map_.print (settings.pre_print_statement, logger::LOG_ALWAYS);

  // lock the context from being updated by any ongoing threads
  {
    MADARA_GUARD_TYPE guard (map_.mutex_);

    // interpret the current expression and then evaluate it
    //tree = interpreter_.interpret (map_, expression);
    last_value = map_.evaluate (root, settings);

    send_modifieds ("KnowledgeBaseImpl:evaluate", settings);

    // print the post statement at highest log level (cannot be masked)
    if (settings.post_print_statement != "")
      map_.print (settings.post_print_statement, logger::LOG_ALWAYS);
  }

  return last_value;
}

int
KnowledgeBaseImpl::send_modifieds (
  const std::string & prefix,
  const EvalSettings & settings)
{
  int result = 0;

  MADARA_GUARD_TYPE guard (map_.mutex_);

  if (transports_.size () > 0 && !settings.delay_sending_modifieds)
  {
    const VariableReferenceMap & modified = map_.get_modifieds ();

    if (modified.size () > 0)
    {
      // if there is not an allowed send_list list
      if (settings.send_list.size () == 0)
      {
        // send across each transport
        for (auto &transport : transports_) {
          transport->send_data (modified);
        }


        // reset the modified map
        map_.reset_modified ();
      }
      // if there is a send_list
      else
      {
        VariableReferenceMap allowed_modifieds;
        // otherwise, we are only allowed to send a subset of modifieds
        for (const auto &entry : modified)
        {
          if (settings.send_list.find (entry.first) != settings.send_list.end ())
          {
            allowed_modifieds.emplace_hint(allowed_modifieds.end(), entry);
          }
        }

        // if the subset was greater than zero, we send the subset
        if (allowed_modifieds.size () > 0)
        {
          // send across each transport
          for (auto &transport : transports_) {
            transport->send_data (allowed_modifieds);
          }

          // reset modified list for the allowed modifications
          for (const auto &entry : allowed_modifieds)
          {
            map_.reset_modified (entry.first);
          }
        }
      }

      map_.inc_clock (settings);

      if (settings.signal_changes)
        map_.signal (false);
    }
    else
    {
      madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
        "%s: no modifications to send\n", prefix.c_str ());

      result = -1;
    }
  }
  else
  {
    if (transports_.size () == 0)
    {
      madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
        "%s: no transport configured\n", prefix.c_str ());

      result = -2;
    }
    else if (settings.delay_sending_modifieds)
    {
      madara_logger_log (map_.get_logger (), logger::LOG_DETAILED,
        "%s: user requested to not send modifieds\n", prefix.c_str ());

      result = -3;
    }
  }

  return result;
}

#endif // _MADARA_NO_KARL_

} }
