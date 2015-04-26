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
import com.madara.KnowledgeRecord;

/**
 * A facade for an array of integers within a single knowledge record
 * within a knowledge base. Use this type of container if you want to
 * have an atomic array, such as position coordinates, where changing
 * one element of the array requres all array elements to be sent as
 * an update.
 **/

public class NativeIntegerVector extends MadaraJNI
{	
  private native long jni_NativeIntegerVector();
  private native long jni_NativeIntegerVector(long cptr);
  private static native void jni_freeNativeIntegerVector(long cptr);
  private native void jni_set(long cptr, int index, long value);
  private native java.lang.String jni_getName(long cptr);
  private native void jni_setName(long cptr, long type, long kb, java.lang.String name);
  private native long jni_get(long cptr, int index);
  private native long jni_toRecord(long cptr, int index);
  private native long jni_toRecord(long cptr);
  private native Object[] jni_toArray(long cptr);
  private native long jni_size(long cptr);
  private native void jni_resize(long cptr, long length);
  private native void jni_modify(long cptr);

  private boolean manageMemory = true;

  public NativeIntegerVector()
  {
    setCPtr(jni_NativeIntegerVector());
  }

  public NativeIntegerVector(NativeIntegerVector input)
  {
    setCPtr(jni_NativeIntegerVector(input.getCPtr()));
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @return a new java instance of the underlying pointer
   */
  public static NativeIntegerVector fromPointer(long cptr)
  {
    NativeIntegerVector ret = new NativeIntegerVector();
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
  public static NativeIntegerVector fromPointer(long cptr, boolean shouldManage)
  {
    NativeIntegerVector ret = new NativeIntegerVector();
    ret.manageMemory=shouldManage;
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Gets the value at the index
   * @param    index   the index of the element to retrieve
   * @return   current value at the index
   */
  public long get(int index)
  {
    return jni_get(getCPtr(), index);
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
   * Sets the value at the specified index
   * @param  index   index of the element in the array to change
   * @param  value   new value
   */
  public void set(int index, long value)
  {
    jni_set(getCPtr(), index, value);
  }

  /**
   * Mark the vector as modified. The vector retains the same values
   * but will resend all values as if they had been modified.
   **/
  public void modify()
  {
    jni_modify(getCPtr());
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
   * Returns the size of the vector
   *
   * @return  the number of elements in the vector
   */
  public long size ()
  {
    return jni_size(getCPtr());
  }
  
  /**
   * Returns a value at the specified index
   *
   * @param  index  the index 
   * @return the value at the index as a knowledge record
   */
  public KnowledgeRecord toRecord(int index)
  {
    return KnowledgeRecord.fromPointer(jni_toRecord(getCPtr(), index));
  }

  /**
   * Returns a value at the specified index
   *
   * @return the vector as an array of records
   */
  public com.madara.KnowledgeRecord[] toArray()
  {
    Object[] objs = jni_toArray(getCPtr());
    KnowledgeRecord[] records = new KnowledgeRecord[objs.length];
    for (int i = 0; i < objs.length; ++i)
    {
      records[i] = (KnowledgeRecord)objs[i];
    }
    return records;
  }

  /**
   * Returns all of the values as a knowledge record
   *
   * @return  knowledge record that contains all indices
   */
  public KnowledgeRecord toRecord()
  {
    return KnowledgeRecord.fromPointer(jni_toRecord(getCPtr()));
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance gets garbage collected
   */
  public void free()
  {
    if (manageMemory)
    {
      jni_freeNativeIntegerVector(getCPtr());
      setCPtr(0);
    }
  }
}

