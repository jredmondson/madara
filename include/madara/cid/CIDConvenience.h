#ifndef _CID_CONVENIENCE_H_
#define _CID_CONVENIENCE_H_

/**
 * @file Convenience.h
 * @author James Edmondson <james.r.edmondson@vanderbilt.edu>
 *
 * This file contains declarations of convenience functions for developers
 * using the CID heuristic.
 */

#include "madara/cid/CIDSettings.h"
#include "madara/MADARA_export.h"

namespace madara
{
  namespace cid
  {
    /**
     * Checks a solution for duplicate entries
     * @param    settings    container for solution and solution_lookup
     **/
    MADARA_Export bool check_solution (Settings & settings);

    /**
     * Generates a random, fully-connected network of latencies
     * @param       size         the number of nodes in the network
     * @param       settings     container for the network_latencies
     **/
    MADARA_Export void generate_random_network (unsigned int size,
      Settings & settings);

    /**
     * Generates latencies 
     * @param       settings     container for the network_latencies
     * @param       worst        the worst case latency to set each to
     **/
    MADARA_Export void reset_latencies (Settings & settings,
      uint64_t worst);

    /**
     * Generates a random, fully-connected network of latencies
     * @param       size                  the number of nodes in the network
     * @param       network_latencies     the generated network latencies
     **/
    MADARA_Export void generate_random_network (unsigned int size,
      LVVector & network_latencies);

    /**
     * Generates a random solution
     * @param    settings    container for solution and solution_lookup
     **/
    MADARA_Export void generate_random_solution (Settings & settings);

    /**
     * Generates the worst solution by applying the worst averages to the
     * deployment
     * @param    settings    container for solution and solution_lookup
     **/
    MADARA_Export void generate_worst_solution (Settings & settings);

    /**
     * Initializes attributes of a container to a specified size. Does
     * not adjust the target_deployment size.
     * @param       size         the number of nodes in the network
     * @param       settings     container to adjust
     **/
    MADARA_Export void init (unsigned int size, Settings & settings);

    /**
     * From the current deployment plan, overlay perfect latencies that mimic
     * the deployment
     * 
     * @param       settings         container for CID settings
     * @param       min_latency      the latency to overlay on the deployment.
     * @param       min_noise        the minimum noise latency to overlay off
     *                               the deployment
     * @return      minimum total latency of the deployment
     **/
    MADARA_Export uint64_t overlay_latencies (Settings & settings,
      unsigned int min_latency, unsigned int min_noise);

    /**
     * Sorts the latencies in a network topology and averages them into a format
     * that can be used by CID. This function performs averages for all possible
     * degrees in the deployment.
     * @param    settings    container for network_latencies and network_summations
     *                       used by the method
     **/
    MADARA_Export void prepare_latencies (Settings & settings);
    
    /**
     * Sorts the latencies and sums them for the degrees in the deployment as
     * they apply to this source id.
     * @param    settings    container for network_latencies and network_summations
     *                       used by the method
     **/
    MADARA_Export std::string prepare_summations (unsigned int source,
      Settings & settings);
    
    /**
     * Sorts the latencies in a network topology and averages them into a format
     * that can be used by CID
     * @param    settings    container for network_latencies and network_summations
     *                       used by the method
     * @param    degree      degree to average
     **/
    MADARA_Export void prepare_latencies (Settings & settings,
      unsigned int degree);
    
    /**
     * Sorts the latencies in a network topology and averages them into a format
     * that can be used by CID
     * @param    network_latencies    source latency matrix which will be sorted
     *                                (@see Madara::Cid::Settings)
     * @param    network_summations     averaged latencies from each row of  
     *                                network_latencies
     *                                (@see Madara::Cid::Settings)
     * @param    target_deployment    application workflow to approximate
     * @param    degree               degree to average
     *                       
     **/
    MADARA_Export void prepare_latencies (LVVector & network_latencies,
                            SummationsMap & network_summations,
                            Workflow & target_deployment, unsigned int degree);

    /**
     * Prints the latencies in the settings container
     * @param    settings    container for network_latencies 
     **/
    MADARA_Export void print_latencies (Settings & settings);

    /**
     * Processes a deployment from a string. The contents may use special
     * characters to ease deployment definition. Examples:
     * 
     * =====start of file======
     * 10               // number of processes in the deployment
     * 0 -> [0,9]       // 0 sends to 0-9
     * 0 -> [1,4)       // process 0 is sending to processes 1 through 3
     * 0 -> 1           // process 0 is sending to process 1
     * 0 -> [size / 4, size / 2]     // 0 -> [2, 5]
     * [1,3] -> [source-1, source+1] // 1 -> [0, 2], 2 -> [1, 3], 3 -> [2, 4]
     * =====end of file======
     * 
     * @param       settings     container for CID settings
     * @param       contents     string containing the deployment info.
     * @return      false if there was a problem with the contents
     **/
    MADARA_Export bool process_deployment (Settings & settings,
      const std::string & contents);

    /**
     * Reads the deployment from a file. @see process_deployment for
     * deployment file configuration information.
     * @param       filename     the name of the file to read
     * @return      true if the file existed and was read
     **/
    MADARA_Export bool read_deployment (Settings & settings,
      const std::string & filename);

    /**
     * Resets solution and solution_map to the defaults
     * @param       settings     container for CID settings
     **/
    MADARA_Export void reset_solution (Settings & settings);

    /**
     * Converts the solution deployment into a string
     * @param       settings     container for CID settings
     **/
    MADARA_Export std::string stringify_solution (Settings & settings);

    
    /**
     * Overwrites the current ids to those indicated in the string. Format
     * for this comes from the stringify_solution function and appears as
     * the following: 0=host:port;1=host:port;2=host:port...
     * @param       settings     container for CID settings
     * @param       source       solution to apply to the CID settings
     **/
    MADARA_Export void read_solution (Settings & settings,
      const std::string & source);

  } // end Cid namespace
} // end Madara namespace

#endif // _CID_CONVENIENCE_H_