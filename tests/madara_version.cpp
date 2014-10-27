#include "madara/utility/Utility.h"
#include <iostream>

int main (int,char **)
{
  std::cout << "MADARA libraries are at version " <<
    Madara::Utility::get_version () << ".\n";

  return 0;
}