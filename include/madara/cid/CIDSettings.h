#ifndef _CID_SETTINGS_H_
#define _CID_SETTINGS_H_

/**
 * @file Heuristic.h
 * @author James Edmondson <james.r.edmondson@vanderbilt.edu>
 *
 * This file contains declarations for the CID heuristic used for approximating
 * a good deployment. The heuristic may be used in conjunction with the genetic
 * algorithm in @see cid/Genetic.h.
 */

#include <string>
#include <vector>
#include <map>
#include "madara/utility/stdint.h"


namespace madara
{
  namespace cid
  {

    enum Types {
      CID        = 0,
      BCID       = 1,
      BCID_GGA   = 2,
      BCID_BGA   = 3,
      CID_BGA    = 4,
      CID_GGA    = 5
    };


    // Uncomment if you would like CID logging enabled
    //#define ENABLE_CID_LOGGING

    #define    INVALID_LOOKUP                (unsigned int)(0-1)

    /**
     * Container class for a link established within a deployment
     **/
    class Link
    {
    public:
      unsigned int target;
      unsigned int length;
      unsigned int degree;

      /**
       * Default constructor
       **/
      Link () : target (0), length (0), degree (0)
      {  
      }
    };

    /**
     * A map of @see Link
     **/
    typedef std::map <unsigned int, Link>            LinkMap;

    /**
     * A vector of @see Link
     **/
    typedef std::vector <Link>                       LinkVector;

    /**
     * A container for all links from a source
     **/
    class Links
    {
    public:
      unsigned int source;
      unsigned int degree;
      LinkMap dest;
      LinkVector list;
    };

    /**
     * A vector of degrees
     **/

    typedef std::vector <unsigned int>                     Degrees;

    /**
     * A pairing of a target (first) and a latency (second)
     **/
    typedef std::pair <unsigned int, uint64_t>   LatencyRecord;

    /**
     * Holds results of running an algorithm
     **/
    struct AlgorithmResult
    {
      int algorithm;
      std::string deployment;
      uint64_t latency;
    };

    /**
     * Holds all algorithm results
     **/
    typedef std::vector <AlgorithmResult>  AlgorithmResults;

    /**
     * Configuration for algorithm
     **/
    struct AlgorithmConfig
    {
      int algorithm;
      double   time;
    };

    /**
     * All configurations for algorithms
     **/
    typedef std::vector <AlgorithmConfig>   AlgorithmConfigs;

    /**
     * A vector of @see LatencyRecord
     **/
    typedef std::vector <LatencyRecord>             LatencyVector;

    /**
     * A vector of @see LatencyVector
     **/
    typedef std::vector <LatencyVector>             LVVector;

    /**
     * A directed edge has a source and destination pair
     **/
    typedef std::pair <unsigned int, unsigned int>   DirectedEdge;

    /**
     * A vector of @see DirectedEdge
     **/
    typedef std::vector <DirectedEdge>              DirectedEdges;

    /**
     * A vector of @see DirectedEdge
     **/
    typedef std::vector <DirectedEdges>             Workflow;

    /**
     * A lookup for all paths in the deployment.
     **/
    typedef std::vector <Links>                      Paths;

    /**
     * A list of deployment ids which correspond to individuals
     * in a @see LVVector
     **/
    typedef std::vector <unsigned int>               Deployment;

    /**
     * A list of latencies, mostly used for averages
     **/
    typedef std::vector <unsigned int>               Latencies;
    
    /**
     * A map of degree to the average latencies for that degree
     */
    typedef std::map <unsigned int, LatencyVector>  SummationsMap;

    /**
     * A map of the solution which allows reverse lookup by identifier
     **/
    typedef std::map <unsigned int, unsigned int>    SolutionMap;

    /**
     * A mapping of ids to string names. Using strings natively in the
     * algorithm would result high overhead (orders of magnitude) even
     * if we used "const std::string &" wherever possible. A map of strings
     * requires O(m) for each comparison within the std::map tree per access
     * of the key (and this is compounded when attempting to sort).
     **/
    typedef std::vector <std::string>     Identifiers;

    /**
     * Comparator for a list of increasing latency records
     **/
    static bool IncreasingLatency (
      const LatencyRecord & u, const LatencyRecord & v)
    {
      return u.second < v.second;
    }

    /**
     * Comparator for a list of increasing algorithm latency records
     **/
    static bool IncreasingAlgorithmLatency (
      const AlgorithmResult & u, const AlgorithmResult & v)
    {
      return u.latency < v.latency;
    }

    /**
     * Comparator for a list of increasing latency records
     **/
    static bool IncreasingId (
      const LatencyRecord & u, const LatencyRecord & v)
    {
      return u.first < v.first;
    }

    /**
     * Comparator for a list of decreasing sizes of latency vectors
     **/
    static bool DecreasingSizeDirectedEdges (
      const DirectedEdges & u, const DirectedEdges & v)
    {
      return u.size () > v.size ();
    }

    /**
     * Comparator for a list of decreasing sizes of number of links
     **/
    static bool DecreasingSizeLinkMap (
      const LinkMap & u, const LinkMap & v)
    {
      return u.size () > v.size ();
    }

    /**
     * Comparator for a list of decreasing sizes of number of links
     **/
    static bool DecreasingSizeAndDegreesLinks (
      const Links & u, const Links & v)
    {
      return u.dest.size () >= v.dest.size () && u.degree > v.degree;
    }

    /**
     * Comparator for an increasing path length
     **/
    static bool IncreasingPathLength (
      const Link & u, const Link & v)
    {
      return u.length < v.length;
    }

    /**
     * Comparator for an increasing target
     **/
    static bool IncreasingTarget (
      const Link & u, const Link & v)
    {
      return u.target < v.target;
    }

    /**
     * Comparator for an increasing source
     **/
    static bool IncreasingSource (
      const Links & u, const Links & v)
    {
      return u.source < v.source;
    }

    /**
     * Comparator for an increasing length
     **/
    static bool IncreasingLength (
      const Link & u, const Link & v)
    {
      return u.length < v.length;
    }

    /**
     * Comparator for an decreasing length
     **/
    static bool DecreasingLength (
      const Link & u, const Link & v)
    {
      return u.length > v.length;
    }

    /**
     * Comparator for decreasing degree
     **/
    static bool DecreasingDegree (
      const Link & u, const Link & v)
    {
      return u.degree > v.degree;
    }

    /**
     * Comparator for increasing degree
     **/
    static bool IncreasingDegree (
      const Link & u, const Link & v)
    {
      return u.degree < v.degree;
    }

    /**
     * Comparator for a list of increasing integers
     **/
    static bool IncreasingInt (
      const unsigned int & u, const unsigned int & v)
    {
      return u < v;
    }

    ///**
    // * Comparator for a list of decreasing integers
    // **/
    //static bool Decreasing (
    //  const int & u, const int & v)
    //{
    //  return v < u;
    //}

    /**
     * A public container for CID operations
     **/
    struct Settings
    {
      /**
       * A list of all the degrees in the deployment--including those that
       * are bidirectional degrees. This is useful in reducing workload during
       * prepare_summation
       **/
      Degrees      degrees;

      /**
       * Mapping of network unique ids to human-readables. These
       * ids correspond to an entry in the network_latencies table.
       * e.g.,
       *
       * ids[0] == "192.168.5.1:30000"
       * ids[1] == "192.168.5.2:30000"
       *
       *    or (if you prefer)
       *
       * ids[0] == "John's PC"
       * ids[1] == "Mike's PC"
       **/
      Identifiers   ids;

      /**
       * These are all of the network latencies between ids. They
       * should have each element sorted with IncreasingLatency so
       * this array can be used directly for populating solutions.
       *
       * ids[0] == "192.168.5.1"
       * ids[1] == "192.168.5.2"
       * ids[2] == "192.168.5.3"
       * ids[3] == "192.168.5.4"
       *
       * Given the above example ids, here is an example table and
       * what it means:
       *
       * [0][0][1,180]   192.168.5.1 => 192.168.5.2     180 us
       *    [1][2,50]    192.168.5.1 => 192.168.5.3      50 us
       *    [2][3,70]    192.168.5.1 => 192.168.5.4      70 us
       * [1][0][0,180]   192.168.5.2 => 192.168.5.1     180 us
       *    [1][2,60]    192.168.5.2 => 192.168.5.3      60 us
       *    [2][3,50]    192.168.5.2 => 192.168.5.4      50 us
       * [2][0][0,50]    192.168.5.3 => 192.168.5.1      50 us
       *    [1][1,60]    192.168.5.3 => 192.168.5.2      60 us
       *    [2][3,80]    192.168.5.3 => 192.168.5.4      80 us
       * [3][0][0,70]    192.168.5.4 => 192.168.5.1      70 us
       *    [1][1,50]    192.168.5.4 => 192.168.5.2      50 us
       *    [2][2,80]    192.168.5.4 => 192.168.5.3      80 us
       **/
      LVVector     network_latencies;

      /**
       * These are the averages for the network_latencies vector
       * e.g. if the network_latencies were the following:
       *
       * [0][0][1,180]   192.168.5.1 => 192.168.5.2     180 us
       *    [1][2,50]    192.168.5.1 => 192.168.5.3      50 us
       *    [2][3,70]    192.168.5.1 => 192.168.5.4      70 us
       * [1][0][0,180]   192.168.5.2 => 192.168.5.1     180 us
       *    [1][2,60]    192.168.5.2 => 192.168.5.3      60 us
       *    [2][3,50]    192.168.5.2 => 192.168.5.4      50 us
       * [2][0][0,50]    192.168.5.3 => 192.168.5.1      50 us
       *    [1][1,60]    192.168.5.3 => 192.168.5.2      60 us
       *    [2][3,80]    192.168.5.3 => 192.168.5.4      80 us
       * [3][0][0,70]    192.168.5.4 => 192.168.5.1      70 us
       *    [1][1,50]    192.168.5.4 => 192.168.5.2      50 us
       *    [2][2,80]    192.168.5.4 => 192.168.5.3      80 us
       *
       * Then the network averages array should be:
       *
       * [0] = 100
       * [1] =  96
       * [2] =  63
       * [3] =  66
       **/
      SummationsMap         network_summations;

      /**
       * This is the target workflow that is being optimized. Note that the
       * ordering doesn't matter, e.g., 
       *
       * [0][1][1,4]            [1][0][0,1]
       * [0][1][1,2]            [1][0][0,2]
       *                and
       * [0][0][0,1]            [1][1][1,4]
       * [0][0][0,2]            [1][1][1,2]
       * 
       * are the same target deployment. Each of them are asking to create
       * a deployment that seeks to optimize links for component 0 and 1 where
       * 
       *        0->1    and     1->2
       *        0->2            1->4
       *
       * which equates to a visual of this:
       *
       *              2
       *           /  
       *        0     |
       *           \    
       *              1  - 4
       *              
       **/
      Workflow     target_deployment;

      /**
       * A listing of all paths
       **/
      Paths        paths;

      /**
       * This is the solution to the target_deployment according
       * to the averages in network_summations. These integers should
       * be mapped to the ids listing for user-readable deployments.
       * e.g., if the target deployment was
       * 
       * [0][1][1,4]            [1][0][0,1]
       * [0][1][1,2]            [1][0][0,2]
       *
       * The solution would require placement of 0, 1, 2, and 4 at least.
       * The result for 3 is arbitrary since it is not indicated as important.
       * Consequently, our solution should have 5 elements and a mapping
       * can be made to the ids array
       **/
      Deployment    solution;

      /**
       * The solution_lookup allows for reverse lookup by network identifier
       * to find its corresponding deployment solution id. This is used by
       * the Madara::Cid::approximate function to check if a network id
       * has already been assigned to one of the other components in the
       * deployment.
       **/
      SolutionMap  solution_lookup;


      /**
       * The current round of algorithm results
       **/
      AlgorithmResults results;

      /**
       * configurations for all algorithms
       **/
      AlgorithmConfigs algorithm_configs;
    };
  }
}

#endif // _CID_SETTINGS_H_
