#ifndef MADARA_COMMON_H
#define MADARA_COMMON_H

#include <map>
#include <string>
#include <vector>

namespace Madara
{
  extern const int BROKER_PORT_START;
  extern const int BROKER_PORT_END;
  extern const int WORKER_PORT_START;


  typedef std::pair <std::string, int> LatencyPair;
  typedef std::map <std::string,int> PeerLatencyMap;
  typedef std::vector <LatencyPair> PeerLatencyVector;

  typedef std::map <std::string, bool> EliminationList;
  typedef std::map <int, bool> DeploymentEliminationList;

  // my lol heuristic
  typedef PeerLatencyVector TopNerds;

  typedef struct
  {
    Madara::PeerLatencyMap latencies;
    Madara::PeerLatencyVector vector;

    double classifier;
    int avg;
  } PeerLatency;

  typedef std::map <std::string,Madara::PeerLatency> BrokerMap;


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