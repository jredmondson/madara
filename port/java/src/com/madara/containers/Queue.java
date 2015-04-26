/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara.containers;

import java.util.Arrays;
import com.madara.MadaraJNI;
import com.madara.KnowledgeBase;
import com.madara.Variables;
import com.madara.KnowledgeRecord;

/**
 * A facade for a dynamically typed vector within a knowledge base.
 **/

public class Queue extends MadaraJNI
{	
  private native long jni_Queue();
  private native long jni_Queue(long cptr);
  private static native void jni_freeQueue(long cptr);
  private native boolean jni_enqueue(long cptr, long record);
  private native boolean jni_enqueueDouble(long cptr, double value);
  private native boolean jni_enqueueLong(long cptr, long value);
  private native boolean jni_enqueueString(long cptr, java.lang.String value);
  private native long jni_dequeue(long cptr, boolean waitForRecord);
  private native long jni_inspect(long cptr, int position);
  private native java.lang.String jni_getName(long cptr);
  private native void jni_setName(long cptr, long type, long kb, java.lang.String name);
  private native long jni_size(long cptr);
  private native long jni_count(long cptr);
  private native void jni_resize(long cptr, long length);
  private native void jni_clear(long cptr);

  private boolean manageMemory = true;
  
  /**
   * Default constructor
   **/
  public Queue()
  {
    setCPtr(jni_Queue());
  }

  /**
   * Copy constructor
   * @param input  instance to copy
   **/
  public Queue(Queue input)
  {
    setCPtr(jni_Queue(input.getCPtr()));
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @return a new java instance of the underlying pointer
   */
  public static Queue fromPointer(long cptr)
  {
    Queue ret = new Queue();
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
  public static Queue fromPointer(long cptr, boolean shouldManage)
  {
    Queue ret = new Queue();
    ret.manageMemory=shouldManage;
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Inspects the record at the specified position
   *
   * @param  position   position in the queue to inspect
   * @return   the record at the position (uncreated record if position is inaccessible)
   */
  public KnowledgeRecord inspect(int position)
  {
    return KnowledgeRecord.fromPointer(jni_inspect(getCPtr(), position));
  }

  /**
   * Attempts to dequeue a record
   * @param  waitForRecord   if true, block on a record being added to queue
   * @return  next record in the queue. An uncreated record if empty and
   * waitForRecord is false.
   */
  public KnowledgeRecord dequeue(boolean waitForRecord)
  {
    return KnowledgeRecord.fromPointer(jni_dequeue(getCPtr(), waitForRecord));
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
   * Resizes the vector
   *
   * @param  length   new number of elements of the vector
   */
  public void resize (long length)
  {
    jni_resize(getCPtr(), length);
  }
  
  /**
   * Attempts to enqueue a record
   *
   * @param  record  the new record to place on the queue
   * @return true if the queue now contains the record. False is returned if
   *         there was not enough room in the queue.
   * 
   */
  public boolean enqueue(KnowledgeRecord record)
  {
    return jni_enqueue(getCPtr(), record.getCPtr());
  }
  
  /**
   * Attempts to enqueue a double
   *
   * @param  value  the new value to place on the queue
   * @return true if the queue now contains the record. False is returned if
   *         there was not enough room in the queue.
   * 
   */
  public boolean enqueue(double value)
  {
    return jni_enqueueDouble(getCPtr(), value);
  }
  
  /**
   * Attempts to enqueue a long
   *
   * @param  value  the new value to place on the queue
   * @return true if the queue now contains the record. False is returned if
   *         there was not enough room in the queue.
   * 
   */
  public boolean enqueue(long value)
  {
    return jni_enqueueLong(getCPtr(), value);
  }
  
  /**
   * Attempts to enqueue a string
   *
   * @param  value  the new value to place on the queue
   * @return true if the queue now contains the record. False is returned if
   *         there was not enough room in the queue.
   * 
   */
  public boolean enqueue(java.lang.String value)
  {
    return jni_enqueueString(getCPtr(), value);
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
   * Returns the maximum size of the queue
   *
   * @return  the maximum number of records in the queue
   */
  public long size()
  {
    return jni_size(getCPtr());
  }
  
  /**
   * Returns the number of records currently in the queue
   *
   * @return  the number of elements in the queue
   */
  public long count()
  {
    return jni_count(getCPtr());
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance gets garbage collected
   */
  public void free()
  {
    if (manageMemory)
    {
      jni_freeQueue(getCPtr());
      setCPtr(0);
    }
  }
}

