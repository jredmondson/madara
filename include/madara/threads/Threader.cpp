#include "Threader.h"
#include "madara/utility/Utility.h"

#ifdef _MADARA_JAVA_
      
#include "java/Java_Thread.h"

#endif  // MADARA_JAVA

madara::Threads::Threader::Threader ()
  : control_(new knowledge::Knowledge_Base ()), data_ (0)
{

}

madara::Threads::Threader::Threader (
  knowledge::Knowledge_Base & data_plane)
  : control_(new knowledge::Knowledge_Base ()), data_ (&data_plane)
{

}

madara::Threads::Threader::~Threader ()
{
  terminate ();
  wait ();

  delete control_;
}

void madara::Threads::Threader::set_data_plane (
  knowledge::Knowledge_Base & data_plane)
{
  data_ = &data_plane;
}

void
madara::Threads::Threader::run (
  const std::string name, Base_Thread * thread, bool paused)
{
  if (name != "" && thread != 0)
  {
    Worker_Thread * worker = new Worker_Thread (name, thread,
      control_, data_);

    if (paused)
      thread->paused = 1;

    threads_[name] = worker;

    worker->run ();
  }
}

#ifdef _MADARA_JAVA_
      
void
madara::Threads::Threader::run (
  const std::string name, jobject thread, bool paused)
{
  if (name != "" && thread != 0)
  {
    // attempt to create a Java Thread
    Java_Thread * new_thread = Java_Thread::create (thread);

    // if successful, run the thread
    if (new_thread)
      run (name, new_thread, paused);
  }
}
      

void
madara::Threads::Threader::run (
  double hertz, const std::string name, jobject thread, bool paused)
{
  if (name != "" && thread != 0)
  {
    // attempt to create a Java Thread
    Java_Thread * new_thread = Java_Thread::create (thread);

    // if successful, run the thread
    if (new_thread)
      run (hertz, name, new_thread, paused);
  }
}

#endif // _MADARA_JAVA_

void
madara::Threads::Threader::run (double hertz,
  const std::string name, Base_Thread * thread, bool paused)
{
  if (name != "" && thread != 0)
  {
    Worker_Thread * worker = new Worker_Thread (name, thread,
      control_, data_, hertz);

    if (paused)
      thread->paused = 1;

    threads_[name] = worker;

    worker->run ();
  }
}

void
madara::Threads::Threader::wait (const std::string name)
{
  Named_Worker_Threads::iterator found = threads_.find (name);

  if (found != threads_.end ())
  {
    std::string finished = found->second->finished_.get_name ();

    utility::wait_true (*control_, finished);

    delete found->second;

    threads_.erase (found);
  }
}

void
madara::Threads::Threader::wait (void)
{
  for (Named_Worker_Threads::iterator i = threads_.begin ();
       i != threads_.end (); ++i)
  {
    std::string finished = i->second->finished_.get_name ();

    utility::wait_true (*control_, finished);

    delete i->second;
  }

  threads_.clear ();
}

void
madara::Threads::Threader::pause (const std::string name)
{
  Named_Worker_Threads::iterator found = threads_.find (name);

  if (found != threads_.end ())
  {
    control_->set (name + ".paused", Knowledge_Record::Integer (1));
  }
}

void
madara::Threads::Threader::pause (void)
{
  for (Named_Worker_Threads::iterator i = threads_.begin ();
       i != threads_.end (); ++i)
  {
    control_->set (i->first + ".paused", Knowledge_Record::Integer (1));
  }
}

void
madara::Threads::Threader::resume (const std::string name)
{
  Named_Worker_Threads::iterator found = threads_.find (name);

  if (found != threads_.end ())
  {
    control_->set (name + ".paused", Knowledge_Record::Integer (0));
  }
}

void
madara::Threads::Threader::resume (void)
{
  for (Named_Worker_Threads::iterator i = threads_.begin ();
       i != threads_.end (); ++i)
  {
    control_->set (i->first + ".paused", Knowledge_Record::Integer (0));
  }
}

void
madara::Threads::Threader::terminate (const std::string name)
{
  Named_Worker_Threads::iterator found = threads_.find (name);

  if (found != threads_.end ())
  {
    control_->set (name + ".terminated", Knowledge_Record::Integer (1));
  }
}

void
madara::Threads::Threader::terminate (void)
{
  for (Named_Worker_Threads::iterator i = threads_.begin ();
       i != threads_.end (); ++i)
  {
    control_->set (i->first + ".terminated", Knowledge_Record::Integer (1));
  }
}
