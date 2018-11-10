
#include <sstream>
#include "MyTransport.h"
#include "MyTransportReadThread.h"

namespace knowledge = madara::knowledge;

transports::MyTransport::MyTransport(
  const std::string & id,
  madara::transport::TransportSettings & new_settings,
  knowledge::KnowledgeBase & knowledge)
: madara::transport::Base(id, new_settings, knowledge.get_context())
{
  // populate variables like buffer_ based on transport settings
  Base::setup();
  
  // set the data plane for read threads
  read_threads_.set_data_plane(knowledge);
  
  // check the read hz settings to see if the user has passed something weird
  double hertz = new_settings.read_thread_hertz;
  if(hertz < 0.0)
  {
    hertz = 0.0;
  }

  // create the read threads specified in TransportSettings  
  for(uint32_t i = 0; i < new_settings.read_threads; ++i)
  {
    // construct a unique id for a new thread
    std::stringstream thread_name;
    thread_name << "read";
    thread_name << i;
    
    // start the thread at the specified hertz
    read_threads_.run(
      hertz,
      thread_name.str(),
      new MyTransportReadThread(
        id_, new_settings, 
        send_monitor_, receive_monitor_, packet_scheduler_));
  }
}

transports::MyTransport::~MyTransport()
{
}

long
transports::MyTransport::send_data(
  const madara::knowledge::KnowledgeMap &)
{
  /**
   * Return number of bytes sent or negative for error
   **/
  long result(0);
  
  /**
   * This is where you should do your custom transport sending logic/actions
   **/
  
  return result;
}
