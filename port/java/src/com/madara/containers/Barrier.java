/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara.containers;

import com.madara.MadaraJNI;
import com.madara.KnowledgeBase;
import com.madara.Variables;

/**
 * A facade for a distributed barrier within a knowledge base
 **/

public class Barrier extends MadaraJNI
{	
  private native long jni_Barrier();
  private native long jni_Barrier(long cptr);
  private static native void jni_freeBarrier(long cptr);
  private native void jni_set(long cptr, long value);
  private native java.lang.String jni_getName(long cptr);
  private native void jni_setName(long cptr, long type, long kb, java.lang.String name);
  private native java.lang.String jni_toString(long cptr);
  private native double jni_toDouble(long cptr);
  private native long jni_toLong(long cptr);
  private native long jni_next(long cptr);
  private native boolean jni_isDone(long cptr);
  private native void jni_modify(long cptr);
  private native void jni_resize(long cptr, int id, int participants);

  private boolean manageMemory = true;

  /**
   * Default constructor
   **/
  public Barrier()
  {
    setCPtr(jni_Barrier());
  }

  /**
   * Copy constructor
   * @param input  instance to copy
   **/
  public Barrier(Barrier input)
  {
    setCPtr(jni_Barrier(input.getCPtr()));
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @return a new java instance of the underlying pointer
   */
  public static Barrier fromPointer(long cptr)
  {
    Barrier ret = new Barrier();
    ret.manageMemory = true;
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @param shouldManage  if true, manage the pointer
   * @return a new java instance of the underlying pointer
   */
  public static Barrier fromPointer(long cptr, boolean shouldManage)
  {
    Barrier ret = new Barrier();
    ret.manageMemory=shouldManage;
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Gets the value
   *
   * @return   current value
   */
  public long get()
  {
    return jni_toLong(getCPtr());
  }

  /**
   * Gets the name of the variable
   *
   * @return  name of the variable within the context
   */
  public java.lang.String getName()
  {
    return jni_getName(getCPtr());
  }

  /**
   * Moves to the next barrier
   */
  public void next()
  {
    jni_next(getCPtr());
  }

  /**
   * Checks to see if the barrier round is done
   * @return new value of the container
   */
  public boolean isDone()
  {
    return jni_isDone(getCPtr());
  }

  /**
   * Mark the value as modified. The Barrier retains the same value
   * but will resend its value as if it had been modified.
   **/
  public void modify()
  {
    jni_modify(getCPtr());
  }
      
  /**
   * Resizes the barrier, usually when number of participants change
   * @param id        the id of this barrier in the barrier ring
   * @param participants the number of participants in barrier ring
   **/
  public void resize(int id, int participants)
  {
   jni_resize(getCPtr(), id, participants); 
  }
  
  /**
   * Sets the name and knowledge base being referred to
   *
   * @param  kb      the knowledge base that contains the name
   * @param  name    the variable name
   */
  public void setName(KnowledgeBase kb, java.lang.String name)
  {
    jni_setName(getCPtr(), 0, kb.getCPtr (), name);
  }

  /**
   * Sets the name and knowledge base being referred to
   *
   * @param  vars    the variables facade that contains the name
   * @param  name    the variable name
   */
  public void setName(Variables vars, java.lang.String name)
  {
    jni_setName(getCPtr(), 1, vars.getCPtr (), name);
  }

  /**
   * Converts the value to a double
   *
   * @return current double value
   */
  public double toDouble()
  {
    return jni_toDouble(getCPtr());
  }

  /**
   * Converts the value to a long
   *
   * @return current long value
   */
  public long toLong()
  {
    return jni_toLong(getCPtr());
  }

  /**
   * Converts the value to a string
   *
   * @return current string value
   */
  public java.lang.String toString()
  {
    return jni_toString(getCPtr());
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance gets garbage collected
   */
  public void free()
  {
    if (manageMemory)
    {
      jni_freeBarrier(getCPtr());
      setCPtr(0);
    }
  }
}

