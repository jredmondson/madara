
#include <iostream>
#include <algorithm>
#include <sstream>
#include "madara/cid/CIDConvenience.h"
#include "madara/cid/CIDHeuristic.h"
#include "madara/knowledge_engine/KnowledgeBase.h"
#include "madara/utility/LogMacros.h"
#include "madara/utility/Utility.h"

/**
 * Checks a solution for duplicate entries
 **/
bool
madara::cid::check_solution (Settings & settings)
{
  Deployment & solution = settings.solution;

  SolutionMap instances;
  bool multiple_entries = false;

  for (unsigned int i = 0; i < solution.size (); ++i)
  {
    ++instances[solution[i]];

    if (instances[solution[i]] > 1)
      multiple_entries = true;
  }

  if (multiple_entries)
  {
    for (unsigned int i = 0; i < solution.size (); ++i)
    {
      if (instances[solution[i]] > 1)
      {
        MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
          DLINFO "madara::cid::check_solution:" \
          " ERROR: %u duplicate entries found for id %u." \
          " Please save a log and file issue at madara.googlecode.com.\n",
            instances[solution[i]], solution[i]));

        /**
         * to stop this from being called multiple times for the same entry,
         * set the instances[solution[i]] to 1.
         **/
        instances[solution[i]] = 1;
      }
    }
  }

  return multiple_entries;
}

/**
 * Reset latencies
 **/
void
madara::cid::reset_latencies (Settings & settings, uint64_t worst)
{
  for (unsigned int i = 0; i < settings.network_latencies.size (); ++i)
  {
    LatencyVector & latencies = settings.network_latencies[i];
    for (LatencyVector::iterator j = latencies.begin ();
      j != latencies.end (); ++j)
    {
      // expect most to be the worst
      if (j->second != worst)
        j->second = worst;
    }
  }
}

/**
 * Generates a random, fully-connected network of latencies
 **/
void
madara::cid::generate_random_network (unsigned int size, Settings & settings)
{
  generate_random_network (size, settings.network_latencies);
}

/**
 * Generates a random, fully-connected network of latencies
 **/
void
madara::cid::generate_random_network (unsigned int size,
                                      LVVector & network_latencies)
{
#ifdef ENABLE_CID_LOGGING
  MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
    DLINFO "madara::cid::generate_random_network:" \
      " Generating random network of size %u\n",
      size));
#endif

  // allocate memory for the latencies
  if (network_latencies.size () != size)
    network_latencies.resize (size);

  for (unsigned int i = 0; i < size; ++i)
  {
#ifdef ENABLE_CID_LOGGING
    MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
      DLINFO "madara::cid::generate_random_network:" \
      " resizing network_latencies[%u]\n",
        i));
#endif

    if (network_latencies[i].size () != size)
      network_latencies[i].resize (size);

    for (unsigned int j = 0; j < size; ++j)
    {
#ifdef ENABLE_CID_LOGGING
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        DLINFO "madara::cid::generate_random_network:" \
        " setting network_latencies[%u][%u]\n",
          i, j));
#endif

      if (network_latencies[i][j].second == 0)
      {
        LatencyVector & source_latencies = network_latencies[i];
        LatencyVector & dest_latencies = network_latencies[j];

        // make the latencies bidirectionally equivalent for realism
        source_latencies[j].first = j;
        source_latencies[j].second = rand () + 10;
        dest_latencies[i].first = i;
        dest_latencies[i].second = source_latencies[j].second;
      }
    }

#ifdef ENABLE_CID_LOGGING
    MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
      DLINFO "madara::cid::generate_random_network:" \
      " sorting network_latencies[%u]\n",
        i));
#endif

    // sort each row of the network_latencies in order of increasing latency
    std::sort (network_latencies[i].begin (), network_latencies[i].end (),
               IncreasingLatency);
  }

}

void
madara::cid::generate_random_solution (Settings & settings)
{
#ifdef ENABLE_CID_LOGGING
  MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
    DLINFO "madara::cid::generate_random_solution:" \
      " Generating random solution of size %u\n",
      settings.solution.size ()));
#endif

  for (unsigned int i = 0; i < settings.solution.size (); ++i)
  {
    settings.solution[i] = i;
    settings.solution_lookup[i] = i;
  }
}

void
madara::cid::generate_worst_solution (Settings & settings)
{
  settings.solution_lookup.clear ();

  if (settings.target_deployment.size () < 1)
    return;

  unsigned int start = 0;

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
  unsigned int degree = settings.network_latencies.size ();

  LatencyVector & cur_summations = settings.network_summations[degree];
  SolutionMap & solution_lookup = settings.solution_lookup;
  Deployment & solution = settings.solution;
  Workflow & deployment = settings.target_deployment;

  for (unsigned int i = 0; 
    start < solution.size () && i < deployment.size (); ++i)
  {
    unsigned int actual = cur_summations.size () - i - 1;
    if (start == 0 ||
      solution_lookup.find (cur_summations[actual].first) == solution_lookup.end ())
    {    
#ifdef ENABLE_CID_LOGGING
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
      DLINFO "madara::cid::fill_by_highest_degree:" \
      " found solution[%u]=%u\n",
      start, cur_summations[i].first));
#endif

      solution_lookup[cur_summations[actual].first] = start;
      solution[start] = cur_summations[actual].first;
      ++start;
    }
  }
}

/**
 * Initializes attributes of a container to a specified size. Does
 * not adjust the target_deployment size.
 **/
void
madara::cid::init (unsigned int size, Settings & settings)
{
#ifdef ENABLE_CID_LOGGING
  MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
    DLINFO "madara::cid::init:" \
      " Resizing latencies, ids, and solution to size %u\n",
      size));
#endif

  settings.network_latencies.resize (size);
  settings.ids.resize (size);
  settings.solution.resize (size);
  settings.paths.resize (size);

#ifdef ENABLE_CID_LOGGING
  MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
    DLINFO "madara::cid::init:" \
      " Resizing individual network latency entries to size %u\n",
      size));
#endif

  for (unsigned int i = 0; i < settings.network_latencies.size (); i++)
  {
    settings.network_latencies[i].resize (size);
    settings.paths[i].source = i;
  }
}

/**
 * From the current deployment plan, overlay perfect latencies that mimic
 * the deployment
 * 
 * @param       settings     container for CID settings
 * @param       min_latency      the latency to overlay on the deployment.
 * @param       min_noise        the minimum noise latency to overlay off
 *                               the deployment
 * @return      minimum latency deployment in the network
 **/
uint64_t
madara::cid::overlay_latencies (Settings & settings, 
                                unsigned int min_latency,
                                unsigned int min_noise)
{
  LVVector & latencies = settings.network_latencies;
  Workflow & deployment = settings.target_deployment;
  Paths & paths = settings.paths;
  uint64_t total = 0;

  // place the minimal noise
  for (unsigned int i = 0; i < latencies.size (); ++i)
  {
    for (unsigned int j = 0; j < latencies[i].size (); ++j)
    {
      latencies[i][j].first = j;
      latencies[i][j].second = rand () + min_noise;
    }
  }

  // place the minimal latencies
  for (unsigned int i = 0; i < deployment.size (); ++i)
  {
    DirectedEdges & edges = deployment[i];
    for (unsigned int j = 0; j < edges.size (); ++j)
    {
      latencies[edges[j].first][edges[j].second].second =
        (uint64_t) min_latency;

      total += latencies[edges[j].first][edges[j].second].second;
    }
  }

  // place the minimal edges
  for (unsigned int i = 0; i < paths.size (); ++i)
  {
    unsigned int & source = paths[i].source;
    for (unsigned int j = 0; j < paths[i].list.size (); ++j)
    {
      unsigned int & dest = paths[i].list[j].target;
      unsigned int & length = paths[i].list[j].length;

      latencies[source][dest].second = length * min_latency;
    }
  }

  return total;
}


void
madara::cid::prepare_latencies (Settings & settings)
{
  // setup averages for the size of the underlying environment
  prepare_latencies (settings, settings.network_latencies.size ());
  
  // setup averages for the size of the intended solution
  prepare_latencies (settings, settings.solution.size ());
  
  // Now create averages[degrees] for the degrees of the deployment
  for (unsigned int i = 0; i < settings.target_deployment.size (); ++i)
    prepare_latencies (settings, settings.target_deployment[i].size ());

  // Now create averages[degrees] for the degrees and neighborhoods
  for (unsigned int i = 0; i < settings.paths.size (); ++i)
  {
    prepare_latencies (settings, settings.paths[i].degree);
    prepare_latencies (settings, settings.paths[i].list.size ());
  }
}

std::string
madara::cid::prepare_summations (unsigned int source, Settings & settings)
{
  if (source >= settings.network_latencies.size ())
    return "";

  LatencyVector & latencies = settings.network_latencies[source];
  SummationsMap & network_summations = settings.network_summations;
  Degrees & degrees = settings.degrees;

  // sort the latencies of this source 
  std::sort (latencies.begin (), latencies.end (), IncreasingLatency);

  uint64_t total = 0;
  std::stringstream buffer;
  unsigned int j = 0;
  unsigned int cur_degree = 0;

  for (Degrees::iterator i = degrees.begin (); i != degrees.end (); ++i)
  {
    cur_degree = *i;
    buffer << cur_degree << "=";
    for (; j < cur_degree && j < latencies.size (); ++j)
    {
      total += latencies[j].second;
    }
    buffer << total << ";";

    network_summations[cur_degree].resize (latencies.size ());
    network_summations[cur_degree][source].second = total;

    //std::cout << "recording summation of " << total << 
    //  " at degree = " << cur_degree << "\n";
  }

  //std::cout << "result is " << buffer.str () << "\n";

  return buffer.str ();
}

void
madara::cid::prepare_latencies (Settings & settings, unsigned int degree)
{
  if (degree != 0)
    prepare_latencies (settings.network_latencies, settings.network_summations,
      settings.target_deployment, degree);
}

void
madara::cid::prepare_latencies (LVVector & network_latencies,
                                SummationsMap & network_summations,
                                Workflow & target_deployment,
                                unsigned int degree)
{
#ifdef ENABLE_CID_LOGGING
  MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
    DLINFO "madara::cid::prepare_latencies:" \
      " Calculating averages for %u\n", degree));
#endif

  // if we've already done this degree, continue to the next deployed element
  if (network_summations.find (degree) != network_summations.end ())
    return;

  // we're dealing with a std::map which has O(log n) lookup. Use ref.
  LatencyVector & cur_summations = network_summations[degree];

  // make sure cur_summations has the right size
  if (cur_summations.size () != network_latencies.size ())
    cur_summations.resize (network_latencies.size ());

#ifdef ENABLE_CID_LOGGING
  MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
    DLINFO "madara::cid::prepare_latencies:" \
      " Entering main loop\n" 
      ));
#endif

  // loop through the network latency table and average the values
  for (unsigned int i = 0; i < network_latencies.size (); ++i)
  {
#ifdef ENABLE_CID_LOGGING
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        DLINFO "madara::cid::prepare_latencies:" \
        " i=%u\n",
        i));
#endif

      // remember which id this average is for and reset the total to zero
      cur_summations[i].first = i;

      // for each element of the list, add the latency to the running total
      for (unsigned int j = 0; j < degree; ++j)
      {
#ifdef ENABLE_CID_LOGGING
        MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
          DLINFO "madara::cid::prepare_latencies:" \
          "   j=%u\n",
          j));
#endif

        // nothing to gain from averaging except less precision
        cur_summations[i].second += network_latencies[i][j].second;
      }

#ifdef ENABLE_CID_LOGGING
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        DLINFO "madara::cid::prepare_latencies:" \
          " Dividing total by degree to give an average\n"));
#endif

  }

#ifdef ENABLE_CID_LOGGING
  MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
    DLINFO "madara::cid::prepare_latencies:" \
    " Sorting averages. network_summations[%u]\n",
    degree));
#endif

  // sort the network averages in increasing order. Best average is smallest.
  std::sort (cur_summations.begin (), cur_summations.end (),
    IncreasingLatency);
}

/**
 * Prints the latencies in the settings container
 * @param    settings    container for network_latencies 
 **/
void
madara::cid::print_latencies (Settings & settings)
{
  MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
    "Printing latencies\n"));

  LVVector & latencies = settings.network_latencies;

  for (unsigned int i = 0; i < latencies.size (); ++i)
  {
    for (unsigned int j = 0; j < latencies[i].size (); ++j)
    {
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
        "  latencies[%u][%u]=%Q\n",
        i, j, latencies[i][j].second));
    }
  }
}


/**
 * Processes a deployment from a string. The contents may use special
 * characters to ease deployment definition. Examples:
 * 
 * =====start of file======
 * 10               // number of processes in the deployment
 * 0 -> [0,9]       // 0 sends to 0-9
 * 0 -> [1,4)       // process 0 is sending to processes 1 through 3
 * 0 -> 1           // process 0 is sending to process 1
 * 0 -> [size / 4, size / 2]   // process 0 is sending to processes 2 through 5
 * [1,3] -> [source-1, source+1] // 1 -> [0, 2], 2 -> [1, 3], 3 -> [2, 4]
 * =====end of file======
 * 
 * @param       contents     string containing the deployment info.
 * @return      false if there was a problem with the contents
 **/
bool
madara::cid::process_deployment (Settings & settings,
                                 const std::string & orig)
{
  // a knowledge base for translation of integer logics
  Madara::KnowledgeEngine::KnowledgeBase knowledge;
  typedef std::map <unsigned int, unsigned int>    End;
  typedef std::map <unsigned int, End>             DeploymentMap;

  // copy the original and strip the comments
  std::string contents (orig), current;
  DeploymentMap map;
  std::stringstream input;
  Madara::Utility::strip_comments (contents);

  Workflow & deployment = settings.target_deployment;
  Deployment & solution = settings.solution;

  std::vector <std::string> link_splitters;
  link_splitters.resize (1);
  link_splitters[0] = "->";

  std::vector <std::string> range_splitters;
  range_splitters.resize (1);
  range_splitters[0] = ",";

  // user is specifying a size
  unsigned int size = 0;

  // keep track of the line number
  unsigned int line = 0;

  // clear the deployment. We use deployment.size () for determining success.
  deployment.clear ();

  // copy contents into a string stream
  input << contents;

#ifdef ENABLE_CID_LOGGING
      MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
        DLINFO "madara::cid::process_deployment:" \
          " processing a new deployment.\n"));
#endif

  if (std::getline (input, current))
  {
    std::stringstream buffer;
    std::string temp_string;
    std::vector <std::string> tokens, pivots;

    Madara::Utility::tokenizer (current, link_splitters, tokens, pivots);

    if (tokens.size () == 1)
    {
      knowledge.evaluate (tokens[0]);

      size = (unsigned int)knowledge.get ("size").to_integer ();
    }

    if (size == 0)
    {
      size = solution.size ();
      // user is using solution.size 
      deployment.resize (size);
      input.str (contents);
      knowledge.set ("size", Madara::KnowledgeRecord::Integer (size));
    }
    else
    {
      // clear the old deployment, resize it, and the solution
      deployment.resize (size);
      solution.resize (size);
    }

#ifdef ENABLE_CID_LOGGING
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        DLINFO "madara::cid::process_deployment:" \
          " using size=%u.\n", size));
#endif

  }

  while (std::getline (input, current))
  {
#ifdef ENABLE_CID_LOGGING
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        DLINFO "madara::cid::process_deployment:" \
          " evaluating %s.\n", current.c_str ()));
#endif

    std::vector <std::string> tokens, pivots;

    // strip all white space
    Madara::Utility::strip_white_space (current);

    // split the line by our defined splitters
    Madara::Utility::tokenizer (current, link_splitters, tokens, pivots);

    // current[0] == source and current[1] == dest or current[0] == evaluate
    if (tokens.size () == 2)
    {
      std::vector <std::string> source_tokens, dest_tokens,
        source_pivots, dest_pivots;
      unsigned int source_begin = 0;
      unsigned int source_end = 0;
      unsigned int source_inc = 1;

      std::string dest_begin ("0");
      std::string dest_end ("0");
      std::string dest_inc ("1");

      bool source_left_inclusive = true;
      bool source_right_inclusive = true;
      bool dest_left_inclusive = true;
      bool dest_right_inclusive = true;

      // if we split with [](), developers will lose ability to use parens.
      Madara::Utility::tokenizer (tokens[0], range_splitters,
        source_tokens, source_pivots);
      Madara::Utility::tokenizer (tokens[1], range_splitters,
        dest_tokens, dest_pivots);

      if (source_tokens.size () >= 1)
      {
        // if we have a right parenthesis, we're not right inclusive
        if (source_tokens.size () >= 2)
        {
          std::vector<std::string>::size_type last_el =
            source_tokens.size () - 1;

          if (source_tokens[last_el].size ())
          {
            std::string::size_type last_char = source_tokens[1].size () - 1;
            // resize the ending parenthesis or brace out
            if (source_tokens[last_el][last_char] == ')')
            {
              // parenthesis is not right inclusive
              source_right_inclusive = false;
              source_tokens[last_el].resize (last_char);
            }
            else if (source_tokens[last_el][last_char] == ']')
              source_tokens[last_el].resize (last_char);
          }
        }

        // if we have a left parenthesis, we're not left inclusive
        if (source_tokens[0].size ())
        {
          if (source_tokens[0][0] == '(')
          {
            source_left_inclusive = false;
            source_tokens[0].erase (0, 1);
          }
          else if (source_tokens[0][0] == '[')
          {
            source_tokens[0].erase (0, 1);
          }

          // if this is the only token, then we need to resize
          if (source_tokens.size () == 1)
          {
            if (source_tokens[0].size ())
            {
              // if the last char is a paren or brace, resize by 1
              std::string::size_type last = source_tokens[0].size () - 1;
              if (source_tokens[0][last] == ')' || 
                source_tokens[0][last] == ']')
                source_tokens[0].resize (last);

              // set source begin and end
              source_begin = 
                (unsigned int)knowledge.evaluate (source_tokens[0]).to_integer ();
              source_end = source_begin;
            }
          }

          else
          {
            // assign begin to token[0], end to token[1] and inc to token[2]
            source_begin =
              (unsigned int) knowledge.evaluate (source_tokens[0]).to_integer ();
            source_end =
              (unsigned int) knowledge.evaluate (source_tokens[1]).to_integer ();

            if (source_tokens.size () >= 3)
            {
              Madara::KnowledgeRecord::Integer value = knowledge.evaluate (source_tokens[2]).to_integer ();
              if (value != 0)
                source_inc = (unsigned int)value;
            }
          }

          if (!source_left_inclusive)
            ++source_begin;

          if (!source_right_inclusive)
            --source_end;

          if (source_end > solution.size () - 1)
            source_end = solution.size () - 1;


          // Start dest parsing

          if (dest_tokens.size () >= 1)
          {
            // if we have a right parenthesis, we're not right inclusive
            if (dest_tokens.size () >= 2)
            {
              std::vector<std::string>::size_type last_el =
                dest_tokens.size () - 1;

              if (dest_tokens[last_el].size ())
              {
                std::string::size_type last_char = dest_tokens[1].size () - 1;
                // resize the ending parenthesis or brace out
                if (dest_tokens[last_el][last_char] == ')')
                {
                  // parenthesis is not right inclusive
                  dest_right_inclusive = false;
                  dest_tokens[last_el].resize (last_char);
                }
                else if (dest_tokens[last_el][last_char] == ']')
                  dest_tokens[last_el].resize (last_char);
              }
            }

            // if we have a left parenthesis, we're not left inclusive
            if (dest_tokens[0].size ())
            {
              if (dest_tokens[0][0] == '(')
              {
                dest_left_inclusive = false;
                dest_tokens[0].erase (0, 1);
              }
              else if (dest_tokens[0][0] == '[')
              {
                dest_tokens[0].erase (0, 1);
              }

              // if this is the only token, then we need to resize
              if (dest_tokens.size () == 1)
              {
                if (dest_tokens[0].size ())
                {
                  // if the last char is a paren or brace, resize by 1
                  std::string::size_type last = dest_tokens[0].size () - 1;
                  if (dest_tokens[0][last] == ')' || 
                    dest_tokens[0][last] == ']')
                    dest_tokens[0].resize (last);

                  // set source begin and end
                  dest_begin = dest_tokens[0];
                  dest_end = dest_begin;
                }
              }

              else
              {
                dest_begin = dest_tokens[0];
                dest_end = dest_tokens[1];

                if (dest_tokens.size () >= 3)
                {
                  dest_inc = dest_tokens[2];
                }
              }
            }
          }
#ifdef ENABLE_CID_LOGGING
          MADARA_DEBUG (MADARA_LOG_EVENT_TRACE, (LM_DEBUG, 
            DLINFO "madara::cid::process_deployment:" \
            " %u,%u,%u -> %s, %s, %s:\n",
            source_begin, source_end, source_inc,
            dest_begin.c_str (), dest_end.c_str (), dest_inc.c_str ()));
#endif

          for (; source_begin <= source_end; source_begin += source_inc)
          {
            knowledge.set ("source",
              Madara::KnowledgeRecord::Integer (source_begin));
            
            unsigned int begin =
              (unsigned int) knowledge.evaluate (dest_begin).to_integer ();

            unsigned int end =
              (unsigned int) knowledge.evaluate (dest_end).to_integer ();

            unsigned int inc =
              (unsigned int) knowledge.evaluate (dest_inc).to_integer ();

            if (!dest_left_inclusive)
              ++begin;

            if (!dest_right_inclusive)
              --end;

            if (end > solution.size () - 1)
              end = solution.size () - 1;

            for (; begin <= end; begin += inc)
            {
              map[source_begin][begin];
              //map[begin][source_begin];
            }
          }
        } // end dest range construction

        // the source tokens are now prepped for 
      }
    }
    else if (tokens.size () == 1)
    {
#ifdef ENABLE_CID_LOGGING
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        DLINFO "madara::cid::process_deployment:" \
        " tokens==1, Evaluating %s:\n", tokens[0].c_str ()));
#endif

      // allow evaluations of 1 token lines but skip pure white space
      if (tokens[0].size () > 0)
        knowledge.evaluate (tokens[0]);
    }
  }

#ifdef ENABLE_CID_LOGGING
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        DLINFO "madara::cid::process_deployment:" \
        " Deployment map contains:\n"));
#endif

  for (DeploymentMap::iterator i = map.begin (); i != map.end (); ++i)
  {
    deployment[i->first].resize (i->second.size ());

    unsigned int index = 0;

    for (End::iterator j = i->second.begin (); 
      j != i->second.end (); ++j, ++index)
    {
      madara::cid::DirectedEdge & edge = deployment[i->first][index];

#ifdef ENABLE_CID_LOGGING
      MADARA_DEBUG (MADARA_LOG_DETAILED_TRACE, (LM_DEBUG, 
        DLINFO "madara::cid::process_deployment:" \
        " %u -> %u:\n", i->first, j->first));
#endif

      edge.first = i->first;
      edge.second = j->first;
    }
  }

  init (size, settings);

  // sort the deployments by degree
  prepare_deployment (settings);

  return deployment.size () > 0;
}

/**
* Reads the deployment from a file. @see process_deployment for
* deployment file configuration information.
* @param       filename     the name of the file to read
* @return      true if the file existed and was read
**/
bool
madara::cid::read_deployment (Settings & settings,
      const std::string & filename)
{
  std::string input (Madara::Utility::file_to_string (filename));
  return process_deployment (settings, input);
}

void
madara::cid::reset_solution (Settings & settings)
{
  for (unsigned int i = 0; i < settings.solution.size (); ++i)
  {
    settings.solution [i] = i;
  }
}

void
madara::cid::read_solution (Settings & settings, const std::string & source)
{
  unsigned int id;
  //std::string host_port;

  std::vector <std::string> tokens;
  std::vector <std::string> splitters;
  std::vector <std::string> pivot_list;

  tokens.resize (2);
  tokens[0] = ";";
  tokens[1] = "=";

  Madara::Utility::tokenizer (source, splitters, tokens, pivot_list);

  for (unsigned int i = 0; i + 1 < tokens.size (); i += 2)
  {
    std::stringstream buffer (tokens[i]);
    buffer >> id;

    settings.ids[id] = tokens[i + 1];

  }

  //while (!stream.eof ())
  //{
  //  stream >> id >> symbol >> host_port >> symbol;

  //  if (id < settings.ids.size ())
  //    settings.ids[id] = host_port;
  //}
}

std::string
madara::cid::stringify_solution (Settings & settings)
{
  std::stringstream buffer;
  Deployment & solution = settings.solution;

  for (unsigned i = 0; i < solution.size (); ++i)
  {
    buffer << i << "=" << settings.ids[solution[i]] << ";";
  }

  return buffer.str ();
}