#ifndef MADARA_COMMON_H
#define MADARA_COMMON_H

#include <string>

namespace Madara
{
  extern const int BROKER_PORT_START;
  extern const int BROKER_PORT_END;
  extern const int WORKER_PORT_START;

  // split a key into a corresponding host and port
  int split_key (const std::string & key, 
    std::string & host, std::string & port);

  // merge a host and port into a key
  int merge_key (std::string & key, 
    const std::string & host, const std::string & port);

  // merge a host and port into a key
  int merge_key (std::string & key, 
    const std::string & host, unsigned short u_port);

  extern volatile int finished_;
}

#endif // MADARA_COMMON_H