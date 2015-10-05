#include "madara/utility/Utility.h"
#include <iostream>

int main (int,char **)
{
  std::cout << "MADARA libraries are at version " <<
    madara::utility::get_version () << ".\n";

  return 0;
}