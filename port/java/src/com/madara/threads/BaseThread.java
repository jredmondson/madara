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

/**
 * An extensible thread
 **/
public abstract class BaseThread
{
  /**
   * Initializes thread with a data plane (a shared MADARA context)
   * @param   context   context for querying/modifying current program state
   **/
  public void init(KnowledgeBase context)
  {
  }
  
  /**
   * Executes the main thread logic.
   **/
  abstract public void run();
  
  /**
   * Cleans up any artifacts not taken care of by the Java VM. It will be
   * very rare to ever override the default behavior.
   **/
  public void cleanup()
  {
  }
  
  /**
   * Returns the unique name of the thread
   * @return the name of the thread
   **/
  public String getName()
  {
    return name; 
  }
  
  /**
   * flag from Threader indicating if thread is requested to be terminated
   **/
  public Integer  terminated;
  
  /**
   * flag from Threader indicating if thread is paused
   **/
  public Integer  paused;
  
  /**
   * the unique name of the thread, set by Threader
   **/
  String name;
}

