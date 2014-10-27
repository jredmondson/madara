#ifndef DEPLOYMENT_CPP
#define DEPLOYMENT_CPP

#include "stdlib.h"
#include "Deployment.h"

Madara::Deployment::Deployment 
Madara::Deployment::read (std::string filename, int & num_ranks)
{
  Madara::Deployment::Deployment deployment;
  std::ifstream input (filename.c_str ());

  if (input.is_open ())
    {
      // read the total number of ranks required for the deployment
      input >> num_ranks;

      while (input)
        {
          // read each deployment requirement
          std::string operation;
          int source;
          int target;

          input >> source;
          input >> operation;
          input >> target;

          // if the op is <->, then links need to be optimized
          // for bidirectional links between target/source
          if (operation == "<->")
            {
              add (deployment, source, target);
              add (deployment, target, source);
            }
          // if the op is ->, then links need to be optimized
          // for unidirectional link between source to target
          else if (operation == "->")
            {
              add (deployment, source, target);
            }
          // if the op is <-, then links need to be optimized
          // for unidirectional link between target to source
          // this is a a convenience mechanism and is somewhat
          // counterintuitive to a deployment file human reader
          else if (operation == "<-")
            {
              add (deployment, target, source);
            }
        }
    }

  return deployment;
}

::size_t
Madara::Deployment::add (Madara::Deployment::Deployment & deployment,
                         int source, int target)
{
  ::size_t size = 0;

  if (source != target)
    {
      deployment[source][target] = true;

      size = deployment[source].size ();
    }

  return size;
}

// currently this just adds all latencies and that is the utility
// thus the lower the utility the better total performance of the deployment

int 
Madara::Deployment::calculateUtility (
  const Madara::Deployment::Deployment& deployment,
  Madara::Deployment::Candidate & candidate, 
  Madara::BrokerMap & map)
{
  int utility = 0;
  int subutility = 0;
  for (Madara::Deployment::Deployment::const_iterator i = deployment.begin (); 
       i != deployment.end (); ++i)
    {
      std::map <int, bool>::const_iterator j = i->second.begin ();
      for (; j != i->second.end (); ++j)
        {
          //Madara::PeerLatencyMap latencies = map_[candidate[i->first]].latencies;
          subutility = map[candidate[i->first]].latencies[candidate[j->first]];
          utility += subutility; //1000 / subutility;
        }
        //output << "  " << i->first << " -> " <<  j->first << "\n";
    }
  return utility;
}

int
Madara::Deployment::mutate (const Madara::Deployment::Deployment & deployment,
           Madara::Deployment::Candidate & candidate,
           Madara::BrokerMap & map)
{
  int cur_best = calculateUtility (deployment, candidate, map);

  Candidate next_gen (candidate);

  int num_changes = rand () % candidate.size ();
  int source = rand () % candidate.size ();
  int dest = rand () % candidate.size ();

  for (int i = 0; i < num_changes; 
    ++i, source = rand () % candidate.size (), 
    dest = rand () % candidate.size ())
    {
      if (dest != source)
        std::swap (next_gen[source], next_gen[dest]);
    }

  int next_gen_utility = calculateUtility (deployment, next_gen, map);

  if (next_gen_utility < cur_best)
    {
      candidate = next_gen;
      return next_gen_utility;
    }

  return cur_best;
}

void
Madara::Deployment::write (Madara::Deployment::Deployment & deployment, 
                           std::ostream& output)
{
  for (Madara::Deployment::Deployment::iterator i = deployment.begin (); 
       i != deployment.end (); ++i)
    {
      std::map <int, bool>::iterator j = i->second.begin ();
      for (; j != i->second.end (); ++j)
        output << "  " << i->first << " -> " <<  j->first << "\n";
    }
}

#endif