#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

#include <utility>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>

#include "Madara_Common.h"

namespace Madara
{

  namespace Deployment
    {

      typedef std::vector <std::string> Candidate;
      typedef std::map <int, std::map <int,bool>> Deployment;

      int calculateUtility (const Deployment & deployment,
        Candidate & candidate, Madara::BrokerMap & map);
      
      int mutate (const Deployment & deployment, 
        Candidate & candidate, Madara::BrokerMap & map);

      Deployment read (std::string filename, int & num_ranks);
      void write (Deployment & deployment, std::ostream& output);
      ::size_t add (Deployment & deployment, int source, int target);
    }
}

#endif