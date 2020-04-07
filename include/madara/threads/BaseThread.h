

#ifndef _MADARA_THREADS_BASE_THREAD_H_
#define _MADARA_THREADS_BASE_THREAD_H_

/**
 * @file BaseThread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a base thread implementation for
 * use when running MADARA threads
 **/

#include <string>
#include <vector>
#include <map>
#include <list>
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/Functions.h"
#include "madara/utility/StdInt.h"
#include "madara/MadaraExport.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/knowledge/containers/Integer.h"

namespace madara
{
namespace threads
{
/**
 * Forward declare some classes
 **/
class WorkerThread;
class Threader;

/**
 * Abstract base class for implementing threads
 **/
class BaseThread
{
public:
  /**
   * Destructor
   **/
  virtual ~BaseThread() {}

  /**
   * Worker threads are MADARA thread entities that manage
   * user-defined threads
   **/
  friend class WorkerThread;
  friend class Threader;

  /**
   * Initializes thread with a MADARA context
   **/
  virtual void init(knowledge::KnowledgeBase&) {}

  /**
   * Executes the main thread logic. In a BaseThread, the
   * execute is called once. Afterwards, cleanup is called.
   **/
  virtual void run(void) = 0;

  /**
   * Cleans up any thread residue (usually instances created in init).
   * It's important to note that the only things that would need to
   * be cleaned up are generally ports opened, memory initialized with
   * new/malloc, etc. Anything MADARA related is generally cleaned up
   * for you.
   **/
  virtual void cleanup(void) {}

protected:
  /**
   * Initializes the thread implementation's control plane variables
   * @param control the control plane between Threader and threads
   **/
  virtual void init_control_vars(knowledge::KnowledgeBase& control)
  {
    terminated.set_name(name + ".terminated", control);
    paused.set_name(name + ".paused", control);

    terminated = 0;
    paused = 0;
  }

  /**
   * The unique name of your thread
   **/
  std::string name;

  /**
   * thread safe terminated flag that may be set by the Threader
   * base
   **/
  madara::knowledge::containers::Integer terminated;

  /**
   * thread safe paused flag that may be set by the Threader
   **/
  madara::knowledge::containers::Integer paused;
};
}  // namespace threads
}  // namespace madara

#endif  // _MADARA_THREADS_BASE_THREAD_H_
