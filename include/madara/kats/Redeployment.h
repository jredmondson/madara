#ifndef _KATS_REDEPLOYMENT_
#define _KATS_REDEPLOYMENT_

#include <vector>
#include <string>
#include <map>
//#include "madara/transport/Settings.h"



namespace madara
{
  namespace Kats
  {
    class Dataflow
    {
    public:
      //Dataflow (TiXmlElement * parent,
      //  madara::transport::Settings & settings);
      //void apply (void);
    private:
      std::string flow_;
      //madara::transport::Settings & settings_;
    };

    struct Identifier
    { 
      int id;
      bool exclude;
    };

    class Identifiers
    {
    public:
      //Identifiers (TiXmlElement * parent);
    private:
      //std::map <int, 
    };

    class Algorithm
    {
      double time;
    };

    class Algorithms
    {

    };

    class Voting
    {
    public:
    private:
      Algorithms algorithms;
      Identifiers ids;
      Dataflow dataflow;

      double min_interval;
      double max_interval;
    };

    class Redeployment
    {
    public:
    private:
      Voting voting;
    };
  }
}

#endif // _KATS_REDEPLOYMENT_