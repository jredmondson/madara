

#ifndef _MADARA_KNOWLEDGE_INTERPRETER_H_
#define _MADARA_KNOWLEDGE_INTERPRETER_H_

#ifndef _MADARA_NO_KARL_

/**
 * @file Interpreter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains classes and methods specific to the numerical
 * madara::expression::Interpreter class
 **/

#include <string>
#include <list>
#include <map>

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/expression/ExpressionTree.h"
#include "madara/knowledge/ThreadSafeContext.h"

namespace madara
{
namespace expression
{
// Forward declaration.
class Symbol;

typedef std::map<std::string, ExpressionTree> ExpressionTreeMap;

/**
 * @class Interpreter
 * @brief Parses incoming expression strings into a parse tree and
 *        generates an expression tree from the parse tree.
 *
 *        This class plays the role of the "interpreter" in the
 *        Intepreter pattern.  It also uses the Builder pattern to
 *        generate the nodes in the expression tree.
 */
class Interpreter
{
public:
  /**
   * Constructor
   **/
  Interpreter();

  /**
   * Destructor
   **/
  virtual ~Interpreter();

  /**
   * Compiles an expression into an expression tree.
   * @param    context    interpreter context
   * @param    input      expression to compile
   * @return   expression tree to evaluate
   * @throw exceptions::KarlException failure during interpret
   **/
  ExpressionTree interpret(
      madara::knowledge::ThreadSafeContext& context, const std::string& input);

  /**
   * Checks a character to see if it is a string literal
   * @param    input      char to check
   * @return   true if the input is an operator
   **/
  static inline bool is_string_literal(char input);

  /**
   * Checks a character to see if it is a mathematical operator
   * @param    input      char to check
   * @return   true if the input is an operator
   **/
  static inline bool is_operator(char input);

  /**
   * Checks a character to see if it is a number
   * @param    input      char to check
   * @return   true if the input is a number
   **/
  static inline bool is_number(char input);

  /**
   * Checks a character to see if it is alphanumeric
   * @param    input      char to check
   * @return   true if the input is alphanumeric
   **/
  static inline bool is_alphanumeric(char input);

  /**
   * Checks a character to see if it is whitespace
   * @param    input      char to check
   * @return   true if the input is whitespace
   **/
  static inline bool is_whitespace(char input);

  /**
   * Checks a character to see if it is in scientific format
   * @param    input      char to check
   * @return   true if the input is in scientific exponential format
   **/
  static inline bool is_exponential(char input);

  /**
   * Checks a given input for a reserved word literal
   * @param    input      string to check
   * @return   true if the input is an operator
   **/
  static inline bool is_reserved_word(const std::string& input);

  /**
   * Attempts to delete an expression from cache
   * @param    expression      expression to erase from cache
   * @return   true if the expression was deleted
   **/
  inline bool delete_expression(const std::string& expression);

private:
  /**
   * extracts precondition, condition, postcondition, and body from input
   * @param    context       interpreter context
   * @param    variable      name of the variable the for loop is based on
   * @param    input         expression to compile
   * @param    i             current position in expression
   * @param    accumulated_precedence  current precedence
   * @param    list          list of symbols to insert for loop into
   * @param    returnableInput  used to interact with lastValidInput if
   *                            an array reference is found
   **/
  void handle_for_loop(madara::knowledge::ThreadSafeContext& context,
      std::string& variable, const std::string& input,
      std::string::size_type& i, int& accumulated_precedence,
      ::std::list<Symbol*>& list, Symbol*& returnableInput);

  /**
   * Inserts a variable into the tree
   * @param    context    interpreter context
   * @param    input      expression to compile
   * @param    i          current position in expression
   * @param    accumulated_precedence  current precedence
   * @param    list       list of symbols in tree that are free
   * @param    lastValidInput          last valid symbol that was read
   **/
  void variable_insert(madara::knowledge::ThreadSafeContext& context,
      const std::string& input, std::string::size_type& i,
      int& accumulated_precedence, ::std::list<Symbol*>& list,
      Symbol*& lastValidInput);

  /**
   * Inserts a variable into the tree
   * @param    opener     the opening string
   * @param    context    interpreter context
   * @param    input      expression to compile
   * @param    i          current position in expression
   * @param    accumulated_precedence  current precedence
   * @param    list       list of symbols in tree that are free
   * @param    lastValidInput          last valid symbol that was read
   **/
  void string_insert(char opener, madara::knowledge::ThreadSafeContext& context,
      const std::string& input, std::string::size_type& i,
      int& accumulated_precedence, ::std::list<Symbol*>& list,
      Symbol*& lastValidInput);

  /**
   * Inserts a system call into the tree
   * @param    context    interpreter context
   * @param    input      expression to compile
   * @param    i          current position in expression
   * @param    accumulated_precedence  current precedence
   * @param    list       list of symbols in tree that are free
   * @param    lastValidInput          last valid symbol that was read
   **/
  void system_call_insert(madara::knowledge::ThreadSafeContext& context,
      const std::string& input, std::string::size_type& i,
      int& accumulated_precedence, ::std::list<Symbol*>& list,
      Symbol*& lastValidInput);

  /**
   * Inserts a number into the tree
   * @param    context    thread safe context handle
   * @param    input      expression to compile
   * @param    i          current position in expression
   * @param    accumulated_precedence  current precedence
   * @param    list       list of symbols in tree that are free
   * @param    lastValidInput          last valid symbol that was read
   **/
  void number_insert(madara::knowledge::ThreadSafeContext& context,
      const std::string& input, std::string::size_type& i,
      int& accumulated_precedence, ::std::list<Symbol*>& list,
      Symbol*& lastValidInput);

  /**
   * Inserts a mathematical operator into the tree
   * @param    context    thread safe context handle
   * @param    op         symbol to insert
   * @param    list       list of symbols in tree that are free
   **/
  void precedence_insert(madara::knowledge::ThreadSafeContext& context,
      Symbol* op, ::std::list<Symbol*>& list);

  /**
   * Inserts a variable into the tree
   * @param    context    interpreter context
   * @param    input      expression to compile
   * @param    i          current position in expression
   * @param    accumulated_precedence  current precedence
   * @param    list       list of symbols in tree that are free
   * @param    lastValidInput          last valid symbol that was read
   * @param    handled    whether or not the pos should be incremented
   * @param    build_argument_list  if true, focuses on building a
   *                                list instead of an expression tree
   **/
  void main_loop(madara::knowledge::ThreadSafeContext& context,
      const std::string& input, std::string::size_type& i,
      Symbol*& lastValidInput, bool& handled, int& accumulated_precedence,
      ::std::list<Symbol*>& list, bool build_argument_list = false);

  /**
   * Handles a parenthesis
   * @param    context    interpreter context
   * @param    input      expression to compile
   * @param    i          current position in expression
   * @param    lastValidInput          last valid symbol that was read
   * @param    list       list of symbols in tree that are free
   * @param    handled    whether or not the pos should be incremented
   * @param    accumulated_precedence  current precedence
   **/
  void handle_array(madara::knowledge::ThreadSafeContext& context,
      const std::string& input, std::string::size_type& i,
      Symbol*& lastValidInput, bool& handled, int& accumulated_precedence,
      ::std::list<Symbol*>& list);

  /**
   * Handles a parenthesis
   * @param    context    interpreter context
   * @param    input      expression to compile
   * @param    i          current position in expression
   * @param    accumulated_precedence  current precedence
   * @param    list       list of symbols in tree that are free
   * @param    lastValidInput          last valid symbol that was read
   * @param    handled    whether or not the pos should be incremented
   * @param    build_argument_list  if true, focuses on building a
   *                                list instead of an expression tree
   **/
  void handle_parenthesis(madara::knowledge::ThreadSafeContext& context,
      const std::string& input, std::string::size_type& i,
      Symbol*& lastValidInput, bool& handled, int& accumulated_precedence,
      ::std::list<Symbol*>& list, bool build_argument_list = false);
  /**
   * Cache of expressions that have been previously compiled
   **/
  ExpressionTreeMap cache_;
};
}
}

#include "Interpreter.inl"

#endif  // _MADARA_NO_KARL_

#endif /* _MADARA_KNOWLEDGE_INTERPRETER_H_ */
