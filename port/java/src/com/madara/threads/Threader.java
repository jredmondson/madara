/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara.threads;

import com.madara.KnowledgeRecord;
import com.madara.Variables;
import com.madara.KnowledgeList;
import com.madara.KnowledgeBase;
import com.madara.containers.Integer;
import com.madara.MadaraJNI;

/**
 * A manager of user threads
 **/
public class Threader extends MadaraJNI
{
  private native long jni_Threader();
  private native long jni_Threader(long dataPlane);
  private native void jni_freeThreader(long cptr);
  private native void jni_run(long cptr, java.lang.String name, Object obj, boolean paused);
  private native void jni_runhz(long cptr, double hertz, java.lang.String name, Object obj, boolean paused);
  private native void jni_pauseThread(long cptr, java.lang.String name);
  private native void jni_pause(long cptr);
  private native void jni_waitThread(long cptr, java.lang.String name);
  private native void jni_wait(long cptr);
  private native void jni_terminateThread(long cptr, java.lang.String name);
  private native void jni_terminate(long cptr);
  private native void jni_resumeThread(long cptr, java.lang.String name);
  private native void jni_resume(long cptr);
  
  /**
   * Default constructor
   **/
  public Threader()
  {
    setCPtr(jni_Threader());
  }
  
  /**
   * Constructor with data plane
   * @param  dataPlane  the knowledge base for threads to use for data
   **/
  public Threader(KnowledgeBase dataPlane)
  {
    setCPtr(jni_Threader(dataPlane.getCPtr()));
  }
  
  /**
   * Deletes the C++ instantiation. To prevent memory leaks, this <b>must</b> be called
   * before an instance of Threader gets garbage collected
   **/
  public void free()
  {
    jni_freeThreader(getCPtr());
    setCPtr(0);
  }
  
  /**
   * Runs a thread. The thread's run method will only be called once.
   * @param  name  the unique name of the thread
   * @param  thread  the thread to run
   **/
  public void run (String name, BaseThread thread)
  {
    thread.name = name;
    jni_run(getCPtr(), name, thread, false);
  }
  
  /**
   * Runs a thread. The thread's run method will be called at the specified
   * hertz (executions of run per second)
   * @param  hertz the intended frequency of executions per second
   * @param  name  the unique name of the thread
   * @param  thread  the thread to run
   **/
  public void run(double hertz, String name, BaseThread thread)
  {
    thread.name = name;
    jni_runhz(getCPtr(), hertz, name, thread, false);
  }
  
  /**
   * Starts a thread in a paused state. Once resumed, the run method will
   * only be called once.
   * @param  name  the unique name of the thread
   * @param  thread  the thread to start in a paused state
   **/
  public void startPaused(String name, BaseThread thread)
  {
    thread.name = name;
    jni_run(getCPtr(), name, thread, true);
  }
  
  /**
   * Starts a thread in a paused state. Once resumed, the run method will
   * be called at the specified hertz.
   * @param  hertz the intended frequency of executions per second
   * @param  name  the unique name of the thread
   * @param  thread  the thread to start in a paused state
   **/
  public void startPaused(double hertz, String name, BaseThread thread)
  {
    thread.name = name;
    jni_runhz(getCPtr(), hertz, name, thread, true);
  }
  
  /**
   * Requests for the specified thread to pause
   * @param name the unique name of the thread requested to pause
   **/
  public void pause(String name)
  {
   jni_pauseThread(getCPtr(), name); 
  }
  
  /**
   * Requests all threads to pause
   **/
  public void pause()
  {
   jni_pause(getCPtr()); 
  }
  
  
  /**
   * Waits for a specific thread to finish
   * @param name the unique name of the thread
   **/
  public void waitForThread(String name)
  {
   jni_waitThread(getCPtr(), name); 
  }
  
  /**
   * Waits for all threads to finish
   **/
  public void waitForThreads()
  {
   jni_wait(getCPtr()); 
  }
   
  /**
   * Requests for the specified thread to terminate
   * @param name the unique name of the thread
   **/
  public void terminate(String name)
  {
   jni_terminateThread(getCPtr(), name); 
  }
  
  /**
   * Requests all threads to terminate
   **/
  public void terminate()
  {
   jni_terminate(getCPtr()); 
  }
   
  /**
   * Requests for the specified thread to resume if paused
   * @param name the unique name of the thread
   **/
  public void resume(String name)
  {
   jni_resumeThread(getCPtr(), name); 
  }
  
  /**
   * Requests all threads to resume if paused
   **/
  public void resume()
  {
   jni_resume(getCPtr()); 
  }
   
}

