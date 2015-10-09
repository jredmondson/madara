#include "MadaraCommon.h"

volatile int Madara::finished_ = 0;

const int Madara::BROKER_PORT_START = 49995;
const int Madara::BROKER_PORT_END = 49999;
const int Madara::WORKER_PORT_START = 50000;

// split a key into a corresponding host and port
int Madara::split_key (const std::string & key, 
    std::string & host, std::string & port)
{
  // delimeter is either a : or an @
  std::string::size_type delim = key.rfind (':');
  delim = delim == key.npos ? key.rfind ('@') : delim;

  // no delimeter found
  if (delim == key.npos)
    {
      host = key;
      port = "";
      
      return 1;
    }

  // otherwise, set the host and port appropriately
  host = key.substr (0, delim);
  port = key.substr (delim + 1, key.size () - delim);

  return 0;
}

// merge a host and port into a key
int Madara::merge_key (std::string & key, 
  const std::string & host, const std::string & port)
{
  key = host;
  key += ':';
  key += port;

  return 0;
}

// merge a host and ushort port into a key
int Madara::merge_key (std::string & key, 
  const std::string & host, unsigned short u_port)
{
  char port_buf[16];
  std::string port;

  // convert the server_port into string form for our context
  itoa (u_port, port_buf, 10);

  port = port_buf;

  return merge_key (key, host, port);
}