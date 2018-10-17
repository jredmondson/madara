
/**
 * @file functions.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple application that showcases
 * the usage of functions as first class support. We demonstrate
 * the inclusion of both external C++ functions and MADARA logics.
 **/

#include "madara/knowledge/KnowledgeBase.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>

/**
 * Madara function that returns a random number. No argument means to generate
 * a random number up to the limits of the stdlib.h implementation's limits. An
 * integer argument means to generate a random up to the specified arg limit.
 **/
madara::knowledge::KnowledgeRecord
  rand_int (madara::knowledge::FunctionArguments & args,
             madara::knowledge::Variables &)
{
  // if the args list is greater than zero, is an integer, and is not 0
  if(args.size () > 0 && 
    args[0].type () == madara::knowledge::KnowledgeRecord::INTEGER &&
    args[0].to_integer () != 0)
  {
    // then call rand () with a modulus up to that integer argument
    return madara::knowledge::KnowledgeRecord (
      rand () % (int)args[0].to_integer ());
  }

  // otherwise, just return rand ()
  else
    return madara::knowledge::KnowledgeRecord (rand ());
}
  
/**
 * Madara function that returns a random fraction between 0 and 1
 **/
madara::knowledge::KnowledgeRecord
  rand_double (madara::knowledge::FunctionArguments &,
             madara::knowledge::Variables & variables)
{
  // empty arguments list to pass to rand_int (the function above)
  madara::knowledge::FunctionArguments rand_args;

  // the denominator is set to RAND_MAX
  madara::knowledge::KnowledgeRecord::Integer denominator (RAND_MAX);

  // the numerator is set to the results of rand ()
  madara::knowledge::KnowledgeRecord numerator (
    rand_int (rand_args, variables));

  return madara::knowledge::KnowledgeRecord (numerator.to_double () /
    denominator);
}


/**
 * Madara function that determines if an argument is true
 **/
madara::knowledge::KnowledgeRecord
  is_true (madara::knowledge::FunctionArguments & args,
             madara::knowledge::Variables &)
{
  madara::knowledge::KnowledgeRecord result;

  // if we've been provided with an argument, check if it is true
  if(args.size () == 1)
    return madara::knowledge::KnowledgeRecord (args[0].is_true ());

  return result;
}

/**
 * Madara function that determines if an argument is false
 **/
madara::knowledge::KnowledgeRecord
  is_false (madara::knowledge::FunctionArguments & args,
             madara::knowledge::Variables &)
{
  // if we've been provided with an argument, check if it is true
  if(args.size () == 1)
    return madara::knowledge::KnowledgeRecord (
      args[0].to_double () < 0.5 && args[0].to_double () >= 0);
  // if the user didn't provide an argument, return false
  else
    return madara::knowledge::KnowledgeRecord (0);
}



int main (int, char **)
{
  // Create a knowledge base
  madara::knowledge::KnowledgeBase knowledge;
  
  /**
   * In the tutorial at system_primitives/fractions, we created the following
   * functions that we used to determine state in a quantum computer.
   * Unfortunately, these functions only work with one variable (.state) and
   * are essentially useless as general purpose functions without reworking.
   *
   * From system_primitives/fractions.cpp:
   *
   * knowledge.define_function ("is_true",
   *                            ".state >= 0.5 && .state <= 1"); 
   *
   * knowledge.define_function ("is_false",
   *                            ".state > 0 && .state < 0.5");
   *
   * As a first part of this function tutorial, let's create a MADARA function
   * that accomplishes the same thing but is extensible for any argument passed
   * to the function. The secret sauce here is the usage of the .0 variable,
   * which maps to the first argument to a function call and is set for us
   * by the MADARA KaRL engine.
   **/


  // Define the two functions is_true and is_false to be MADARA logic functions
  knowledge.define_function ("is_true",  ".0 >= 0.5 && .0 <= 1"); 
  knowledge.define_function ("is_false", ".0 >= 0 && .0 < 0.5");

  /**
   * Define a random integer generating function based on C's rand ().
   * Note that to do proper random generation, you will want to use srand
   * with a seed that makes sense (most people use the system clock)
   **/
  knowledge.define_function ("rand", rand_int);
  knowledge.define_function ("rand_double", rand_double);

  // set the random seed to be the number of ns since January 1st, 1970
  srand ((unsigned int)madara::utility::get_time ());

  // do a couple of rands to get the seed away from standard seconds
  rand ();
  rand ();

  /**
   * Let's generate three random doubles with rand_double () and then check
   * whether or not they are true or false;
   **/
  knowledge.evaluate (
    ".var1 = rand_double ();"
    ".var2 = rand_double ();"
    ".var3 = rand_double ();"
    "is_true (.var1) => #print ('The generated var1 ({.var1}) is true\n');"
    "is_false (.var1) => #print ('The generated var1 ({.var1}) is false\n');"
    "is_true (.var2) => #print ('The generated var1 ({.var2}) is true\n');"
    "is_false (.var2) => #print ('The generated var1 ({.var2}) is false\n');"
    "is_true (.var3) => #print ('The generated var1 ({.var3}) is true\n');"
    "is_false (.var3) => #print ('The generated var1 ({.var3}) is false\n');"
  );

  /**
   * Print all the variables just to see what we did. This is very useful for
   * debugging purposes as you are developing a MADARA application.
   **/
  knowledge.print ();

  return 0;
}
