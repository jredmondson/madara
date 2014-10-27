
/**
 * @file system_calls.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple application that prints all
 * system calls supported in the MADARA version.
 **/

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <iostream>
#include <string>

int main (int argc, char * argv[])
{
  Madara::Knowledge_Engine::Knowledge_Base knowledge;
  
  knowledge.evaluate ("#print_system_calls ()");

  return 0;
}
