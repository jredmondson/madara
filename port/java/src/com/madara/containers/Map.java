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
 * A facade for a map of variable names to values within a knowledge base
 **/

public class Map extends MadaraJNI
{	
  private native long jni_Map();
  private native long jni_Map(long cptr);
  private static native void jni_freeMap(long cptr);
  private native void jni_setString(long cptr,
    java.lang.String key, java.lang.String value);
  private native void jni_setDouble(long cptr,
    java.lang.String key, double value);
  private native void jni_set(long cptr,
    java.lang.String key, long type, long value);
  private native java.lang.String jni_getName(long cptr);
  private native void jni_setName(long cptr,
    long type, long kb, java.lang.String name);
  private native long jni_get(long cptr, java.lang.String key);
  private native long jni_toRecord(long cptr, java.lang.String key);
  private native void jni_modify(long cptr);
  private native void jni_modifyIndex(long cptr, java.lang.String index);

  private boolean manageMemory = true;

  /**
   * Default constructor
   **/
  public Map()
  {
    setCPtr(jni_Map());
  }

  /**
   * Copy constructor
   * @param input  instance to copy
   **/
  public Map(Map input)
  {
    setCPtr(jni_Map(input.getCPtr()));
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @return a new java instance of the underlying pointer
   */
  public static Map fromPointer(long cptr)
  {
    Map ret = new Map();
    ret.setCPtr(cptr);
    ret.manageMemory = true;
    return ret;
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @param shouldManage  if true, manage the pointer
   * @return a new java instance of the underlying pointer
   */
  public static Map fromPointer(long cptr, boolean shouldManage)
  {
    Map ret = new Map();
    ret.manageMemory=shouldManage;
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Gets the value
   *
   * @param  key  the location in the map
   * @return   current value
   */
  public KnowledgeRecord get(java.lang.String key)
  {
    return KnowledgeRecord.fromPointer(jni_toRecord(getCPtr(), key));
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
   * Mark the map as modified. The maps retains the same values
   * but will resend all values as if they had been modified.
   **/
  public void modify()
  {
    jni_modify(getCPtr());
  }
  
  /**
   * Mark an element as modified. The element retains the same value
   * but will resend the value as if it had been modified.
   * @param key  the element key
   **/
  public void modify(java.lang.String key)
  {
    jni_modifyIndex(getCPtr(), key);
  }
  
  /**
   * Sets the value
   *
   * @param  key  the location in the map
   * @param  value   new value
   */
  public void set(java.lang.String key, java.lang.String value)
  {
    jni_setString(getCPtr(), key, value);
  }

  /**
   * Sets the value
   *
   * @param  key  the location in the map
   * @param  value   new value
   */
  public void setDouble(java.lang.String key, double value)
  {
    jni_setDouble(getCPtr(), key, value);
  }

  /**
   * Sets the value
   *
   * @param  key  the location in the map
   * @param  value   new value
   */
  public void set(java.lang.String key, long value)
  {
    jni_set(getCPtr(), key, 0, value);
  }

  /**
   * Sets the value
   *
   * @param  key  the location in the map
   * @param  value   new value
   */
  public void set(java.lang.String key, KnowledgeRecord value)
  {
    jni_set(getCPtr(), key, 1, value.getCPtr ());
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
   * Returns a value at the specified key
   *
   * @param  key  the location in the map
   * @return the value at the index as a knowledge record
   */
  public KnowledgeRecord toRecord(java.lang.String key)
  {
    return KnowledgeRecord.fromPointer(jni_toRecord(getCPtr(), key));
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance gets garbage collected
   */
  public void free()
  {
    if (manageMemory)
    {
      jni_freeMap(getCPtr());
    }
  }
}

