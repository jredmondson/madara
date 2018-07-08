/*********************************************************************
 * Copyright (c) 2013-2015 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following acknowledgments and disclaimers.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The names "Carnegie Mellon University," "SEI" and/or
 * "Software Engineering Institute" shall not be used to endorse or promote
 * products derived from this software without prior written permission. For
 * written permission, please contact permission@sei.cmu.edu.
 *
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 * appear in their names without prior written permission of
 * permission@sei.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 * acknowledgment:
 *
 * This material is based upon work funded and supported by the Department of
 * Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon University
 * for the operation of the Software Engineering Institute, a federally funded
 * research and development center. Any opinions, findings and conclusions or
 * recommendations expressed in this material are those of the author(s) and
 * do not necessarily reflect the views of the United States Department of
 * Defense.
 *
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
 * AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR
 * PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
 * MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
 * WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 *
 * This material has been approved for public release and unlimited
 * distribution.
 *
 * @author James Edmondson <jedmondson@gmail.com>
 *********************************************************************/
package ai.madara.threads;

import ai.madara.MadaraJNI;
import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.KnowledgeBase;

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
  public Threader(KnowledgeBase dataPlane) throws MadaraDeadObjectException
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
   * Cleans up underlying C resources
   * @throws Throwable necessary for override but unused
   */
  @Override
  protected void finalize() throws Throwable
  {
    try {
      free();
    } catch (Throwable t) {
      throw t;
    } finally {
      super.finalize();
    }
  }

  /**
   * Runs a thread. The thread's run method will only be called once.
   * @param  name  the unique name of the thread
   * @param  thread  the thread to run
   **/
  public void run (String name, BaseThread thread) throws MadaraDeadObjectException
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
  public void run(double hertz, String name, BaseThread thread) throws MadaraDeadObjectException
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
  public void startPaused(String name, BaseThread thread) throws MadaraDeadObjectException
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
  public void startPaused(double hertz, String name, BaseThread thread) throws MadaraDeadObjectException
  {
    thread.name = name;
    jni_runhz(getCPtr(), hertz, name, thread, true);
  }

  /**
   * Requests for the specified thread to pause
   * @param name the unique name of the thread requested to pause
   **/
  public void pause(String name) throws MadaraDeadObjectException
  {
   jni_pauseThread(getCPtr(), name);
  }

  /**
   * Requests all threads to pause
   **/
  public void pause() throws MadaraDeadObjectException
  {
   jni_pause(getCPtr());
  }


  /**
   * Waits for a specific thread to finish
   * @param name the unique name of the thread
   **/
  public void waitForThread(String name) throws MadaraDeadObjectException
  {
   jni_waitThread(getCPtr(), name);
  }

  /**
   * Waits for all threads to finish
   **/
  public void waitForThreads() throws MadaraDeadObjectException
  {
   jni_wait(getCPtr());
  }

  /**
   * Requests for the specified thread to terminate
   * @param name the unique name of the thread
   **/
  public void terminate(String name) throws MadaraDeadObjectException
  {
   jni_terminateThread(getCPtr(), name);
  }

  /**
   * Requests all threads to terminate
   **/
  public void terminate() throws MadaraDeadObjectException
  {
   jni_terminate(getCPtr());
  }

  /**
   * Requests for the specified thread to resume if paused
   * @param name the unique name of the thread
   **/
  public void resume(String name) throws MadaraDeadObjectException
  {
   jni_resumeThread(getCPtr(), name);
  }

  /**
   * Requests all threads to resume if paused
   **/
  public void resume() throws MadaraDeadObjectException
  {
   jni_resume(getCPtr());
  }

}

