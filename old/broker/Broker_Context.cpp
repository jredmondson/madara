
#include <iostream>
#include <fstream>
#include <algorithm>
#include "ace/High_Res_Timer.h"

#include "Madara_Common.h"
#include "Broker_Context.h"

Madara::Broker_Context::Broker_Context(int nerd_lookahead, bool average_all)
: nerd_lookahead_ (nerd_lookahead), average_all_ (average_all), 
  deployment_degree_ (0)
{
}

void
Madara::Broker_Context::read (const std::string& filename)
{
  Context_Guard guard (mutex_);
  
  int num_ranks = 0;
  deployment_ = Madara::Deployment::read (filename, num_ranks);
}

void
Madara::Broker_Context::write (const std::string& filename)
{
  Context_Guard guard (mutex_);
  std::ofstream output (filename.c_str ());

  if (output.is_open ())
    {
      write (output);
      output.close ();
    }
}

int 
Madara::Broker_Context::averageLatency (Madara::PeerLatency & list, int degree)
{
  //std::sort (list.vector.begin (), list.vector.end (), Madara::SortByPairValue);

  int total = 0;
  int min = !average_all_ && degree < list.vector.size () ? 
            degree : list.vector.size ();

  // don't blow up if the people put no restrictions on us
  min = min > 0 ? min : list.vector.size ();

  for (int i = 0; i < min; ++i)
    {
      //std::cout << list.vector[i].second << std::endl;
      total += list.vector[i].second;
    }

  return list.avg = total / min;
}

Madara::Deployment::Candidate
Madara::Broker_Context::learnDeployment (void)
{
  Madara::Deployment::Candidate best (map_.size ());
  Madara::Deployment::Candidate last (map_.size ());
  Madara::Deployment::Candidate cur (map_.size ());
  Madara::BrokerMap::iterator i;
  Madara::TopNerds nerds (map_.size ());
  int lookahead = nerd_lookahead_ * deployment_.size ();
  int num_candidates = map_.size ();
  int max_nerds = lookahead < num_candidates ? lookahead : num_candidates;
  int j = 0;
  // keep track of time
  ACE_hrtime_t measured;
  ACE_High_Res_Timer timer;

  timer.start ();

  //std::cout << "Computing averages over the deployment degree.\n";

  for (i = map_.begin (), j = 0; i != map_.end (); ++i, ++j)
    {
       averageLatency (i->second, deployment_degree_);
       nerds[j] = Madara::LatencyPair (i->first, i->second.avg);
       //std::cout << i->first << ".avg = " << i->second.avg << std::endl;
    }

  timer.stop ();
  timer.elapsed_time (measured);
  unsigned long long nanoseconds = measured;

  std::cerr << "***Averaging latencies took " << nanoseconds
         << " ns" << std::endl;

  std::sort (nerds.begin (), nerds.end (), Madara::SortByPairValue);

  //nerds.resize (max_nerds);
  
  //for (j = 0; j < nerds.size (); ++j)
   // {
      //std::cout << "Nerd " << j << ": " << nerds[j].first << " = " <<
                   //nerds[j].second << std::endl; 
   // }

  generateBestGuess (best, nerds);

  return best;
}

void
Madara::Broker_Context::generateBestGuess (
  Madara::Deployment::Candidate & cur,
  Madara::TopNerds & nerds)
{
  //Madara::Deployment::Candidate cur (map_.size ());
  Madara::Deployment::Deployment leftoverReqs = deployment_;
  EliminationList leftoverNodes;
  DeploymentEliminationList leftoverRanks;


  int i = 0;
  int selection = 0;
  std::string key;

  // setup the leftover ranks
  for (i = 0; i < nerds.size (); ++i)
    leftoverRanks[i] = true;

  // setup the leftover processors
  for (i = 0; i < map_.size (); ++i)
    leftoverNodes[nerds[i].first] = true;

  // at this point, leftoverReqs should be the deployment reqs
  // leftoverNodes should be all nodes

  for (i = 0; leftoverRanks.size () > 0 && i < map_.size (); ++i)
    {
      selection = getHighestDegreeNode (leftoverReqs, leftoverRanks);
      std::map <int,bool> connections = leftoverReqs[selection];

      if (cur[selection].length () == 0)
        {
          cur[selection] = getNextLowestLatencyAvailable (leftoverNodes,
                nerds); 
          leftoverRanks.erase (selection);
          leftoverNodes.erase (cur[selection]);
        }

      for (std::map <int,bool>::iterator j = connections.begin ();
           leftoverRanks.size () > 0 && j != connections.end (); ++j)
        {
          if (leftoverRanks.find (j->first) != leftoverRanks.end ())
            {
              key = getNextLowestLatencyAvailable (leftoverNodes,
                map_[cur[selection]].vector);
              cur[j->first] = key;
              leftoverNodes.erase (key);
              leftoverRanks.erase (j->first);
            }
        }

      leftoverReqs.erase (selection);
    }
}

void
Madara::Broker_Context::generateDeployment (
  Madara::Deployment::Candidate & best, Madara::Deployment::Candidate & last,
  Madara::TopNerds & nerds)
{

}

std::string Madara::Broker_Context::getNextLowestLatencyAvailable (
  const Madara::EliminationList & remainingNodes, 
  const Madara::PeerLatencyVector & sortedLatencies)
{

  if (remainingNodes.size () == 0)
    return "";

  for (int i = 0; i < sortedLatencies.size (); ++i)
    {
      if (remainingNodes.find (sortedLatencies[i].first) != remainingNodes.end ())
        return sortedLatencies[i].first;
    }
  return remainingNodes.begin ()->first;
}

int Madara::Broker_Context::getHighestDegreeNode (Madara::Deployment::Deployment & deployment,
        DeploymentEliminationList & remainingRanks)
{
  if (remainingRanks.size () == 0)
    return -1;

  int j, cur_max_node = remainingRanks.begin()->first, cur_max = 0;

  for (Madara::Deployment::Deployment::iterator i = deployment.begin (); 
       i != deployment.end (); ++i)
    {
      if (i->second.size () > cur_max)
        {
          cur_max_node = i->first;
          cur_max = i->second.size ();
        }
    }

  return cur_max_node;
}

void
Madara::Broker_Context::write (std::ostream& output)
{
  for (BrokerMap::iterator i = map_.begin (); i != map_.end (); ++i)
    {
      output << i->first << " " << i->second.latencies.size () << "\n";
      PeerLatencyMap::iterator j = i->second.latencies.begin ();
      for (; j != i->second.latencies.end (); ++j)
        output << "    " << j->first << " " << j->second << "\n";
    }
}

void 
Madara::Broker_Context::addLatency (const std::string& key, 
                                    const std::string& target, int latency)
{
  Context_Guard guard (mutex_);

  if (key != target)
    {
      map_[key].latencies[target] = latency;
      map_[key].vector.push_back (Madara::LatencyPair (target, latency));
    }
}

void 
Madara::Broker_Context::addRequirement (int source, int target)
{
  Context_Guard guard (mutex_);

  Madara::Deployment::add (deployment_, source, target);

  if (deployment_[source].size () > deployment_degree_)
    deployment_degree_ = deployment_[source].size ();
}

void 
Madara::Broker_Context::removeRequirement (int source, int target)
{
  Context_Guard guard (mutex_);

  if (deployment_.find (source) != deployment_.end ())
    {
      deployment_.find (source)->second.erase (target);
    }
}

void 
Madara::Broker_Context::clearRequirements (void)
{
  Context_Guard guard (mutex_);

  deployment_.erase (deployment_.begin (), deployment_.end ());
}

int
Madara::Broker_Context::mutate (Madara::Deployment::Candidate & candidate)
{
  return Madara::Deployment::mutate (deployment_, candidate,
    map_);
}

Madara::Deployment::Deployment &
Madara::Broker_Context::getDeployment (void)
{
  return deployment_;
}

Madara::BrokerMap & 
Madara::Broker_Context::getBrokerMap (void)
{
  return map_;
}

void 
Madara::Broker_Context::writeRequirements (std::ostream& output)
{
  Context_Guard guard (mutex_);

  for (Madara::Deployment::Deployment::iterator i = deployment_.begin (); 
       i != deployment_.end (); ++i)
    {
      output << i->first << " " << i->second.size () << "\n";
      std::map <int, bool>::iterator j = i->second.begin ();
      for (; j != i->second.end (); ++j)
        output << "    " << j->first << " " << j->second << "\n";
    }
}

void 
Madara::Broker_Context::writeRequirements (const std::string& filename)
{
  Context_Guard guard (mutex_);
  std::ofstream output (filename.c_str ());

  if (output.is_open ())
    {
      writeRequirements (output);
      output.close ();
    }
}

void 
Madara::Broker_Context::writeRequirementsLegible (std::ostream& output)
{
  Context_Guard guard (mutex_);

  output << "\nOptimize the following links\n";

  Madara::Deployment::write (deployment_, output);
}

void 
Madara::Broker_Context::writeDeploymentCandidate (std::ostream& output, 
                              Madara::Deployment::Candidate & candidate)
{
  Context_Guard guard (mutex_);

  output << "\nDeployment Candidate\n";

  for (int i = 0; i < candidate.size (); ++i)
    output << "  " << i << ": " << candidate[i] << "\n";
}

unsigned int  
Madara::Broker_Context::getNumKeys ()
{
  return map_.size ();
}

std::string  
Madara::Broker_Context::getKey (unsigned int pos)
{
  std::string unfound = "";

  if (pos < map_.size ())
    {
      BrokerMap::iterator i = map_.begin ();
      for(unsigned int cur = 0; cur < pos; ++cur, ++i);

      return i->first;
    }
  return unfound;
}

const std::string&  
Madara::Broker_Context::getHost ()
{
  Context_Guard guard (mutex_);
  return host_;
}

void 
Madara::Broker_Context::setHost (const std::string& host)
{
  Context_Guard guard (mutex_);
  host_ = host;
}

const std::string&  
Madara::Broker_Context::getPort ()
{
  Context_Guard guard (mutex_);
  return port_;
}

void 
Madara::Broker_Context::setPort (const std::string& port)
{
  Context_Guard guard (mutex_);
  port_ = port;
}

Madara::PeerLatency
Madara::Broker_Context::getLatencyMap (unsigned int pos)
{
  return map_[getKey (pos)];
}

Madara::PeerLatency
Madara::Broker_Context::getLatencyMap (const std::string & key)
{
  Context_Guard guard (mutex_);
  return map_[key];
}

bool
Madara::Broker_Context::peerExists (const std::string& key)
{
  return map_.find (key) != map_.end ();
}

bool 
Madara::Broker_Context::peerExists (const std::string& host, u_short u_port)
{
  char port_buf[16];
  std::string key;
  std::string port;

  // convert the server_port into string form for our context
  itoa (u_port, port_buf, 10);

  port = port_buf;

  Madara::merge_key (key, host, port);

  return peerExists (key);
}

bool 
Madara::Broker_Context::peerExists (const std::string& host, const std::string& port)
{
  std::string key;
  Madara::merge_key (key, host, port);

  return peerExists (key);
}

