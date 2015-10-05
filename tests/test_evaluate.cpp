
/**
 * @file test_evaluate.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file reads a KaRL expression from the command line,
 * evaluates the expression and prints out the resulting variables.
 **/

#include "madara/knowledge/Knowledge_Base.h"
#include <iostream>
#include <string>

int main (int argc, char * argv[])
{
  madara::knowledge::Knowledge_Base knowledge;
  
  if (argc != 2)
  {
    knowledge.print ("To use this program, you must provide 1 argument.\n");
    knowledge.print ("  test_evaluate 'some expression to evaluate'\n");

    return -1;
  }
  
  
#ifndef _MADARA_NO_KARL_
  knowledge.evaluate (argv[1]);
  knowledge.print ();
  
#else
  std::cout << "This test is disabled due to karl feature being disabled.\n";
#endif // _MADARA_NO_KARL_
  return 0;
}
