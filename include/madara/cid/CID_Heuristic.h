#ifndef _CID_HEURISTIC_H_
#define _CID_HEURISTIC_H_

/**
 * @file Heuristic.h
 * @author James Edmondson <james.r.edmondson@vanderbilt.edu>
 *
 * This file contains declarations for the CID heuristic used for approximating
 * a good deployment. The heuristic may be used in conjunction with the genetic
 * algorithm in @see cid/Genetic.h.
 */

#include "madara/cid/CID_Settings.h"
#include "madara/MADARA_export.h"
#include "madara/utility/stdint.h"

namespace madara
{
  namespace cid
  {

    /**
     * Approximates the solution with the CID heuristic
     * @param    settings    container for network averages, target
     *                       deployment, and generated solution
     *                       (@see Madara::Cid::Settings)
     **/
    MADARA_Export void approximate (Settings & settings);

    /**
     * Approximates the solution with the CID heuristic
     * @param    settings    container for network averages, target
     *                       deployment, and generated solution
     *                       (@see Madara::Cid::Settings)
     * @param    node        the deployment node we are approximating
     **/
    MADARA_Export void approximate (Settings & settings, unsigned int node);

    /**
     * Approximates the solution with the CID heuristic
     * @param    network_summations  sorted array of network averages
     *                             (@see Madara::Cid::Settings)
     * @param    target_deployment sorted target deployment by degree
     * @param    solution          results of heuristic
     * @param    solution_lookup   map of network ids to their deployments
     * @param    node        the deployment node we are approximating
     **/
    MADARA_Export void approximate (Summations_Map & network_summations, 
      Workflow & target_deployment, Deployment & solution, 
      Solution_Map & solution_lookup, unsigned int node);

    /**
     * Calculates the total utility (i.e., latency) for the deployment.
     * Useful for determining gains made from deployment changes.
     * @param    settings    container which contains the target_deployment
     * @return               total latency on nodes with degree > 0
     **/
    MADARA_Export uint64_t calculate_latency (Settings & settings);

    /**
     * Calculates the total utility (i.e., latency) for the deployment.
     * Useful for determining gains made from deployment changes.
     * @param    latencies    network latencies
     * @param    workflow     the deployment workflow
     * @param    solution     the solution that was generated
     * @return                total latency on nodes with degree > 0
     **/
    MADARA_Export uint64_t calculate_latency (LV_Vector & latencies,
      Workflow & workflow, Deployment & solution);

    /**
     * Fills the remaining deployment from [start, end) with the best 
     * candidates from the lower averages of the highest degree.
     * @param    settings    container which contains the target_deployment
     **/
    MADARA_Export void fill_by_highest_degree (Settings & settings);

    /**
     * Fills the remaining deployment from [start, end) with the best 
     * candidates from the lower averages of the highest degree.
     * @param    settings       container which contains the target_deployment
     * @param    use_workflow   use workflow information. This should be
     *                          true unless you are only using this method
     *                          to fill in unconnected nodes of the deployment
     **/
    MADARA_Export void fill_by_highest_degree (Settings & settings,
      bool use_workflow);

    /**
     * Fills according to the solution lookup and remaining workflow.
     * @param    settings    container which contains the target_deployment
     **/
    MADARA_Export void fill_from_solution_map (Settings & settings);

    /**
     * Approximates the solution with path information
     * @param    settings    container which contains the target_deployment
     **/
    MADARA_Export void pathwise_approximate (Settings & settings);

    /**
     * Approximates the solution with path information
     * @param    settings    container which contains the target_deployment
     * @param    solution    the solution that will be filled
     * @param    lookup      a map of the solution locations
     **/
    MADARA_Export void pathwise_approximate (Settings & settings,
      Deployment & solution, Solution_Map & lookup);

    /**
     * Adds links to the source from the connected Links container
     * @param    settings    Settings container
     * @param    source      source instance
     * @param    connected   connected instance to populate more links from
     * @param    depth       current depth
     **/
    MADARA_Export void populate_links (Settings & settings, Links & source,
      Links & connected, unsigned int depth);

    /**
     * Sorts the target deployment by degree to prepare it for the CID heuristic
     * @param    settings    container which contains the target_deployment
     **/
    MADARA_Export void prepare_deployment (Settings & settings);

  } // end Cid namespace
} // end Madara namespace

#endif // _CID_HEURISTIC_H_
