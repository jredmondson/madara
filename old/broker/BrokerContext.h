#ifndef BROKER_CONTEXT_H
#define BROKER_CONTEXT_H

#include <utility>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>

#include "ace/Guard_T.h"
#include "ace/Recursive_Thread_Mutex.h"

#include "MadaraCommon.h"
#include "Deployment.h"

namespace Madara
  {
  

    static bool SortByPairValue (LatencyPair u, LatencyPair v)
    {
      return u.second < v.second;
    }


    class BrokerContext
    {
    public:
      BrokerContext(int nerd_lookahead = 3, bool average_all = false);
      void read (const std::string& filename);
      void write (std::ostream& output);
      void write (const std::string& filename);
      void addLatency (const std::string& key, 
        const std::string & target, int latency);
      void addRequirement (int source, int target);
      void removeRequirement (int source, int target);
      void writeRequirements (std::ostream& output);
      void writeDeploymentCandidate (std::ostream& output, 
                              Madara::Deployment::Candidate & candidate);
      void writeRequirementsLegible (std::ostream& output);
      void writeRequirements (const std::string& filename);

      void clearRequirements ();

      const std::string& getHost ();
      void setHost (const std::string& host);
      const std::string& getPort ();
      void setPort (const std::string& port);

      bool peerExists (const std::string& key);
      bool peerExists (const std::string& host, u_short port);
      bool peerExists (const std::string& host, const std::string& port);

      Madara::Deployment::Candidate learnDeployment (void);
      int mutate (
        Madara::Deployment::Candidate & candidate);

      unsigned int getNumKeys (void);
      std::string getKey (unsigned int pos);
      Madara::PeerLatency getLatencyMap (unsigned int pos);
      Madara::PeerLatency getLatencyMap (const std::string & key);

      Madara::Deployment::Deployment & getDeployment (void);
      Madara::BrokerMap & getBrokerMap (void);

    private:
      std::string getNextLowestLatencyAvailable (
            const Madara::EliminationList & remainingNodes, 
            const Madara::PeerLatencyVector & sortedLatencies);
      typedef ACE_Guard<ACE_Recursive_Thread_Mutex> ContextGuard;
      void createEntry (const std::string&);

      int getHighestDegreeNode (Madara::Deployment::Deployment & deployment,
        DeploymentEliminationList & remainingRanks);

      int averageLatency (Madara::PeerLatency & list, int degree);
      void generateBestGuess (Madara::Deployment::Candidate & best, 
                      Madara::TopNerds & nerds);
      void generateDeployment ( Madara::Deployment::Candidate & best, 
        Madara::Deployment::Candidate & last,
        Madara::TopNerds & nerds);

      Madara::BrokerMap map_;
      Madara::Deployment::Deployment deployment_;
      int nerd_lookahead_;
      bool average_all_;
      int deployment_degree_;
      std::string host_;
      std::string port_;

      ACE_Recursive_Thread_Mutex mutex_;
    };

  }
#endif // BROKER_CONTEXT_H