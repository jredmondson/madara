

#ifndef _MADARA_KNOWLEDGE_INTERPRETER_INL_
#define _MADARA_KNOWLEDGE_INTERPRETER_INL_

#ifndef _MADARA_NO_KARL_

#include "Interpreter.h"

/**
 * @file Interpreter.inl
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains inlined methods of the Interpreter class
 **/


// method for checking if a character is a valid operator
inline bool
madara::expression::Interpreter::is_operator (char input)
{
  return input == '+'
    || input == '-'
    || input == '*'
    || input == '/' || input == '%';
}

// method for checking if a character is a number
inline bool
madara::expression::Interpreter::is_number (char input)
{
  return input >= '0' && input <= '9';
}

// method for checking if a character is a number
inline bool
madara::expression::Interpreter::is_exponential (char input)
{
  return input == 'e' || input == 'E' || input == '-' ||
        (input >= '0' && input <= '9');
}

inline bool
madara::expression::Interpreter::is_reserved_word (const std::string & input)
{
  return input == "nan" || input == "inf";  
}

// method for checking if a character is a number
inline bool
madara::expression::Interpreter::is_string_literal (char input)
{
  return input == '"' || input == '\'';
}

// method for checking if a character is a candidate
// for a part of a variable name
inline bool
madara::expression::Interpreter::is_alphanumeric (char input)
{
  return (input >= 'a' && input <= 'z')
    || (input >= 'A' && input <= 'Z')
    || (input == '_')
    || (input >= '0' && input <= '9') || input == '.'
    || input == '{' || input == '}';
}

// method for checking if input is whitespace
inline bool
madara::expression::Interpreter::is_whitespace (char input)
{
  return input == ' ' || input == '\t' || input == '\r' || input == '\n';
}

/**
* Attempts to delete an expression from cache
* @param    expression      expression to erase from cache
* @return   true if the expression was deleted
**/
inline bool
madara::expression::Interpreter::delete_expression (
  const std::string & expression)
{
  return cache_.erase (expression) == 1;
}


#endif // _MADARA_NO_KARL_

#endif // _MADARA_KNOWLEDGE_INTERPRETER_INL_