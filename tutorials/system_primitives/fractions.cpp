
/**
 * @file fractions.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple application that showcases
 * the usage of doubles as first class support.
 **/

#include "madara/knowledge/KnowledgeBase.h"
#include <iostream>
#include <string>

namespace knowledge = madara::knowledge;

int main (int, char **)
{
  // Create a knowledge base
  knowledge::KnowledgeBase knowledge;
  
  /**
   * There are three main ways to set doubles. We showcase
   * those three ways below.
   **/

  // 1, set the variable directly through the KnowledgeBase::Set method
  knowledge.set (".state", 0.1);

  /**
   * 2, set the variable to a double in an evaluate or wait call. Doubles
   * are differentiated from integers by the inclusion of a period.
   **/
  knowledge.evaluate (".state = 0.3");

  /**
   * 3, mutating an integer value via addition, subtraction, multiplication,
   * division or similar operations. Comparisons always return integer values
   * (typically 0 or 1), so do not rely on them to create doubles.
   **/
  knowledge.evaluate (".state = 10 + 0.3");

  /**
   * IMPORTANT NOTE: Doubles currently must start with an integer. I may
   * correct this functionality later, but currently, .3 is a variable whereas
   * 0.3 is a double.
   **/

  /**
   * For an example, let's consider a test application we want to develop for
   * a new quantum computer. Quatum computers are typified by fuzzy logic--a
   * programming paradigm that features 0 (false), 1 (true) and everything in
   * between representing a gray area that must be reasoned about state. For
   * this application, let's assume a 50/50 split on the false and true scales.
   *
   * Variable .state
   * [0----false----0.50----true----1]
   *
   * Or, false = state < 0.5 && state >= 0
   *     true  = state <= 1    && .state >= 0.5
   *
   * These relations are so useful that we should make it into functions that
   * we can use. In KaRL, functions are created via the define_function method
   * of the KnowledgeBase. 
   **/

  knowledge.define_function ("is_true",
    ".state >= 0.5 && .state <= 1"); 

  knowledge.define_function ("is_false",
    ".state >= 0 && .state < 0.5");

  /**
   * Defining the functions gives us the ability to call the logics from inside
   * other KaRL logics. In the following code block, we set the state to a value
   * and call the is_true () and is_false () functions to check whether or not
   * the value was true. Any function that starts with a # is a MADARA system
   * function and cannot be overridden by users. #print is a trusted function
   * that prints to stderr.
   **/

  knowledge.evaluate (
    ".state = 0.3;"
    "#print ('.state = {.state}: ');"
    "(is_true () => #print ('.state is true\n'))"
    "||"
    "(is_false () => #print ('.state is false\n'))"
    );

  /**
   * Let's prompt the user for a fractional value and use it to determine
   * whether it is true or false.
   **/
  std::string my_fraction;
  knowledge.print ("Please enter a fractional value (0-1): ");
  std::getline (std::cin, my_fraction);

  /**
   * Now, we have a string representation of the value. We could convert
   * this to a double using std::stringstream and then set it through the
   * KnowledgeBase::set method, but we'll show how to convert it within
   * the engine after you've set the variable as a string. In the future,
   * we will also provide a #to_double, #to_integer, and #to_string method
   **/
  knowledge.set (".state", my_fraction);
  
  // convert the .state to a double
  knowledge.evaluate (".state = 0.0 + .state");

  /**
   * Let's report the truthiness of the variable the user wanted to set
   **/
  knowledge.evaluate (
    "is_true () => #print ('The value you entered ({.state}) is true.\n');"
    "is_false () => #print ('The value you entered ({.state}) is false\n');"
  );

  /**
   * Print all the variables just to see what we did. This is very useful for
   * debugging purposes as you are developing a MADARA application.
   **/
  knowledge.print ();

  return 0;
}
