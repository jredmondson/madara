#ifndef _INTERPRETER_CPP_
#define _INTERPRETER_CPP_

#ifndef _MADARA_NO_KARL_

#include <math.h>
#include <iostream>
#include <sstream>
#include <memory>

#include "madara/expression/ComponentNode.h"
#include "madara/expression/LeafNode.h"
#include "madara/expression/VariableNode.h"
#include "madara/expression/VariableCompareNode.h"
#include "madara/expression/VariableDecrementNode.h"
#include "madara/expression/VariableDivideNode.h"
#include "madara/expression/VariableIncrementNode.h"
#include "madara/expression/VariableMultiplyNode.h"
#include "madara/expression/ListNode.h"
#include "madara/expression/CompositeConstArray.h"
#include "madara/expression/CompositeNegateNode.h"
#include "madara/expression/CompositePostdecrementNode.h"
#include "madara/expression/CompositePostincrementNode.h"
#include "madara/expression/CompositePredecrementNode.h"
#include "madara/expression/CompositePreincrementNode.h"
#include "madara/expression/CompositeNotNode.h"
#include "madara/expression/CompositeAddNode.h"
#include "madara/expression/CompositeAndNode.h"
#include "madara/expression/CompositeOrNode.h"
#include "madara/expression/CompositeArrayReference.h"
#include "madara/expression/CompositeAssignmentNode.h"
#include "madara/expression/CompositeEqualityNode.h"
#include "madara/expression/CompositeInequalityNode.h"
#include "madara/expression/CompositeGreaterThanEqualNode.h"
#include "madara/expression/CompositeGreaterThanNode.h"
#include "madara/expression/CompositeLessThanEqualNode.h"
#include "madara/expression/CompositeLessThanNode.h"
#include "madara/expression/CompositeSubtractNode.h"
#include "madara/expression/CompositeDivideNode.h"
#include "madara/expression/CompositeMultiplyNode.h"
#include "madara/expression/CompositeModulusNode.h"
#include "madara/expression/CompositeBothNode.h"
#include "madara/expression/CompositeReturnRightNode.h"
#include "madara/expression/CompositeFunctionNode.h"
#include "madara/expression/CompositeForLoop.h"
#include "madara/expression/CompositeSequentialNode.h"
#include "madara/expression/CompositeSquareRootNode.h"
#include "madara/expression/CompositeImpliesNode.h"
#include "madara/expression/SystemCallClearVariable.h"
#include "madara/expression/SystemCallCos.h"
#include "madara/expression/SystemCallDeleteVariable.h"
#include "madara/expression/SystemCallEval.h"
#include "madara/expression/SystemCallExpandEnv.h"
#include "madara/expression/SystemCallExpandStatement.h"
#include "madara/expression/SystemCallFragment.h"
#include "madara/expression/SystemCallGetClock.h"
#include "madara/expression/SystemCallGetTime.h"
#include "madara/expression/SystemCallGetTimeSeconds.h"
#include "madara/expression/SystemCallIsinf.h"
#include "madara/expression/SystemCallLogLevel.h"
#include "madara/expression/SystemCallPow.h"
#include "madara/expression/SystemCallPrint.h"
#include "madara/expression/SystemCallPrintSystemCalls.h"
#include "madara/expression/SystemCallRandDouble.h"
#include "madara/expression/SystemCallRandInt.h"
#include "madara/expression/SystemCallReadFile.h"
#include "madara/expression/SystemCallSetClock.h"
#include "madara/expression/SystemCallSetFixed.h"
#include "madara/expression/SystemCallSetPrecision.h"
#include "madara/expression/SystemCallSetScientific.h"
#include "madara/expression/SystemCallSin.h"
#include "madara/expression/SystemCallSize.h"
#include "madara/expression/SystemCallSleep.h"
#include "madara/expression/SystemCallSqrt.h"
#include "madara/expression/SystemCallTan.h"
#include "madara/expression/SystemCallToBuffer.h"
#include "madara/expression/SystemCallToDouble.h"
#include "madara/expression/SystemCallToDoubles.h"
#include "madara/expression/SystemCallToHostDirs.h"
#include "madara/expression/SystemCallToInteger.h"
#include "madara/expression/SystemCallToIntegers.h"
#include "madara/expression/SystemCallToString.h"
#include "madara/expression/SystemCallType.h"
#include "madara/expression/SystemCallWriteFile.h"
#include "madara/expression/Interpreter.h"


namespace madara
{
  namespace expression
  {
    enum
    {
      BOTH_PRECEDENCE = 1,
      IMPLIES_PRECEDENCE = 2,
      ASSIGNMENT_PRECEDENCE = 3,
      LOGICAL_PRECEDENCE = 4,
      CONDITIONAL_PRECEDENCE = 5,
      ADD_PRECEDENCE = 6,
      SUBTRACT_PRECEDENCE = 6,
      MULTIPLY_PRECEDENCE = 7,
      MODULUS_PRECEDENCE = 7,
      DIVIDE_PRECEDENCE = 7,
      NEGATE_PRECEDENCE = 8,
      NUMBER_PRECEDENCE = 9,
      VARIABLE_PRECEDENCE = 9,
      FUNCTION_PRECEDENCE = 9,
      FOR_LOOP_PRECEDENCE = 9,
      PARENTHESIS_PRECEDENCE = 10
    };

    /**
    * @class Symbol
    * @brief Abstract base class of all parse tree nodes.
    */
    class Symbol
    {
    public:
      /// constructor
      Symbol (logger::Logger & logger,
        Symbol * left, Symbol * right, int precedence_ = 0);

      /// destructor
      virtual ~Symbol (void);

      /// abstract method for returning precedence level (higher
      /// value means higher precedence
      virtual int precedence (void)
      {
        return precedence_;
      }

      virtual int add_precedence (int accumulated_precedence) = 0;

      /// abstract method for building an Expression ExpressionTree Node

      virtual ComponentNode * build (void) = 0;

      /// left and right pointers

      logger::Logger * logger_;
      Symbol * left_;
      Symbol * right_;
      int precedence_;
    };

    typedef  std::vector <Symbol *>   Symbols;

    /**
    * @class Operator
    * @brief Abstract base class for all parse tree node operators
    * @see   Add, Subtract, Multiply, Divide
    */
    class Operator : public Symbol
    {
    public:
      /// constructor
      Operator (logger::Logger & logger,
        Symbol * left, Symbol * right, int precedence_ = 1);

      /// destructor
      virtual ~Operator (void);
    };

    /**
    * @class TernaryOperator
    * @brief Abstract base class for operators with 3+ potential subnodes
    * @see   Both
    */
    class TernaryOperator : public Operator
    {
    public:
      /// constructor
      TernaryOperator (logger::Logger & logger,
        Symbol * left, Symbol * right, int precedence_ = 1);

      /// destructor
      virtual ~TernaryOperator (void);

      ComponentNodes nodes_;
    };

    /**
    * @class SystemCall
    * @brief Abstract base class for operators with 3+ potential subnodes
    * @see   Both
    */
    class SystemCall : public TernaryOperator
    {
    public:
      /// constructor
      SystemCall (madara::knowledge::ThreadSafeContext & context_);

      /// destructor
      virtual ~SystemCall (void);

      /// Context for variables
      madara::knowledge::ThreadSafeContext & context_;
    };

    /**
    * @class ClearVariable
    * @brief Clears a variable in the knowledge base
    */
    class ClearVariable : public SystemCall
    {
    public:
      /// constructor
      ClearVariable (
        madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~ClearVariable (void);
    };


    /**
    * @class DeleteVariable
    * @brief Deletes a variable from the knowledge base
    */
    class DeleteVariable : public SystemCall
    {
    public:
      /// constructor
      DeleteVariable (
        madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~DeleteVariable (void);
    };

    /**
    * @class Eval
    * @brief Evaluates a Knowledge Record and returns result
    */
    class Eval : public SystemCall
    {
    public:
      /// constructor
      Eval (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~Eval (void);
    };

    /**
    * @class ExpandEnv
    * @brief Expands a statement, e.g. var${.i} into var0 if .i == 0
    */
    class ExpandEnv : public SystemCall
    {
    public:
      /// constructor
      ExpandEnv (
        madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~ExpandEnv (void);
    };

    /**
    * @class ExpandStatement
    * @brief Expands a statement, e.g. var{.i} into var0 if .i == 0
    */
    class ExpandStatement : public SystemCall
    {
    public:
      /// constructor
      ExpandStatement (
        madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~ExpandStatement (void);
    };

    /**
    * @class Fragment
    * @brief Fragment the Knowledge Record
    */
    class Fragment : public SystemCall
    {
    public:
      /// constructor
      Fragment (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~Fragment (void);
    };

    /**
    * @class Print
    * @brief Prints a Knowledge Record to the stderr
    */
    class Print : public SystemCall
    {
    public:
      /// constructor
      Print (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~Print (void);
    };

    /**
    * @class Cos
    * @brief Returns the cosine of a term (radians)
    */
    class Cos : public SystemCall
    {
    public:
      /// constructor
      Cos (
        madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~Cos (void);
    };

    /**
    * @class Sin
    * @brief Returns the sin of a term (radians)
    */
    class Sin : public SystemCall
    {
    public:
      /// constructor
      Sin (
        madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~Sin (void);
    };

    /**
    * @class Tan
    * @brief Returns the tangent of a term (radians)
    */
    class Tan : public SystemCall
    {
    public:
      /// constructor
      Tan (
        madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~Tan (void);
    };

    /**
    * @class Power
    * @brief Returns a base term taken to a power (exponent)
    */
    class Power : public SystemCall
    {
    public:
      /// constructor
      Power (
        madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~Power (void);
    };

    /**
    * @class SquareRoot
    * @brief Returns the square root of a term
    */
    class SquareRoot : public SystemCall
    {
    public:
      /// constructor
      SquareRoot (
        madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~SquareRoot (void);
    };

    /**
    * @class PrintSystemCalls
    * @brief Prints a help menu for all system calls
    */
    class PrintSystemCalls : public SystemCall
    {
    public:
      /// constructor
      PrintSystemCalls (
        madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~PrintSystemCalls (void);
    };

    /**
    * @class RandDouble
    * @brief Generates a random double
    */
    class RandDouble : public SystemCall
    {
    public:
      /// constructor
      RandDouble (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~RandDouble (void);
    };

    /**
    * @class RandInt
    * @brief Generates a random integer
    */
    class RandInt : public SystemCall
    {
    public:
      /// constructor
      RandInt (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~RandInt (void);
    };

    /**
    * @class ReadFile
    * @brief Reads a file
    */
    class ReadFile : public SystemCall
    {
    public:
      /// constructor
      ReadFile (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~ReadFile (void);
    };

    /**
    * @class WriteFile
    * @brief Writes a file
    */
    class WriteFile : public SystemCall
    {
    public:
      /// constructor
      WriteFile (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~WriteFile (void);
    };

    /**
    * @class Size
    * @brief Returns the size of a record
    */
    class Size : public SystemCall
    {
    public:
      /// constructor
      Size (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~Size (void);
    };

    /**
    * @class SetFixed
    * @brief Sets the output format to std::fixed
    */
    class SetFixed : public SystemCall
    {
    public:
      /// constructor
      SetFixed (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~SetFixed (void);
    };

    /**
    * @class SetPrecision
    * @brief Sets the precision of doubles
    */
    class SetPrecision : public SystemCall
    {
    public:
      /// constructor
      SetPrecision (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~SetPrecision (void);
    };

    /**
    * @class SetScientific
    * @brief Sets the output to std::scientific
    */
    class SetScientific : public SystemCall
    {
    public:
      /// constructor
      SetScientific (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~SetScientific (void);
    };

    /**
    * @class ToBuffer
    * @brief Returns a buffer
    */
    class ToBuffer : public SystemCall
    {
    public:
      /// constructor
      ToBuffer (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~ToBuffer (void);
    };

    /**
    * @class ToDouble
    * @brief Returns a double
    */
    class ToDouble : public SystemCall
    {
    public:
      /// constructor
      ToDouble (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~ToDouble (void);
    };

    /**
    * @class ToDoubles
    * @brief Returns a double array
    */
    class ToDoubles : public SystemCall
    {
    public:
      /// constructor
      ToDoubles (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~ToDoubles (void);
    };

    /**
    * @class ToHostDirs
    * @brief Returns a version that has a directory structure appropriate to
    *        the OS.
    */
    class ToHostDirs : public SystemCall
    {
    public:
      /// constructor
      ToHostDirs (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~ToHostDirs (void);
    };

    /**
    * @class ToInteger
    * @brief Returns an integer
    */
    class ToInteger : public SystemCall
    {
    public:
      /// constructor
      ToInteger (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~ToInteger (void);
    };

    /**
    * @class ToIntegers
    * @brief Returns an integers
    */
    class ToIntegers : public SystemCall
    {
    public:
      /// constructor
      ToIntegers (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~ToIntegers (void);
    };

    /**
    * @class ToString
    * @brief Returns a string
    */
    class ToString : public SystemCall
    {
    public:
      /// constructor
      ToString (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~ToString (void);
    };

    /**
    * @class Sleep
    * @brief Sleeps for a certain amount of time
    */
    class Sleep : public SystemCall
    {
    public:
      /// constructor
      Sleep (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~Sleep (void);
    };

    /**
    * @class Type
    * @brief Returns the type of a record
    */
    class Type : public SystemCall
    {
    public:
      /// constructor
      Type (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~Type (void);
    };

    /**
    * @class Isinf
    * @brief Returns whether the first argument is an infinite number
    */
    class Isinf : public SystemCall
    {
    public:
      /// constructor
      Isinf (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~Isinf (void);
    };

    /**
    * @class LogLevel
    * @brief Reads or sets the MADARA log level
    */
    class LogLevel : public SystemCall
    {
    public:
      /// constructor
      LogLevel (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~LogLevel (void);
    };

    /**
    * @class GetClock
    * @brief Returns the clock of the argument or the system clock
    */
    class GetClock : public SystemCall
    {
    public:
      /// constructor
      GetClock (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~GetClock (void);
    };

    /**
    * @class GetTime
    * @brief Returns the wall clock time
    */
    class GetTime : public SystemCall
    {
    public:
      /// constructor
      GetTime (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~GetTime (void);
    };

    /**
    * @class GetTimeSeconds
    * @brief Returns the wall clock time in seconds
    */
    class GetTimeSeconds : public SystemCall
    {
    public:
      /// constructor
      GetTimeSeconds (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~GetTimeSeconds (void);
    };

    /**
    * @class SetClock
    * @brief Sets the system or a variable clock
    */
    class SetClock : public SystemCall
    {
    public:
      /// constructor
      SetClock (madara::knowledge::ThreadSafeContext & context_);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      /// destructor
      virtual ~SetClock (void);
    };


    /**
    * @class UnaryOperator
    * @brief Abstract base class for all parse tree node operators
    * @see   Add, Subtract, Multiply, Divide
    */
    class UnaryOperator : public Symbol
    {
    public:
      /// constructor
      UnaryOperator (logger::Logger & logger,
        Symbol * right, int precedence_ = 1);

      /// destructor
      virtual ~UnaryOperator (void);
    };

    /**
    * @class Number
    * @brief Leaf node of parse tree
    */

    class Number : public Symbol
    {
    public:
      /// constructors
      Number (logger::Logger & logger, std::string input);
      Number (logger::Logger & logger, madara::knowledge::KnowledgeRecord::Integer input);
      Number (logger::Logger & logger, double input);

      /// destructor
      virtual ~Number (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode *build (void);
      //   private:
      /// contains the value of the leaf node
      madara::knowledge::KnowledgeRecord item_;
    };

    /**
    * @class Variable
    * @brief Leaf node of parse tree
    */
    class Variable : public Symbol
    {
    public:
      /// constructors
      Variable (const std::string & key,
        madara::knowledge::ThreadSafeContext & context);

      /// destructor
      virtual ~Variable (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
      //private:
      /// Key for retrieving value of this variable.
      const std::string key_;

      /// Context for variables
      madara::knowledge::ThreadSafeContext & context_;
    };

    /**
    * @class ArrayRef
    * @brief Leaf node for an array reference
    */
    class ArrayRef : public Symbol
    {
    public:
      /// constructors
      ArrayRef (const std::string & key, Symbol * index,
        madara::knowledge::ThreadSafeContext & context);

      /// destructor
      virtual ~ArrayRef (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
      //private:
      /// Key for retrieving value of this variable.
      const std::string key_;

      /// Context for variables
      madara::knowledge::ThreadSafeContext & context_;
      Symbol * index_;
    };

    /**
    * @class VariableDecrement
    * @brief Decrement a variable by a certain amount
    */

    class VariableDecrement : public Operator
    {
    public:
      /// constructors
      VariableDecrement (Symbol * lhs,
        madara::knowledge::KnowledgeRecord value,
        Symbol * rhs,
        madara::knowledge::ThreadSafeContext & context);

      /// destructor
      virtual ~VariableDecrement (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode *build (void);
      //private:
      Symbol * left_;

      /// value can be faster than rhs_, so use it if possible
      madara::knowledge::KnowledgeRecord value_;

      /// Context for variables
      madara::knowledge::ThreadSafeContext & context_;
    };

    /**
    * @class VariableDivide
    * @brief Divide a variable by a certain amount
    */

    class VariableDivide : public Operator
    {
    public:
      /// constructors
      VariableDivide (Symbol * lhs,
        madara::knowledge::KnowledgeRecord value,
        Symbol * rhs,
        madara::knowledge::ThreadSafeContext & context);

      /// destructor
      virtual ~VariableDivide (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode *build (void);
      //private:
      Symbol * left_;

      /// value can be faster than rhs_, so use it if possible
      madara::knowledge::KnowledgeRecord value_;

      /// Context for variables
      madara::knowledge::ThreadSafeContext & context_;
    };

    /**
    * @class VariableIncrement
    * @brief Increment a variable by a certain amount
    */

    class VariableIncrement : public Operator
    {
    public:
      /// constructors
      VariableIncrement (Symbol * lhs,
        madara::knowledge::KnowledgeRecord value,
        Symbol * rhs,
        madara::knowledge::ThreadSafeContext & context);

      /// destructor
      virtual ~VariableIncrement (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode *build (void);
      //private:
      Symbol * left_;

      /// value can be faster than rhs_, so use it if possible
      madara::knowledge::KnowledgeRecord value_;

      /// Context for variables
      madara::knowledge::ThreadSafeContext & context_;
    };

    /**
    * @class VariableMultiply
    * @brief Multiply a variable by a certain amount
    */

    class VariableMultiply : public Operator
    {
    public:
      /// constructors
      VariableMultiply (Symbol * lhs,
        madara::knowledge::KnowledgeRecord value,
        Symbol * rhs,
        madara::knowledge::ThreadSafeContext & context);

      /// destructor
      virtual ~VariableMultiply (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
      //private:
      Symbol * left_;

      /// value can be faster than rhs_, so use it if possible
      madara::knowledge::KnowledgeRecord value_;

      /// Context for variables
      madara::knowledge::ThreadSafeContext & context_;
    };

    /**
    * @class VariableCompare
    * @brief Increment a variable by a certain amount
    */

    class VariableCompare : public Symbol
    {
    public:
      /// constructors
      VariableCompare (Symbol * lhs,
        madara::knowledge::KnowledgeRecord value,
        Symbol * rhs, int compare_type,
        madara::knowledge::ThreadSafeContext & context);

      /// destructor
      virtual ~VariableCompare (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode *build (void);
      //private:
      Symbol * left_;

      /// value can be faster than rhs_, so use it if possible
      madara::knowledge::KnowledgeRecord value_;

      /// rhs is used for complex rhs types (not a simple number)
      Symbol * rhs_;

      /// type of comparison. See madara/Globals.h
      int compare_type_;

      /// Context for variables
      madara::knowledge::ThreadSafeContext & context_;
    };


    /**
    * @class List
    * @brief Parameter List
    */

    class List : public Symbol
    {
    public:
      /// constructors
      List (
        madara::knowledge::ThreadSafeContext & context);

      /// destructor
      virtual ~List (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    private:

      /// Context for variables
      madara::knowledge::ThreadSafeContext & context_;
    };

    /**
    * @class Subtract
    * @brief Subtraction node of the parse tree
    */

    class Subtract : public Operator
    {
    public:
      /// constructor
      Subtract (logger::Logger & logger);

      /// destructor
      virtual ~Subtract (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Add
    * @brief Addition node of the parse tree
    */

    class Add : public TernaryOperator
    {
    public:
      /// constructor
      Add (logger::Logger & logger);

      /// destructor
      virtual ~Add (void);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class And
    * @brief Logically and node of the parse tree
    */

    class And : public TernaryOperator
    {
    public:
      /// constructor
      And (logger::Logger & logger);

      /// destructor
      virtual ~And (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Or
    * @brief Logically or node of the parse tree
    */

    class Or : public TernaryOperator
    {
    public:
      /// constructor
      Or (logger::Logger & logger);

      /// destructor
      virtual ~Or (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Both
    * @brief Evaluates both left and right children, regardless of values
    */

    class Both : public TernaryOperator
    {
    public:
      /// constructor
      Both (logger::Logger & logger);

      /// destructor
      virtual ~Both (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class ReturnRight
    * @brief Evaluates both left and right children and returns right value
    */

    class ReturnRight : public TernaryOperator
    {
    public:
      /// constructor
      ReturnRight (logger::Logger & logger);

      /// destructor
      virtual ~ReturnRight (void);

      /// returns the precedence level
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Sequence
    * @brief Evaluates both left and right children, regardless of values
    */

    class Sequence : public TernaryOperator
    {
    public:
      /// constructor
      Sequence (logger::Logger & logger);

      /// destructor
      virtual ~Sequence (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Implies
    * @brief Assign the value of an expression to a variable
    */

    class Implies : public Operator
    {
    public:
      /// constructor
      Implies (logger::Logger & logger);

      /// destructor
      virtual ~Implies (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Assignment
    * @brief Assign the value of an expression to a variable
    */

    class Assignment : public Operator
    {
    public:
      /// constructor
      Assignment (logger::Logger & logger);

      /// destructor
      virtual ~Assignment (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Equality
    * @brief Check and left and right arguments for equality
    */

    class Equality : public Operator
    {
    public:
      /// constructor
      Equality (logger::Logger & logger);

      /// destructor
      virtual ~Equality (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Inequality
    * @brief Check and left and right arguments for inequality
    */

    class Inequality : public Operator
    {
    public:
      /// constructor
      Inequality (logger::Logger & logger);

      /// destructor
      virtual ~Inequality (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class GreaterThanEqual
    * @brief Check and left and right arguments for greater than or equal to
    */

    class GreaterThanEqual : public Operator
    {
    public:
      /// constructor
      GreaterThanEqual (logger::Logger & logger);

      /// destructor
      virtual ~GreaterThanEqual (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class GreaterThan
    * @brief Check and left and right arguments for greater than
    */

    class GreaterThan : public Operator
    {
    public:
      /// constructor
      GreaterThan (logger::Logger & logger);

      /// destructor
      virtual ~GreaterThan (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class LessThanEqual
    * @brief Check and left and right arguments for less than or equal to
    */

    class LessThanEqual : public Operator
    {
    public:
      /// constructor
      LessThanEqual (logger::Logger & logger);

      /// destructor
      virtual ~LessThanEqual (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class LessThan
    * @brief Check and left and right arguments for less than
    */

    class LessThan : public Operator
    {
    public:
      /// constructor
      LessThan (logger::Logger & logger);

      /// destructor
      virtual ~LessThan (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Function
    * @brief Function node of the parse tree
    */

    class Function : public TernaryOperator
    {
    public:
      /// constructor
      Function (const std::string & name,
        madara::knowledge::ThreadSafeContext & context);

      /// destructor
      virtual ~Function (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);

      std::string name_;
      madara::knowledge::ThreadSafeContext & context_;
    };

    /**
    * @class ConstArray
    * @brief A constant array that should not be changed
    */

    class ConstArray : public TernaryOperator
    {
    public:
      /// constructor
      ConstArray (
        madara::knowledge::ThreadSafeContext & context);

      /// destructor
      virtual ~ConstArray (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode *build (void);

      madara::knowledge::ThreadSafeContext & context_;
    };

    /**
    * @class ForLoop
    * @brief Iterative looping node of the parse tree
    */

    class ForLoop : public UnaryOperator
    {
    public:
      /// constructor
      ForLoop (Symbol * precondition, Symbol * condition,
        Symbol * postcondition, Symbol * body,
        madara::knowledge::ThreadSafeContext & context);

      /// destructor
      virtual ~ForLoop (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode *build (void);

      Symbol * precondition_;
      Symbol * condition_;
      Symbol * postcondition_;
      Symbol * body_;
      madara::knowledge::ThreadSafeContext & context_;
    };

    /**
    * @class Negate
    * @brief Negate node of the parse tree
    */

    class SquareRootUnary : public UnaryOperator
    {
    public:
      /// constructor
      SquareRootUnary (logger::Logger & logger);

      /// destructor
      virtual ~SquareRootUnary (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Negate
    * @brief Negate node of the parse tree
    */

    class Negate : public UnaryOperator
    {
    public:
      /// constructor
      Negate (logger::Logger & logger);

      /// destructor
      virtual ~Negate (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Postdecrement
    * @brief Postdecrement node of the parse tree
    */

    class Postdecrement : public UnaryOperator
    {
    public:
      /// constructor
      Postdecrement (logger::Logger & logger);

      /// destructor
      virtual ~Postdecrement (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Postincrement
    * @brief Postincrement node of the parse tree
    */

    class Postincrement : public UnaryOperator
    {
    public:
      /// constructor
      Postincrement (logger::Logger & logger);

      /// destructor
      virtual ~Postincrement (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Predecrement
    * @brief Predecrement node of the parse tree
    */

    class Predecrement : public UnaryOperator
    {
    public:
      /// constructor
      Predecrement (logger::Logger & logger);

      /// destructor
      virtual ~Predecrement (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Preincrement
    * @brief Preincrement node of the parse tree
    */

    class Preincrement : public UnaryOperator
    {
    public:
      /// constructor
      Preincrement (logger::Logger & logger);

      /// destructor
      virtual ~Preincrement (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Not
    * @brief Logically not the right node
    */

    class Not : public UnaryOperator
    {
    public:
      /// constructor
      Not (logger::Logger & logger);

      /// destructor
      virtual ~Not (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Multiply
    * @brief Multiplication node of the parse tree
    */

    class Multiply : public TernaryOperator
    {
    public:
      /// constructor
      Multiply (logger::Logger & logger);

      /// destructor
      virtual ~Multiply (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Modulus
    * @brief Modulus node of the parse tree (10 % 4 == 2)
    */

    class Modulus : public Operator
    {
    public:
      /// constructor
      Modulus (logger::Logger & logger);

      /// destructorm
      virtual ~Modulus (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

    /**
    * @class Divide
    * @brief Division node of the parse tree
    */

    class Divide : public Operator
    {
    public:
      /// constructor
      Divide (logger::Logger & logger);

      /// destructor
      virtual ~Divide (void);

      /// returns the precedence level
      //virtual int precedence (void);
      virtual int add_precedence (int accumulated_precedence);

      /// builds an equivalent ExpressionTree node
      virtual ComponentNode * build (void);
    };

  }
}

// constructor
madara::expression::Symbol::Symbol (
  logger::Logger & logger,
  madara::expression::Symbol * left,
  madara::expression::Symbol * right, int precedence)
  : logger_ (&logger), left_ (left), right_ (right), precedence_ (precedence)
{
}

// destructor
madara::expression::Symbol::~Symbol (void)
{
  delete left_;
  delete right_;
}

// constructor
madara::expression::Operator::Operator (logger::Logger & logger,
  Symbol * left, Symbol * right, int precedence)
  : Symbol (logger, left, right, precedence)
{
}

// destructor
madara::expression::Operator::~Operator (void)
{
}

// constructor
madara::expression::TernaryOperator::TernaryOperator (logger::Logger & logger,
  Symbol * left, Symbol * right, int precedence)
  : Operator (logger, left, right, precedence)
{
}

// destructor
madara::expression::TernaryOperator::~TernaryOperator (void)
{
}

// constructor
madara::expression::UnaryOperator::UnaryOperator (
  logger::Logger & logger,
  madara::expression::Symbol * right, int precedence)
  : madara::expression::Symbol (logger, 0, right, precedence)
{
}

// destructor
madara::expression::UnaryOperator::~UnaryOperator (void)
{
}

// constructor
madara::expression::Number::Number (logger::Logger & logger,
  madara::knowledge::KnowledgeRecord::Integer input)
  : Symbol (logger, 0, 0, NUMBER_PRECEDENCE)
{
  item_.set_value (input);
}

// constructor
madara::expression::Number::Number (logger::Logger & logger,
  double input)
  : Symbol (logger, 0, 0, NUMBER_PRECEDENCE)
{
  item_.set_value (input);
}

// constructor
madara::expression::Number::Number (logger::Logger & logger,
  std::string input)
  : Symbol (logger, 0, 0, NUMBER_PRECEDENCE)
{
  item_.set_value (input);
}

// destructor
madara::expression::Number::~Number (void)
{
}

// returns the precedence level
int
madara::expression::Number::add_precedence (int precedence)
{
  return this->precedence_ = NUMBER_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Number::build (void)
{
  if (left_ || right_)
  {
    std::string message = 
      "Number::build: KARL COMPILE ERROR: "
      "Number ";
    message += item_.to_string ();
    message += " has a left or right child. Likely missing a semi-colon\n";

    madara_logger_ptr_log (logger_, logger::LOG_ERROR, message.c_str ());

    throw KarlException (message);
  }

  return new LeafNode (*(this->logger_), item_);
}

// constructor
madara::expression::Negate::Negate (logger::Logger & logger)
  : UnaryOperator (logger, 0, NEGATE_PRECEDENCE)
{
}

// destructor
madara::expression::Negate::~Negate (void)
{
}

// returns the precedence level
int
madara::expression::Negate::add_precedence (int precedence)
{
  return this->precedence_ = NEGATE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Negate::build ()
{
  // check for cascading nots
  Negate * next = dynamic_cast <Negate *> (right_);
  Symbol * right = right_;
  unsigned int i;

  if (left_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "Negate::build: KARL COMPILE ERROR: "
      "Negate(-) has a left child. Likely missing a semi-colon\n");

    throw KarlException ("Negate::build: KARL COMPILE ERROR: "
      "Negate(-) has a left child. Likely missing a semi-colon\n");
  }
  
  if (right_)
  {
    for (i = 1; next;
      ++i, right = next->right_, next = dynamic_cast <Negate *> (next->right_)) {}

    if (i % 2 == 1)
      return new CompositeNegateNode (*(this->logger_), right->build ());
    else
      return new CompositeNegateNode (*(this->logger_),
      new CompositeNegateNode (*(this->logger_), right->build ()));
  }
  else
  {
    return new CompositeNegateNode (*(this->logger_), 0);
  }
}

// constructor
madara::expression::Function::Function (
  const std::string & name,
  madara::knowledge::ThreadSafeContext & context)
  : TernaryOperator (context.get_logger (), 0, 0, VARIABLE_PRECEDENCE), name_ (name), context_ (context)
{
}

// destructor
madara::expression::Function::~Function (void)
{
}

// returns the precedence level
int
madara::expression::Function::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Function::build ()
{
  return new CompositeFunctionNode (name_, context_, nodes_);
}



// constructor
madara::expression::ConstArray::ConstArray (
  madara::knowledge::ThreadSafeContext & context)
: TernaryOperator (context.get_logger (), 0, 0, VARIABLE_PRECEDENCE),
    context_ (context)
{
}

// destructor
madara::expression::ConstArray::~ConstArray (void)
{
}

// returns the precedence level
int
madara::expression::ConstArray::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ConstArray::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "array[]::build: KARL COMPILE ERROR: "
      "array[] has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "array[]::build: KARL COMPILE ERROR: "
      "array[] has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new CompositeConstArray (*(this->logger_), nodes_);
}





// constructor
madara::expression::SystemCall::SystemCall (
  madara::knowledge::ThreadSafeContext & context)
: TernaryOperator (context.get_logger (), 0, 0, VARIABLE_PRECEDENCE),
  context_ (context)
{
}

// destructor
madara::expression::SystemCall::~SystemCall (void)
{
}


// constructor
madara::expression::ClearVariable::ClearVariable (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::ClearVariable::~ClearVariable (void)
{
}

// returns the precedence level
int
madara::expression::ClearVariable::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ClearVariable::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#clear_var::build: KARL COMPILE ERROR: "
      "#clear_var has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#clear_var::build: KARL COMPILE ERROR: "
      "#clear_var has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallClearVariable (context_, nodes_);
}


// constructor
madara::expression::DeleteVariable::DeleteVariable (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::DeleteVariable::~DeleteVariable (void)
{
}

// returns the precedence level
int
madara::expression::DeleteVariable::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::DeleteVariable::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#delete_var::build: KARL COMPILE ERROR: "
      "#delete_var has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#delete_var::build: KARL COMPILE ERROR: "
      "#delete_var has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallDeleteVariable (context_, nodes_);
}



// constructor
madara::expression::Eval::Eval (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::Eval::~Eval (void)
{
}

// returns the precedence level
int
madara::expression::Eval::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Eval::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#evaluate::build: KARL COMPILE ERROR: "
      "#evaluate has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#evaluate::build: KARL COMPILE ERROR: "
      "#evaluate has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallEval (context_, nodes_);
}



// constructor
madara::expression::ExpandEnv::ExpandEnv (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::ExpandEnv::~ExpandEnv (void)
{
}

// returns the precedence level
int
madara::expression::ExpandEnv::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ExpandEnv::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#expand_env::build: KARL COMPILE ERROR: "
      "#expand_env has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#expand_env::build: KARL COMPILE ERROR: "
      "#expand_env has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallExpandEnv (context_, nodes_);
}



// constructor
madara::expression::ExpandStatement::ExpandStatement (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::ExpandStatement::~ExpandStatement (void)
{
}

// returns the precedence level
int
madara::expression::ExpandStatement::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ExpandStatement::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#expand::build: KARL COMPILE ERROR: "
      "#expand has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#expand::build: KARL COMPILE ERROR: "
      "#expand has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallExpandStatement (context_, nodes_);
}




// constructor
madara::expression::Fragment::Fragment (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::Fragment::~Fragment (void)
{
}

// returns the precedence level
int
madara::expression::Fragment::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Fragment::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#fragment::build: KARL COMPILE ERROR: "
      "#fragment has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#fragment::build: KARL COMPILE ERROR: "
      "#fragment has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallFragment (context_, nodes_);
}



// constructor
madara::expression::LogLevel::LogLevel (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::LogLevel::~LogLevel (void)
{
}

// returns the precedence level
int
madara::expression::LogLevel::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::LogLevel::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#log_level::build: KARL COMPILE ERROR: "
      "#log_level has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#log_level::build: KARL COMPILE ERROR: "
      "#log_level has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallLogLevel (context_, nodes_);
}




// constructor
madara::expression::GetClock::GetClock (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::GetClock::~GetClock (void)
{
}

// returns the precedence level
int
madara::expression::GetClock::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::GetClock::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#get_clock::build: KARL COMPILE ERROR: "
      "#get_clock has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#get_clock::build: KARL COMPILE ERROR: "
      "#get_clock has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallGetClock (context_, nodes_);
}


// constructor
madara::expression::GetTime::GetTime (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::GetTime::~GetTime (void)
{
}

// returns the precedence level
int
madara::expression::GetTime::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::GetTime::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#get_time::build: KARL COMPILE ERROR: "
      "#get_time has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#get_time::build: KARL COMPILE ERROR: "
      "#get_time has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallGetTime (context_, nodes_);
}


// constructor
madara::expression::GetTimeSeconds::GetTimeSeconds (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::GetTimeSeconds::~GetTimeSeconds (void)
{
}

// returns the precedence level
int
madara::expression::GetTimeSeconds::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::GetTimeSeconds::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#get_time_s::build: KARL COMPILE ERROR: "
      "#get_time_s has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#get_time_s::build: KARL COMPILE ERROR: "
      "#get_time_s has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallGetTimeSeconds (context_, nodes_);
}


// constructor
madara::expression::SetClock::SetClock (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::SetClock::~SetClock (void)
{
}

// returns the precedence level
int
madara::expression::SetClock::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::SetClock::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#set_clock::build: KARL COMPILE ERROR: "
      "#set_clock has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#set_clock::build: KARL COMPILE ERROR: "
      "#set_clock has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallSetClock (context_, nodes_);
}

// constructor
madara::expression::SetFixed::SetFixed (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::SetFixed::~SetFixed (void)
{
}

// returns the precedence level
int
madara::expression::SetFixed::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::SetFixed::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#set_fixed::build: KARL COMPILE ERROR: "
      "#set_fixed has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#set_fixed::build: KARL COMPILE ERROR: "
      "#set_fixed has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallSetFixed (context_, nodes_);
}


// constructor
madara::expression::SetPrecision::SetPrecision (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::SetPrecision::~SetPrecision (void)
{
}

// returns the precedence level
int
madara::expression::SetPrecision::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::SetPrecision::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#set_precision::build: KARL COMPILE ERROR: "
      "#set_precision has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#set_precision::build: KARL COMPILE ERROR: "
      "#set_precision has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallSetPrecision (context_, nodes_);
}


// constructor
madara::expression::SetScientific::SetScientific (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::SetScientific::~SetScientific (void)
{
}

// returns the precedence level
int
madara::expression::SetScientific::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::SetScientific::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#scientific::build: KARL COMPILE ERROR: "
      "#scientific has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#scientific::build: KARL COMPILE ERROR: "
      "#scientific has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallSetScientific (context_, nodes_);
}



// constructor
madara::expression::Print::Print (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::Print::~Print (void)
{
}

// returns the precedence level
int
madara::expression::Print::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Print::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#print::build: KARL COMPILE ERROR: "
      "#print has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#print::build: KARL COMPILE ERROR: "
      "#print has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallPrint (context_, nodes_);
}

// constructor
madara::expression::Cos::Cos (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::Cos::~Cos (void)
{
}

// returns the precedence level
int
madara::expression::Cos::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Cos::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#cos::build: KARL COMPILE ERROR: "
      "#cos has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#cos::build: KARL COMPILE ERROR: "
      "#cos has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallCos (context_, nodes_);
}


// constructor
madara::expression::Sin::Sin (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::Sin::~Sin (void)
{
}

// returns the precedence level
int
madara::expression::Sin::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Sin::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#sin::build: KARL COMPILE ERROR: "
      "#sin has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#sin::build: KARL COMPILE ERROR: "
      "#sin has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallSin (context_, nodes_);
}

// constructor
madara::expression::Tan::Tan (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::Tan::~Tan (void)
{
}

// returns the precedence level
int
madara::expression::Tan::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Tan::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#tan::build: KARL COMPILE ERROR: "
      "#tan has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#tan::build: KARL COMPILE ERROR: "
      "#tan has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallTan (context_, nodes_);
}

// constructor
madara::expression::Power::Power (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::Power::~Power (void)
{
}

// returns the precedence level
int
madara::expression::Power::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Power::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#pow::build: KARL COMPILE ERROR: "
      "#pow has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#pow::build: KARL COMPILE ERROR: "
      "#pow has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallPow (context_, nodes_);
}

// constructor
madara::expression::SquareRoot::SquareRoot (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::SquareRoot::~SquareRoot (void)
{
}

// returns the precedence level
int
madara::expression::SquareRoot::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::SquareRoot::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#sqrt::build: KARL COMPILE ERROR: "
      "#sqrt has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#sqrt::build: KARL COMPILE ERROR: "
      "#sqrt has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallSqrt (context_, nodes_);
}



// constructor
madara::expression::PrintSystemCalls::PrintSystemCalls (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::PrintSystemCalls::~PrintSystemCalls (void)
{
}

// returns the precedence level
int
madara::expression::PrintSystemCalls::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::PrintSystemCalls::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#print_system_calls::build: KARL COMPILE ERROR: "
      "#print_system_calls has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#print_system_calls::build: KARL COMPILE ERROR: "
      "#print_system_calls has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallPrintSystemCalls (context_, nodes_);
}


// constructor
madara::expression::RandDouble::RandDouble (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::RandDouble::~RandDouble (void)
{
}

// returns the precedence level
int
madara::expression::RandDouble::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::RandDouble::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#rand_double::build: KARL COMPILE ERROR: "
      "#rand_double has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#rand_double::build: KARL COMPILE ERROR: "
      "#rand_double has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallRandDouble (context_, nodes_);
}


// constructor
madara::expression::RandInt::RandInt (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::RandInt::~RandInt (void)
{
}

// returns the precedence level
int
madara::expression::RandInt::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::RandInt::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#rand_int::build: KARL COMPILE ERROR: "
      "#rand_int has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#rand_int::build: KARL COMPILE ERROR: "
      "#rand_int has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallRandInt (context_, nodes_);
}


// constructor
madara::expression::ReadFile::ReadFile (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::ReadFile::~ReadFile (void)
{
}

// returns the precedence level
int
madara::expression::ReadFile::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ReadFile::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#read_file::build: KARL COMPILE ERROR: "
      "#read_file has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#read_file::build: KARL COMPILE ERROR: "
      "#read_file has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallReadFile (context_, nodes_);
}


// constructor
madara::expression::WriteFile::WriteFile (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::WriteFile::~WriteFile (void)
{
}

// returns the precedence level
int
madara::expression::WriteFile::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::WriteFile::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#write_file::build: KARL COMPILE ERROR: "
      "#write_file has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#write_file::build: KARL COMPILE ERROR: "
      "#write_file has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallWriteFile (context_, nodes_);
}



// constructor
madara::expression::Size::Size (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::Size::~Size (void)
{
}

// returns the precedence level
int
madara::expression::Size::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Size::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#size::build: KARL COMPILE ERROR: "
      "#size has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#size::build: KARL COMPILE ERROR: "
      "#size has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallSize (context_, nodes_);
}


// constructor
madara::expression::Sleep::Sleep (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::Sleep::~Sleep (void)
{
}

// returns the precedence level
int
madara::expression::Sleep::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Sleep::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#sleep::build: KARL COMPILE ERROR: "
      "#sleep has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#sleep::build: KARL COMPILE ERROR: "
      "#sleep has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallSleep (context_, nodes_);
}


// constructor
madara::expression::ToBuffer::ToBuffer (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::ToBuffer::~ToBuffer (void)
{
}

// returns the precedence level
int
madara::expression::ToBuffer::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ToBuffer::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#to_buffer::build: KARL COMPILE ERROR: "
      "#to_buffer has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#to_buffer::build: KARL COMPILE ERROR: "
      "#to_buffer has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallToBuffer (context_, nodes_);
}


// constructor
madara::expression::ToDouble::ToDouble (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::ToDouble::~ToDouble (void)
{
}

// returns the precedence level
int
madara::expression::ToDouble::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ToDouble::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#to_double::build: KARL COMPILE ERROR: "
      "#to_double has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#to_double::build: KARL COMPILE ERROR: "
      "#to_double has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallToDouble (context_, nodes_);
}


// constructor
madara::expression::ToDoubles::ToDoubles (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::ToDoubles::~ToDoubles (void)
{
}

// returns the precedence level
int
madara::expression::ToDoubles::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ToDoubles::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#to_doubles::build: KARL COMPILE ERROR: "
      "#to_doubles has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#to_doubles::build: KARL COMPILE ERROR: "
      "#to_doubles has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallToDoubles (context_, nodes_);
}


// constructor
madara::expression::ToHostDirs::ToHostDirs (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::ToHostDirs::~ToHostDirs (void)
{
}

// returns the precedence level
int
madara::expression::ToHostDirs::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ToHostDirs::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#to_host_dirs::build: KARL COMPILE ERROR: "
      "#to_host_dirs has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#to_host_dirs::build: KARL COMPILE ERROR: "
      "#to_host_dirs has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallToHostDirs (context_, nodes_);
}


// constructor
madara::expression::ToInteger::ToInteger (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::ToInteger::~ToInteger (void)
{
}

// returns the precedence level
int
madara::expression::ToInteger::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ToInteger::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#to_integer::build: KARL COMPILE ERROR: "
      "#to_integer has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#to_integer::build: KARL COMPILE ERROR: "
      "#to_integer has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallToInteger (context_, nodes_);
}


// constructor
madara::expression::ToIntegers::ToIntegers (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::ToIntegers::~ToIntegers (void)
{
}

// returns the precedence level
int
madara::expression::ToIntegers::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ToIntegers::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#to_integers::build: KARL COMPILE ERROR: "
      "#to_integers has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#to_integers::build: KARL COMPILE ERROR: "
      "#to_integers has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallToIntegers (context_, nodes_);
}


// constructor
madara::expression::ToString::ToString (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::ToString::~ToString (void)
{
}

// returns the precedence level
int
madara::expression::ToString::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ToString::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#to_string::build: KARL COMPILE ERROR: "
      "#to_string has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#to_string::build: KARL COMPILE ERROR: "
      "#to_string has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallToString (context_, nodes_);
}


// constructor
madara::expression::Type::Type (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::Type::~Type (void)
{
}

// returns the precedence level
int
madara::expression::Type::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Type::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#type::build: KARL COMPILE ERROR: "
      "#type has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#type::build: KARL COMPILE ERROR: "
      "#type has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallType (context_, nodes_);
}


// constructor
madara::expression::Isinf::Isinf (
  madara::knowledge::ThreadSafeContext & context)
  : SystemCall (context)
{
}

// destructor
madara::expression::Isinf::~Isinf (void)
{
}

// returns the precedence level
int
madara::expression::Isinf::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Isinf::build ()
{
  if (left_ || right_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "#is_inf::build: KARL COMPILE ERROR: "
      "#is_inf has a left or right child. Likely missing a semi-colon\n");

    throw KarlException (
      "#is_inf::build: KARL COMPILE ERROR: "
      "#is_inf has a left or right child. Likely missing a semi-colon\n");
  }
  
  return new SystemCallIsinf (context_, nodes_);
}



// constructor
madara::expression::ForLoop::ForLoop (Symbol * precondition,
  Symbol * condition, Symbol * postcondition,
  Symbol * body,
  madara::knowledge::ThreadSafeContext & context)
: UnaryOperator (context.get_logger (), 0, VARIABLE_PRECEDENCE),
  precondition_ (precondition), condition_ (condition),
  postcondition_ (postcondition), body_ (body), context_ (context)  
{
}

// destructor
madara::expression::ForLoop::~ForLoop (void)
{
  delete precondition_;
  delete postcondition_;
  delete condition_;
  delete body_;
}

// returns the precedence level
int
madara::expression::ForLoop::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ForLoop::build ()
{
  if (body_)
    return new CompositeForLoop (
      precondition_->build (), condition_->build (),
      postcondition_->build (), body_->build (), context_);
  else
  {
    ComponentNode * left (0), * right (0);

    if (left_)
      left = precondition_->left_->build ();

    if (right_)
      right = condition_->right_->build ();

    return new CompositeAssignmentNode (
      context_.get_logger (), left, right);
  }
}

// constructor
madara::expression::Postdecrement::Postdecrement (
  logger::Logger & logger)
: UnaryOperator (logger, 0, NEGATE_PRECEDENCE)
{
}

// destructor
madara::expression::Postdecrement::~Postdecrement (void)
{
}

// returns the precedence level
int
madara::expression::Postdecrement::add_precedence (int precedence)
{
  return this->precedence_ = NEGATE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Postdecrement::build ()
{
  ComponentNode * right (0);

  if (left_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "var--::build: KARL COMPILE ERROR: "
      "var-- has a left child. Likely missing a semi-colon\n");

    throw KarlException (
      "#var--::build: KARL COMPILE ERROR: "
      "#var-- has a left child. Likely missing a semi-colon\n");
  }
  
  if (right_)
    right = right_->build ();

  return new CompositePostdecrementNode (
    *(this->logger_), right);
}

// constructor
madara::expression::Postincrement::Postincrement (logger::Logger & logger)
: UnaryOperator (logger, 0, NEGATE_PRECEDENCE)
{
}

// destructor
madara::expression::Postincrement::~Postincrement (void)
{
}

// returns the precedence level
int
madara::expression::Postincrement::add_precedence (int precedence)
{
  return this->precedence_ = NEGATE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Postincrement::build ()
{
  ComponentNode * right (0);

  if (left_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "var++::build: KARL COMPILE ERROR: "
      "var++ has a left child. Likely missing a semi-colon\n");

    throw KarlException (
      "#var++::build: KARL COMPILE ERROR: "
      "#var++ has a left child. Likely missing a semi-colon\n");
  }
  
  if (right_)
    right = right_->build ();

  return new CompositePostincrementNode (
    *(this->logger_), right);
}

// constructor
madara::expression::Predecrement::Predecrement (logger::Logger & logger)
: UnaryOperator (logger, 0, NEGATE_PRECEDENCE)
{
}

// destructor
madara::expression::Predecrement::~Predecrement (void)
{
}

// returns the precedence level
int
madara::expression::Predecrement::add_precedence (int precedence)
{
  return this->precedence_ = NEGATE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Predecrement::build ()
{
  ComponentNode * right (0);

  if (left_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "--var::build: KARL COMPILE ERROR: "
      "--var has a left child. Likely missing a semi-colon\n");

    throw KarlException (
      "--var::build: KARL COMPILE ERROR: "
      "--var has a left child. Likely missing a semi-colon\n");
  }
  
  if (right_)
    right = right_->build ();

  return new CompositePredecrementNode (*(this->logger_), right);
}

// constructor
madara::expression::Preincrement::Preincrement (logger::Logger & logger)
  : UnaryOperator (logger, 0, NEGATE_PRECEDENCE)
{
}

// destructor
madara::expression::Preincrement::~Preincrement (void)
{
}

// returns the precedence level
int
madara::expression::Preincrement::add_precedence (int precedence)
{
  return this->precedence_ = NEGATE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Preincrement::build ()
{
  ComponentNode * right (0);

  if (left_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "++var::build: KARL COMPILE ERROR: "
      "++var has a left child. Likely missing a semi-colon\n");

    throw KarlException (
      "++var::build: KARL COMPILE ERROR: "
      "++var has a left child. Likely missing a semi-colon\n");
  }
  
  if (right_)
    right = right_->build ();

  return new CompositePreincrementNode (*(this->logger_), right);
}

// constructor
madara::expression::Not::Not (logger::Logger & logger)
  : UnaryOperator (logger, 0, NEGATE_PRECEDENCE)
{
}

// destructor
madara::expression::Not::~Not (void)
{
}

// returns the precedence level
int
madara::expression::Not::add_precedence (int precedence)
{
  return this->precedence_ = NEGATE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Not::build ()
{
  // check for cascading nots
  Not * next = dynamic_cast <Not *> (right_);
  Symbol * right = right_;
  unsigned int i;

  if (left_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "Not(!)::build: KARL COMPILE ERROR: "
      "Logical Not(!) has a left child. Likely missing a semi-colon\n");

    throw KarlException (
      "Not(!)::build: KARL COMPILE ERROR: "
      "Logical Not(!) has a left child. Likely missing a semi-colon\n");
  }
  
  if (right_)
  {
    for (i = 1; next;
      ++i, right = next->right_, next = dynamic_cast <Not *> (next->right_)) {}

    if (i % 2 == 1)
      return new CompositeNotNode (*(this->logger_), right->build ());
    else
      return new CompositeNotNode (*(this->logger_),
      new CompositeNotNode (*(this->logger_), right->build ()));
  }
  else
  {
    return new CompositeNotNode (*(this->logger_), 0);
  }
}

// constructor
madara::expression::SquareRootUnary::SquareRootUnary (logger::Logger & logger)
  : UnaryOperator (logger, 0, NEGATE_PRECEDENCE)
{
}

// destructor
madara::expression::SquareRootUnary::~SquareRootUnary (void)
{
}

// returns the precedence level
int
madara::expression::SquareRootUnary::add_precedence (int precedence)
{
  return this->precedence_ = NEGATE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::SquareRootUnary::build ()
{
  ComponentNode * right (0);

  if (left_)
  {
    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      "sqrt::build: KARL COMPILE ERROR: "
      "sqrt has a left child. Likely missing a semi-colon\n");

    throw KarlException (
      "sqrt::build: KARL COMPILE ERROR: "
      "sqrt has a left child. Likely missing a semi-colon\n");
  }
  
  if (right_)
    right = right_->build ();

  return new CompositeSquareRootNode (*(this->logger_), right);
}

// constructor
madara::expression::Variable::Variable (const std::string & key,
  madara::knowledge::ThreadSafeContext & context)
  : Symbol (context.get_logger (), 0, 0, VARIABLE_PRECEDENCE),
  key_ (key), context_ (context)
{
}

// destructor
madara::expression::Variable::~Variable (void)
{
}

// returns the precedence level
int
madara::expression::Variable::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Variable::build (void)
{
  if (left_ || right_)
  {
    std::string message = 
      "Variable::build: KARL COMPILE ERROR: "
      "Variable (";
    message += key_;
    message += ") has a left or right child. Likely missing a semi-colon\n";

    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      message.c_str ());

    throw KarlException (message);
  }
  
  // is reserved?
  if (key_ == "nan")
  {
    return new LeafNode (context_.get_logger (), NAN);
  }
  else if (key_ == "inf")
  {
    return new LeafNode (context_.get_logger (), INFINITY);
  }
  else
  {
    return new VariableNode (key_, context_);
  }
}


// constructor
madara::expression::ArrayRef::ArrayRef (const std::string & key,
  Symbol * index,
  madara::knowledge::ThreadSafeContext & context)
  : Symbol (context.get_logger (), 0, 0, VARIABLE_PRECEDENCE),
  key_ (key), context_ (context), index_ (index)
{
}

// destructor
madara::expression::ArrayRef::~ArrayRef (void)
{
}

// returns the precedence level
int
madara::expression::ArrayRef::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ArrayRef::build (void)
{
  if (left_ || right_)
  {
    std::string message = 
      "ArrayRef::build: KARL COMPILE ERROR: ";
    message += key_;
    message += "[] has a left or right child. Likely missing a semi-colon\n";

    madara_logger_ptr_log (logger_, logger::LOG_ERROR,
      message.c_str ());

    throw KarlException (message);
  }
  
  return new CompositeArrayReference (key_, index_->build (), context_);
}


// constructor
madara::expression::VariableDecrement::VariableDecrement (
  Symbol * lhs,
  madara::knowledge::KnowledgeRecord value, Symbol * rhs,
  madara::knowledge::ThreadSafeContext & context)
  : Operator (context.get_logger (), 0, rhs, ASSIGNMENT_PRECEDENCE), left_ (lhs), value_ (value),
  context_ (context)
{
}

// destructor
madara::expression::VariableDecrement::~VariableDecrement (void)
{
}

// returns the precedence level
int
madara::expression::VariableDecrement::add_precedence (int precedence)
{
  return this->precedence_ = ASSIGNMENT_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::VariableDecrement::build (void)
{
  if (this->right_)
    return new VariableDecrementNode (left_->build (), value_, this->right_->build (), context_);
  else
    return new VariableDecrementNode (left_->build (), value_, 0, context_);
}


// constructor
madara::expression::VariableDivide::VariableDivide (Symbol * lhs,
  knowledge::KnowledgeRecord value, Symbol * rhs,
  madara::knowledge::ThreadSafeContext & context)
  : Operator (context.get_logger (), 0, rhs, ASSIGNMENT_PRECEDENCE), left_ (lhs), value_ (value),
  context_ (context)
{
}

// destructor
madara::expression::VariableDivide::~VariableDivide (void)
{
}

// returns the precedence level
int
madara::expression::VariableDivide::add_precedence (int precedence)
{
  return this->precedence_ = ASSIGNMENT_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::VariableDivide::build (void)
{
  if (this->right_)
    return new VariableDivideNode (left_->build (), value_, this->right_->build (), context_);
  else
    return new VariableDivideNode (left_->build (), value_, 0, context_);
}

// constructor
madara::expression::VariableIncrement::VariableIncrement (Symbol * lhs,
  madara::knowledge::KnowledgeRecord value, Symbol * rhs,
  madara::knowledge::ThreadSafeContext & context)
  : Operator (context.get_logger (), 0, rhs, ASSIGNMENT_PRECEDENCE),
  left_ (lhs), value_ (value),
  context_ (context)
{
}

// destructor
madara::expression::VariableIncrement::~VariableIncrement (void)
{
}

// returns the precedence level
int
madara::expression::VariableIncrement::add_precedence (int precedence)
{
  return this->precedence_ = ASSIGNMENT_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::VariableIncrement::build (void)
{
  if (this->right_)
    return new VariableIncrementNode (left_->build (),
    value_, this->right_->build (), context_);
  else
    return new VariableIncrementNode (left_->build (), value_, 0, context_);
}

// constructor
madara::expression::VariableMultiply::VariableMultiply (Symbol * lhs,
  madara::knowledge::KnowledgeRecord value, Symbol * rhs,
  madara::knowledge::ThreadSafeContext & context)
  : Operator (context.get_logger (), 0, rhs, ASSIGNMENT_PRECEDENCE),
  left_ (lhs), value_ (value),
  context_ (context)
{
}

// destructor
madara::expression::VariableMultiply::~VariableMultiply (void)
{
}

// returns the precedence level
int
madara::expression::VariableMultiply::add_precedence (int precedence)
{
  return this->precedence_ = ASSIGNMENT_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::VariableMultiply::build (void)
{
  if (this->right_)
    return new VariableMultiplyNode (left_->build (), value_, this->right_->build (), context_);
  else
    return new VariableMultiplyNode (left_->build (), value_, 0, context_);
}


// constructor
madara::expression::VariableCompare::VariableCompare (Symbol * lhs,
  madara::knowledge::KnowledgeRecord value, Symbol * rhs, int compare_type,
  madara::knowledge::ThreadSafeContext & context)
  : Symbol (context.get_logger (), 0, 0, VARIABLE_PRECEDENCE),
  left_ (lhs), value_ (value), rhs_ (rhs),
  compare_type_ (compare_type), context_ (context)
{
}

// destructor
madara::expression::VariableCompare::~VariableCompare (void)
{
}

// returns the precedence level
int
madara::expression::VariableCompare::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::VariableCompare::build (void)
{
  if (rhs_)
    return new VariableCompareNode (left_->build (), value_, compare_type_,
    rhs_->build (), context_);
  else
    return new VariableCompareNode (left_->build (), value_, compare_type_,
    0, context_);
}



// constructor
madara::expression::List::List (
  madara::knowledge::ThreadSafeContext & context)
  : Symbol (context.get_logger (), 0, 0, VARIABLE_PRECEDENCE),
    context_ (context)
{
}

// destructor
madara::expression::List::~List (void)
{
}

// returns the precedence level
int
madara::expression::List::add_precedence (int precedence)
{
  return this->precedence_ = VARIABLE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::List::build (void)
{
  return new ListNode (context_);
}

// constructor
madara::expression::Add::Add (logger::Logger & logger)
  : TernaryOperator (logger, 0, 0, ADD_PRECEDENCE)
{
}

// destructor
madara::expression::Add::~Add (void)
{
}

// returns the precedence level
int
madara::expression::Add::add_precedence (int precedence)
{
  return this->precedence_ = ADD_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Add::build (void)
{
  if (left_ && right_)
  {
    // check for cascading max
    Add * next = dynamic_cast <Add *> (left_);
    Symbol * left = left_;

    // push the right onto the deque
    nodes_.push_back (right_->build ());
    delete right_;
    right_ = 0;

    for (; next; next = dynamic_cast <Add *> (left))
    {
      // we have a chained max node. Move the left into our nodes list
      if (next->right_)
      {
        nodes_.push_front (next->right_->build ());
        delete next->right_;
        next->right_ = 0;
      }

      // set right to next->right_ and then clear next->right_ before deletion
      left = next->left_;
      next->left_ = 0;
      delete next;
    }

    // push the rightmost build from the compressed node and delete it.
    // then reset our right_, since we've already taken care of deletion
    if (left)
    {
      nodes_.push_front (left->build ());
      delete left;
    }
    left_ = 0;

    return new CompositeAddNode (*(this->logger_), nodes_);
  }
  else if (left_)
    // all we have is a valid left child, so there is no reason to build
    // an Add operator
    return left_->build ();
  else if (right_)
    // all we have is a valid right child, so there is no reason to build
    // a Add operator
    return right_->build ();
  else
  {
    // we've got nothing. This node should eventually be pruned out of the
    // picture if at all possible.
    return new CompositeAddNode (*(this->logger_), nodes_);
  }
}


// constructor
madara::expression::And::And (logger::Logger & logger)
  : TernaryOperator (logger, 0, 0, LOGICAL_PRECEDENCE)
{
}

// destructor
madara::expression::And::~And (void)
{
}

// returns the precedence level
int
madara::expression::And::add_precedence (int precedence)
{
  return this->precedence_ = LOGICAL_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::And::build (void)
{
  if (left_ && right_)
  {
    // check for cascading max
    And * next = dynamic_cast <And *> (left_);
    Symbol * left = left_;

    // push the right onto the deque
    nodes_.push_back (right_->build ());
    delete right_;
    right_ = 0;

    for (; next; next = dynamic_cast <And *> (left))
    {
      // we have a chained max node. Move the left into our nodes list
      if (next->right_)
      {
        nodes_.push_front (next->right_->build ());
        delete next->right_;
        next->right_ = 0;
      }

      // set right to next->right_ and then clear next->right_ before deletion
      left = next->left_;
      next->left_ = 0;
      delete next;
    }

    // push the rightmost build from the compressed node and delete it.
    // then reset our right_, since we've already taken care of deletion
    if (left)
    {
      nodes_.push_front (left->build ());
      delete left;
    }
    left_ = 0;

    return new CompositeAndNode (*(this->logger_), nodes_);
  }
  else if (left_)
    // all we have is a valid left child, so there is no reason to build
    // an And operator
    return left_->build ();
  else if (right_)
    // all we have is a valid right child, so there is no reason to build
    // an And operator
    return right_->build ();
  else
  {
    // we've got nothing. This node should eventually be pruned out of the
    // picture if at all possible.
    return new CompositeAndNode (*(this->logger_), nodes_);
  }
}

// constructor
madara::expression::Or::Or (logger::Logger & logger)
  : TernaryOperator (logger, 0, 0, LOGICAL_PRECEDENCE)
{
}

// destructor
madara::expression::Or::~Or (void)
{
}

// returns the precedence level
int
madara::expression::Or::add_precedence (int precedence)
{
  return this->precedence_ = LOGICAL_PRECEDENCE + precedence;
}



// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Or::build (void)
{
  if (left_ && right_)
  {
    // check for cascading max
    Or * next = dynamic_cast <Or *> (left_);
    Symbol * left = left_;

    // push the right onto the deque
    nodes_.push_back (right_->build ());
    delete right_;
    right_ = 0;

    for (; next; next = dynamic_cast <Or *> (left))
    {
      // we have a chained max node. Move the left into our nodes list
      if (next->right_)
      {
        nodes_.push_front (next->right_->build ());
        delete next->right_;
        next->right_ = 0;
      }

      // set right to next->right_ and then clear next->right_ before deletion
      left = next->left_;
      next->left_ = 0;
      delete next;
    }

    // push the rightmost build from the compressed node and delete it.
    // then reset our right_, since we've already taken care of deletion
    if (left)
    {
      nodes_.push_front (left->build ());
      delete left;
    }
    left_ = 0;

    return new CompositeOrNode (*(this->logger_), nodes_);
  }
  else if (left_)
    // all we have is a valid left child, so there is no reason to build
    // a Both operator
    return left_->build ();
  else if (right_)
    // all we have is a valid right child, so there is no reason to build
    // a Both operator
    return right_->build ();
  else
  {
    // we've got nothing. This node should eventually be pruned out of the
    // picture if at all possible.
    return new CompositeOrNode (*(this->logger_), nodes_);
  }
}


// constructor
madara::expression::Both::Both (logger::Logger & logger)
  : TernaryOperator (logger, 0, 0, BOTH_PRECEDENCE)
{
}

// destructor
madara::expression::Both::~Both (void)
{
}

// returns the precedence level
int
madara::expression::Both::add_precedence (int precedence)
{
  return this->precedence_ = BOTH_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Both::build (void)
{
  // Since users can say something like ";;;;;;;;", it is very possible
  // that a both operation contains no valid children. So, we need
  // to check whether or not we have a valid child.
  if (left_ && right_)
  {
    // check for cascading max
    Both * next = dynamic_cast <Both *> (left_);
    Symbol * left = left_;

    // push the right onto the deque
    //nodes_.push_back (right_->build ());
    //delete right_;
    //right_ = 0;

    for (; next; next = dynamic_cast <Both *> (left))
    {
      // we have a chained max node. Move the left into our nodes list
      if (next->right_)
      {
        nodes_.push_front (next->right_->build ());
        delete next->right_;
        next->right_ = 0;
      }

      // set right to next->right_ and then clear next->right_ before deletion
      left = next->left_;
      next->left_ = 0;
      delete next;
    }

    // push the rightmost build from the compressed node and delete it.
    // then reset our right_, since we've already taken care of deletion
    if (left)
    {
      nodes_.push_front (left->build ());
      delete left;
    }
    left_ = 0;


    next = dynamic_cast <Both *> (right_);
    Symbol * right = right_;

    for (; next; next = dynamic_cast <Both *> (right))
    {
      // we have a chained max node. Move the left into our nodes list
      if (next->left_)
      {
        nodes_.push_back (next->left_->build ());
        delete next->left_;
        next->left_ = 0;
      }

      // set right to next->right_ and then clear next->right_ before deletion
      right = next->right_;
      next->right_ = 0;
      delete next;
    }

    // push the rightmost build from the compressed node and delete it.
    // then reset our right_, since we've already taken care of deletion
    if (right)
    {
      nodes_.push_back (right->build ());
      delete right;
    }
    right_ = 0;

    return new CompositeBothNode (*(this->logger_), nodes_);
  }
  else if (left_)
    // all we have is a valid left child, so there is no reason to build
    // a Both operator
    return left_->build ();
  else if (right_)
    // all we have is a valid right child, so there is no reason to build
    // a Both operator
    return right_->build ();
  else
    // we've got nothing. This node should eventually be pruned out of the
    // picture if at all possible.
    return new LeafNode (*(this->logger_),
    (madara::knowledge::KnowledgeRecord::Integer)0);
}


// constructor
madara::expression::ReturnRight::ReturnRight (logger::Logger & logger)
  : TernaryOperator (logger, 0, 0, BOTH_PRECEDENCE)
{
}

// destructor
madara::expression::ReturnRight::~ReturnRight (void)
{
}

// returns the precedence level
int
madara::expression::ReturnRight::add_precedence (int precedence)
{
  return this->precedence_ = BOTH_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::ReturnRight::build (void)
{
  // Since users can say something like ";;;;;;;;", it is very possible
  // that a both operation contains no valid children. So, we need
  // to check whether or not we have a valid child.
  if (left_ && right_)
  {
    // check for cascading max
    ReturnRight * next = dynamic_cast <ReturnRight *> (left_);
    Symbol * left = left_;

    // push the right onto the deque
    //nodes_.push_back (right_->build ());
    //delete right_;
    //right_ = 0;

    for (; next; next = dynamic_cast <ReturnRight *> (left))
    {
      // we have a chained max node. Move the left into our nodes list
      if (next->right_)
      {
        nodes_.push_front (next->right_->build ());
        delete next->right_;
        next->right_ = 0;
      }

      // set right to next->right_ and then clear next->right_ before deletion
      left = next->left_;
      next->left_ = 0;
      delete next;
    }

    // push the rightmost build from the compressed node and delete it.
    // then reset our right_, since we've already taken care of deletion
    if (left)
    {
      nodes_.push_front (left->build ());
      delete left;
    }
    left_ = 0;


    next = dynamic_cast <ReturnRight *> (right_);
    Symbol * right = right_;

    for (; next; next = dynamic_cast <ReturnRight *> (right))
    {
      // we have a chained max node. Move the left into our nodes list
      if (next->left_)
      {
        nodes_.push_back (next->left_->build ());
        delete next->left_;
        next->left_ = 0;
      }

      // set right to next->right_ and then clear next->right_ before deletion
      right = next->right_;
      next->right_ = 0;
      delete next;
    }

    // push the rightmost build from the compressed node and delete it.
    // then reset our right_, since we've already taken care of deletion
    if (right)
    {
      nodes_.push_back (right->build ());
      delete right;
    }
    right_ = 0;

    return new CompositeReturnRightNode (*(this->logger_), nodes_);
  }
  else if (left_)
    // all we have is a valid left child, so there is no reason to build
    // a Both operator
    return left_->build ();
  else if (right_)
    // all we have is a valid right child, so there is no reason to build
    // a Both operator
    return right_->build ();
  else
    // we've got nothing. This node should eventually be pruned out of the
    // picture if at all possible.
    return new LeafNode (*(this->logger_),
    (madara::knowledge::KnowledgeRecord::Integer)0);
}

// constructor
madara::expression::Sequence::Sequence (logger::Logger & logger)
  : TernaryOperator (logger, 0, 0, BOTH_PRECEDENCE)
{
}

// destructor
madara::expression::Sequence::~Sequence (void)
{
}

// returns the precedence level
int
madara::expression::Sequence::add_precedence (int precedence)
{
  return this->precedence_ = BOTH_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Sequence::build (void)
{
  // Since users can say something like ";;;;;;;;", it is very possible
  // that a both operation contains no valid children. So, we need
  // to check whether or not we have a valid child.
  if (left_ && right_)
  {
    // check for cascading max
    Sequence * next = dynamic_cast <Sequence *> (left_);
    Symbol * left = left_;

    // push the right onto the deque
    //nodes_.push_back (right_->build ());
    //delete right_;
    //right_ = 0;

    for (; next; next = dynamic_cast <Sequence *> (left))
    {
      // we have a chained max node. Move the left into our nodes list
      if (next->right_)
      {
        nodes_.push_front (next->right_->build ());
        delete next->right_;
        next->right_ = 0;
      }

      // set right to next->right_ and then clear next->right_ before deletion
      left = next->left_;
      next->left_ = 0;
      delete next;
    }

    // push the rightmost build from the compressed node and delete it.
    // then reset our right_, since we've already taken care of deletion
    if (left)
    {
      nodes_.push_front (left->build ());
      delete left;
    }
    left_ = 0;


    next = dynamic_cast <Sequence *> (right_);
    Symbol * right = right_;

    for (; next; next = dynamic_cast <Sequence *> (right))
    {
      // we have a chained max node. Move the left into our nodes list
      if (next->left_)
      {
        nodes_.push_back (next->left_->build ());
        delete next->left_;
        next->left_ = 0;
      }

      // set right to next->right_ and then clear next->right_ before deletion
      right = next->right_;
      next->right_ = 0;
      delete next;
    }

    // push the rightmost build from the compressed node and delete it.
    // then reset our right_, since we've already taken care of deletion
    if (right)
    {
      nodes_.push_back (right->build ());
      delete right;
    }
    right_ = 0;

    return new CompositeSequentialNode (*(this->logger_), nodes_);
  }
  else if (left_)
    // all we have is a valid left child, so there is no reason to build
    // a Both operator
    return left_->build ();
  else if (right_)
    // all we have is a valid right child, so there is no reason to build
    // a Both operator
    return right_->build ();
  else
    // we've got nothing. This node should eventually be pruned out of the
    // picture if at all possible.
    return new LeafNode (*(this->logger_),
    (madara::knowledge::KnowledgeRecord::Integer)0);
}

// constructor
madara::expression::Implies::Implies (logger::Logger & logger)
  : Operator (logger, 0, 0, IMPLIES_PRECEDENCE)
{
}

// destructor
madara::expression::Implies::~Implies (void)
{
}

// returns the precedence level
int
madara::expression::Implies::add_precedence (int precedence)
{
  return this->precedence_ = IMPLIES_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Implies::build (void)
{
  ComponentNode * left (0), * right (0);

  if (left_)
    left = left_->build ();

  if (right_)
    right = right_->build ();

  return new CompositeImpliesNode (*(this->logger_), left, right);
}


// constructor
madara::expression::Assignment::Assignment (logger::Logger & logger)
  : Operator (logger, 0, 0, ASSIGNMENT_PRECEDENCE)
{
}

// destructor
madara::expression::Assignment::~Assignment (void)
{
}

// returns the precedence level
int
madara::expression::Assignment::add_precedence (int precedence)
{
  return this->precedence_ = ASSIGNMENT_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Assignment::build (void)
{
  ComponentNode * left (0), * right (0);

  if (left_)
    left = left_->build ();

  if (right_)
    right = right_->build ();

  return new CompositeAssignmentNode (*(this->logger_), left, right);
}


// constructor
madara::expression::Equality::Equality (logger::Logger & logger)
  : Operator (logger, 0, 0, CONDITIONAL_PRECEDENCE)
{
}

// destructor
madara::expression::Equality::~Equality (void)
{
}

// returns the precedence level
int
madara::expression::Equality::add_precedence (int precedence)
{
  return this->precedence_ = CONDITIONAL_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Equality::build (void)
{
  ComponentNode * left (0), * right (0);

  if (left_)
    left = left_->build ();

  if (right_)
    right = right_->build ();

  return new CompositeEqualityNode (*(this->logger_), left, right);
}

// constructor
madara::expression::Inequality::Inequality (logger::Logger & logger)
  : Operator (logger, 0, 0, CONDITIONAL_PRECEDENCE)
{
}

// destructor
madara::expression::Inequality::~Inequality (void)
{
}

// returns the precedence level
int
madara::expression::Inequality::add_precedence (int precedence)
{
  return this->precedence_ = CONDITIONAL_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Inequality::build (void)
{
  ComponentNode * left (0), * right (0);

  if (left_)
    left = left_->build ();

  if (right_)
    right = right_->build ();

  return new CompositeInequalityNode (*(this->logger_), left, right);
}

// constructor
madara::expression::GreaterThanEqual::GreaterThanEqual (
  logger::Logger & logger)
  : Operator (logger, 0, 0, CONDITIONAL_PRECEDENCE)
{
}

// destructor
madara::expression::GreaterThanEqual::~GreaterThanEqual (void)
{
}

// returns the precedence level
int
madara::expression::GreaterThanEqual::add_precedence (int precedence)
{
  return this->precedence_ = CONDITIONAL_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::GreaterThanEqual::build (void)
{
  ComponentNode * left (0), * right (0);

  if (left_)
    left = left_->build ();

  if (right_)
    right = right_->build ();

  return new CompositeGreaterThanEqualNode (
    *(this->logger_), left, right);
}

// constructor
madara::expression::GreaterThan::GreaterThan (logger::Logger & logger)
  : Operator (logger, 0, 0, CONDITIONAL_PRECEDENCE)
{
}

// destructor
madara::expression::GreaterThan::~GreaterThan (void)
{
}

// returns the precedence level
int
madara::expression::GreaterThan::add_precedence (int precedence)
{
  return this->precedence_ = CONDITIONAL_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::GreaterThan::build (void)
{
  ComponentNode * left (0), * right (0);

  if (left_)
    left = left_->build ();

  if (right_)
    right = right_->build ();

  return new CompositeGreaterThanNode (*(this->logger_), left, right);
}

// constructor
madara::expression::LessThanEqual::LessThanEqual (
  logger::Logger & logger)
  : Operator (logger, 0, 0, CONDITIONAL_PRECEDENCE)
{
}

// destructor
madara::expression::LessThanEqual::~LessThanEqual (void)
{
}

// returns the precedence level
int
madara::expression::LessThanEqual::add_precedence (int precedence)
{
  return this->precedence_ = CONDITIONAL_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::LessThanEqual::build (void)
{
  ComponentNode * left (0), * right (0);

  if (left_)
    left = left_->build ();

  if (right_)
    right = right_->build ();

  return new CompositeLessThanEqualNode (*(this->logger_), left, right);
}

// constructor
madara::expression::LessThan::LessThan (logger::Logger & logger)
  : Operator (logger, 0, 0, CONDITIONAL_PRECEDENCE)
{
}

// destructor
madara::expression::LessThan::~LessThan (void)
{
}

// returns the precedence level
int
madara::expression::LessThan::add_precedence (int precedence)
{
  return this->precedence_ = CONDITIONAL_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::LessThan::build (void)
{
  ComponentNode * left (0), * right (0);

  if (left_)
    left = left_->build ();

  if (right_)
    right = right_->build ();

  return new CompositeLessThanNode (*(this->logger_), left, right);
}


// constructor
madara::expression::Subtract::Subtract (logger::Logger & logger)
  : Operator (logger, 0, 0, SUBTRACT_PRECEDENCE)
{
}

// destructor
madara::expression::Subtract::~Subtract (void)
{
}

// returns the precedence level
int
madara::expression::Subtract::add_precedence (int precedence)
{
  return this->precedence_ = ADD_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Subtract::build (void)
{
  ComponentNode * left (0), * right (0);

  if (left_)
    left = left_->build ();

  if (right_)
    right = right_->build ();

  return new CompositeSubtractNode (*(this->logger_), left, right);
}

// constructor
madara::expression::Multiply::Multiply (logger::Logger & logger)
  : TernaryOperator (logger, 0, 0, MULTIPLY_PRECEDENCE)
{
}

// destructor
madara::expression::Multiply::~Multiply (void)
{
}

// returns the precedence level
int
madara::expression::Multiply::add_precedence (int precedence)
{
  return this->precedence_ = MULTIPLY_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Multiply::build (void)
{
  if (left_ && right_)
  {
    Multiply * rhs = dynamic_cast <Multiply *> (right_);

    nodes_.push_back (left_->build ());

    if (rhs)
    {
      nodes_.insert (nodes_.end (), rhs->nodes_.begin (), rhs->nodes_.end ());
      rhs->nodes_.clear ();
    }
    else
    {
      nodes_.push_back (right_->build ());
    }
    return new CompositeMultiplyNode (*(this->logger_), nodes_);
  }
  else
  {
    return new CompositeMultiplyNode (*(this->logger_), nodes_);
  }
}

// constructor
madara::expression::Modulus::Modulus (logger::Logger & logger)
  : Operator (logger, 0, 0, MODULUS_PRECEDENCE)
{
}

// destructor
madara::expression::Modulus::~Modulus (void)
{
}

// returns the precedence level
int
madara::expression::Modulus::add_precedence (int precedence)
{
  return this->precedence_ = MODULUS_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Modulus::build (void)
{
  ComponentNode * left (0), * right (0);

  if (left_)
    left = left_->build ();

  if (right_)
    right = right_->build ();

  return new CompositeModulusNode (*(this->logger_),
    left, right);
}

// constructor
madara::expression::Divide::Divide (logger::Logger & logger)
  : Operator (logger, 0, 0, DIVIDE_PRECEDENCE)
{
}

// destructor
madara::expression::Divide::~Divide (void)
{
}

// returns the precedence level
int
madara::expression::Divide::add_precedence (int precedence)
{
  return this->precedence_ = DIVIDE_PRECEDENCE + precedence;
}

// builds an equivalent ExpressionTree node
madara::expression::ComponentNode *
madara::expression::Divide::build (void)
{
  ComponentNode * left (0), * right (0);

  if (left_)
    left = left_->build ();

  if (right_)
    right = right_->build ();

  return new CompositeDivideNode (*(this->logger_), left, right);
}

// constructor
madara::expression::Interpreter::Interpreter ()
{
}

// destructor
madara::expression::Interpreter::~Interpreter ()
{
}

// extracts precondition, condition, postcondition, and body from input 
void
madara::expression::Interpreter::handle_for_loop (
madara::knowledge::ThreadSafeContext &context,
std::string &variable,
const std::string &input,
std::string::size_type &i,
int & accumulated_precedence,
::std::list<Symbol *>& list,
Symbol *& returnableInput)
{
  ::std::list <Symbol *> substr_list;
  Symbol * lastValidInput (0);
  std::string::size_type begin = i;
  Operator * precondition (0); //, * condition (0), * postcondition (0);
  Symbol * body (0), *user_pre (0), *user_cond (0), *user_post (0);

  // for extracting and using substrings of input
  std::string::size_type count (0);
  std::string substr;

  if (variable == "")
  {
    variable = ".MADARA_I";
  }

  bool delimiter_found = false, handled = false, equal_to = false;
  std::string::size_type delimiter_begin = 0;
  std::string::size_type delimiter_end = 0;
  int paren_depth = 0;
  int index_depth = 0;

  // search for end of for_loop conditions. Be on lookout for delimiter.
  for (; i < input.length (); ++i)
  {
    if (index_depth == 0 && paren_depth == 0 &&
      input[i] == '-' && !delimiter_found)
    {
      delimiter_found = true;
      delimiter_begin = i;
    }
    else if (index_depth == 0 && paren_depth == 0 &&
      delimiter_found && input[i] == '>')
    {
      delimiter_end = i;
    }
    else if (input[i] == ']')
    {
      if (index_depth == 0 && paren_depth == 0)
      {
        break;
      }
      else
      {
        --index_depth;
      }
    }
    else if (input[i] == '(')
    {
      ++paren_depth;
    }
    else if (input[i] == '[')
    {
      ++index_depth;
    }
    else if (input[i] == ')')
    {
      if (paren_depth == 0)
      {
        break;
      }
      else
      {
        --paren_depth;
      }
    }
  }

  /**
   * if we manage to find a delimiter but not its end, we're seeing an
   * array reference with subtraction in it
   **/
  if (delimiter_found && delimiter_end == 0)
  {
    delimiter_found = false;
  }

  // this is actually an array index
  if (!delimiter_found)
  {
    // variable
    // begin to end--the index
    substr = input.substr (begin, i - begin);

    Symbol * index = nullptr;

    for (count = 0;
      count < substr.length ();)
    {
      main_loop (context, substr, count, lastValidInput,
        handled, accumulated_precedence, substr_list);
    }

    madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
      "madara::expression::Interpreter: "
      "KaRL: For loop: Array reference created at %s\n",
      substr.c_str ());

    // we have a precondition
    if (!substr_list.empty ())
    {
      index = substr_list.back ();
      substr_list.clear ();
    }

    Symbol * op = new ArrayRef (variable, index, context);
    op->add_precedence (accumulated_precedence);

    // check for post increments and decrements
    if (i + 2 < input.size ())
    {
      if (input[i + 1] == '+' && input[i + 2] == '+')
      {
        Symbol * array_ref = op;
        op = new Postincrement (context.get_logger ());
        op->add_precedence (accumulated_precedence);
        op->right_ = array_ref;
        i += 2;
      }
      else if (input[i + 1] == '-' && input[i + 2] == '-')
      {
        Symbol * array_ref = op;
        op = new Postdecrement (context.get_logger ());
        op->add_precedence (accumulated_precedence);
        op->right_ = array_ref;
        i += 2;
      }
    }

    lastValidInput = op;

    precedence_insert (context, op, list);

    returnableInput = op;
    ++i;
    return;
  }

  madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
    "madara::expression::Interpreter: "
    "KaRL: For loop: Within input string, the for loop delimiter begins at %d"
    " and ends at %d (should be at least 1). Loop construct begins at "
    "%d and ends at %d\n",
    (int)delimiter_begin, (int)delimiter_end, (int)begin, (int)i);


  // What did we end with? Less than? Greater than?
  if (input[i] == ']')
    equal_to = true;
  else if (input[i] != ')')
  {
    // this is an error. Essentially, it means the user did not close the
    // for loop.
    madara_logger_log (context.get_logger (), logger::LOG_ERROR,
      "madara::expression::Interpreter: "
      "KARL COMPILE ERROR:: No closing delimiter (']' or ')')"
      " has been specified on the for loop.\n");

    throw KarlException ("madara::expression::Interpreter: "
      "KARL COMPILE ERROR:: No closing delimiter (']' or ')')"
      " has been specified on the for loop.\n");
  }

  // get the precondition, postcondition and condition ready
  precondition = new Assignment (context.get_logger ());
  precondition->left_ = new Variable (variable, context);


  // this is the non-short-hand way of specifying, e.g., var[0,30] {}
  if (delimiter_found)
  {
    // setup precondition
    if (delimiter_begin - begin > 0)
    {
      // run main_loop on the precondition substring
      substr = input.substr (begin, delimiter_begin - begin);

      for (count = 0;
        count < substr.length ();)
      {
        main_loop (context, substr, count, lastValidInput,
          handled, accumulated_precedence, substr_list);
      }

      madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
        "madara::expression::Interpreter: "
        "KaRL: For loop: Precondition is set to %s\n", substr.c_str ());

      // we have a precondition
      if (!substr_list.empty ())
      {
        user_pre = substr_list.back ();
        substr_list.clear ();
      }
    }
    else
    {
      madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
        "madara::expression::Interpreter: "
        "KaRL: For loop: No loop precondition was specified\n");
    }

    // check for special increment
    if (delimiter_end - delimiter_begin > 1)
    {
      count = 0;
      lastValidInput = 0;
      substr = input.substr (delimiter_begin + 1, delimiter_end - (delimiter_begin + 1));

      for (count = 0;
        count < substr.length ();)
      {
        main_loop (context, substr, count, lastValidInput,
          handled, accumulated_precedence, substr_list);
      }

      madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
        "madara::expression::Interpreter: "
        "KaRL: For loop: Postcondition is set to %s\n", substr.c_str ());

      // we have a postcondition
      if (!substr_list.empty ())
      {
        user_post = substr_list.back ();

        substr_list.clear ();
      }
    }
    else
    {
      madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
        "KaRL: For loop: No loop special increment was specified\n");
    }

    // set condition
    if (i - delimiter_end >= 2)
    {
      lastValidInput = 0;
      substr = input.substr (delimiter_end + 1, i - (delimiter_end + 1));

      for (count = 0;
        count < substr.length ();)
      {
        main_loop (context, substr, count, lastValidInput,
          handled, accumulated_precedence, substr_list);
      }

      // we have a condition
      if (!substr_list.empty ())
      {
        madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
          "KaRL: For loop: Condition is set to %s\n", substr.c_str ());

        user_cond = substr_list.back ();
        substr_list.clear ();
      }
      else
      {
        madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
          "KaRL: For loop: Condition was not set to %s\n", substr.c_str ());
      }
    }
    else
    {
      madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
        "KaRL: For loop: No loop condition was specified\n");
    }
  }
  // if no delimiter found, this is the shorthand
  else
  {
    lastValidInput = 0;
    substr = input.substr (begin, i - begin);

    for (count = 0;
      count < substr.length ();)
    {
      main_loop (context, substr, count, lastValidInput,
        handled, accumulated_precedence, substr_list);
    }

    madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
      "KaRL: For loop: Condition only is set to %s\n", substr.c_str ());

    // we have a condition
    if (!substr_list.empty ())
    {
      user_cond = substr_list.back ();
      substr_list.clear ();
    }
  }

  // if precondition not set, set to default
  if (!user_pre)
  {
    user_pre = new Number (context.get_logger (),
      (madara::knowledge::KnowledgeRecord::Integer)0);
  }

  // set condition to default if not yet set
  if (!user_cond)
  {
    user_cond = new Number (context.get_logger (),
    (madara::knowledge::KnowledgeRecord::Integer) - 1);
  }

  // set postcondition to default if not yet set
  if (!user_post)
  {
    user_post = new Number (context.get_logger (),
      (madara::knowledge::KnowledgeRecord::Integer)1);
    madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
      "KaRL: For loop: Postcondition is set to 1 (def)\n");
  }

  // eat up whitespace so we can check for a parenthesis (function)
  for (++i; i < input.length () && is_whitespace (input[i]); ++i);

  // can't have a body without a parenthesis or brace
  if (i < input.length () && input[i] == '(')
  {
    ++i;
    lastValidInput = 0;

    madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
      "KaRL: For loop: Body is reading from %s\n",
      input.substr (i, input.size () - i).c_str ());


    // we have a function instead of a variable
    handle_parenthesis (context, input, i, lastValidInput, handled,
      accumulated_precedence, substr_list);

    if (!substr_list.empty ())
    {
      body = substr_list.back ();
      substr_list.clear ();
    }
  }

  // now, see if we can locate a body for the for loop
  if (body)
  {
    Assignment * assignment = dynamic_cast <Assignment *> (body);
    if (assignment)
    {
      Variable * variable_node = dynamic_cast <Variable *> (assignment->left_);
      Number * number = dynamic_cast <Number *> (assignment->right_);

      if (variable_node && number)
      {
        madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
          "KaRL: For loop: Body is a simple assignment of variable %s to %s\n",
          variable_node->key_.c_str (), number->item_.to_string ().c_str ());
      }
      else
      {
        madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
          "KaRL: For loop: For loop has a complex body\n");
      }
    }

    precondition->right_ = user_pre;
    madara::knowledge::KnowledgeRecord post_val;
    madara::knowledge::KnowledgeRecord cond_val;

    // optimize postcondition
    Number * number = dynamic_cast <Number *> (user_post);
    if (number)
    {
      post_val = number->item_;
      delete number;
      user_post = 0;
    }

    // optimize condition
    number = dynamic_cast <Number *> (user_cond);
    if (number)
    {
      cond_val = number->item_;
      delete number;
      user_cond = 0;
    }

    int compare_type (0);

    if (equal_to)
      compare_type = 1;

    Variable * var_node = new Variable (variable, context);

    VariableIncrement * postcondition = new VariableIncrement (
      var_node, post_val, user_post, context);
    postcondition->add_precedence (
      accumulated_precedence + FOR_LOOP_PRECEDENCE);

    VariableCompare * condition = new VariableCompare (var_node, cond_val,
      user_cond, compare_type, context);
    condition->add_precedence (accumulated_precedence + FOR_LOOP_PRECEDENCE);

    Symbol * op = new ForLoop (
      precondition, condition, postcondition, body, context);
    op->add_precedence (accumulated_precedence);

    precedence_insert (context, op, list);

    lastValidInput = 0;
  }
  else
  {
    // user forgot to specify a for loop body, so they apparently just want us to loop
    // the variable from the precondition to the condition. Since we assume the user
    // doesn't want a busy loop, which this is, we instead create an assignment for
    // the variable to equal the condition and clean up the pointers

    // if they specified a ']', they actually want something 1 greater than the condition
    if (equal_to)
    {
      Number * number = dynamic_cast <Number *> (user_cond);

      if (number)
        ++number->item_;
      else
      {
        // if it wasn't already a number, then it must be something more complex. We'll
        // just add one to it and see if the prune () method can optimize it a bit.
        Add * add = new Add (context.get_logger ());
        add->left_ = new Number (context.get_logger (),
          (madara::knowledge::KnowledgeRecord::Integer)1);
        add->right_ = user_cond;
        user_cond = add;
      }
    }

    delete precondition->right_;
    precondition->right_ = user_cond;
    precondition->add_precedence (accumulated_precedence);

    precedence_insert (context, precondition, list);

    lastValidInput = 0;
  }
}

// inserts a variable (leaf node / number) into the parse tree
void
madara::expression::Interpreter::variable_insert (
madara::knowledge::ThreadSafeContext &context,
const std::string &input,
std::string::size_type &i,
int & accumulated_precedence,
::std::list<Symbol *>& list,
Symbol *& lastValidInput)
{
  // build a potential variable name (this could also be a function)
  std::string::size_type j = 1;

  for (; i + j < input.length () && is_alphanumeric (input[i + j]); ++j)
    continue;

  // the variable or function name is stored in input.substr (i,j)
  // is the next char a parenthesis?

  std::string name = input.substr (i, j);

  i += j;

  // eat up whitespace so we can check for a parenthesis (function)
  for (; i < input.length () && is_whitespace (input[i]); ++i);

  // if this is a reserved word, then treat it as a Leaf
  if (is_reserved_word (name))
  {
    if (name == "nan")
    {
      madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
        "madara::expression::Interpreter: "
        "Inserting NAN into expression tree.\n");

      Number * number = new Number (context.get_logger (), NAN);
      number->add_precedence (accumulated_precedence);
      lastValidInput = number;

      precedence_insert (context, number, list);
    }
    else if (name == "inf")
    {
      madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
        "madara::expression::Interpreter: "
        "Inserting INFINITY into expression tree.\n");

      Number * number = new Number (context.get_logger (), INFINITY);
      number->add_precedence (accumulated_precedence);
      lastValidInput = number;

      precedence_insert (context, number, list);
    }
  }

  else if (i < input.length () && input[i] == '(')
  {
    // save the function name and update i
    Function * function = new Function (name, context);
    function->add_precedence (accumulated_precedence);

    bool handled = false;

    ::std::list<Symbol *> param_list;

    int local_precedence = 0;
    Symbol * local_last_valid = 0;

    ++i;

    // we have a function instead of a variable
    handle_parenthesis (context, input, i, local_last_valid, handled,
      local_precedence, param_list, true);

    //if (param_list.size () > 0)
    //  function->right_ = param_list.back ();

    function->nodes_.resize (param_list.size ());
    int cur = 0;

    for (::std::list<Symbol *>::iterator arg = param_list.begin ();
      arg != param_list.end (); ++arg, ++cur)
    {
      function->nodes_[cur] = (*arg)->build ();
    }

    //function->right_ = new List (context);

    precedence_insert (context, function, list);
    lastValidInput = 0;
  }
  else if (i < input.length () && input[i] == '[')
  {
    ++i;
    handle_for_loop (context, name, input, i, accumulated_precedence,
      list, lastValidInput);
  }
  else
  {
    Symbol * op = new Variable (name, context);
    op->add_precedence (accumulated_precedence);

    // check for post increments and decrements
    if (i + 1 < input.size ())
    {
      if (input[i] == '+' && input[i + 1] == '+')
      {
        Symbol * variable = op;
        op = new Postincrement (context.get_logger ());
        op->add_precedence (accumulated_precedence);
        op->right_ = variable;
        i += 2;
      }
      else if (input[i] == '-' && input[i + 1] == '-')
      {
        Symbol * variable = op;
        op = new Postdecrement (context.get_logger ());
        op->add_precedence (accumulated_precedence);
        op->right_ = variable;
        i += 2;
      }
    }

    lastValidInput = op;

    precedence_insert (context, op, list);
  }
}

// inserts a leaf node / number into the parse tree
void
madara::expression::Interpreter::string_insert (
  char opener,
  knowledge::ThreadSafeContext & context,
  const std::string &input,
  std::string::size_type &i, int & accumulated_precedence,
  ::std::list<madara::expression::Symbol *>& list,
  madara::expression::Symbol *& lastValidInput)
{
  std::string::size_type j = 0;
  Number * number = 0;

  for (; i + j < input.length (); ++j)
  {
    if (input[i + j] == opener && input[i + j - 1] != '\\')
      break;
  }

  number = new Number (context.get_logger (), input.substr (i, j));

  number->add_precedence (accumulated_precedence);

  lastValidInput = number;

  // update i to next char for main loop or handle parenthesis.

  i += j + 1;

  precedence_insert (context, number, list);
}

// inserts a variable (leaf node / number) into the parse tree
void
madara::expression::Interpreter::system_call_insert (
  madara::knowledge::ThreadSafeContext &context,
  const std::string &input,
  std::string::size_type &i,
  int & accumulated_precedence,
  ::std::list<Symbol *>& list,
  Symbol *& lastValidInput)
{
  // build a potential variable name (this could also be a function)
  std::string::size_type j = 1;

  for (; i + j < input.length () && is_alphanumeric (input[i + j]); ++j)
    continue;

  // the variable or function name is stored in input.substr (i,j)
  // is the next char a parenthesis?

  std::string name = input.substr (i, j);

  i += j;

  // eat up whitespace so we can check for a parenthesis (function)
  for (; i < input.length () && is_whitespace (input[i]); ++i);

  if (i < input.length () && input[i] == '(')
  {
    // save the function name and update i
    SystemCall * call = 0;
    char first_char = 0;

    if (name.size () > 1)
    {
      first_char = name[1];
    }

    switch (first_char)
    {
    case 'b':
      if (name == "#buffer")
      {
        call = new ToBuffer (context);
      }
      break;
    case 'c':
      if (name == "#clear_var" || name == "#clear_variable")
      {
        call = new ClearVariable (context);
      }
      else if (name == "#cos")
      {
        call = new Cos (context);
      }
      break;
    case 'd':
      if (name == "#delete_var" || name == "#delete_variable")
      {
        call = new DeleteVariable (context);
      }
      else if (name == "#double")
      {
        call = new ToDouble (context);
      }
      else if (name == "#doubles")
      {
        call = new ToDoubles (context);
      }
      break;
    case 'e':
      if (name == "#eval" || name == "#evaluate")
      {
        call = new Eval (context);
      }
      else if (name == "#expand" || name == "#expand_statement")
      {
        call = new ExpandStatement (context);
      }
      else if (name == "#expand_env" || name == "#expand_envs")
      {
        call = new ExpandEnv (context);
      }
      break;
    case 'f':
      if (name == "#fragment")
      {
        call = new Fragment (context);
      }
      else if (name == "#fixed")
      {
        call = new SetFixed (context);
      }
      break;
    case 'g':
      if (name == "#get_clock")
      {
        call = new GetClock (context);
      }
      else if (name == "#get_time" || name == "#get_time_ns" || name == "#get_time_nano")
      {
        call = new GetTime (context);
      }
      else if (name == "#get_time_seconds" || name == "#get_time_s")
      {
        call = new GetTimeSeconds (context);
      }
      break;
    case 'i':
      if (name == "#integer")
      {
        call = new ToInteger (context);
      }
      else if (name == "#integers")
      {
        call = new ToIntegers (context);
      }
      else if (name == "#isinf")
      {
        call = new Isinf (context);
      }
      break;
    case 'l':
      if (name == "#log_level")
      {
        call = new LogLevel (context);
      }
      break;
    case 'p':
      if (name == "#pow")
      {
        call = new Power (context);
      }
      else if (name == "#print")
      {
        call = new Print (context);
      }
      else if (name == "#print_system_calls" || name == "#print_system_call")
      {
        call = new PrintSystemCalls (context);
      }
      else if (name == "#precision")
      {
        call = new SetPrecision (context);
      }
      break;
    case 'r':
      if (name == "#rand_double")
      {
        call = new RandDouble (context);
      }
      else if (name == "#rand_int" || name == "#rand_integer")
      {
        call = new RandInt (context);
      }
      else if (name == "#read_file")
      {
        call = new ReadFile (context);
      }
      break;
    case 's':
      if (name == "#scientific")
      {
        call = new SetScientific (context);
      }
      else if (name == "#set_clock")
      {
        call = new SetClock (context);
      }
      else if (name == "#set_fixed")
      {
        call = new SetFixed (context);
      }
      else if (name == "#set_precision")
      {
        call = new SetPrecision (context);
      }
      else if (name == "#set_scientific")
      {
        call = new SetScientific (context);
      }
      else if (name == "#sin")
      {
        call = new Sin (context);
      }
      else if (name == "#size")
      {
        call = new Size (context);
      }
      else if (name == "#sleep")
      {
        call = new Sleep (context);
      }
      else if (name == "#sqrt")
      {
        call = new SquareRoot (context);
      }
      else if (name == "#string")
      {
        call = new ToString (context);
      }
      break;
    case 't':
      if (name == "#tan")
      {
        call = new Tan (context);
      }
      else if (name == "#to_buffer")
      {
        call = new ToBuffer (context);
      }
      else if (name == "#to_double")
      {
        call = new ToDouble (context);
      }
      else if (name == "#to_doubles")
      {
        call = new ToDoubles (context);
      }
      else if (name == "#to_host_dirs")
      {
        call = new ToHostDirs (context);
      }
      else if (name == "#to_integer")
      {
        call = new ToInteger (context);
      }
      else if (name == "#to_integers")
      {
        call = new ToIntegers (context);
      }
      else if (name == "#to_string")
      {
        call = new ToString (context);
      }
      else if (name == "#type")
      {
        call = new Type (context);
      }
      break;
    case 'w':
      if (name == "#write_file")
      {
        call = new WriteFile (context);
      }
      break;
    default:
      break;
    }

    if (!call)
    {
      madara_logger_log (context.get_logger (), logger::LOG_ERROR,
        "madara::expression::Interpreter: "
        "System call %s is unsupported in this version of MADARA, "
        "defaulting to print_system_calls help menu.\n", name.c_str ());

      throw KarlException ("madara::expression::Interpreter: "
        "System call %s does not exist.");
    }
    else
    {
      call->add_precedence (accumulated_precedence);
    }

    bool handled = false;

    ::std::list<Symbol *> param_list;

    ++i;

    // we have a function instead of a variable
    handle_parenthesis (context, input, i, lastValidInput, handled,
      accumulated_precedence, param_list, true);

    call->nodes_.resize (param_list.size ());
    int cur = 0;

    for (::std::list<Symbol *>::iterator arg = param_list.begin ();
      arg != param_list.end (); ++arg, ++cur)
    {
      call->nodes_[cur] = (*arg)->build ();
    }

    precedence_insert (context, call, list);
    lastValidInput = 0;
  }
  else
  {
    std::string message = 
      "madara::expression::Interpreter: "
      "KARL COMPILE ERROR: System call ";
    message += name;
    message += " does not have appropriate parentheses\n";
    
    madara_logger_log (context.get_logger (), logger::LOG_ERROR,
      message.c_str ());

    throw KarlException (message.c_str ()); 
  }
}

// inserts a leaf node / number into the parse tree
void
madara::expression::Interpreter::number_insert (
  knowledge::ThreadSafeContext & context,
  const std::string &input,
  std::string::size_type &i,
  int & accumulated_precedence,
  ::std::list<madara::expression::Symbol *>& list,
  madara::expression::Symbol *& lastValidInput)
{
  // merge all consecutive number chars into a single Number symbol,
  // eg '123' = int (123). Scope of j needs to be outside of the for
  // loop.

  std::string::size_type j = 1;
  Number * number = 0;

  for (; i + j <= input.length () && is_number (input[i + j]); ++j)
    continue;

  // do we have a float?
  if (i + j <= input.length () && input[i + j] == '.')
  {
    ++j;
    for (; i + j <= input.length () && is_number (input[i + j]); ++j)
      continue;

    // scientific notation
    if (i + j <= input.length () &&
      (input[i + j] == 'e' || input[i + j] == 'E'))
    {
      ++j;
      if (i + j <= input.length () &&
        (input[i + j] == '+' || input[i + j] == '-'))
      {
        ++j;
      }

      for (; i + j <= input.length () && is_number (input[i + j]); ++j)
        continue;
    }

    double new_number;

    std::stringstream buffer;
    buffer << input.substr (i, j);
    buffer >> new_number;

    number = new Number (context.get_logger (), new_number);
  }
  else
  {
    // we have an integer

    madara::knowledge::KnowledgeRecord::Integer new_number;

    std::stringstream buffer;
    buffer << input.substr (i, j);
    buffer >> new_number;

    number = new Number (context.get_logger (), new_number);
  }

  number->add_precedence (accumulated_precedence);

  lastValidInput = number;

  // update i to next char for main loop or handle parenthesis.

  i += j;

  precedence_insert (context, number, list);
}


// inserts a multiplication or division into the parse tree
void
madara::expression::Interpreter::precedence_insert (
knowledge::ThreadSafeContext & context,
madara::expression::Symbol *op,
::std::list<madara::expression::Symbol *>& list)
{
  if (!list.empty ())
  {
    // if last element was a number, then make that our left_

    Symbol *parent = list.back ();
    Symbol *child = 0;
    Symbol *grandparent = 0;

    // check to see if op is an assignment or implication, which are treated
    // uniquely
    Assignment * op_assignment = dynamic_cast <Assignment *> (op);
    Implies * op_implies = dynamic_cast <Implies *> (op);
    UnaryOperator * op_unary = dynamic_cast <UnaryOperator *> (op);

    // move down the right side of the tree until we find either a null or
    // a precedence that is >= this operation's precedence. This puts us
    // in the situation that we know our op should be performed after child
    // or child should be null (assignment or implication not withstanding)
    for (child = parent->right_;
      child && child->precedence () < op->precedence ();
      child = child->right_)
    {
      grandparent = parent;
      parent = child;
    }

    // child casts
    Operator * child_operator = dynamic_cast <Operator *> (child);
    Operator * child_ternary = dynamic_cast <TernaryOperator *> (child);

    if (child && (child_operator == 0 && child_ternary == 0))
    {
      madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
          "Interpreter::precedence_insert: "
          "Level 1: child is neither an operator or ternary\n");
    }

    // parent->precedence is < op->precedence at this point

    if (op_assignment || op_implies || op_unary)
    {
      // if we are an assignment, implies, or unary op, we actually
      // need this op to have slightly higher precedence (it needs to be
      // evaluated first). This situation is signified by something like this
      // var1 = var2 = var3 = 1. In the list, will be var1 = var2 = var3, so parent will be
      // and assignment, and parent left will be var1, right and child will be assignment
      // and it will have a left of var2


      for (child = parent->right_;
        child && child->precedence () <= op->precedence ();
        child = child->right_)
      {
        grandparent = parent;
        parent = child;
      }

      // child casts
      child_operator = dynamic_cast <Operator *> (child);
      child_ternary = dynamic_cast <TernaryOperator *> (child);

      if (child && (child_operator == 0 && child_ternary == 0))
      {
        madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
          "Interpreter::precedence_insert: "
          "Level 2: child is neither an operator or ternary\n");
      }
    }

    // Now that we have our parent and child setup appropriately according
    // to precedence relationships, we should be able to modify or replace
    // the tree in the list

    if (parent->precedence () < op->precedence () ||
      (parent->precedence () == op->precedence () &&
      (op_assignment || op_implies || op_unary)))
    {
      // this op needs to be evaluated before the parent

      // first let's do some error checking, so we can give helpful compile errors
      Operator * parent_binary = dynamic_cast <Operator *> (parent);
      UnaryOperator * parent_unary = dynamic_cast <UnaryOperator *> (parent);

      // if the parent is a binary (like addition or &&), then it needs a left hand side
      // if it doesn't have this, let's report a warning and insert a Leaf Node with a value
      // of zero
      if (parent_binary && !parent->left_)
      {
        // try to give as specific message as possible
        Both * parent_both = dynamic_cast <Both *> (parent);
        if (parent_both)
        {
          madara_logger_log (context.get_logger (), logger::LOG_WARNING,
            "KARL COMPILE WARNING: Empty statements between ';' may"
            " cause slower execution, attempting to prune away the extra "
            "statement\n");
        }
        else
        {
          madara_logger_log (context.get_logger (), logger::LOG_WARNING,
            "madara::expression::Interpreter: "
            "KARL COMPILE WARNING: Binary operation has no left child. "
            "Inserting a zero\n");

          parent->left_ = new Number (context.get_logger (),
            (madara::knowledge::KnowledgeRecord::Integer)0);
        }
      }

      // if the parent is a unary operator (like negate or not), then it should
      // NOT have a left child. This would only happen if someone input
      // something like 5 ! 3, which has no meaning. This is a compile error.
      if (parent_unary && parent->left_)
      {
        madara_logger_log (context.get_logger (), logger::LOG_ERROR,
          "madara::expression::Interpreter: "
          "KARL COMPILE ERROR: Unary operation shouldn't have a left child\n");

        throw KarlException ("madara::expression::Interpreter: "
          "KARL COMPILE ERROR: "
          "Parent unary node has a left child, shouldn't be possible\n"); 
      }

      // if we've gotten to this point, then we need to
      // replace the child with ourself in the tree
      if (child)
      {
        if (op_unary)
        {
          // This is a compile error. Unary cannot have a left
          // child, and that is the only way that being at this
          // point would make sense.
          madara_logger_log (context.get_logger (), logger::LOG_ERROR,
            "madara::expression::Interpreter: "
            "KARL COMPILE ERROR: unary operation shouldn't have a left child\n");

          throw KarlException ("madara::expression::Interpreter: "
            "KARL COMPILE ERROR: "
            "Unary node has a left child, shouldn't be possible\n"); 
        }
        else
        {
          madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
            "Interpreter::precedence_insert: "
            "Level 3: op is setting its left to the existing child\n");

          op->left_ = child;
        }
      }

      madara_logger_log (context.get_logger (), logger::LOG_DETAILED,
        "Interpreter::precedence_insert: "
        "Level 3: parent->right is being set to current operator\n");

      parent->right_ = op;
    }
    else
    {
      // we are a lower precedence than our parent, so we need to replace
      // the tree in the list. This typically happens with assignment, implies,
      // logical operations (&&, ||) and equality checks

      op->left_ = parent;

      if (grandparent)
        grandparent->right_ = op;
      else
      {
        list.pop_back ();
        list.push_back (op);
      }
    }
  }
  else
  {
    list.push_back (op);
  }
}

void
madara::expression::Interpreter::main_loop (
madara::knowledge::ThreadSafeContext & context,
const std::string &input, std::string::size_type &i,
madara::expression::Symbol *& lastValidInput,
bool & handled, int & accumulated_precedence,
::std::list<madara::expression::Symbol *>& list,
bool build_argument_list)
{
  handled = false;
  if (is_number (input[i]))
  {
    handled = true;
    // leaf node
    number_insert (context, input, i, accumulated_precedence,
      list, lastValidInput);
  }
  else if (is_alphanumeric (input[i]))
  {
    handled = true;
    // variable leaf node
    variable_insert (context, input, i, accumulated_precedence,
      list, lastValidInput);
  }
  else if (is_string_literal (input[i]))
  {
    char opener = input[i];
    ++i;
    handled = true;
    // string
    string_insert (opener, context, input, i, accumulated_precedence,
      list, lastValidInput);
  }
  else if (i < input.length () && input[i] == '[')
  {
    // save the function name and update i
    ConstArray * object = new ConstArray (context);
    object->add_precedence (accumulated_precedence);

    bool handled = false;

    ::std::list<Symbol *> param_list;

    int local_precedence = 0;
    Symbol * local_last_valid = 0;

    ++i;

    // we have an array
    handle_array (context, input, i, local_last_valid, handled,
      local_precedence, param_list);

    object->nodes_.resize (param_list.size ());
    int cur = 0;

    for (::std::list<Symbol *>::iterator arg = param_list.begin ();
      arg != param_list.end (); ++arg, ++cur)
    {
      object->nodes_[cur] = (*arg)->build ();
    }

    precedence_insert (context, object, list);
    lastValidInput = 0;
  }
  else if (input[i] == '#')
  {
    handled = true;
    // variable leaf node
    system_call_insert (context, input, i, accumulated_precedence,
      list, lastValidInput);
  }
  else if (input[i] == '+')
  {
    handled = true;
    Symbol * op = 0;

    // is this a predecrement?
    if (i + 1 < input.size () && input[i + 1] == '+')
    {
      op = new Preincrement (context.get_logger ());
      ++i;
    }
    // is this an atomic increment?
    else if (i + 1 < input.size () && input[i + 1] == '=')
    {
      Variable * var = dynamic_cast <Variable *> (lastValidInput);
      ArrayRef * array_ref = dynamic_cast <ArrayRef *> (lastValidInput);
      if (var || array_ref)
      {
        op = new VariableIncrement (lastValidInput,
        madara::knowledge::KnowledgeRecord (), 0, context);
      }
      else
      {
        madara_logger_log (context.get_logger (), logger::LOG_ERROR,
          "madara::expression::Interpreter: "
          "KARL COMPILE ERROR (+=): "
          "Assignments must have a variable left hand side.\n");

        throw KarlException ("madara::expression::Interpreter: "
          "KARL COMPILE ERROR (+=): "
          "Assignments must have a variable left hand side");
      }
      ++i;
    }
    else
      op = new Add (context.get_logger ());

    // insert the op according to left-to-right relationships
    op->add_precedence (accumulated_precedence);
    lastValidInput = 0;
    precedence_insert (context, op, list);
    ++i;
  }
  else if (input[i] == '-')
  {
    handled = true;
    Symbol * op = 0;

    // is this a predecrement?
    if (i + 1 < input.size () && input[i + 1] == '-')
    {
      op = new Predecrement (context.get_logger ());
      ++i;
    }
    // is this an atomic decrement?
    else if (i + 1 < input.size () && input[i + 1] == '=')
    {
      Variable * var = dynamic_cast <Variable *> (lastValidInput);
      ArrayRef * array_ref = dynamic_cast <ArrayRef *> (lastValidInput);
      if (var || array_ref)
      {
        op = new VariableDecrement (lastValidInput, madara::knowledge::KnowledgeRecord (),
          0, context);
      }
      else
      {
        madara_logger_log (context.get_logger (), logger::LOG_ERROR,
          "madara::expression::Interpreter: "
          "KARL COMPILE ERROR (-=): "
          "Assignments must have a variable left hand side.\n");

        throw KarlException ("madara::expression::Interpreter: "
          "KARL COMPILE ERROR (-=): "
          "Assignments must have a variable left hand side");
      }
      ++i;
    }
    // is this a number literal? Handling this way allows for INT64_MIN
    else if (i + 1 < input.size () && is_number (input[i + 1]))
    {
      handled = true;
      // leaf node
      number_insert (context, input, i, accumulated_precedence,
        list, lastValidInput);
    }
    // Negate
    else if (!lastValidInput)
      op = new Negate (context.get_logger ());
    // Subtract
    else
      op = new Subtract (context.get_logger ());

    if (op)
    {
      // insert the op according to left-to-right relationships
      lastValidInput = 0;
      op->add_precedence (accumulated_precedence);
      precedence_insert (context, op, list);
      ++i;
    }
  }
  else if (input[i] == '*')
  {
    handled = true;
    Symbol * op = 0;

    // is this an atomic multiply?
    if (i + 1 < input.size () && input[i + 1] == '=')
    {
      Variable * var = dynamic_cast <Variable *> (lastValidInput);
      ArrayRef * array_ref = dynamic_cast <ArrayRef *> (lastValidInput);
      if (var || array_ref)
      {
        op = new VariableMultiply (lastValidInput, madara::knowledge::KnowledgeRecord (),
        0, context);
      }
      else
      {
        madara_logger_log (context.get_logger (), logger::LOG_ERROR,
          "madara::expression::Interpreter: "
          "KARL COMPILE ERROR (*=): "
          "Assignments must have a variable left hand side.\n");

        throw KarlException ("madara::expression::Interpreter: "
          "KARL COMPILE ERROR (*=): "
          "Assignments must have a variable left hand side");
      }
      ++i;
    }
    // multiplication operation
    else
      op = new Multiply (context.get_logger ());

    // insert the op according to precedence relationships
    op->add_precedence (accumulated_precedence);
    lastValidInput = 0;
    precedence_insert (context, op, list);
    ++i;
  }
  else if (input[i] == '%')
  {
    // multiplication operation
    handled = true;
    Modulus *op = new Modulus (context.get_logger ());

    // insert the op according to precedence relationships
    op->add_precedence (accumulated_precedence);
    lastValidInput = 0;
    precedence_insert (context, op, list);
    ++i;
  }
  else if (input[i] == '/')
  {
    // is this a one line comment?
    if (i + 1 < input.size () && input[i + 1] == '/')
    {
      // we have a one line comment
      for (; i < input.size () && input[i] != '\n'; ++i);
    }
    // is this a multi-line comment?
    else if (i + 1 < input.size () && input[i + 1] == '*')
    {
      // find the matching close
      std::string::size_type found = input.find ("*/", i + 1);

      // if we were able to find the matching close,
      // then set i to the '/' in '*/'
      if (found != std::string::npos)
        i = found + 1;

      // otherwise, the user apparently wanted to
      // comment out the rest of the file
      else
        i = input.size ();
    }
    // is this an atomic decrement?
    else
    {
      handled = true;
      // division operation
      Symbol * op = 0;

      // atomic division?
      if (i + 1 < input.size () && input[i + 1] == '=')
      {
        Variable * var = dynamic_cast <Variable *> (lastValidInput);
        ArrayRef * array_ref = dynamic_cast <ArrayRef *> (lastValidInput);
        if (var || array_ref)
        {
          op = new VariableDivide (lastValidInput, madara::knowledge::KnowledgeRecord (), 0, context);
        }
        else
        {
          madara_logger_log (context.get_logger (), logger::LOG_WARNING,
            "madara::expression::Interpreter: "
            "KARL COMPILE ERROR (/=): "
            "Assignments must have a variable left hand side.\n");

          throw KarlException ("madara::expression::Interpreter: "
            "KARL COMPILE ERROR (/=): "
            "Assignments must have a variable left hand side");
        }
        ++i;
      }
      else
        op = new Divide (context.get_logger ());

      op->add_precedence (accumulated_precedence);
      lastValidInput = 0;
      precedence_insert (context, op, list);
    }
    ++i;
  }
  else if (input[i] == '=')
  {
    handled = true;
    Symbol * op = 0;

    // is this an equality?
    if (i + 1 < input.size () && input[i + 1] == '=')
    {
      op = new Equality (context.get_logger ());
      op->add_precedence (accumulated_precedence);

      lastValidInput = 0;
      ++i;

      // insert the op according to precedence relationships
      precedence_insert (context, op, list);
    }
    // is this an implication?
    else if (i + 1 < input.size () && input[i + 1] == '>')
    {
      op = new Implies (context.get_logger ());
      op->add_precedence (accumulated_precedence);

      lastValidInput = 0;
      ++i;

      // insert the op according to precedence relationships
      precedence_insert (context, op, list);
    }
    // must be an assignment then
    else
    {
      op = new Assignment (context.get_logger ());
      op->add_precedence (accumulated_precedence);

      lastValidInput = 0;

      // insert the op according to precedence relationships
      // assignment_insert (op, list);
      precedence_insert (context, op, list);
    }
    ++i;
  }
  else if (input[i] == '!')
  {
    handled = true;
    Symbol * op = 0;

    // is this an inequality?
    if (i + 1 < input.size () && input[i + 1] == '=')
    {
      op = new Inequality (context.get_logger ());
      ++i;
    }
    // must be a logical not then
    else
    {
      op = new Not (context.get_logger ());
    }

    // insert the op according to precedence relationships
    op->add_precedence (accumulated_precedence);
    lastValidInput = 0;
    precedence_insert (context, op, list);
    ++i;
  }
  // square root is ASCII 251 (UTF 8 format)
  else if ((uint8_t)input[i] == 251)
  {
    handled = true;
    Symbol * op = 0;

    op = new SquareRootUnary (context.get_logger ());

    // insert the op according to precedence relationships
    op->add_precedence (accumulated_precedence);
    lastValidInput = 0;
    precedence_insert (context, op, list);
    ++i;
  }
  else if (input[i] == '&')
  {
    // is this a logical and?
    if (i + 1 < input.size () && input[i + 1] == '&')
    {
      handled = true;
      Symbol * op = new And (context.get_logger ());
      ++i;

      // insert the op according to precedence relationships
      op->add_precedence (accumulated_precedence);
      lastValidInput = 0;
      precedence_insert (context, op, list);
    }
    else
    {
      // error. We currently don't allow logical and (A & B)
      madara_logger_log (context.get_logger (), logger::LOG_ERROR,
        "madara::expression::Interpreter: "
        "KARL COMPILE ERROR: "
        "Logical And (&) not available. " \
        "You may want to use && instead in %s.\n", input.c_str ());

      throw KarlException ("madara::expression::Interpreter: "
        "KARL COMPILE ERROR: "
        "Logical And (&) not available.");
    }
    ++i;
  }
  else if (input[i] == '|')
  {
    // is this a logical and?
    if (i + 1 < input.size () && input[i + 1] == '|')
    {
      handled = true;
      Symbol * op = new Or (context.get_logger ());
      ++i;

      // insert the op according to precedence relationships
      op->add_precedence (accumulated_precedence);
      lastValidInput = 0;
      precedence_insert (context, op, list);
    }
    else
    {
      // error. We don't currently support logical or
      madara_logger_log (context.get_logger (), logger::LOG_ERROR,
        "KARL COMPILE ERROR: "
        "Logical Or (|) not available. " \
        "You may want to use || instead in %s.\n", input.c_str ());

      throw KarlException ("madara::expression::Interpreter: "
        "KARL COMPILE ERROR: "
        "Logical Or (|) not available.");
    }
    ++i;
  }
  else if (input[i] == ';')
  {
    handled = true;
    Symbol * op = 0;

    // is this a logical and?
    if (i + 1 < input.size () && input[i + 1] == '>')
    {
      op = new ReturnRight (context.get_logger ());
      ++i;
    }
    else
    {
      op = new Both (context.get_logger ());
    }

    // insert the op according to precedence relationships
    op->add_precedence (accumulated_precedence);
    lastValidInput = 0;
    precedence_insert (context, op, list);
    ++i;
  }
  else if (input[i] == ',')
  {
    if (build_argument_list)
      return;

    handled = true;
    Symbol * op = new Sequence (context.get_logger ());

    // insert the op according to precedence relationships
    op->add_precedence (accumulated_precedence);
    lastValidInput = 0;
    precedence_insert (context, op, list);
    ++i;
  }
  else if (input[i] == '<')
  {
    handled = true;
    Symbol * op = 0;

    // is this a less than or equal to operator?
    if (i + 1 < input.size () && input[i + 1] == '=')
    {
      op = new LessThanEqual (context.get_logger ());
      ++i;
    }
    // must be a less than operator
    else
      op = new LessThan (context.get_logger ());

    // insert the op according to precedence relationships
    op->add_precedence (accumulated_precedence);
    lastValidInput = 0;
    precedence_insert (context, op, list);
    ++i;
  }
  else if (input[i] == '>')
  {
    handled = true;
    Symbol * op = 0;

    // is this a less than or equal to operator?
    if (i + 1 < input.size () && input[i + 1] == '=')
    {
      op = new GreaterThanEqual (context.get_logger ());
      ++i;
    }
    // must be a less than operator
    else
      op = new GreaterThan (context.get_logger ());

    // insert the op according to precedence relationships
    op->add_precedence (accumulated_precedence);
    lastValidInput = 0;
    precedence_insert (context, op, list);
    ++i;
  }
  else if (input[i] == '(')
  {
    handled = true;
    ++i;
    handle_parenthesis (context, input, i, lastValidInput,
      handled, accumulated_precedence, list);
  }
  else if (input[i] == '\t' || input[i] == ' '
    || input[i] == '\r' || input[i] == '\n')
  {
    handled = true;
    ++i;
    // skip whitespace
  }
}


void
madara::expression::Interpreter::handle_array (
madara::knowledge::ThreadSafeContext & context,
const std::string &input, std::string::size_type &i,
madara::expression::Symbol *& lastValidInput,
bool & handled, int & accumulated_precedence,
::std::list<madara::expression::Symbol *>& master_list)
{
  /* handle parenthesis is a lot like handling a new interpret.
  the difference is that we have to worry about how the calling
  function has its list setup */

  accumulated_precedence += PARENTHESIS_PRECEDENCE;
  int initial_precedence = accumulated_precedence;

  ::std::list<Symbol *> list;

  handled = false;
  bool closed = false;
  while (i < input.length ())
  {
    main_loop (context, input, i, lastValidInput,
      handled, accumulated_precedence, list, true);

    if (input[i] == ']')
    {
      handled = true;
      closed = true;
      ++i;
      accumulated_precedence -= PARENTHESIS_PRECEDENCE;
      break;
    }
    else if (input[i] == ',')
    {
      ++i;
      while (list.size ())
      {
        master_list.push_back (list.back ());
        list.pop_back ();
      }
      accumulated_precedence = initial_precedence;
    }
    else if (i == input.length () - 1)
    {
      break;
    }
  }

  if (!closed)
  {
    madara_logger_log (context.get_logger (), logger::LOG_ERROR,
      "madara::expression::Interpreter: "
      "KARL COMPILE ERROR: "
      "Forgot to close parenthesis in %s.\n", input.c_str ());
  }

  if (list.size () > 0)
  {
    if (list.size () > 1)
    {
      madara_logger_log (context.get_logger (), logger::LOG_ERROR,
        "madara::expression::Interpreter: "
        "KARL COMPILE ERROR: "
        "A parenthesis was closed, leaving multiple list items (there should "
        "be a max of 1) in %s.\n", input.c_str ());
    }

    while (list.size ())
    {
      master_list.push_back (list.back ());
      list.pop_back ();
    }
  }

  list.clear ();
}



void
madara::expression::Interpreter::handle_parenthesis (
  madara::knowledge::ThreadSafeContext & context,
  const std::string &input, std::string::size_type &i,
  madara::expression::Symbol *& lastValidInput,
  bool & handled, int & accumulated_precedence,
  ::std::list<madara::expression::Symbol *>& master_list,
  bool build_argument_list)
{
  /* handle parenthesis is a lot like handling a new interpret.
  the difference is that we have to worry about how the calling
  function has its list setup */

  accumulated_precedence += PARENTHESIS_PRECEDENCE;
  int initial_precedence = accumulated_precedence;

  ::std::list<Symbol *> list;

  handled = false;
  bool closed = false;
  while (i < input.length ())
  {
    main_loop (context, input, i, lastValidInput,
      handled, accumulated_precedence, list, build_argument_list);

    if (input[i] == ')')
    {
      handled = true;
      closed = true;
      ++i;
      accumulated_precedence -= PARENTHESIS_PRECEDENCE;
      break;
    }
    else if (build_argument_list && input[i] == ',')
    {
      ++i;
      while (list.size ())
      {
        master_list.push_back (list.back ());
        list.pop_back ();
      }
      accumulated_precedence = initial_precedence;
    }
    else if (i == input.length () - 1)
    {
      break;
    }
  }

  if (!build_argument_list && !closed)
  {
    madara_logger_log (context.get_logger (), logger::LOG_ERROR,
      "madara::expression::Interpreter: "
      "KARL COMPILE ERROR: "
      "Forgot to close parenthesis in %s.\n", input.c_str ());
  }

  if (!build_argument_list && master_list.size () > 0 && list.size () > 0)
  {
    Symbol * lastSymbol = master_list.back ();
    Operator * op = dynamic_cast <Operator *> (lastSymbol);
    UnaryOperator * unary = dynamic_cast < UnaryOperator * >
      (lastSymbol);


    // is it a node with 2 children?
    if (op || unary)
    {
      precedence_insert (context, list.back (), master_list);
    }
    else
    {
      // is it a terminal node (Number)
      // error
    }
  }
  else if (list.size () > 0)
  {
    if (list.size () > 1)
    {
      madara_logger_log (context.get_logger (), logger::LOG_ERROR,
        "madara::expression::Interpreter: "
        "KARL COMPILE ERROR: "
        "A parenthesis was closed, leaving multiple list items (there should "
        "be a max of 1) in %s.\n", input.c_str ());
    }

    while (list.size ())
    {
      master_list.push_back (list.back ());
      list.pop_back ();
    }
  }

  list.clear ();
}

// converts a string and context into a parse tree, and builds an
// expression tree out of the parse tree
madara::expression::ExpressionTree
madara::expression::Interpreter::interpret (
  knowledge::ThreadSafeContext &context,
  const std::string &input)
{
  // return the cached expression tree if it exists
  ExpressionTreeMap::const_iterator found = cache_.find (input);
  if (found != cache_.end ())
    return found->second;

  ::std::list<Symbol *> list;
  //list.clear ();
  Symbol * lastValidInput = 0;
  bool handled = false;
  int accumulated_precedence = 0;
  std::string::size_type last_i = 0;

  for (std::string::size_type i = 0;
    i < input.length ();)
  {
    // we took out the loop update from the for loop
    // and the main_loop or handle_parenthesis call
    // should now take care of this.
    main_loop (context, input, i, lastValidInput,
      handled, accumulated_precedence, list);

    if (i == last_i)
    {
      size_t start = i > 10 ? i - 10 : 0;
      size_t end = input.size () - i > 10 ? i + 10 : input.size ();

      std::string snippet = input.substr (start, end - start);

      {
        madara_logger_log (context.get_logger (), logger::LOG_MINOR,
          "madara::expression::Interpreter: "
          "KARL: "
          "Compilation is spinning at %d in %s. Char is %c. Breaking out.\n",
          (int)i, snippet.c_str (), input[i]);
      }
      break;
    }
    else
      last_i = i;

    // store last valid input symbol. this is useful to the '-' operator
    // and will help us determine if '-' is a subtraction or a negation
    //if (input[i] != ' ' && input[i] != '\n')
    // lastValidInput = input[i];
  }

  // if the list has an element in it, then return the back of the list.
  if (!list.empty ())
  {
    // Invoke a recursive ExpressionTree build starting with the root
    // symbol. This is an example of the builder pattern. See pg 97
    // in GoF book.

    ExpressionTree tree (context.get_logger (),
      list.back ()->build (), false);

    // optimize the tree
    tree.prune ();
    delete list.back ();

    // store this optimized tree into cached memory
    cache_[input] = tree;

    return tree;
  }

  // If we reach this, we didn't have any symbols.
  return ExpressionTree (context.get_logger ());
}

#endif // _MADARA_NO_KARL_

#endif // _INTERPRETER_CPP_
