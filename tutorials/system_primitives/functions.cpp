
/**
 * @file functions.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple application that showcases
 * the usage of functions as first class support. We demonstrate
 * the inclusion of both external C++ functions and MADARA logics.
 **/

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>

/**
 * Madara function that returns a random number. No argument means to generate
 * a random number up to the limits of the stdlib.h implementation's limits. An
 * integer argument means to generate a random up to the specified arg limit.
 **/
Madara::Knowledge_Record
  rand_int (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  // if the args list is greater than zero, is an integer, and is not 0
  if (args.size () > 0 && 
    args[0].type () == Madara::Knowledge_Record::INTEGER &&
    args[0].to_integer () != 0)
  {
    // then call rand () with a modulus up to that integer argument
    return Madara::Knowledge_Record::Integer (
      rand () % (int)args[0].to_integer ());
  }

  // otherwise, just return rand ()
  else
    return Madara::Knowledge_Record::Integer (rand ());
}
  
/**
 * Madara function that returns a random fraction between 0 and 1
 **/
Madara::Knowledge_Record
  rand_double (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  // empty arguments list to pass to rand_int (the function above)
  Madara::Knowledge_Engine::Function_Arguments rand_args;

  // the denominator is set to RAND_MAX
  Madara::Knowledge_Record::Integer denominator (RAND_MAX);

  // the numerator is set to the results of rand ()
  Madara::Knowledge_Record numerator (
    rand_int (rand_args, variables));

  Madara::Knowledge_Record result = numerator.to_double () /
    denominator;

  return result;
}


/**
 * Madara function that determines if an argument is true
 **/
Madara::Knowledge_Record
  is_true (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  // if we've been provided with an argument, check if it is true
  if (args.size () == 1)
    return Madara::Knowledge_Record::Integer (
      args[0].to_double () >= 0.5 && args[0].to_double () <= 1);
  // if the user didn't provide an argument, return false
  else
    return Madara::Knowledge_Record::Integer (0);
}

/**
 * Madara function that determines if an argument is false
 **/
Madara::Knowledge_Record
  is_false (Madara::Knowledge_Engine::Function_Arguments & args,
             Madara::Knowledge_Engine::Variables & variables)
{
  // if we've been provided with an argument, check if it is true
  if (args.size () == 1)
    return Madara::Knowledge_Record::Integer (
      args[0].to_double () < 0.5 && args[0].to_double () >= 0);
  // if the user didn't provide an argument, return false
  else
    return Madara::Knowledge_Record::Integer (0);
}



int main (int argc, char * argv[])
{
  // Create a knowledge base
  Madara::Knowledge_Engine::Knowledge_Base knowledge;
  
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

  // set the random seed to be the number of seconds since January 1st, 1970
  srand ((unsigned int)time (NULL));

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
