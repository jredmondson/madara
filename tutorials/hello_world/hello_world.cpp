
/**
 * @file hello_world.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple hello application that
 * reads a name from the command line and prints out a
 * greeting to that person.
 **/

#include "madara/knowledge/KnowledgeBase.h"
#include <iostream>
#include <string>

int main (int, char **)
{
  madara::knowledge::KnowledgeBase knowledge;
  
  // Prompt the user for their name
  std::string my_name;
  knowledge.print ("Please enter your name: ");
  std::getline (std::cin, my_name);

  // Save the name to the ".name" variable and then print the message
  knowledge.set (".name", my_name);
  knowledge.print ("Hello {.name}!\n");

  return 0;
}
