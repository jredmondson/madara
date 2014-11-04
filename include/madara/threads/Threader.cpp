#include "Threader.h"

#ifdef _MADARA_JAVA_
      
#include "java\Java_Thread.h"

#endif  // MADARA_JAVA

Madara::Threads::Threader::Threader ()
  : control_(new Knowledge_Engine::Knowledge_Base ()), data_ (0)
{

}

Madara::Threads::Threader::Threader (
  Knowledge_Engine::Knowledge_Base & data_plane)
  : control_(new Knowledge_Engine::Knowledge_Base ()), data_ (&data_plane)
{

}

Madara::Threads::Threader::~Threader ()
{
  terminate ();
  wait ();

  delete control_;
}


void
Madara::Threads::Threader::run (
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
Madara::Threads::Threader::run (
  const std::string name, jobject thread, bool paused)
{
  if (name != "" && thread != 0)
  {
    run (name, new Java_Thread (thread), paused);
  }
}
      

void
Madara::Threads::Threader::run (
  double hertz, const std::string name, jobject thread, bool paused)
{
  if (name != "" && thread != 0)
  {
    run (name, new Java_Thread (thread), paused);
  }
}

#endif // _MADARA_JAVA_

void
Madara::Threads::Threader::run (double hertz,
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
Madara::Threads::Threader::wait (const std::string name)
{
  Named_Worker_Threads::iterator found = threads_.find (name);

  if (found != threads_.end ())
  {
    std::string finished = found->second->finished_.get_name ();
    control_->wait (finished);

    delete found->second;

    threads_.erase (found);
  }
}

void
Madara::Threads::Threader::wait (void)
{
  for (Named_Worker_Threads::iterator i = threads_.begin ();
       i != threads_.end (); ++i)
  {
    std::string finished = i->second->finished_.get_name ();
    control_->wait (finished);

    delete i->second;
  }

  threads_.clear ();
}

void
Madara::Threads::Threader::pause (const std::string name)
{
  Named_Worker_Threads::iterator found = threads_.find (name);

  if (found != threads_.end ())
  {
    control_->evaluate (name + ".paused = 1");
  }
}

void
Madara::Threads::Threader::pause (void)
{
  for (Named_Worker_Threads::iterator i = threads_.begin ();
       i != threads_.end (); ++i)
  {
    control_->evaluate (i->first + ".paused = 1");
  }
}

void
Madara::Threads::Threader::resume (const std::string name)
{
  Named_Worker_Threads::iterator found = threads_.find (name);

  if (found != threads_.end ())
  {
    control_->evaluate (name + ".paused = 0");
  }
}

void
Madara::Threads::Threader::resume (void)
{
  for (Named_Worker_Threads::iterator i = threads_.begin ();
       i != threads_.end (); ++i)
  {
    control_->evaluate (i->first + ".paused = 0");
  }
}

void
Madara::Threads::Threader::terminate (const std::string name)
{
  Named_Worker_Threads::iterator found = threads_.find (name);

  if (found != threads_.end ())
  {
    control_->evaluate (name + ".terminated = 1");
  }
}

void
Madara::Threads::Threader::terminate (void)
{
  for (Named_Worker_Threads::iterator i = threads_.begin ();
       i != threads_.end (); ++i)
  {
    control_->evaluate (i->first + ".terminated = 1");
  }
}
