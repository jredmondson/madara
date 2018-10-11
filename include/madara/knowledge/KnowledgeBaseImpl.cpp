#include "madara/utility/Utility.h"
#include "madara/knowledge/KnowledgeBaseImpl.h"
#include "madara/expression/Interpreter.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/transport/udp/UdpTransport.h"
#include "madara/transport/udp/UdpRegistryServer.h"
#include "madara/transport/udp/UdpRegistryClient.h"
#include "madara/transport/multicast/MulticastTransport.h"
#include "madara/transport/broadcast/BroadcastTransport.h"
#include "madara/utility/EpochEnforcer.h"
#include "madara/Boost.h"

#include <sstream>

#ifdef _MADARA_USING_ZMQ_
#include "madara/transport/zmq/ZMQTransport.h"
#endif

#ifdef _USE_OPEN_SPLICE_
#include "madara/transport/splice/SpliceDDSTransport.h"
#endif  // _USE_OPEN_SPLICE_

#ifdef _USE_NDDS_
#include "madara/transport/ndds/NddsTransport.h"
#endif  // _USE_NDDS_

#include <iostream>
#include <random>

namespace utility = madara::utility;

typedef utility::EpochEnforcer<std::chrono::steady_clock> EpochEnforcer;

namespace madara
{
namespace knowledge
{
std::string KnowledgeBaseImpl::setup_unique_hostport(std::string host)
{
  // placeholder for our ip address
  std::string actual_host(std::move(host));

  if (actual_host == "")
  {
    try
    {
      actual_host = boost::asio::ip::host_name();
    }
    catch (const std::exception&)
    {
      actual_host = "localhost";
    }
  }

  if (actual_host.size() > 30)
  {
    actual_host.resize(30);
  }

  actual_host += ":";

  boost::mt19937 ran(std::random_device{}());
  auto uuid = boost::uuids::basic_random_generator<boost::mt19937>(ran)();
  auto uuid_str = boost::uuids::to_string(uuid);
  uuid_str.erase(
      std::remove(uuid_str.begin(), uuid_str.end(), '-'), uuid_str.end());

  actual_host += uuid_str;

  return actual_host;
}

size_t KnowledgeBaseImpl::attach_transport(
    const std::string& id, transport::TransportSettings& settings)
{
  madara::transport::Base* transport(0);
  std::string originator(id);

  if (originator == "")
  {
    if (id_.size() > 0)
      originator = id_;
    else
      originator = id_ = setup_unique_hostport();
  }

  madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::attach_transport:"
      " activating transport type %d\n",
      settings.type);

  if (settings.type == madara::transport::BROADCAST)
  {
    transport = new madara::transport::BroadcastTransport(
        originator, map_, settings, true);
  }
  else if (settings.type == madara::transport::MULTICAST)
  {
    transport = new madara::transport::MulticastTransport(
        originator, map_, settings, true);
  }
  else if (settings.type == madara::transport::SPLICE)
  {
#ifdef _USE_OPEN_SPLICE_
    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::activate_transport:"
        " creating Open Splice DDS transport.\n");

    transport = new madara::transport::SpliceDDSTransport(
        originator, map_, settings, true);
#else
    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::activate_transport:"
        " project was not generated with opensplice=1. Transport is "
        "invalid.\n");
#endif
  }
  else if (settings.type == madara::transport::NDDS)
  {
#ifdef _USE_NDDS_
    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::activate_transport:"
        " creating NDDS transport.\n");

    transport =
        new madara::transport::NddsTransport(originator, map_, settings, true);
#else
    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::activate_transport:"
        " project was not generated with ndds=1. Transport is invalid.\n");
#endif
  }
  else if (settings.type == madara::transport::UDP)
  {
    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::activate_transport:"
        " creating UDP transport.\n");

    transport =
        new madara::transport::UdpTransport(originator, map_, settings, true);
  }
  else if (settings.type == madara::transport::ZMQ)
  {
#ifdef _MADARA_USING_ZMQ_
    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::activate_transport:"
        " creating ZMQ transport.\n");

    transport =
        new madara::transport::ZMQTransport(originator, map_, settings, true);
#else
    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::activate_transport:"
        " project was not generated with zmq=1. Transport is invalid.\n");
#endif
  }
  else if (settings.type == madara::transport::REGISTRY_SERVER)
  {
    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::activate_transport:"
        " creating UDP Registry Server transport.\n");

    transport = new madara::transport::UdpRegistryServer(
        originator, map_, settings, true);
  }
  else if (settings.type == madara::transport::REGISTRY_CLIENT)
  {
    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::activate_transport:"
        " creating UDP Registry Client transport.\n");

    transport = new madara::transport::UdpRegistryClient(
        originator, map_, settings, true);
  }
  else
  {
    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::activate_transport:"
        " no transport was specified. Setting transport to null.\n");
  }

  {
    MADARA_GUARD_TYPE guard(transport_mutex_);

    // if we have a valid transport, add it to the transports vector
    if (transport != 0)
    {
      transports_.emplace_back(transport);
    }

    return transports_.size();
  }
}

void KnowledgeBaseImpl::close_transport(void)
{
  decltype(transports_) old_transports;
  {
    MADARA_GUARD_TYPE guard(transport_mutex_);
    using std::swap;
    swap(old_transports, transports_);
  }

  for (auto& transport : old_transports)
  {
    transport->close();

    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::close_transport:"
        " transport has been closed\n");
  }
}

#ifndef _MADARA_NO_KARL_

CompiledExpression KnowledgeBaseImpl::compile(const std::string& expression)
{
  madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::compile:"
      " compiling %s\n",
      expression.c_str());

  return map_.compile(expression);
}

KnowledgeRecord KnowledgeBaseImpl::wait(
    const std::string& expression, const WaitSettings& settings)
{
  CompiledExpression compiled = compile(expression);
  return wait(compiled, settings);
}

KnowledgeRecord KnowledgeBaseImpl::wait(
    CompiledExpression& ce, const WaitSettings& settings)
{
  // use the EpochEnforcer utility to keep track of sleeps
  EpochEnforcer enforcer(settings.poll_frequency, settings.max_wait_time);

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    map_.print(settings.pre_print_statement, logger::LOG_EMERGENCY);

  // lock the context

  KnowledgeRecord last_value;
  {
    MADARA_GUARD_TYPE guard(map_.mutex_);

    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::wait:"
        " waiting on %s\n",
        ce.logic.c_str());

    last_value = ce.expression.evaluate(settings);

    madara_logger_log(map_.get_logger(), logger::LOG_DETAILED,
        "KnowledgeBaseImpl::wait:"
        " completed first eval to get %s\n",
        last_value.to_string().c_str());

    send_modifieds("KnowledgeBaseImpl:wait", settings);
  }

  // wait for expression to be true
  while (!last_value.to_integer() &&
         (settings.max_wait_time < 0 || !enforcer.is_done()))
  {
    madara_logger_log(map_.get_logger(), logger::LOG_DETAILED,
        "KnowledgeBaseImpl::wait:"
        " last value didn't result in success\n");

    // Unlike the other wait statements, we allow for a time based wait.
    // To do this, we allow a user to specify a
    if (settings.poll_frequency > 0)
    {
      enforcer.sleep_until_next();
    }
    else
    {
      map_.wait_for_change(true);
    }

    // relock - basically we need to evaluate the tree again, and
    // we can't have a bunch of people changing the variables as
    // while we're evaluating the tree.
    {
      MADARA_GUARD_TYPE guard(map_.mutex_);

      madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
          "KnowledgeBaseImpl::wait:"
          " waiting on %s\n",
          ce.logic.c_str());

      last_value = ce.expression.evaluate(settings);

      madara_logger_log(map_.get_logger(), logger::LOG_DETAILED,
          "KnowledgeBaseImpl::wait:"
          " completed eval to get %s\n",
          last_value.to_string().c_str());

      send_modifieds("KnowledgeBaseImpl:wait", settings);
    }
    map_.signal();

  }  // end while (!last)

  if (enforcer.is_done())
  {
    madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
        "KnowledgeBaseImpl::wait:"
        " Evaluate did not succeed. Timeout occurred\n");
  }

  // print the post statement at highest log level (cannot be masked)
  if (settings.post_print_statement != "")
    map_.print(settings.post_print_statement, logger::LOG_ALWAYS);

  return last_value;
}

KnowledgeRecord KnowledgeBaseImpl::evaluate(
    CompiledExpression& ce, const EvalSettings& settings)
{
  KnowledgeRecord last_value;

  madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::evaluate:"
      " evaluating %s.\n",
      ce.logic.c_str());

  // iterators and tree for evaluation of interpreter results
  // madara::expression::ExpressionTree tree;

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    map_.print(settings.pre_print_statement, logger::LOG_ALWAYS);

  // lock the context from being updated by any ongoing threads
  {
    MADARA_GUARD_TYPE guard(map_.mutex_);

    // interpret the current expression and then evaluate it
    // tree = interpreter_.interpret (map_, expression);
    last_value = ce.expression.evaluate(settings);

    send_modifieds("KnowledgeBaseImpl:evaluate", settings);

    // print the post statement at highest log level (cannot be masked)
    if (settings.post_print_statement != "")
      map_.print(settings.post_print_statement, logger::LOG_ALWAYS);
  }

  return last_value;
}

KnowledgeRecord KnowledgeBaseImpl::evaluate(
    expression::ComponentNode* root, const EvalSettings& settings)
{
  KnowledgeRecord last_value;

  madara_logger_log(map_.get_logger(), logger::LOG_MAJOR,
      "KnowledgeBaseImpl::evaluate:"
      " evaluating ComponentNode rooted tree\n");

  // iterators and tree for evaluation of interpreter results
  // madara::expression::ExpressionTree tree;

  // print the post statement at highest log level (cannot be masked)
  if (settings.pre_print_statement != "")
    map_.print(settings.pre_print_statement, logger::LOG_ALWAYS);

  // lock the context from being updated by any ongoing threads
  {
    MADARA_GUARD_TYPE guard(map_.mutex_);

    // interpret the current expression and then evaluate it
    // tree = interpreter_.interpret (map_, expression);
    last_value = map_.evaluate(root, settings);

    send_modifieds("KnowledgeBaseImpl:evaluate", settings);

    // print the post statement at highest log level (cannot be masked)
    if (settings.post_print_statement != "")
      map_.print(settings.post_print_statement, logger::LOG_ALWAYS);
  }

  return last_value;
}

#endif  // _MADARA_NO_KARL_

int KnowledgeBaseImpl::send_modifieds(
    const std::string& prefix, const EvalSettings& settings)
{
  int result = 0;

  bool do_send_modifieds;
  {
    MADARA_GUARD_TYPE guard(transport_mutex_);
    do_send_modifieds = (!settings.delay_sending_modifieds && transports_.size() > 0);
  }

  if (do_send_modifieds)
  {
    KnowledgeMap modified;
    
    // get the modifieds and reset those that will be sent, atomically
    {
      MADARA_GUARD_TYPE guard(map_.mutex_);
      modified = map_.get_modifieds_current(settings.send_list, true);
    }

    if (modified.size() > 0)
    {
      {
        MADARA_GUARD_TYPE guard(transport_mutex_);
        // send across each transport
        for (auto& transport : transports_)
        {
          transport->send_data(modified);
        }
      }

      map_.inc_clock(settings);

      if (settings.signal_changes)
        map_.signal(false);
    }
    else
    {
      madara_logger_log(map_.get_logger(), logger::LOG_DETAILED,
          "%s: no modifications to send\n", prefix.c_str());

      result = -1;
    }
  }
  else
  {

    if (settings.delay_sending_modifieds)
    {
      madara_logger_log(map_.get_logger(), logger::LOG_DETAILED,
          "%s: user requested to not send modifieds\n", prefix.c_str());

      result = -3;
      return result;
    }

    size_t num_transports;
    {
      MADARA_GUARD_TYPE guard(transport_mutex_);
      num_transports = transports_.size();
    }

    if (num_transports)
    {
      madara_logger_log(map_.get_logger(), logger::LOG_DETAILED,
          "%s: no transport configured\n", prefix.c_str());

      result = -2;
    }
  }

  return result;
}
}
}
