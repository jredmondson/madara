
/**
 * @file controlling_types.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple application that demonstrates
 * controlling conversions of types in MADARA.
 **/

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/logger/GlobalLogger.h"
#include <iostream>
#include <string>
#include <iomanip>

/**
 * Define helpful shortened namespaces that we can refer to later
 **/
namespace knowledge = madara::knowledge;
namespace logger = madara::logger;

int main (int argc, char * argv[])
{
  /**
   * Create a knowledge base with no transport. All changes made to the
   * knowledge base stay within the knowledge base.
   **/
  knowledge::KnowledgeBase knowledge;

  /**
   * One aspect of control in MADARA is logging. MADARA can print out very
   * detailed information about what is going on in the evaluation engine
   * as well as in the transport layer. To see more information, change
   * the log level. There are multiple ways to change the log level, and
   * these are shown below. However, the most important thing to understand
   * is that the log levels range from 0 to 10, with 10 being the most
   * information.
   **/

  // set logging level to maximum level (6)
  logger::global_logger->set_level (logger::LOG_MADARA_MAX);

  // set logging level to 5
  knowledge.evaluate ("#log_level (5)");

  // set logging level to 1 (ERRORS, EMERGENCY and ALWAYS)
  logger::global_logger->set_level (logger::LOG_ERROR);
  
  /**
   * Create a double in the knowledge base. Doubles can be created in a number
   * of ways, and we demonstrate some of them right now. Each of the following
   * operations sets ".double_value" to a double containing 5.0
   **/
  knowledge.print ("*************CREATING DOUBLES SECTION******\n\n");
  knowledge.evaluate (".double_value = 5.1");
  knowledge.print ("[ASSIGNMENT RESULTS] .double_value = {.double_value}\n");

  knowledge.set (".double_value", 5.1);
  knowledge.print ("[SET RESULTS] .double_value = {.double_value}\n");
  
  knowledge.evaluate (".double_value = #double ('5.1')");
  knowledge.print (
    "[SYSTEM CALL #DOUBLE RESULTS] .double_value = {.double_value}\n");
  
  knowledge.evaluate (".double_value = #to_double ('5.1')");
  knowledge.print (
    "[SYSTEM CALL #TO_DOUBLE RESULTS] .double_value = {.double_value}\n");

  /**
   * Precision in MADARA is the similar to precision in C++. However, in
   * MADARA, precision is always fixed. You can set precision with the 
   * set_precision method in madara::knowledge::KnowledgeRecord or inside of an evaluate
   * statement with #set_precision or #precision (alias). Here are examples
   * of how to use precision.
   **/
  
  knowledge.print ("\n\n*************SETTING PRECISION SECTION******\n\n");
  knowledge.evaluate (".double_value = 2.0/3");
  knowledge.print ("[DEFAULT PRECISION] .double_value = {.double_value}\n");
  
  madara::knowledge::KnowledgeRecord::set_precision (2);
  knowledge.print ("[PRECISION = 2] .double_value = {.double_value}\n");
  
  knowledge.evaluate ("#set_precision (5)");
  knowledge.print ("[PRECISION = 5] .double_value = {.double_value}\n");
  
  knowledge.evaluate ("#precision (10) // alias for #set_precision");
  knowledge.print ("[PRECISION = 10] .double_value = {.double_value}\n");
  
  madara::knowledge::KnowledgeRecord::set_precision (-1);
  knowledge.print (
    "[DEFAULT PRECISION (-1)] .double_value = {.double_value}\n");

  /**
   * Converting to other types can be done through the KnowledgeRecord class
   * (calling evaluate, wait, or get should return one of these objects) or
   * internally inside of an evaluate call with system calls specific to the
   * results you want. Here is a list of system calls that cast values to
   * other types, followed by examples of their usage.
   *
   * #string or #to_string     convert to a string
   * #double or #to_double     convert to a double
   * #doubles or #to_doubles   convert to an array of doubles
   * #integer or #to_integer   convert to an integer
   * #integers or #to_integers convert to an array of integers
   * #buffer or #to_buffers    convert to an unsigned char array
   **/
  
  knowledge.print ("\n\n*************TYPE CASTING SECTION******\n\n");
  knowledge.evaluate (".double_value = 3.14159265359");
  knowledge.print (".double_value = {.double_value}\n");
  
  knowledge.evaluate (".int_value = #to_integer (.double_value)");
  knowledge.print (
    "[#to_integer (.double_value)] .int_value = {.int_value}\n");
  
  knowledge.evaluate (".string_value = #to_string (.double_value)");
  knowledge.print (
    "[#to_string (.double_value)] .string_value = {.string_value}\n");
  
  knowledge.evaluate (".double_array [1] = .double_value * .double_value");
  knowledge.evaluate (".double_array [0] = .double_value");
  knowledge.print (".double_array = {.double_array}\n");

  knowledge.evaluate (".integer_array = #to_integers (.double_array)");
  knowledge.print (
    "[#to_integers (.double_array)] .integer_array = {.integer_array}\n");
  
  madara::knowledge::KnowledgeRecord::set_precision (2);
  knowledge.evaluate (".double_array = #to_doubles (.integer_array)");
  knowledge.print (
    "[#to_doubles (.integer_array)] .double_array = {.double_array}\n");
  
  knowledge.evaluate (".double_array [1] = .double_value * .double_value");
  knowledge.evaluate (".double_array [0] = .double_value");
  knowledge.evaluate (
    ".double_value = .double_array[0]; .double_value2 = .double_array[1]");
  knowledge.print (
    ".double_value = {.double_value}, .double_value2 = {.double_value2}\n");

  return 0;
}
