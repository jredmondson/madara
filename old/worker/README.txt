This zip contains for the following folders:

Madara

  This can be thought of as the Madara server, although its intention is to be a self-contained agent
  that initiates connections to other agents. The intent is to schedule a timer and every time this
  times out, the agent will connect with one of the other agents and conduct a latency test.

  Client_Acceptor - Accepts new connections, keeps a thread pool and enqueues new data into the thread pool's message queue.
  Client_Handler - Handles input and output. I also tried to have it handle timeouts. Since ClientAcceptor appears
    to inherit from ACE_Acceptor < Client_Handler, ACE_SOCK_ACCEPTOR >, I figured that the ClientAcceptor would
    call the handle_timeout from ClientHandler, but apparently not.

  The code that I am attempting to perform timer scheduling at is in main.cpp. It is the following line (44):


    timer.schedule (&peer_acceptor, &arg1, initial, interval);

  peer_acceptor is a Client_Acceptor. Arg1 is just an arbitrary int set to 0. Initial is set to gettimeofday (). Interval
  should be 1 second. However, I'm not getting anything. The handle_timeout is in Client_Handler (line 189), and it
  is supposed to print out "handled a timeout!" but it doesn't.
  


Madara Client

  This is a very simple client that connects to the server. In fact, it just contains a main.cpp.


