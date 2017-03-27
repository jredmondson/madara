#include "zmq.h"
#include <string>
#include <sstream>
#include <iostream>
#include <errno.h>
#include <thread>
#include <stdlib.h>
#include <string.h>

int id (0);
std::string address ("tcp://127.0.0.1:30000");

void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg (argv[i]);

    std::cerr << "arg[" << i << "] = " << arg << std::endl;

    if (arg == "-i" && i + 1 < argc)
    {
      std::cerr << "setting id to argv[" << (i + 1) << "] (" <<
        argv[i + 1] << ")\n";

      std::stringstream buffer;
      buffer << argv[i + 1];
      buffer >> id;

      ++i;
    }
    else if (arg == "-a" && i + 1 < argc)
    {
      address = argv[i + 1];
      ++i;
    }
  }
}

int main (int argc, char ** argv)
{
  std::cerr << "handling " << argc << "arguments.\n";

  handle_arguments (argc, argv);

  void * context = zmq_ctx_new ();
  //void * context = zmq_init (1);
  void * socket;
  char update[20];

  std::cerr << "id is " << id << std::endl;

  std::cerr << "context is " << context << std::endl;

  if (id == 0)
  {
    int option = 0;
    std::cerr << "initializing pusher.\n";

    void * socket = zmq_socket (context, ZMQ_PUSH);
    std::cerr << "socket is " << socket << std::endl;

    // set the underlying send buffer to 100KB
    option = 100000;
    zmq_setsockopt (socket, ZMQ_SNDBUF, (void *)&option, sizeof (int));
    zmq_setsockopt (socket, ZMQ_RCVBUF, (void *)&option, sizeof (int));

    int rc = zmq_bind (socket, address.c_str ());
    if (rc == 0)
    {
      std::cerr << "->Successfully bound to " << address << "\n";
      strcpy (update, "this is an update.");
      int length = zmq_send (socket, (void *)update, 20, 0);
      std::cerr << "->Sent update of " << length << " bytes\n";
    }
    else
    {
      std::cerr << "->ERROR: Could not bind to " << address << "\n";
      std::cerr << zmq_strerror (zmq_errno ()) << std::endl;
      strcpy (update, "No update is possible.");
    }

    rc = zmq_unbind (socket, address.c_str ());

    if (rc == 0)
    {
      std::cerr << "->unbound from " << address << "\n";
    }
    else
    {
      std::cerr << "->ERROR: unable to unbind from " << address << "\n";
      std::cerr << zmq_strerror (zmq_errno ()) << std::endl;
    }

    option = 2000;

    // if you don't do this, ZMQ waits forever for no reason. Super smart.
    zmq_setsockopt (socket, ZMQ_LINGER, (void *)&option, sizeof (int));

    std::this_thread::sleep_for (std::chrono::milliseconds (100));

    std::cerr << "closing socket\n";

    //std::this_thread::sleep_for (std::chrono::seconds (2));

    zmq_close (socket);

  }
  else
  {
    std::cerr << "initializing puller.\n";

    socket = zmq_socket (context, ZMQ_SUB);

    // set the underlying send buffer to 100KB
    int option = 100000;
    zmq_setsockopt (socket, ZMQ_SNDBUF, (void *)&option, sizeof (int));
    zmq_setsockopt (socket, ZMQ_RCVBUF, (void *)&option, sizeof (int));


    zmq_setsockopt (socket, ZMQ_SUBSCRIBE, 0, 0);
    std::cerr << "socket is " << socket << std::endl;

    int rc = zmq_connect (socket, address.c_str ());

    if (rc == 0)
    {
      std::cerr << "->Successfully connected to " << address << "\n";
      int length = zmq_recv (socket, (void *)update, 20, 0);
      std::cerr << "->Received update of " << length << " bytes\n";
    }
    else
    {
      std::cerr << "->ERROR: Could not connect to " << address << "\n";
      std::cerr << zmq_strerror (zmq_errno ()) << std::endl;
      strcpy (update, "No update is possible.");
    }
    std::cerr << "closing socket\n";

    option = 2000;
    // if you don't do this, ZMQ waits forever for no reason. Super smart.
    zmq_setsockopt (socket, ZMQ_LINGER, (void *)&option, sizeof (int));

    zmq_close (socket);

  }

  std::cerr << "update is: " << update << "\n";

  std::cerr << "destroying context\n";

  zmq_ctx_destroy (context);

  std::cerr << "finished destroying context\n";

  return 0;
}
