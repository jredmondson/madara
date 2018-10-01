
#include "madara/transport/BasicASIOTransport.h"
#include "madara/transport/TransportContext.h"

#include "madara/transport/ReducedMessageHeader.h"
#include "madara/utility/Utility.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/expression/Interpreter.h"
#include "madara/transport/Fragmentation.h"

#include <iostream>
#include "madara/utility/IntTypes.h"

namespace madara
{
namespace transport
{
namespace mcast = ip::multicast;

// const double BasicASIOTransport::default_read_hertz = 10.0;

BasicASIOTransport::BasicASIOTransport(const std::string& id,
    knowledge::ThreadSafeContext& context, TransportSettings& config)
  : Base(id, config, context)
{
  // create a reference to the knowledge base for threading
  knowledge_.use(context);

  // set the data plane for the read threads
  read_threads_.set_data_plane(knowledge_);
}

BasicASIOTransport::~BasicASIOTransport()
{
  BasicASIOTransport::close();
}

void BasicASIOTransport::close(void)
{
  this->invalidate_transport();

  read_threads_.terminate();

  read_threads_.wait();

  socket_.close();
}

int BasicASIOTransport::setup(void)
{
  // call base setup method to initialize certain common variables
  if (Base::setup() < 0) {
    return -1;
  }

  // resize addresses to be the size of the list of hosts
  addresses_.clear();
  addresses_.reserve(this->settings_.hosts.size());

  if (settings_.hosts.size() == 0) {
    madara_logger_log(context_.get_logger(), logger::LOG_MINOR,
        "BasicASIOTransport::setup:"
        " No host addresses. Aborting setup.\n");
    this->invalidate_transport();
    return -1;
  }

  // convert the string host:port into an asio address
  for (unsigned int i = 0; i < settings_.hosts.size(); ++i) {
    try {
      auto addr_parts = utility::parse_address(settings_.hosts[i]);

      auto addr = ip::address::from_string(addr_parts.first);
      addresses_.emplace_back(addr, addr_parts.second);

      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "BasicASIOTransport::setup:"
          " settings address[%d] to %s:%d\n",
          i, addresses_.back().address().to_string().c_str(),
          addresses_.back().port());
    } catch (const boost::system::system_error& e) {
      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "BasicASIOTransport::setup:"
          " Error parsing address %s: %s\n",
          settings_.hosts[i].c_str(), e.what());
    }
  }

  if (addresses_.size() == 0) {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "BasicASIOTransport::setup:"
        " No valid addresses. Aborting setup.\n");
    this->invalidate_transport();
    return -1;
  }

  try {
    socket_.open(addresses_[0].protocol());
  } catch (const boost::system::system_error& e) {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "BasicASIOTransport::setup:"
        " Error opening sockets: %s\n",
        e.what());

    this->invalidate_transport();
    return -1;
  }

  int ret = setup_sockets();
  if (ret < 0) {
    return ret;
  }

  ret = setup_read_threads();
  if (ret < 0) {
    return ret;
  }

  return this->validate_transport();
}

int BasicASIOTransport::setup_socket(udp::socket& socket)
{
  int tar_buff_size(settings_.queue_length);

  asio::socket_base::send_buffer_size send_buffer_option;
  asio::socket_base::receive_buffer_size receive_buffer_option;

  try {
    socket.get_option(send_buffer_option);
    int send_buff_size = send_buffer_option.value();

    socket.get_option(receive_buffer_option);
    int rcv_buff_size = receive_buffer_option.value();

    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "BasicASIOTransport::setup:"
        " default socket buff size is send=%d, rcv=%d\n",
        send_buff_size, rcv_buff_size);

    if (send_buff_size < tar_buff_size) {
      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "BasicASIOTransport::setup:"
          " setting send buff size to settings.queue_length (%d)\n",
          tar_buff_size);

      send_buffer_option = tar_buff_size;
      socket.set_option(send_buffer_option);

      socket.get_option(send_buffer_option);
      send_buff_size = send_buffer_option.value();

      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "BasicASIOTransport::setup:"
          " current socket buff size is send=%d, rcv=%d\n",
          send_buff_size, rcv_buff_size);
    }

    if (rcv_buff_size < tar_buff_size) {
      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "BasicASIOTransport::setup:"
          " setting rcv buff size to settings.queue_length (%d)\n",
          tar_buff_size);

      receive_buffer_option = tar_buff_size;
      socket.set_option(receive_buffer_option);

      socket.get_option(receive_buffer_option);
      rcv_buff_size = receive_buffer_option.value();

      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "BasicASIOTransport::setup:"
          " current socket buff size is send=%d, rcv=%d\n",
          send_buff_size, rcv_buff_size);
    }
  } catch (const boost::system::system_error& e) {
    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "BasicASIOTransport::setup:"
        " Error setting up sockets: %s\n",
        e.what());

    this->invalidate_transport();
    return -1;
  }

  return 0;
}

int BasicASIOTransport::setup_read_socket(void)
{
  return setup_socket(socket_);
}

int BasicASIOTransport::setup_write_socket(void)
{
  // return setup_socket (write_socket_);
  return 0;
}

int BasicASIOTransport::setup_sockets(void)
{
  int ret = setup_read_socket();
  if (ret < 0) {
    return ret;
  }
  ret = setup_write_socket();
  if (ret < 0) {
    return ret;
  }

  return 0;
}

int BasicASIOTransport::setup_read_threads()
{
  if (!settings_.no_receiving) {
    double hertz = settings_.read_thread_hertz;
    if (hertz < 0.0) {
      // we need to maintain backwards compatibility
      // people should be capable of bursting reads especially
      hertz = 0.0;
    }

    madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
        "BasicASIOTransport::setup:"
        " starting %d threads at %f hertz\n",
        settings_.read_threads, hertz);

    for (uint32_t i = 0; i < settings_.read_threads; ++i) {
      std::stringstream thread_name;
      thread_name << "read";
      thread_name << i;

      madara_logger_log(context_.get_logger(), logger::LOG_MAJOR,
          "BasicASIOTransport::setup:"
          " starting thread %s at %f hertz\n",
          thread_name.str().c_str(), hertz);
      setup_read_thread(hertz, thread_name.str());
    }
  }

  return 0;
}
}
}
