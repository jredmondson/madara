
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>

#include "ace/SOCK_Dgram_Mcast.h"
#include "ace/Time_Value.h"

std::string multicast_target ("239.255.0.1:4150");
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
        " [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
        " [-t|--type type]         type of client (0=pub, 1=sub)\n" \
        "\n";
      exit (0);
    }
    else if (arg1 == "-m" || arg1 == "--multicast")
    {
      if (i + 1 < argc)
        multicast_target = argv[i + 1];

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

  ACE_INET_Addr address (multicast_target.c_str ());

  std::cout << "Multicast address is " << address.get_host_addr () << ":" <<
    address.get_port_number () << "\n";

  char buffer [64];
  memset (buffer, 0, 64);
  int num_bytes;

  if (client_type == 0)
  {
    ACE_SOCK_Dgram socket (ACE_sap_any_cast (ACE_INET_Addr &), PF_INET);
    int ttl = 1;
    if (socket.set_option (IPPROTO_IP,
                            IP_MULTICAST_TTL,
                            (void*) &ttl,
                            sizeof ttl) != 0)
    {
      std::cout << "Multicast socket failed to open\n";
      return -1;
    }

    strncpy (buffer, "Hello World.", sizeof (buffer));
    // publisher
    num_bytes = socket.send (&buffer, sizeof (buffer), address);

    std::cout << "Publisher sent " << buffer << " in " << num_bytes <<
      " byte packet.\n";

    socket.close ();
  }
  else
  {
    ACE_Time_Value wait_time (10);

    // subscriber
    ACE_SOCK_Dgram_Mcast socket;
    ACE_INET_Addr  remote;
    
    if (socket.join (address, 1) == -1)
    {
      std::cout << "Multicast socket failed to open\n";
      return -1;
    }

    num_bytes = socket.recv (buffer, sizeof (buffer), remote, 0, &wait_time);
    
    std::cout << "Subscriber received " << buffer << " in " << num_bytes << 
      " byte packet from " << remote.get_host_addr () << ".\n";

    socket.close ();
  }

  return 0;
}