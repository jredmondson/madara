

#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#ifndef _MADARA_NO_KARL_

/**
 * @file Interpreter.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains classes and methods specific to the numerical
 * Madara::Expression_Tree::Interpreter class
 **/

#include <string>
#include <list>
#include <map>

#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/expression_tree/Expression_Tree.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"

namespace Madara
{
  namespace Expression_Tree
  {
    // Forward declaration.
    class Symbol;

    typedef std::map <std::string, 
      Madara::Expression_Tree::Expression_Tree> ExpressionTreeMap;

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
      Interpreter ();

      /**
       * Destructor
       **/
      virtual ~Interpreter ();

      /**
       * Compiles an expression into an expression tree.
       * @param    context    interpreter context
       * @param    input      expression to compile
       * @return   expression tree to evaluate
       **/
      Expression_Tree interpret (
        Madara::Knowledge_Engine::Thread_Safe_Context &context, 
                                 const std::string &input);
      
      /**
       * Checks a character to see if it is a string literal
       * @param    input      char to check
       * @return   true if the input is an operator
       **/
      static bool is_string_literal (char input);

      /**
       * Checks a character to see if it is a mathematical operator
       * @param    input      char to check
       * @return   true if the input is an operator
       **/
      static bool is_operator (char input);

      /**
       * Checks a character to see if it is a number
       * @param    input      char to check
       * @return   true if the input is a number
       **/
      static bool is_number (char input);
      
      /**
       * Checks a character to see if it is alphanumeric
       * @param    input      char to check
       * @return   true if the input is alphanumeric
       **/
      static bool is_alphanumeric (char input);
      
      /**
       * Checks a character to see if it is whitespace
       * @param    input      char to check
       * @return   true if the input is whitespace
       **/
      static bool is_whitespace (char input);
      
      /**
       * Attempts to delete an expression from cache
       * @param    expression      expression to erase from cache
       * @return   true if the expression was deleted
       **/
      inline bool delete_expression (const std::string & expression)
      {
        return cache_.erase (expression) == 1;
      }

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
      void handle_for_loop (
        Madara::Knowledge_Engine::Thread_Safe_Context &context,
                                         std::string &variable,
                                            const std::string &input,
                                           std::string::size_type &i,
                                          int & accumulated_precedence,
                                           ::std::list<Symbol *>& list,
                                          Symbol *& returnableInput);

      /**
       * Inserts a variable into the tree
       * @param    context    interpreter context
       * @param    input      expression to compile
       * @param    i          current position in expression
       * @param    accumulated_precedence  current precedence
       * @param    list       list of symbols in tree that are free
       * @param    lastValidInput          last valid symbol that was read
       **/
      void variable_insert (
        Madara::Knowledge_Engine::Thread_Safe_Context &context,
                            const std::string &input,
                            std::string::size_type &i,
                          int & accumulated_precedence,
                          ::std::list<Symbol *>& list,
                           Symbol *& lastValidInput);

       /**
       * Inserts a variable into the tree
       * @param    input      expression to compile
       * @param    i          current position in expression
       * @param    accumulated_precedence  current precedence
       * @param    list       list of symbols in tree that are free
       * @param    lastValidInput          last valid symbol that was read
       **/
      void string_insert (const std::string &input, 
                          std::string::size_type &i,
                          int & accumulated_precedence,
                          ::std::list<Symbol *>& list,
                           Symbol *& lastValidInput);
      
      /**
       * Inserts a system call into the tree
       * @param    context    interpreter context
       * @param    input      expression to compile
       * @param    i          current position in expression
       * @param    accumulated_precedence  current precedence
       * @param    list       list of symbols in tree that are free
       * @param    lastValidInput          last valid symbol that was read
       **/
      void system_call_insert (
        Madara::Knowledge_Engine::Thread_Safe_Context &context,
                            const std::string &input,
                            std::string::size_type &i,
                          int & accumulated_precedence,
                          ::std::list<Symbol *>& list,
                           Symbol *& lastValidInput);

      /**
       * Inserts a number into the tree
       * @param    input      expression to compile
       * @param    i          current position in expression
       * @param    accumulated_precedence  current precedence
       * @param    list       list of symbols in tree that are free
       * @param    lastValidInput          last valid symbol that was read
       **/
      void number_insert (const std::string &input, 
                          std::string::size_type &i,
                          int & accumulated_precedence,
                          ::std::list<Symbol *>& list,
                           Symbol *& lastValidInput);

      /**
       * Inserts a mathematical operator into the tree
       * @param    op         symbol to insert
       * @param    list       list of symbols in tree that are free
       **/
      void precedence_insert (Symbol *op, ::std::list<Symbol *>& list);

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
      void main_loop (Madara::Knowledge_Engine::Thread_Safe_Context & context,
                           const std::string &input,
                           std::string::size_type &i,
                           Symbol *& lastValidInput,
                           bool & handled,
                           int & accumulated_precedence,
                           ::std::list<Symbol *>& list,
                           bool build_argument_list = false);


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
      void handle_array (
        Madara::Knowledge_Engine::Thread_Safe_Context & context,
        const std::string &input,
        std::string::size_type &i,
        Symbol *& lastValidInput,
        bool & handled,
        int & accumulated_precedence,
        ::std::list<Symbol *>& list);

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
      void handle_parenthesis (
        Madara::Knowledge_Engine::Thread_Safe_Context & context,
                           const std::string &input,
                           std::string::size_type &i,
                           Symbol *& lastValidInput,
                           bool & handled,
                           int & accumulated_precedence,
                           ::std::list<Symbol *>& list,
                           bool build_argument_list = false);
      /**
       * Cache of expressions that have been previously compiled
       **/
      ExpressionTreeMap cache_;
    };
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _INTERPRETER_H_ */
