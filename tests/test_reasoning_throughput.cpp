
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <iomanip>

#include "madara/knowledge/CompiledExpression.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "madara/knowledge/containers/Integer.h"
#include "madara/knowledge/containers/IntegerStaged.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/Utility.h"
#include "madara/utility/Timer.h"

#include <mutex>
#include <atomic>

namespace logger = madara::logger;

typedef madara::knowledge::KnowledgeRecord::Integer Integer;
typedef std::chrono::steady_clock Clock;

// command line arguments
void handle_arguments(int argc, char* argv[]);

// test functions
uint64_t test_virtual_reinforcement(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_volatile_reinforcement(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_volatile_assignment(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_volatile_inference(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_optimal_assignment(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_optimal_reinforcement(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_optimal_inference(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_simple_reinforcement(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_simple_inference(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_large_reinforcement(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_large_inference(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_container_assignment(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_container_increment(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_staged_container_assignment(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_staged_container_increment(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);

uint64_t test_compiled_sr(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_compiled_si(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_compiled_lr(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_compiled_li(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);

uint64_t test_compiled_sa(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_compiled_la(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);

uint64_t test_compiled_sfi(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_compiled_lfi(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);

uint64_t test_extern_call(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);

uint64_t test_looped_sr(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_looped_si(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_optimal_loop(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_looped_li(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_normal_set(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_var_ref_set(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_get_ref(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_get_expand_ref(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_variables_inc_var_ref(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);

uint64_t test_stl_inc_atomic(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_stl_inc_recursive(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);
uint64_t test_stl_inc_mutex(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations);

// C++ function for increment with boolean check, rather than allowing C++
// to optimize by putting things in registers
long increment(bool check, long value);

// C++ function for increment, rather than allowing C++ to optimize by putting
// things in registers. I would make this inline, but I have a feeling that
// the compiler would further optimize the increment into a register after
// inlining, which is precisely what I am trying to avoid.
long increment(long value);

// default iterations
uint32_t num_iterations = 100000;
uint32_t num_runs = 10;
bool conditional = true;
uint32_t step = 1;

// still trying to stop this darn thing from optimizing the increments
class Incrementer
{
public:
  Incrementer() : value_(0) {}

  long increment()
  {
    return ++value_;
  }

  long increment(bool check)
  {
    return check ? ++value_ : value_;
  }

  long value()
  {
    return value_;
  }

private:
  volatile long value_;
};

class BaseOperation
{
public:
  virtual ~BaseOperation() {}

  virtual madara::knowledge::KnowledgeRecord::Integer evaluate(void)
  {
    return value_;
  }

  madara::knowledge::KnowledgeRecord::Integer get_value(void)
  {
    return value_;
  }

protected:
  madara::knowledge::KnowledgeRecord::Integer value_;
};

class IncrementOperation : public BaseOperation
{
  virtual ~IncrementOperation() {}

  virtual madara::knowledge::KnowledgeRecord::Integer evaluate(void)
  {
    return ++value_;
  }
};

void print(uint64_t time, madara::knowledge::KnowledgeRecord value,
    uint32_t iterations, const std::string& type)
{
  std::stringstream buffer;

  std::locale loc("C");
  buffer.imbue(loc);

  buffer << type;
  buffer << "\n  ops=";
  buffer << iterations;
  buffer << ", time=";
  buffer << time;
  buffer << " ns, value=";
  buffer << value.to_integer();
  buffer << "\n";

  madara_logger_ptr_log(
      logger::global_logger.get(), logger::LOG_ALWAYS, buffer.str().c_str());
}

#ifndef _MADARA_NO_KARL_
madara::knowledge::CompiledExpression increment_ce;
#endif  // _MADARA_NO_KARL_
madara::knowledge::VariableReference increment_var;

madara::knowledge::KnowledgeRecord increment_var1(
    madara::knowledge::FunctionArguments&,
    madara::knowledge::Variables& variables)
{
#ifndef _MADARA_NO_KARL_
  return variables.evaluate(
      increment_ce, madara::knowledge::KnowledgeUpdateSettings(false, false));
#else
  return Integer(0);
#endif  // _MADARA_NO_KARL_
}

madara::knowledge::KnowledgeRecord increment_var1_through_variables(
    madara::knowledge::FunctionArguments&,
    madara::knowledge::Variables& variables)
{
  return variables.inc(increment_var);
}

madara::knowledge::KnowledgeRecord no_op(
    madara::knowledge::FunctionArguments&, madara::knowledge::Variables&)
{
  return madara::knowledge::KnowledgeRecord(0);
}

std::string to_legible_hertz(uint64_t hertz)
{
  std::stringstream buffer;

  std::locale loc("C");
  buffer.imbue(loc);

  const int ghz_mark = 1000000000;
  const int mhz_mark = 1000000;
  const int khz_mark = 1000;

  double freq = (double)hertz / ghz_mark;

  if(freq >= 1)
  {
    buffer << std::setprecision(2) << std::fixed;
    buffer << freq;
    buffer << " ghz";
    return buffer.str().c_str();
  }

  freq = (double)hertz / mhz_mark;

  if(freq >= 1)
  {
    buffer << std::setprecision(2) << std::fixed;
    buffer << freq;
    buffer << " mhz";
    return buffer.str().c_str();
  }

  freq = (double)hertz / khz_mark;

  if(freq >= 1)
  {
    buffer << std::setprecision(2) << std::fixed;
    buffer << freq;
    buffer << " khz";
    return buffer.str().c_str();
  }

  freq = (double)hertz;

  buffer << std::setprecision(2) << std::fixed;
  buffer << freq;
  buffer << "  hz";
  return buffer.str().c_str();
}

int main(int argc, char* argv[])
{
  handle_arguments(argc, argv);

  // set thread priority to max
  madara::utility::set_thread_priority();

  madara::knowledge::KnowledgeBase knowledge;

#ifndef _MADARA_NO_KARL_
  increment_ce = knowledge.compile("++.var1");
#endif

  increment_var = knowledge.get_ref(".var1");

  if(num_runs == 0 || num_iterations == 0)
  {
    madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
        "\nERROR: num_runs (%d) and num_iterations (%d) cannot be set to 0:\n",
        num_runs, num_iterations);

    exit(-1);
  }

  const int num_test_types = 36;

  // make everything all pretty and for-loopy
  uint64_t results[num_test_types];
  uint64_t averages[num_test_types];
  uint64_t (*test_functions[num_test_types])(
      madara::knowledge::KnowledgeBase & knowledge, uint32_t iterations);
  const char* printouts[num_test_types] = {"KaRL: Simple Increments           ",
      "KaRL: Multiple Increments         ",
      "KaRL: Simple Ternary Increments   ",
      "KaRL: Multiple Ternary Increments ",
      "KaRL: Compiled Simple Increments  ",
      "KaRL: Compiled Multiple Inc       ",
      "KaRL: Compiled Simple Tern Inc    ",
      "KaRL: Compiled Multiple Tern Inc  ",
      "KaRL: Compiled Single Assign      ",
      "KaRL: Compiled Multiple Assign    ",
      "KaRL: Extern Function Call        ",
      "KaRL: Compiled Extern Inc Func    ",
      "KaRL: Compiled Extern Multi Calls ",
      "KaRL: Looped Simple Increments    ",
      "KaRL: Optimized Loop              ",
      "KaRL: Looped Simple Ternary Inc   ",
      "KaRL: Looped Multiple Ternary Inc ",
      "KaRL: Get Variable Reference      ",
      "KaRL: Get Expanded Reference      ",
      "KaRL: Normal Set Operation        ",
      "KaRL: Variable Reference Set      ",
      "KaRL: Variables Inc Var Ref       ",
      "KaRL container: Assignment        ",
      "KaRL container: Increments        ",
      "KaRL staged container: Assignment ",
      "KaRL staged container: Increments ",
      "C++: Optimized Assignments        ",
      "C++: Optimized Increments         ",
      "C++: Optimized Ternary Increments ",
      "C++: Virtual Increments           ",
      "C++: Volatile Assignments         ",
      "C++: Volatile Increments          ",
      "C++: Volatile Ternary Increments  ",
      "C++: STL Atomic Increments        ",
      "C++: STL Recursive Increments     ",
      "C++: STL Mutex Increments         "};

  // enums for referencing
  enum
  {
    SimpleReinforcement,
    LargeReinforcement,
    SimpleInference,
    LargeInference,
    CompiledSR,
    CompiledLR,
    CompiledSI,
    CompiledLI,
    CompiledSA,
    CompiledLA,
    ExternCall,
    CompiledSFI,
    CompiledLFI,
    LoopedSR,
    OptimalLoop,
    LoopedSI,
    LoopedLI,
    GetVariableReference,
    GetExpandedReference,
    NormalSet,
    VariableReferenceSet,
    VariablesIncVarRef,
    ContainerAssignment,
    ContainerIncrement,
    StagedContainerAssignment,
    StagedContainerIncrement,
    OptimizedAssignment,
    OptimizedReinforcement,
    OptimizedInference,
    VirtualReinforcement,
    VolatileAssignment,
    VolatileReinforcement,
    VolatileInference,
    StlAtomicIncrement,
    StlRecursiveIncrement,
    StlMutexIncrement
  };

  // start from zero
  memset((void*)results, 0, sizeof(uint64_t) * num_test_types);
  memset((void*)averages, 0, sizeof(uint64_t) * num_test_types);

  test_functions[SimpleReinforcement] = test_simple_reinforcement;
  test_functions[LargeReinforcement] = test_large_reinforcement;
  test_functions[SimpleInference] = test_simple_inference;
  test_functions[LargeInference] = test_large_inference;

  test_functions[CompiledSR] = test_compiled_sr;
  test_functions[CompiledLR] = test_compiled_lr;
  test_functions[CompiledSI] = test_compiled_si;
  test_functions[CompiledLI] = test_compiled_li;
  test_functions[CompiledSA] = test_compiled_sa;
  test_functions[CompiledLA] = test_compiled_la;

  test_functions[ExternCall] = test_extern_call;
  test_functions[CompiledSFI] = test_compiled_sfi;
  test_functions[CompiledLFI] = test_compiled_lfi;

  test_functions[LoopedSR] = test_looped_sr;
  test_functions[OptimalLoop] = test_optimal_loop;
  test_functions[LoopedSI] = test_looped_si;
  test_functions[LoopedLI] = test_looped_li;

  test_functions[GetExpandedReference] = test_get_expand_ref;
  test_functions[GetVariableReference] = test_get_ref;
  test_functions[NormalSet] = test_normal_set;
  test_functions[VariableReferenceSet] = test_var_ref_set;
  test_functions[VariablesIncVarRef] = test_variables_inc_var_ref;

  test_functions[ContainerAssignment] = test_container_assignment;
  test_functions[ContainerIncrement] = test_container_increment;
  test_functions[StagedContainerAssignment] = test_staged_container_assignment;
  test_functions[StagedContainerIncrement] = test_staged_container_increment;

  test_functions[OptimizedAssignment] = test_optimal_assignment;
  test_functions[OptimizedReinforcement] = test_optimal_reinforcement;
  test_functions[OptimizedInference] = test_optimal_inference;
  test_functions[VirtualReinforcement] = test_virtual_reinforcement;
  test_functions[VolatileAssignment] = test_volatile_assignment;
  test_functions[VolatileReinforcement] = test_volatile_reinforcement;
  test_functions[VolatileInference] = test_volatile_inference;

  test_functions[StlAtomicIncrement] = test_stl_inc_atomic;
  test_functions[StlRecursiveIncrement] = test_stl_inc_recursive;
  test_functions[StlMutexIncrement] = test_stl_inc_mutex;

#ifndef _MADARA_NO_KARL_
  knowledge.define_function("inc", increment_var1);
  knowledge.define_function("no_op", no_op);
  knowledge.define_function("inc_var_ref", increment_var1_through_variables);
#endif

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "Testing throughput for MADARA v%s\n",
      madara::utility::get_version().c_str());

  for(uint32_t i = 0; i < num_runs; ++i)
  {
    // run tests
    for(int j = 0; j < num_test_types; ++j)
    {
      results[j] += test_functions[j](knowledge, num_iterations);
    }
  }

  // to find out the number of iterations per second, we need to
  // multiply the numerator by the factor of the result, which is
  // given in microseconds.

  uint64_t evaluations = num_iterations * num_runs;

  for(int i = 0; i < num_test_types; ++i)
  {
    // avoid blowing up with a division by zero on fast processors. Culprit here
    // is the C++ optimization of the for loop which simply sets a register to
    // the max increment, so time taken is often 0 ns.
    if(results[i] == 0)
      results[i] = 1;

    averages[i] = (1000000000 * evaluations) / results[i];
  }

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "\n\nTotal time taken for each test with %d iterations * %d tests was:\n",
      num_iterations, num_runs);

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "========================================================================"
      "=\n");

  for(int i = 0; i < num_test_types; ++i)
  {
    std::stringstream buffer;

    std::locale loc("C");
    buffer.imbue(loc);

    buffer << " ";
    buffer << printouts[i];
    buffer << "\t\t";
    buffer << std::setw(22);
    buffer << results[i];
    buffer << " ns\n";

    madara_logger_ptr_log(
        logger::global_logger.get(), logger::LOG_ALWAYS, buffer.str().c_str());
  }

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "========================================================================"
      "=\n\n");

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "\nAverage time taken per rule evaluation was:\n", num_iterations,
      num_runs);

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "========================================================================"
      "=\n");

  for(int i = 0; i < num_test_types; ++i)
  {
    std::stringstream buffer;

    std::locale loc("C");
    buffer.imbue(loc);

    buffer << " ";
    buffer << printouts[i];
    buffer << "\t\t";
    buffer << std::setw(22);
    buffer << (results[i] / (num_iterations * num_runs));
    buffer << " ns\n";

    madara_logger_ptr_log(
        logger::global_logger.get(), logger::LOG_ALWAYS, buffer.str().c_str());
  }

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "========================================================================"
      "=\n\n");

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "\nHertz for each test with %d iterations * %d tests was:\n",
      num_iterations, num_runs);

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "========================================================================"
      "=\n");

  for(int i = 0; i < num_test_types; ++i)
  {
    std::stringstream buffer;
    buffer << " ";
    buffer << printouts[i];
    buffer << "\t\t";
    buffer << std::setw(25);
    buffer << to_legible_hertz(averages[i]);
    buffer << "\n";

    madara_logger_ptr_log(
        logger::global_logger.get(), logger::LOG_ALWAYS, buffer.str().c_str());
  }

  madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
      "========================================================================"
      "=\n\n");

  return 0;
}

long increment(bool check, long value)
{
  return check ? ++value : value;
}

long increment(long value)
{
  return ++value;
}

long assignment(long& var, long value)
{
  return var = value;
}

/// Tests logicals operators (&&, ||)
uint64_t test_simple_reinforcement(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  // keep track of time
  madara::utility::Timer<Clock> timer;
  uint64_t measured(0);

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
#ifndef _MADARA_NO_KARL_
    // test literals in conditionals
    knowledge.evaluate(
        "++.var1", madara::knowledge::EvalSettings(false, false, false));
#endif
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Simple Reinforcement: ");

  return measured;
}

/// Tests Integer container increment
uint64_t test_container_assignment(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  madara::knowledge::containers::Integer var1(
      ".var1", knowledge, madara::knowledge::EvalSettings(false, false, false));

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    var1 = i;
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Container assignment: ");

  return measured;
}

/// Tests Integer container increment
uint64_t test_container_increment(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  madara::knowledge::containers::Integer var1(
      ".var1", knowledge, madara::knowledge::EvalSettings(false, false, false));

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    ++var1;
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Container increment: ");

  return measured;
}

/// Tests Integer container increment
uint64_t test_staged_container_assignment(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  madara::knowledge::containers::IntegerStaged var1(
      ".var1", knowledge, madara::knowledge::EvalSettings(false, false, false));

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  var1.read();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    var1 = i;
  }

  var1.write();

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations,
      "Staged Container assignment: ");

  return measured;
}

/// Tests Integer container increment
uint64_t test_staged_container_increment(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  madara::knowledge::containers::IntegerStaged var1(
      ".var1", knowledge, madara::knowledge::EvalSettings(false, false, false));

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  var1.read();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    ++var1;
  }

  var1.write();

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations,
      "Staged Container increment: ");

  return measured;
}

uint64_t test_compiled_sr(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();
#ifndef _MADARA_NO_KARL_
  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile("++.var1");

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // test literals in conditionals
    knowledge.evaluate(
        ce, madara::knowledge::EvalSettings(false, false, false));
  }

  timer.stop();
  measured = timer.duration_ns();
  print(measured, knowledge.get(".var1"), iterations,
      "Compiled Simple Increment: ");

  return measured;
#else
  return 0;
#endif
}

uint64_t test_compiled_sa(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();
#ifndef _MADARA_NO_KARL_
  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile(".var1=1");

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // test literals in conditionals
    knowledge.evaluate(
        ce, madara::knowledge::EvalSettings(false, false, false, true, false));
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations,
      "Compiled Single Assignments: ");

  return measured;
#else
  return 0;
#endif
}

uint64_t test_compiled_sfi(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();
#ifndef _MADARA_NO_KARL_
  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile("inc ()");

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // test literals in conditionals
    knowledge.evaluate(
        ce, madara::knowledge::EvalSettings(false, false, false));
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations,
      "Compiled Extern Increment Function: ");

  return measured;
#else
  return 0;
#endif
}

uint64_t test_extern_call(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();
#ifndef _MADARA_NO_KARL_
  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile("no_op ()");

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // test literals in conditionals
    knowledge.evaluate(
        ce, madara::knowledge::EvalSettings(false, false, false));
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Extern function call: ");

  return measured;
#else
  return 0;
#endif
}

/// Tests looped long inferences (++.var1)
uint64_t test_looped_sr(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  // build a large chain of simple reinforcements
  std::string buffer;

  unsigned actual_iterations = iterations > 10000 ? iterations / 10000 : 1;

  knowledge.set(".iterations", iterations);
  knowledge.set(".actual_iterations", actual_iterations);

  buffer = ".var2[0->.iterations) (++.var1)";

  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile(buffer);

  timer.start();

  // execute that chain of reinforcements
  knowledge.evaluate(ce, madara::knowledge::EvalSettings(false, false, false));

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Compiled Looped LR: ");

  return measured;
#else
  return 0;
#endif
}

/// Tests logicals operators (&&, ||)
uint64_t test_large_reinforcement(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  // build a large chain of simple reinforcements
  std::stringstream buffer;

  unsigned max_size = iterations > 1000 ? 1000 : iterations;
  unsigned actual_iterations = iterations > 1000 ? iterations / 1000 : 1;

  for(uint32_t i = 0; i < max_size; ++i)
  {
    buffer << "++.var1;";
  }

  timer.start();

  // execute that chain of reinforcements
  for(uint32_t i = 0; i < actual_iterations; ++i)
    knowledge.evaluate(
        buffer.str(), madara::knowledge::EvalSettings(false, false, false));

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Large Reinforcement: ");

  return measured;
#else
  return 0;
#endif
}

/// Tests logicals operators (&&, ||)
uint64_t test_compiled_lr(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();
#ifndef _MADARA_NO_KARL_

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  // build a large chain of simple reinforcements
  std::stringstream buffer;

  unsigned max_size = iterations > 1000 ? 1000 : iterations;
  unsigned actual_iterations = iterations > 1000 ? iterations / 1000 : 1;

  for(uint32_t i = 0; i < max_size; ++i)
  {
    buffer << "++.var1;";
  }

  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile(buffer.str());

  timer.start();

  // execute that chain of reinforcements
  for(uint32_t i = 0; i < actual_iterations; ++i)
    knowledge.evaluate(
        ce, madara::knowledge::EvalSettings(false, false, false));

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations,
      "Compiled Multiple Increments: ");

  return measured;
#else
  return 0;
#endif
}

/// Tests assignment speed
uint64_t test_compiled_la(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();
#ifndef _MADARA_NO_KARL_

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  // build a large chain of simple reinforcements
  std::stringstream buffer;

  unsigned max_size = iterations > 1000 ? 1000 : iterations;
  unsigned actual_iterations = iterations > 1000 ? iterations / 1000 : 1;

  for(uint32_t i = 0; i < max_size; ++i)
  {
    buffer << "var1=";
    buffer << i;
    buffer << ";";
  }

  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile(buffer.str());

  timer.start();

  // execute that chain of reinforcements
  for(uint32_t i = 0; i < actual_iterations; ++i)
    knowledge.evaluate(
        ce, madara::knowledge::EvalSettings(false, false, false));

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations,
      "Compiled Multiple Assignments: ");

  return measured;
#else
  return 0;
#endif
}

/// Tests assignment speed
uint64_t test_compiled_lfi(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  // build a large chain of simple reinforcements
  std::stringstream buffer;

  unsigned max_size = iterations > 1000 ? 1000 : iterations;
  unsigned actual_iterations = iterations > 1000 ? iterations / 1000 : 1;

  for(uint32_t i = 0; i < max_size; ++i)
  {
    buffer << "inc ();";
  }

  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile(buffer.str());

  timer.start();

  // execute that chain of reinforcements
  for(uint32_t i = 0; i < actual_iterations; ++i)
    knowledge.evaluate(
        ce, madara::knowledge::EvalSettings(false, false, false));

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations,
      "Compiled Extern Multi Calls: ");

  return measured;
#else
  return 0;
#endif
}

/// Tests looped long inferences (1 => ++.var1)
uint64_t test_optimal_loop(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  // build a large chain of simple reinforcements
  std::string buffer;

  unsigned actual_iterations = iterations > 10000 ? iterations / 10000 : 1;

  knowledge.set(".iterations", iterations);
  knowledge.set(".actual_iterations", actual_iterations);

  buffer = ".var2[.iterations)";

  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile(buffer);

  timer.start();

  // execute that chain of reinforcements
  knowledge.evaluate(ce, madara::knowledge::EvalSettings(false, false, false));

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Optimal Loop: ");

  return measured;
#else
  return 0;
#endif
}

uint64_t test_simple_inference(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // test literals in conditionals
    knowledge.evaluate(
        "1 => ++.var1", madara::knowledge::EvalSettings(false, false, false));
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Simple Inference: ");

  return measured;
#else
  return 0;
#endif
}

uint64_t test_normal_set(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    knowledge.set("var1", i);
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Normal Set Operation: ");

  return measured;
}

uint64_t test_get_ref(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    madara::knowledge::VariableReference variable = knowledge.get_ref("var1");
    (void)variable;
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations,
      "Get Reference Operation: ");

  return measured;
}

uint64_t test_get_expand_ref(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    madara::knowledge::VariableReference variable = knowledge.get_ref(
        "var1", madara::knowledge::KnowledgeReferenceSettings(true));
    (void)variable;
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Get Expanded Ref: ");

  return measured;
}

uint64_t test_var_ref_set(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  timer.start();

  madara::knowledge::VariableReference variable = knowledge.get_ref("var1");

  for(uint32_t i = 0; i < iterations; ++i)
  {
    knowledge.set(variable, i);
  }

  timer.stop();
  measured = timer.duration_ns();

  print(
      measured, knowledge.get(".var1"), iterations, "Variable Reference Set: ");

  return measured;
}

uint64_t test_variables_inc_var_ref(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile("inc_var_ref ()");

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    knowledge.evaluate(ce);
  }

  timer.stop();
  measured = timer.duration_ns();

  print(
      measured, knowledge.get(".var1"), iterations, "Variables Inc Var Ref: ");

  return measured;
#else
  return 0;
#endif
}

uint64_t test_stl_inc_atomic(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  madara::knowledge::CompiledExpression ce;

  std::atomic<long> counter;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    ++counter;
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, madara::knowledge::KnowledgeRecord(counter.load()),
      iterations, "STL Atomic Increment: ");

  return measured;
}

uint64_t test_stl_inc_recursive(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  madara::knowledge::CompiledExpression ce;

  long counter = 0;
  std::recursive_mutex critical_section;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    critical_section.lock();
    ++counter;
    critical_section.unlock();
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, madara::knowledge::KnowledgeRecord(counter), iterations,
      "STL Recursive Increment: ");

  return measured;
}

uint64_t test_stl_inc_mutex(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  madara::knowledge::CompiledExpression ce;

  long counter = 0;
  std::mutex critical_section;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    critical_section.lock();
    ++counter;
    critical_section.unlock();
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, madara::knowledge::KnowledgeRecord(counter), iterations,
      "STL Mutex Increment: ");

  return measured;
}

uint64_t test_compiled_si(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile("1 => ++.var1");

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // test literals in conditionals
    knowledge.evaluate(
        ce, madara::knowledge::EvalSettings(false, false, false));
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations,
      "Compiled Simple Ternary Increment: ");

  return measured;
#else
  return 0;
#endif
}

/// Tests looped simple inferences (1 => ++.var1)
uint64_t test_looped_si(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  // build a large chain of simple reinforcements
  std::string buffer;

  knowledge.set(".iterations", iterations);

  buffer = ".var2[0->.iterations) (1 => ++.var1)";

  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile(buffer);

  timer.start();

  // execute that chain of reinforcements
  knowledge.evaluate(ce, madara::knowledge::EvalSettings(false, false, false));

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Compiled Looped LR: ");

  return measured;
#else
  return 0;
#endif
}

uint64_t test_large_inference(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  // build a large chain of simple reinforcements
  std::stringstream buffer;

  unsigned max_size = iterations > 1000 ? 1000 : iterations;
  unsigned actual_iterations = iterations > 1000 ? iterations / 1000 : 1;

  for(uint32_t i = 0; i < max_size; ++i)
  {
    buffer << "1 => ++.var1;";
  }

  timer.start();

  // execute that chain of reinforcements
  for(uint32_t i = 0; i < actual_iterations; ++i)
    knowledge.evaluate(
        buffer.str(), madara::knowledge::EvalSettings(false, false, false));

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Large Inference: ");

  return measured;
#else
  return 0;
#endif
}

uint64_t test_compiled_li(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  // build a large chain of simple reinforcements
  std::stringstream buffer;

  unsigned max_size = iterations > 1000 ? 1000 : iterations;
  unsigned actual_iterations = iterations > 1000 ? iterations / 1000 : 1;

  for(uint32_t i = 0; i < max_size; ++i)
  {
    buffer << "1 => ++.var1;";
  }

  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile(buffer.str());

  timer.start();

  // execute that chain of reinforcements
  for(uint32_t i = 0; i < actual_iterations; ++i)
    knowledge.evaluate(
        ce, madara::knowledge::EvalSettings(false, false, false));

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations,
      "Compiled Multiple Ternary Increments: ");

  return measured;
#else
  return 0;
#endif
}

/// Tests looped long inferences (1 => ++.var1)
uint64_t test_looped_li(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

#ifndef _MADARA_NO_KARL_
  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  // build a large chain of simple reinforcements
  std::string buffer;

  unsigned actual_iterations = iterations > 10000 ? iterations / 10000 : 1;

  knowledge.set(".iterations", iterations);
  knowledge.set(".actual_iterations", actual_iterations);

  buffer = ".var2[0->.iterations) (1 => ++.var1)";

  madara::knowledge::CompiledExpression ce;

  ce = knowledge.compile(buffer);

  timer.start();

  // execute that chain of reinforcements
  knowledge.evaluate(ce, madara::knowledge::EvalSettings(false, false, false));

  timer.stop();
  measured = timer.duration_ns();

  print(measured, knowledge.get(".var1"), iterations, "Compiled Looped LR: ");

  return measured;
#else
  return 0;
#endif
}

/// Tests logicals operators (&&, ||)
uint64_t test_optimal_inference(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  madara::knowledge::KnowledgeRecord::Integer var1 = 0;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // we use a global var that can be changed from the command
    // line to trick the compiler into not optimizing out this
    // loop
    if(conditional)
      ++var1;
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, madara::knowledge::KnowledgeRecord(var1), iterations,
      "Optimal Inference: ");

  return measured;
}

/// Tests logicals operators (&&, ||)
uint64_t test_optimal_assignment(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  // keep track of time
  uint64_t measured = 0;
  madara::utility::Timer<Clock> timer;

  long var1 = 0;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // I'm going to have to admit defeat. I can't use __asm because
    // it's non-portable. The compiler will compile this away
    // with either a mov of the final value or imul (if I try using a step
    // to confuse the compiler). If I add in a print statement or something
    // else, the loop will of course be altered and time is wasted.

    //
    // I understand that compilers don't care if we are trying to do
    // performance testing of loops without resorting to volatile which
    // means drastic performance decrease (at least 1/3), but there should
    // be some way to portably force a compiler to NOT optimize away this
    // loop without causing speed decrease via printing, outputting to a file,
    // synchronizing, library calls, or using volatile on the accumulator
    //
    //
    assignment(var1, (long)i);

    // the following works but is non-portable to non-VS
    // force compiler to do an operation here so it doesn't just
    // add all the iterations up and set var1 to that count
    //__asm
    //{
    //  nop;
    //}
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, madara::knowledge::KnowledgeRecord(var1), iterations,
      "Optimal Reinforcement: ");

  return measured;
}

/// Tests logicals operators (&&, ||)
uint64_t test_optimal_reinforcement(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  // keep track of time
  uint64_t measured = 0;
  madara::utility::Timer<Clock> timer;

  madara::knowledge::KnowledgeRecord::Integer var1 = 0;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // I'm going to have to admit defeat. I can't use __asm because
    // it's non-portable. The compiler will compile this away
    // with either a mov of the final value or imul (if I try using a step
    // to confuse the compiler). If I add in a print statement or something
    // else, the loop will of course be altered and time is wasted.

    //
    // I understand that compilers don't care if we are trying to do
    // performance testing of loops without resorting to volatile which
    // means drastic performance decrease (at least 1/3), but there should
    // be some way to portably force a compiler to NOT optimize away this
    // loop without causing speed decrease via printing, outputting to a file,
    // synchronizing, library calls, or using volatile on the accumulator
    //
    //
    ++var1;

    // the following works but is non-portable to non-VS
    // force compiler to do an operation here so it doesn't just
    // add all the iterations up and set var1 to that count
    //__asm
    //{
    //  nop;
    //}
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, madara::knowledge::KnowledgeRecord(var1), iterations,
      "Optimal Reinforcement: ");

  return measured;
}

/// Tests C++ function inference
uint64_t test_volatile_inference(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  volatile madara::knowledge::KnowledgeRecord::Integer var1 = 0;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // guarded increment of the volatile variable
    if(conditional)
      ++var1;
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, madara::knowledge::KnowledgeRecord(var1), iterations,
      "Volatile Inference: ");

  return measured;
}

/// Tests C++ function inference
uint64_t test_volatile_reinforcement(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();
  Incrementer accumulator;

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  volatile madara::knowledge::KnowledgeRecord::Integer var1 = 0;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // increment the volatile variable
    ++var1;
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, madara::knowledge::KnowledgeRecord(var1), iterations,
      "Volatile Reinforcement: ");

  return measured;
}

/// Tests C++ function inference
uint64_t test_virtual_reinforcement(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();
  Incrementer accumulator;

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  BaseOperation* var1 = new IncrementOperation();

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // increment the volatile variable
    var1->evaluate();
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, madara::knowledge::KnowledgeRecord(var1->get_value()),
      iterations, "Virtual Reinforcement: ");

  delete var1;

  return measured;
}

/// Tests C++ function inference
uint64_t test_volatile_assignment(
    madara::knowledge::KnowledgeBase& knowledge, uint32_t iterations)
{
  knowledge.clear();
  Incrementer accumulator;

  // keep track of time
  uint64_t measured(0);
  madara::utility::Timer<Clock> timer;

  volatile madara::knowledge::KnowledgeRecord::Integer var1 = 0;

  timer.start();

  for(uint32_t i = 0; i < iterations; ++i)
  {
    // increment the volatile variable
    var1 = madara::knowledge::KnowledgeRecord::Integer(i);
  }

  timer.stop();
  measured = timer.duration_ns();

  print(measured, madara::knowledge::KnowledgeRecord(var1), iterations,
      "Volatile Assignment: ");

  return measured;
}

// handle command line arguments
void handle_arguments(int argc, char* argv[])
{
  for(int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if(arg1 == "-c" || arg1 == "--conditional")
    {
      if(i + 1 < argc)
      {
        std::string arg2(argv[i + 1]);

        if(arg2 == "false")
        {
          conditional = false;
        }
        else
        {
          conditional = true;
        }
      }

      ++i;
    }
    else if(arg1 == "-f" || arg1 == "--logfile")
    {
      if(i + 1 < argc)
      {
        logger::global_logger->add_file(argv[i + 1]);
      }

      ++i;
    }
    else if(arg1 == "-l" || arg1 == "--level")
    {
      if(i + 1 < argc)
      {
        int level;
        std::stringstream buffer(argv[i + 1]);
        buffer >> level;
        logger::global_logger->set_level(level);
      }

      ++i;
    }
    else if(arg1 == "-n" || arg1 == "--iterations")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> num_iterations;
      }

      ++i;
    }
    else if(arg1 == "-r" || arg1 == "--runs")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> num_runs;
      }

      ++i;
    }
    else if(arg1 == "-s" || arg1 == "--step")
    {
      if(i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> step;
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(),
          logger::LOG_ALWAYS, "Program Summary for %s:\n\n\
This stand-alone application runs a variety of tests to determine\n\
performance on a host system. For a more comprehensive and\n\
customizeable tests, see profile_architecture\n\n\
-n (--iterations)  number of iterations      \n\
-r (--runs)        number of runs            \n\
-s (--step)        number of iterations      \n\
-c (--conditional) false if guard==false     \n\
-- author's note. The last two are only necessary \n\
-- because C++ compilers are trying to opimize \n\
-- away the loops we are trying to test \n\
-h (--help)        print this menu           \n\n", argv[0]);
      exit(0);
    }
  }
}
