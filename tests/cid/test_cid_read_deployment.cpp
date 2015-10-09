
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "madara/cid/CIDConvenience.h"
#include "madara/cid/CIDHeuristic.h"
#include "madara/utility/LogMacros.h"

void check_fanout_deployment (unsigned int fanout, Madara::Cid::Settings & settings)
{
  unsigned int size = settings.solution.size () / fanout;

  for (unsigned int i = 0; i < fanout; ++i)
  {
    if (!(settings.target_deployment[i].size () == size || 
           (i == 0 && fanout == 3)))
    {
      std::cerr << " read_deployment failed on " << fanout << " fanouts\n.";
      std::cerr << " deployment[" << i << "] was " << 
        settings.target_deployment[i].size () << " instead of " << 
        size << "\n";
      return;
    }
  }
}

int main (int argc, char *argv[])
{
  MADARA_debug_level = 10;
  // container for the deployment settings
  Madara::Cid::Settings settings;

  // create the filenames for the deployments to read
  std::string first_deployment (::getenv ("MADARA_ROOT"));
  std::string second_deployment (first_deployment);
  std::string third_deployment (first_deployment);
  std::string fourth_deployment (first_deployment);
  std::string fifth_deployment (first_deployment);
  std::string mway_deployment (first_deployment);
  std::string three_deep_deployment (first_deployment);
  std::string specialized_deployment (first_deployment);

  first_deployment += 
    "/configs/cid/deployments/10_procs_2_full_fans_disjoint.txt";

  second_deployment +=
    "/configs/cid/deployments/2_full_fans_disjoint.template";

  third_deployment +=
    "/configs/cid/deployments/size10_toall_forloop.txt";

  fourth_deployment +=
    "/configs/cid/deployments/test_cid/3waytree.template";

  fifth_deployment +=
    "/configs/cid/deployments/test_cid/3waytree_strict.template";

  mway_deployment +=
    "/configs/cid/deployments/test_cid/mwaytree.template";

  three_deep_deployment +=
    "/configs/cid/deployments/test_cid/3_deep_tree.template";

  specialized_deployment +=
    "/configs/cid/deployments/test_cid/specialized.template";

  // read a three deep tree
  if (!Madara::Cid::read_deployment (settings, specialized_deployment))
  {
    std::cerr << "ERROR: Deployment size is 0 for " 
      << specialized_deployment << "\n";
  }

  Madara::Cid::overlay_latencies (settings, 500, 501);
  Madara::Cid::prepare_latencies (settings);
  Madara::Cid::pathwise_approximate (settings);

  settings.solution.resize (20);

  // read a three deep tree
  if (!Madara::Cid::read_deployment (settings, three_deep_deployment))
  {
    std::cerr << "ERROR: Deployment size is 0 for " 
      << three_deep_deployment << "\n";
  }

  // read an mway tree
  if (!Madara::Cid::read_deployment (settings, mway_deployment))
  {
    std::cerr << "ERROR: Deployment size is 0 for " 
      << mway_deployment << "\n";
  }

  // read the strict 3-way tree (0-size)
  if (!Madara::Cid::read_deployment (settings, fifth_deployment))
  {
    std::cerr << "ERROR: Deployment size is 0 for " 
      << fifth_deployment << "\n";
  }

  // read the loose 3-way tree (1-size, 1 extra)
  if (!Madara::Cid::read_deployment (settings, fourth_deployment))
  {
    std::cerr << "ERROR: Deployment size is 0 for " 
      << fourth_deployment << "\n";
  }

  // read the two full fanout deployments
  if (!Madara::Cid::read_deployment (settings, first_deployment))
  {
    std::cerr << "ERROR: Deployment size is 0 for " 
      << first_deployment << "\n";
  }

  // the second deployment is a template that depends on solution.size ()
  settings.solution.resize (100);

  // read the second deployment
  if (!Madara::Cid::read_deployment (settings, second_deployment))
  {
    std::cerr << "ERROR: Deployment size is 0 for " 
      << second_deployment << "\n";
  }

  for (unsigned int fan_out = 1; fan_out <= 4; ++fan_out)
  {
    // create filename 
    std::stringstream filename;
    filename << getenv ("MADARA_ROOT");
    filename << "/configs/cid/deployments/test_cid/";
    filename << fan_out << "_even_fans_disjoint.template";

    // notify user of current est
    std::cout << "  Testing " << filename.str () << "\n";

    Madara::Cid::read_deployment (settings, filename.str ());

    ::check_fanout_deployment (fan_out, settings);
  }

  return 0;
}
