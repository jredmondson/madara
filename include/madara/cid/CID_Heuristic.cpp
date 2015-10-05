
/**
 * @file Heuristic.cpp
 * @author James Edmondson <james.r.edmondson@vanderbilt.edu>
 *
 * This file contains definitions for the CID heuristic used for approximating
 * a good deployment. The heuristic may be used in conjunction with the genetic
 * algorithm in @see cid/Genetic.h.
 */

#include <algorithm>
#include <iostream>

#include "madara/utility/Log_Macros.h"
#include "madara/cid/CID_Convenience.h"
#include "madara/cid/CID_Heuristic.h"

void
madara::cid::approximate (Settings & settings)
{
  typedef std::map <unsigned int, unsigned int>  Candidate_Map;

  Workflow & deployment = settings.target_deployment;
  Solution_Map & lookup = settings.solution_lookup;
  Deployment & solution = settings.solution;
  Candidate_Map candidates;

  for (Workflow::size_type i = 0; i < deployment.size (); ++i)
  {
    unsigned int degree = deployment[i].size ();
    if (degree > 0)
    {
      unsigned int & candidate = candidates[degree];
      Latency_Vector & cur_summations = settings.network_summations[degree];

      unsigned int & source = deployment[i][0].first;

      for (; lookup.find (cur_summations[candidate].first) != lookup.end ();
               ++candidate);

      unsigned int & source_id = cur_summations[candidate].first;

      solution[source] = source_id;
      lookup[source_id] = source;

      ++candidate;
    }
    else
    {
#ifdef ENABLE_CID_LOGGING
      MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
        DLINFO "madara::cid::approximate:" \
          " calling fill_from_solution_map for i=%u\n",
          i));
#endif

      fill_from_solution_map (settings);
      i = deployment.size ();
    }
  }
}

void
madara::cid::approximate (Settings & settings, unsigned int node)
{
  approximate (settings.network_summations, 
    settings.target_deployment, settings.solution,
    settings.solution_lookup, node);
}

void
madara::cid::approximate (Summations_Map & network_summations,
      Workflow & target_deployment, Deployment & solution, 
      Solution_Map & solution_lookup, unsigned int node)
{
  /**
   * Some things to remember:
   * node = the node you are trying to solve
   * degree = target_deployment[node].size ()
   * network_summations[node] = sorted list of averages for that degree
   *                          remember that this is a map and to use a ref
   * This entire function is for solving 1 node and that is solution[node]
   **/

  Directed_Edges & current_flow = target_deployment[node];
  unsigned int degree = current_flow.size ();
  Latency_Vector & cur_summations = network_summations[degree];
  unsigned int source;

  if (degree > 0)
  {
    source = current_flow[0].first;

#ifdef ENABLE_CID_LOGGING
    MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
      DLINFO "madara::cid::approximate:" \
        " degree=%u, source=%u\n",
        degree, source));
#endif

    for (Latency_Vector::size_type i = 0; i < cur_summations.size (); ++i)
    {
      /**
       * If this is the first node in the deployment, or if we haven't tried
       * this solution before, then we have a match
       **/

      if (solution_lookup.find (cur_summations[i].first) == solution_lookup.end ())
      {
#ifdef ENABLE_CID_LOGGING
        MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
          DLINFO "madara::cid::approximate:" \
          " degree=%u, source=%u, i=%u: found solution for %u\n",
            degree, source, i, cur_summations[i].first));
#endif

        solution_lookup[cur_summations[i].first] = source;
        solution[source] = cur_summations[i].first;
        break;
      }
    }
  }
}

uint64_t
madara::cid::calculate_latency (Settings & settings)
{
  return calculate_latency (settings.network_latencies,
    settings.target_deployment, settings.solution);
}

uint64_t
madara::cid::calculate_latency (LV_Vector & latencies, Workflow & workflow,
                                Deployment & solution)
{
  uint64_t total_latency = 0;

  for (Workflow::size_type i = 0; i < workflow.size (); ++i)
  {
    Directed_Edges & current = workflow[i];

    if (current.size () > 0)
    {
      /**
       * source = the solution index number of the source of the directed edge
       * dest = the solution index number of the dest of the directed edge
       * solution[source] = the network id of the source
       * solution[dest] = the network id of the destination
       **/
      unsigned int & source = current[0].first;
      Latency_Vector source_latencies = latencies[solution[source]];

      // sort by ID so we can use the array as a O(1) lookup
      std::sort (source_latencies.begin (), source_latencies.end (),
                 Increasing_Id);

      for (Directed_Edges::size_type j = 0; j < current.size (); ++j)
      {
#ifdef ENABLE_CID_LOGGING
        MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
          DLINFO "madara::cid::calculate_latency:" \
          " workflow[%u]: adding current[%u].second to total_latency (%Q)\n",
            i, j, total_latency));
#endif

        unsigned int & dest = current[j].second;
        total_latency += source_latencies [solution[dest]].second;
      }

      // resort the latencies by Increasing Latency
      std::sort (source_latencies.begin (), source_latencies.end (),
                 Increasing_Latency);
    }
  }

#ifdef ENABLE_CID_LOGGING
  MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
    DLINFO "madara::cid::calculate_latency:" \
    " total_latency for deployment is %Q\n",
      total_latency));
#endif

  return total_latency;
}

void
madara::cid::fill_by_highest_degree (Settings & settings)
{
  fill_by_highest_degree (settings, true);
}

void
madara::cid::fill_by_highest_degree (Settings & settings, bool use_workflow)
{
  if (settings.target_deployment.size () < 1)
    return;

  // if the solution has not been allocated
  if (settings.solution.size () < settings.target_deployment.size ())
    settings.solution.resize (settings.target_deployment.size ());

  // old version which looked at highest degree in deployment
  // unsigned int degree = settings.target_deployment[0].size ();

  /**
   * New version looks at the degree of the deployment size. The logic
   * behind this decision is that we'll prefer to place unused nodes that
   * have the lowest latency in case of conflicts with other workflows. These
   * nodes are more likely to fit within a heuristic's good destination ids
   **/
  unsigned int degree = settings.solution.size ();

  if (degree == 0 || degree == 1)
    MADARA_DEBUG (MADARA_LOG_ERROR, (LM_DEBUG, 
      DLINFO "madara::cid::fill_by_highest_degree:" \
      " ERROR: highest degree equals %u\n",
        degree));

  Latency_Vector & cur_summations = settings.network_summations[degree];
  Solution_Map & lookup = settings.solution_lookup;
  Deployment & solution = settings.solution;
  Workflow & deployment = settings.target_deployment;

  unsigned int candidate = 0;
  
  if (use_workflow)
  {
    // solve the high degree nodes in the deployment first 
    for (Workflow::size_type i = 0; i < deployment.size (); ++i)
    {
      Directed_Edges & source_flow = deployment[i];
      if (source_flow.size () > 0)
      {
        unsigned int & source = source_flow[0].first;
        unsigned int & source_id = solution[source];

        Solution_Map::iterator found = lookup.find (source_id);

        // if we haven't solved for source_id, then do that.
        if (found == lookup.end () || found->second != source)
        {
          for (; candidate < cur_summations.size (); ++candidate)
          {
            if (lookup.find (cur_summations[candidate].first) == lookup.end ())
            {
              source_id = cur_summations[candidate].first;
              lookup[cur_summations[candidate].first] = source;
              ++candidate;
              break;
            }
          }
        }
        
      } // end if the degree was greater than 1
      else
        // break out of solving the source nodes
        break;
    }

    // solve for all connected nodes in the deployment
    for (unsigned int i = 0; i < deployment.size (); ++i)
    {
      Directed_Edges & source_flow = deployment[i];
      if (source_flow.size () > 0)
      { 
        for (Directed_Edges::size_type j = 0; j < source_flow.size (); ++j)
        {
          unsigned int & dest = source_flow[j].second;
          unsigned int & dest_id = solution[dest];

          Solution_Map::iterator found = lookup.find (dest_id);

          // if we can't find the id, we need to place our best candidate
          if (found == lookup.end () || found->second != dest)
          {
            for (; candidate < cur_summations.size (); ++candidate)
            {
              // if we find a winner, place it and break back to j loop
              if (lookup.find (cur_summations[candidate].first) == lookup.end ())
              {
  #ifdef ENABLE_CID_LOGGING
                MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
                DLINFO "madara::cid::fill_from_solution_map:" \
                " found solution[%u]=%u\n",
                dest, cur_summations[candidate].first));
  #endif

                solution[dest] = cur_summations[candidate].first;
                lookup[cur_summations[candidate].first] = dest;
                ++candidate;
                break;
              }
            } // end for k
          } // end couldn't find dest_id
        } // end for j
      } // end if the degree was greater than 1
    }
  } // end if use_workflow

  // loop through solution and solve any outstanding nodes
  for (unsigned int i = 0; i < solution.size (); ++i)
  {
    Solution_Map::iterator found = lookup.find (solution[i]);
    if (found == lookup.end () || found->second != i)
    {
      // we haven't solved this solution element yet. Look to the averages
      for (; candidate < cur_summations.size (); ++candidate)
      {
        // if we find a winner, place it and break back to j loop
        if (lookup.find (cur_summations[candidate].first) == lookup.end ())
        {
#ifdef ENABLE_CID_LOGGING
          MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
          DLINFO "madara::cid::fill_by_highest_degree:" \
          " found solution[%u]=%u\n",
          i, cur_summations[candidate].first));
#endif

          solution[i] = cur_summations[candidate].first;
          lookup[cur_summations[candidate].first] = i;
          ++candidate;
          break;
        }
      } // end for k
    } // end if solution[i] not found in lookup
  } // end for i -> solution.size ()
}

void
madara::cid::fill_from_solution_map (Settings & settings)
{
  // setup some references for simplicity
  Workflow & deployment = settings.target_deployment;
  Solution_Map & lookup = settings.solution_lookup;
  Deployment & solution = settings.solution;

  // iterate until we find a 1 degree deployment node
  for (Workflow::size_type i = 0; i < deployment.size (); ++i)
  {
    Directed_Edges & source_flow = deployment[i];
    if (source_flow.size () > 0)
    {
      unsigned int & source = source_flow[0].first;
      unsigned int & source_id = solution[source];
      Latency_Vector & latencies = settings.network_latencies[source_id];
      
      unsigned int candidate = 0;

      for (Directed_Edges::size_type j = 0; j < source_flow.size (); ++j)
      {
        unsigned int & dest = source_flow[j].second;
        unsigned int & dest_id = solution[dest];

        Solution_Map::iterator found = lookup.find (dest_id);

        // if we can't find the id, we need to place our best latency endpoint
        if (found == lookup.end () || found->second != dest)
        {
          for (; candidate < latencies.size (); ++candidate)
          {
            // if we find a winner, place it and break back to j loop
            if (lookup.find (latencies[candidate].first) == lookup.end ())
            {
#ifdef ENABLE_CID_LOGGING
              MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
              DLINFO "madara::cid::fill_from_solution_map:" \
              " found solution[%u]=%u\n",
              dest, latencies[candidate].first));
#endif

              dest_id = latencies[candidate].first;
              lookup[latencies[candidate].first] = dest;
              ++candidate;
              break;
            }
          } // end for k
        } // end couldn't find dest_id
      } // end for j
    } // end if the degree was greater than 0
    else
    {
      // fill the remaining in with arbitrary ids
      fill_by_highest_degree (settings, false);
      i = deployment.size ();
    }
  }
}


void
madara::cid::pathwise_approximate (Settings & settings,
             Deployment & solution, Solution_Map & lookup)
{

}

void
madara::cid::pathwise_approximate (Settings & settings)
{
#define    MAX_SOLUTIONS     1
#define    MAX_PATHS         5
  // need to use the updates from prepare_deployment and populate_links
  Paths & paths = settings.paths;
  Solution_Map & lookup = settings.solution_lookup;
  Deployment & solution = settings.solution;
  LV_Vector & latencies = settings.network_latencies;
//  std::vector<Deployment> solutions;
  //std::vector<Solution_Map> lookups;
//  Deployment new_lookup (settings.network_latencies.size (),
//    madara::cid::INVALID_LOOKUP);
//  Latency_Vector utilities;
//  utilities.resize (MAX_SOLUTIONS);
//  solutions.resize (MAX_SOLUTIONS);
//  lookups.resize (MAX_SOLUTIONS);

  // this approximation algorithm will find 5 solutions
  //for (unsigned int i = 0; i < MAX_SOLUTIONS; ++i)
  //{
  //  solutions[i].resize (solution.size ());
  //  utilities[i].first = i;
  //}

  for (Paths::size_type i = 0; i < paths.size (); ++i)
  {
    unsigned int & source = paths[i].source;
    unsigned int & source_id = solution[source];

    Solution_Map::iterator found = lookup.find (source_id);
    if (found == lookup.end () || found->second != source)
    {
      // we know that this neighborhood is not solved yet
      unsigned int neighborhood_size = paths[i].degree > 0 ?
        paths[i].degree : solution.size ();

      // 
      Latency_Vector & cur_summations =
        settings.network_summations[neighborhood_size];

      unsigned int best = 0;
      for (Paths::size_type j = 0; j < MAX_SOLUTIONS && j < paths.size (); ++j)
      {
        // use the current lookup
        //lookups[j] = lookup;
        //std::copy (solution.begin (), solution.end (), solutions[j].begin ());

        for (; best < cur_summations.size (); ++best)
        {
          found = lookup.find (cur_summations[best].first);
          if (found == lookup.end ())
          {
            solution[source] = cur_summations[best].first;
            lookup[cur_summations[best].first] = source;
            ++best;
            break;
          }
        }

        // solve for everything in source's neighborhood
        Link_Vector & list = paths[i].list;
        Latency_Vector & source_latencies = latencies[cur_summations[j].first];

        std::sort (source_latencies.begin (), source_latencies.end (),
          Increasing_Id);

        for (Link_Vector::size_type k = 0; k < list.size (); ++k)
        {
          // is it even possible for this to be duplicated?
          found = lookup.find (solution[list[k].target]);
          if (found == lookup.end () || found->second != list[k].target)
          {
            Latency_Record path_latencies[MAX_PATHS];
            //Latency_Vector path_latencies;
            //path_latencies.resize (MAX_SOLUTIONS);

            unsigned int & degree = list[k].degree;
            Latency_Vector & target_averages = 
              settings.network_summations[degree];

            unsigned int m = 0;
            unsigned int actuals = 0;
            // take the best latency of the first MAX_SOLUTIONS
            for (Latency_Vector::size_type l = 0;
              l < MAX_PATHS && l < target_averages.size (); ++l)
            {
              for (; m < target_averages.size (); ++m)
              {
                // this should only happen if the lookup doesn't contain
                // the target_averages[m]
                found = lookup.find (target_averages[m].first);
                if (found == lookup.end ())
                {
                  path_latencies[l].first = target_averages[m].first;
                  path_latencies[l].second = 
                    source_latencies[target_averages[m].first].second +
                    target_averages[m].second;
                  ++m;
                  ++actuals;
                  break;
                }
              }
            }

            if (actuals > 1)
              std::sort (path_latencies, &path_latencies[actuals],
                Increasing_Latency);

            solution[list[k].target] = path_latencies[0].first;
            lookup[path_latencies[0].first] = list[k].target;
          }
        }

        std::sort (source_latencies.begin (), source_latencies.end (),
          Increasing_Latency);

        //pathwise_approximate (settings, solutions[j], lookups[j]);

        //utilities[j].second = calculate_latency (settings.network_latencies,
          //settings.target_deployment, solutions[j]);
      }

      // sort the results by latency. Smallest will be the best.
      //std::sort (utilities.begin (), utilities.end (), Increasing_Latency);

      // copy the best solution to the one in the settings container
      //std::copy (solutions[utilities[0].first].begin (),
        //solutions[utilities[0].first].end (), solution.begin ());

      // most importantly, copy the lookup or we'll refill everything
      //lookup = lookups[utilities[0].first];
    }
  }
}

void
madara::cid::populate_links (Settings & settings, Links & source, Links & connected,
                             unsigned int depth)
{
  Paths & paths = settings.paths;
  Workflow & deployment = settings.target_deployment;

  // go through the candidates in connected's list
  for (Directed_Edges::iterator j = deployment[connected.source].begin ();
    j != deployment[connected.source].end (); ++j)
  {
    unsigned int dest_id = j->second;

    // don't add the source to the list of paths
    if (dest_id == source.source)
      continue;

    bool needs_expanding = false;

    Link_Map::iterator found = source.dest.find (dest_id);

    if (found == source.dest.end ())
    {
      needs_expanding = true;
      source.dest[dest_id].length = depth + connected.dest[dest_id].length;
      source.dest[dest_id].target = dest_id;
      source.dest[dest_id].degree = deployment[dest_id].size ();
    }
    else if (found->second.length > depth + connected.dest[dest_id].length)
    {
      needs_expanding = true;
      source.dest[dest_id].length = depth + connected.dest[dest_id].length;
      source.dest[dest_id].target = dest_id;
      source.dest[dest_id].degree = deployment[dest_id].size ();
    }

    if (needs_expanding)
    {
      Links & dest = paths[dest_id];
      populate_links (settings, source, dest,
        source.dest[dest_id].length);
    }
  }
}

void
madara::cid::prepare_deployment (Settings & settings)
{
  Workflow & deployment = settings.target_deployment;
  Paths & paths = settings.paths;

  paths.clear ();
  std::map <unsigned int, bool> degrees;

  if (paths.size () != deployment.size ())
  {
    paths.resize (deployment.size ());

    for (unsigned int i = 0; i < deployment.size (); ++i)
    {
      paths[i].source = i;
    }
  }

#ifdef ENABLE_CID_LOGGING
  MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
  DLINFO "madara::cid::prepare_deployment:" \
  " filling paths with deployment info\n"));
#endif
  // 1. create links from current deployment degrees
  for (Workflow::size_type i = 0;
    i < deployment.size (); ++i)
  {
    if (deployment[i].size () != 0)
    {
      unsigned int & source = deployment[i][0].first;

      for (Directed_Edges::size_type j = 0; j < deployment[i].size (); ++j)
      {
        unsigned int & dest = deployment[i][j].second;
        Link & current = paths[source].dest[dest];

        // copy degrees over to paths, except consider all links bidirectional
        current.target = dest;
        current.length = 1;
        current.degree = deployment[i].size ();
        degrees[current.degree] = true;
        paths[dest].dest[source].target = source;
        paths[dest].dest[source].length = 1;
      }
    }
  }

  // 2. update degree information inside of the paths structure.
  for (Paths::size_type source = 0;   
         source < paths.size () && paths[source].dest.size () > 0; ++source)
  {
    paths[source].degree = paths[source].dest.size ();
    degrees[paths[source].degree] = true;

    Link_Map & dest_map = paths[source].dest;

    for (Link_Map::iterator dest = dest_map.begin ();
         dest != dest_map.end (); ++dest)
    {
      dest->second.degree = paths[dest->second.target].dest.size ();
    }
  }

#ifdef ENABLE_CID_LOGGING
  MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
  DLINFO "madara::cid::prepare_deployment:" \
  " searching to fill all local neighborhoods\n"));
#endif
  // 3. iterate over these links until we have all possible links
  for (Paths::size_type i = 0;
         i < paths.size () && paths[i].dest.size () > 0; ++i)
  {
    for (unsigned int j = 0; j < deployment[i].size (); ++j)
    {
      unsigned int dest_id = deployment[i][j].second;
      Links & dest = paths[dest_id];
      populate_links (settings, paths[i], dest, 1);
    }
  }

#ifdef ENABLE_CID_LOGGING
  MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
  DLINFO "madara::cid::prepare_deployment:" \
  " copying the map to the vector\n"));
#endif
  // 4. copy the map to a vector
  for (Paths::size_type i = 0;
         i < paths.size () && paths[i].dest.size () > 0; ++i)
  {
    // create a list for sortable referencing
    paths[i].list.resize (paths[i].dest.size ());

    unsigned int cur = 0;
    // 
    for (Link_Map::iterator j = paths[i].dest.begin ();
      j != paths[i].dest.end (); ++j, ++cur)
    {
      paths[i].list[cur] = j->second;
    }

    // we won't use the map anymore. Can potentially save us a lot of space.
    paths[i].dest.clear ();

    // sort the list by decreasing depth
    std::sort (paths[i].list.begin (), paths[i].list.end (),
      Decreasing_Degree);

    // prepare latencies based on the size of the neighborhood
  }

#ifdef ENABLE_CID_LOGGING
  MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
  DLINFO "madara::cid::prepare_deployment:" \
  " sorting the deployment and paths\n"));
#endif
  // 5. sort 
  std::sort (deployment.begin (), deployment.end (),
             Decreasing_Size_Directed_Edges);
  std::sort (paths.begin (), paths.end (), Decreasing_Size_And_Degrees_Links);

  // 6. populate settings.degrees, so we can cheat in prepare_summation
  degrees[settings.network_latencies.size ()] = true;
  settings.degrees.resize (degrees.size ());
  unsigned int k = 0;
  for (std::map <unsigned int, bool>::iterator i = degrees.begin ();
       i != degrees.end (); ++i, ++k)
  {
    // setup degrees array
    settings.degrees[k] = i->first;

    // setup the summations vector for this degree
    Latency_Vector & summations = settings.network_summations[i->first];
    summations.resize (settings.network_latencies.size ());
    for (unsigned int l = 0; l < summations.size (); ++l)
    {
      summations[l].first = l;
    }
  }

  std::sort (settings.degrees.begin (), settings.degrees.end (),
    Increasing_Int);
}
