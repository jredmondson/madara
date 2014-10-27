
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "ace/SOCK_Dgram_Bcast.h"

std::string broadcast_target ("192.168.1.255:15000");
int client_type (0);

void handle_arguments (int argc, char ** argv)
{
  for (int i = 0; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "--h" || arg1 == "-h")
    {
      std::cout << 
        "Program arguments for " << argv[0] << ":\n" \
        " [-b|--broadcast ip:port] the broadcast ip to send and listen to\n" \
        " [-t|--type type]         type of client (0=pub, 1=sub)\n" \
        "\n";
      exit (0);
    }
    else if (arg1 == "-b" || arg1 == "--broadcast")
    {
      if (i + 1 < argc)
        broadcast_target = argv[i + 1];

      ++i;
    }
    else if (arg1 == "-t" || arg1 == "--type")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> client_type;
      }

      ++i;
    }
  }
}


int main (int argc, char ** argv)
{
  handle_arguments (argc, argv);

  ACE_INET_Addr address (broadcast_target.c_str ());

  std::cout << "Broadcast address is " << address.get_host_addr () << ":" <<
    address.get_port_number () << "\n";

  char buffer [64];
  memset (buffer, 0, 64);
  unsigned int num_bytes;

  if (client_type == 0)
  {
    // publisher
    ACE_SOCK_Dgram_Bcast socket;

    if (socket.open (address, 2, 0, 1) == -1)
    {
      std::cout << "Broadcast socket failed to open\n";
      return -1;
    }

    strncpy (buffer, "Hello World.", sizeof (buffer));
    num_bytes = socket.send (&buffer, sizeof (buffer), address);

    std::cout << "Publisher sent " << buffer << " in " << num_bytes <<
      " byte packet.\n";

    socket.close ();
  }
  else
  {
    // subscriber
    ACE_SOCK_Dgram socket;
    ACE_INET_Addr  remote;
    //ACE_INET_Addr  local (address.get_port_number ());

    // open with defaults at the port number, except set reuse to 1
    if (socket.open (address, 2, 0, 1) == -1)
    {
      std::cout << "Broadcast socket failed to open\n";
      return -1;
    }

    num_bytes = socket.recv (buffer, sizeof (buffer),
                     remote, 0);
    
    std::cout << "Subscriber received " << buffer << " in " << num_bytes << 
      " byte packet from " << remote.get_host_addr () << ".\n";

    socket.close ();
  }

  return 0;
}