#ifndef _CID_GENETIC_H_
#define _CID_GENETIC_H_

/**
 * @file Genetic.h
 * @author James Edmondson <james.r.edmondson@vanderbilt.edu>
 *
 * This file contains declarations of genetic algorithms that may be useful in
 * improving a deployment generated by the CID heuristic.
 */

#include "madara/cid/CID_Settings.h"
#include "madara/MADARA_export.h"

namespace Madara
{
  namespace Cid
  {
    /**
     * Randomly mutates chromosomes in the solution and updates solution
     * with best total latency, if found.
     * @param settings the constraint settings for the deployment
     * @param duration the time in seconds to allow the algorithm to run
     **/
    MADARA_Export void ga_naive (Settings & settings,
      double duration);

    /**
     * Randomly mutates chromosomes in the solution with a preference for
     * high constraint degree (the number of connections from the target).
     * @param settings the constraint settings for the deployment
     * @param duration the time in seconds to allow the algorithm to run
     **/
    MADARA_Export void ga_degree (Settings & settings,
      double duration);

  } // end namespace Cid

} // end namespace Madara

#endif 