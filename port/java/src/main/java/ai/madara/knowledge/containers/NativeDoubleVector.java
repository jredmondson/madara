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
package ai.madara.knowledge.containers;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.knowledge.KnowledgeRecord;
import ai.madara.knowledge.UpdateSettings;
import ai.madara.knowledge.Variables;

/**
 * A facade for an array of doubles within a single knowledge record
 * within a knowledge base. Use this type of container if you want to
 * have an atomic array, such as position coordinates, where changing
 * one element of the array requres all array elements to be sent as
 * an update.
 **/

public class NativeDoubleVector extends BaseContainer
{
  private native long jni_NativeDoubleVector();
  private native long jni_NativeDoubleVector(long cptr);
  private static native void jni_freeNativeDoubleVector(long cptr);
  private native void jni_set(long cptr, int index, double value);
  private native void jni_pushback(long cptr, double value);
  private native java.lang.String jni_getName(long cptr);
  private native void jni_setName(long cptr, long type, long kb, java.lang.String name);
  private native double jni_get(long cptr, int index);
  private native long jni_toRecord(long cptr, int index);
  private native long jni_toRecord(long cptr);
  private native Object[] jni_toArray(long cptr);
  private native long jni_size(long cptr);
  private native void jni_resize(long cptr, long length);
  private native void jni_modify(long cptr);
  private native void jni_setSettings(long cptr, long settings);
  private native boolean jni_isTrue(long cptr);
  private native boolean jni_isFalse(long cptr);

  private boolean manageMemory = true;

  /**
   * Default constructor
   **/
  public NativeDoubleVector()
  {
    setCPtr(jni_NativeDoubleVector());
  }

  /**
   * Copy constructor
   * @param input  instance to copy
   **/
  public NativeDoubleVector(NativeDoubleVector input)
  {
    setCPtr(jni_NativeDoubleVector(input.getCPtr()));
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @return a new java instance of the underlying pointer
   */
  public static NativeDoubleVector fromPointer(long cptr)
  {
    NativeDoubleVector ret = new NativeDoubleVector();
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
  public static NativeDoubleVector fromPointer(long cptr, boolean shouldManage)
  {
    NativeDoubleVector ret = new NativeDoubleVector();
    ret.manageMemory=shouldManage;
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Gets the value at a specified index
   * @param    index  index of the element to retrieve
   * @return   current value at the index
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public double get(int index) throws MadaraDeadObjectException
  {
    return jni_get(getCPtr(), index);
  }

  /**
   * Gets the name of the variable
   *
   * @return  name of the variable within the context
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public java.lang.String getName() throws MadaraDeadObjectException
  {
    return jni_getName(getCPtr());
  }

  /**
   * Resizes the vector
   *
   * @param  length   new number of elements of the vector
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public void resize (long length) throws MadaraDeadObjectException
  {
    jni_resize(getCPtr(), length);
  }

  /**
   * Mark the vector as modified. The vector retains the same values
   * but will resend all values as if they had been modified.
 * @throws MadaraDeadObjectException throws exception if object is already released 
   **/
  public void modify() throws MadaraDeadObjectException
  {
    jni_modify(getCPtr());
  }

  /**
   * Returns true if the container evaluates to true
   * @return true if container has all true values
 * @throws MadaraDeadObjectException throws exception if object is already released 
   **/
  public boolean isTrue() throws MadaraDeadObjectException
  {
    return jni_isTrue(getCPtr());
  }

  /**
   * Returns true if the container evaluates to false
   * @return true if container has any false values or is uninitialized
 * @throws MadaraDeadObjectException throws exception if object is already released 
   **/
  public boolean isFalse() throws MadaraDeadObjectException
  {
    return jni_isFalse(getCPtr());
  }

  /**
   * Sets the value at the index
   * @param  index   the index of the element to change
   * @param  value   new value
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public void set(int index, double value) throws MadaraDeadObjectException
  {
    jni_set(getCPtr(), index, value);
  }

  /**
   * Pushes a value to the end of the vector
   * @param  value   new value to add to vector
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public void pushback(double value) throws MadaraDeadObjectException
  {
    jni_pushback(getCPtr(), value);
  }

  /**
   * Sets the name and knowledge base being referred to
   *
   * @param  kb      the knowledge base that contains the name
   * @param  name    the variable name
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public void setName(KnowledgeBase kb, java.lang.String name) throws MadaraDeadObjectException
  {
    jni_setName(getCPtr(), 0, kb.getCPtr (), name);
  }

  /**
   * Sets the name and knowledge base being referred to
   *
   * @param  vars    the variables facade that contains the name
   * @param  name    the variable name
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public void setName(Variables vars, java.lang.String name) throws MadaraDeadObjectException
  {
    jni_setName(getCPtr(), 1, vars.getCPtr (), name);
  }

  /**
   * Sets the settings for updating variables in the Knowledge Base
   *
   * @param  settings  the settings to use for updating the Knowledge Base
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public void setSettings(UpdateSettings settings) throws MadaraDeadObjectException
  {
    jni_setSettings(getCPtr(), settings.getCPtr());
  }

  /**
   * Returns the size of the vector
   *
   * @return  the number of elements in the vector
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public long size () throws MadaraDeadObjectException
  {
    return jni_size(getCPtr());
  }

  /**
   * Returns a value at the specified index
   *
   * @return the vector as an array of records
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public KnowledgeRecord[] toArray() throws MadaraDeadObjectException
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
   * Returns a value at the specified index
   *
   * @param  index  the index
   * @return the value at the index as a knowledge record
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public KnowledgeRecord toRecord(int index) throws MadaraDeadObjectException
  {
    return KnowledgeRecord.fromPointer(jni_toRecord(getCPtr(), index));
  }

  /**
   * Returns all of the values as a knowledge record
   *
   * @return  knowledge record that contains all indices
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public KnowledgeRecord toRecord() throws MadaraDeadObjectException
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
      jni_freeNativeDoubleVector(getCPtr());
      setCPtr(0);
    }
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
}

