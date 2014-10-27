#ifndef AGENT_CONTEXT_H
#define AGENT_CONTEXT_H

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>

#include "ace/Guard_T.h"
#include "ace/Recursive_Thread_Mutex.h"


namespace Madara
  {
    extern const int NUM_CLIENT_LATENCIES;

    typedef struct
    {
      std::vector<int> latencies;
      unsigned cur;
      unsigned int total_recorded;
    } LatencyTable;

    typedef std::map <std::string,LatencyTable> LatencyMap;

    class Agent_Context
    {
    public:
      Agent_Context();
      Agent_Context(const std::string& host, const std::string& port);
      void read (const std::string& filename);
      void write (std::ostream& output);
      void write (const std::string& filename);
      void addLatency (const std::string&, int latency);
      int getAverageLatency (const std::string& key);
      const std::string& getHost ();
      void setHost (const std::string& host);
      const std::string& getPort ();
      void setPort (const std::string& port);

      bool peerExists (const std::string& key);
      bool peerExists (const std::string& host, u_short port);
      bool peerExists (const std::string& host, const std::string& port);

      bool selfCheck (const std::string& host, u_short port);
      bool selfCheck (const std::string& host, const std::string& port);

      unsigned int getNumKeys (void);
      std::string getKey (unsigned int pos);
      LatencyTable getLatencyTable (unsigned int pos);
      LatencyTable getLatencyTable (const std::string & key);
    private:

      typedef ACE_Guard<ACE_Recursive_Thread_Mutex> Context_Guard;
      void createEntry (const std::string&);

      LatencyMap map_;
      std::string host_;
      std::string port_;
      int num_latencies_;
      ACE_Recursive_Thread_Mutex mutex_;
    };

  }
#endif // AGENT_CONTEXT_H