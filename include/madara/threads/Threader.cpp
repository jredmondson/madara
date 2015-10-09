#include "Threader.h"
#include "madara/utility/Utility.h"

#ifdef _MADARA_JAVA_
      
#include "java/JavaThread.h"

#endif  // MADARA_JAVA

madara::threads::Threader::Threader ()
  : control_(new knowledge::KnowledgeBase ()), data_ (0)
{

}

madara::threads::Threader::Threader (
  knowledge::KnowledgeBase & data_plane)
  : control_(new knowledge::KnowledgeBase ()), data_ (&data_plane)
{

}

madara::threads::Threader::~Threader ()
{
  terminate ();
  wait ();

  delete control_;
}

void madara::threads::Threader::set_data_plane (
  knowledge::KnowledgeBase & data_plane)
{
  data_ = &data_plane;
}

void
madara::threads::Threader::run (
  const std::string name, BaseThread * thread, bool paused)
{
  if (name != "" && thread != 0)
  {
    WorkerThread * worker = new WorkerThread (name, thread,
      control_, data_);

    if (paused)
      thread->paused = 1;

    threads_[name] = worker;

    worker->run ();
  }
}

#ifdef _MADARA_JAVA_
      
void
madara::threads::Threader::run (
  const std::string name, jobject thread, bool paused)
{
  if (name != "" && thread != 0)
  {
    // attempt to create a Java Thread
    JavaThread * new_thread = JavaThread::create (thread);

    // if successful, run the thread
    if (new_thread)
      run (name, new_thread, paused);
  }
}
      

void
madara::threads::Threader::run (
  double hertz, const std::string name, jobject thread, bool paused)
{
  if (name != "" && thread != 0)
  {
    // attempt to create a Java Thread
    JavaThread * new_thread = JavaThread::create (thread);

    // if successful, run the thread
    if (new_thread)
      run (hertz, name, new_thread, paused);
  }
}

#endif // _MADARA_JAVA_

void
madara::threads::Threader::run (double hertz,
  const std::string name, BaseThread * thread, bool paused)
{
  if (name != "" && thread != 0)
  {
    WorkerThread * worker = new WorkerThread (name, thread,
      control_, data_, hertz);

    if (paused)
      thread->paused = 1;

    threads_[name] = worker;

    worker->run ();
  }
}

void
madara::threads::Threader::wait (const std::string name)
{
  NamedWorkerThreads::iterator found = threads_.find (name);

  if (found != threads_.end ())
  {
    std::string finished = found->second->finished_.get_name ();

    utility::wait_true (*control_, finished);

    delete found->second;

    threads_.erase (found);
  }
}

void
madara::threads::Threader::wait (void)
{
  for (NamedWorkerThreads::iterator i = threads_.begin ();
       i != threads_.end (); ++i)
  {
    std::string finished = i->second->finished_.get_name ();

    utility::wait_true (*control_, finished);

    delete i->second;
  }

  threads_.clear ();
}

void
madara::threads::Threader::pause (const std::string name)
{
  NamedWorkerThreads::iterator found = threads_.find (name);

  if (found != threads_.end ())
  {
    control_->set (name + ".paused", KnowledgeRecord::Integer (1));
  }
}

void
madara::threads::Threader::pause (void)
{
  for (NamedWorkerThreads::iterator i = threads_.begin ();
       i != threads_.end (); ++i)
  {
    control_->set (i->first + ".paused", KnowledgeRecord::Integer (1));
  }
}

void
madara::threads::Threader::resume (const std::string name)
{
  NamedWorkerThreads::iterator found = threads_.find (name);

  if (found != threads_.end ())
  {
    control_->set (name + ".paused", KnowledgeRecord::Integer (0));
  }
}

void
madara::threads::Threader::resume (void)
{
  for (NamedWorkerThreads::iterator i = threads_.begin ();
       i != threads_.end (); ++i)
  {
    control_->set (i->first + ".paused", KnowledgeRecord::Integer (0));
  }
}

void
madara::threads::Threader::terminate (const std::string name)
{
  NamedWorkerThreads::iterator found = threads_.find (name);

  if (found != threads_.end ())
  {
    control_->set (name + ".terminated", KnowledgeRecord::Integer (1));
  }
}

void
madara::threads::Threader::terminate (void)
{
  for (NamedWorkerThreads::iterator i = threads_.begin ();
       i != threads_.end (); ++i)
  {
    control_->set (i->first + ".terminated", KnowledgeRecord::Integer (1));
  }
}
