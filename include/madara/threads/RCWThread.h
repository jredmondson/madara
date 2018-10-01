

#ifndef INCL_MADARA_THREADS_RCWTHREAD_H
#define INCL_MADARA_THREADS_RCWTHREAD_H

/**
 * @file RCWThread.h
 * @author David Kyle <dskyle@sei.cmu.edu>
 *
 * Base class for Run-Compute-Write threads
 **/

#include <string>
#include <vector>
#include <map>
#include <list>
#include <type_traits>
#include <initializer_list>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/Functions.h"
#include "madara/utility/StdInt.h"
#include "madara/MadaraExport.h"
#include "madara/knowledge/VariableReference.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/KnowledgeCast.h"
#include "madara/knowledge/ContextGuard.h"
#include "madara/threads/BaseThread.h"
#include "madara/knowledge/rcw/Transaction.h"

namespace madara
{
namespace threads
{
/**
 * Abstract base class for implementing RCW threads
 *
 * Users should override setup, compute, and finalize, but should
 * not override the init, run, and cleanup functions from BaseThread
 **/
class MADARA_EXPORT RCWThread : public BaseThread
{
public:
  /**
   * Destructor
   **/
  virtual ~RCWThread() {}

  /**
   * Worker threads are MADARA thread entities that manage
   * user-defined threads
   **/
  friend class WorkerThread;
  friend class Threader;

  /**
   * Setup the thread for later read-compute-write loops
   **/
  virtual void setup(knowledge::rcw::Transaction&) {}

  /**
   * Executes the compute porition of a read-compute-write loop
   **/
  virtual void compute(const knowledge::rcw::Transaction&) = 0;

  /**
   * Cleans up any thread residue (usually instances created in setup).
   * It's important to note that the only things that would need to
   * be cleaned up are generally ports opened, memory initialized with
   * new/malloc, etc. Anything MADARA related is generally cleaned up
   * for you.
   **/
  virtual void finalize(knowledge::rcw::Transaction&) {}

private:
  /**
   * Initializes thread with a MADARA context
   * @param kb the knowledge base to interact with
   **/
  virtual void init(knowledge::KnowledgeBase& kb) override final;

  /**
   * Executes the main thread logic. In a BaseThread, the
   * execute is called once. Afterwards, cleanup is called.
   **/
  virtual void run(void) override final;

  /**
   * Cleans up any thread residue (usually instances created in init).
   * It's important to note that the only things that would need to
   * be cleaned up are generally ports opened, memory initialized with
   * new/malloc, etc. Anything MADARA related is generally cleaned up
   * for you.
   **/
  virtual void cleanup(void) override final;

  std::unique_ptr<knowledge::rcw::Transaction> tx_;
};
}
}

#endif  // INCL_MADARA_THREADS_RCWTHREAD_H
