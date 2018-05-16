
/**
 * @file system_calls.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple application that prints all
 * system calls supported in the MADARA version.
 **/

#include "madara/knowledge/KnowledgeBase.h"
#include <iostream>
#include <string>

int main (int, char **)
{
  madara::knowledge::KnowledgeBase knowledge;
  
  knowledge.evaluate ("#print_system_calls ()");

  return 0;
}
