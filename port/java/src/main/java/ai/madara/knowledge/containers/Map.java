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
 * A facade for a map of variable names to values within a knowledge base
 **/

public class Map extends BaseContainer
{
  private native long jni_Map();
  private native long jni_Map(long cptr);
  private static native void jni_freeMap(long cptr);
  private native void jni_clear(long cptr,
    boolean erase_variables);
  private native void jni_erase(long cptr,
    java.lang.String key);
  private native void jni_setString(long cptr,
    java.lang.String key, java.lang.String value);
  private native void jni_setDouble(long cptr,
    java.lang.String key, double value);
  private native void jni_set(long cptr,
    java.lang.String key, long type, long value);
  private native java.lang.String jni_getDelimiter(long cptr);
  private native void jni_setDelimiter(long cptr, java.lang.String delimiter);
  private native java.lang.String jni_getName(long cptr);
  private native void jni_setName(long cptr,
    long type, long kb, java.lang.String name);
  private native long jni_get(long cptr, java.lang.String key);
  private native long jni_toRecord(long cptr, java.lang.String key);
  private native java.lang.String[] jni_keys(long cptr);
  private native void jni_sync(long cptr);
  private native void jni_modify(long cptr);
  private native void jni_modifyIndex(long cptr, java.lang.String index);
  private native void jni_setSettings(long cptr, long settings);
  private native boolean jni_isTrue(long cptr);
  private native boolean jni_isFalse(long cptr);

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
  public KnowledgeRecord get(java.lang.String key) throws MadaraDeadObjectException
  {
    return KnowledgeRecord.fromPointer(jni_toRecord(getCPtr(), key));
  }

  /**
   * Gets the name of the variable
   *
   * @return  name of the variable within the context
   */
  public java.lang.String getName() throws MadaraDeadObjectException
  {
    return jni_getName(getCPtr());
  }

  /**
   * Gets the delimiter that separates the name of the map with its elements
   *
   * @return  the delimiter that separates map name with elements
   */
  public java.lang.String getDelimiter() throws MadaraDeadObjectException
  {
    return jni_getDelimiter(getCPtr());
  }

  /**
   * Sets the delimiter that separates the name of the map with its elements
   *
   * @param delimiter  the separator between the map and elements
   */
  public void setDelimiter(java.lang.String delimiter) throws MadaraDeadObjectException
  {
    jni_setDelimiter(getCPtr(), delimiter);
  }

  /**
   * Gets the current keys in the map
   *
   * @return  name of the variable within the context
   */
  public java.lang.String[] keys() throws MadaraDeadObjectException
  {
    return jni_keys(getCPtr());
  }


  /**
   * Syncs the map to the underlying knowledge base. Call this method
   * if you believe the map's keys may have changed in the knowledge base.
   */
  public void sync() throws MadaraDeadObjectException
  {
    jni_sync(getCPtr());
  }

  /**
   * Mark the map as modified. The maps retains the same values
   * but will resend all values as if they had been modified.
   **/
  public void modify() throws MadaraDeadObjectException
  {
    jni_modify(getCPtr());
  }

  /**
   * Returns true if the container evaluates to true
   * @return true if container has all true values
   **/
  public boolean isTrue() throws MadaraDeadObjectException
  {
    return jni_isTrue(getCPtr());
  }

  /**
   * Returns true if the container evaluates to false
   * @return true if container has any false values or is uninitialized
   **/
  public boolean isFalse() throws MadaraDeadObjectException
  {
    return jni_isFalse(getCPtr());
  }

  /**
   * Mark an element as modified. The element retains the same value
   * but will resend the value as if it had been modified.
   * @param key  the element key
   **/
  public void modify(java.lang.String key) throws MadaraDeadObjectException
  {
    jni_modifyIndex(getCPtr(), key);
  }

  /**
   * Clears the variables in the map
   * @param clear_variables  if true, clear the variables from the knowledge
   *                         base as well as the map
   **/
  public void clear(boolean clear_variables) throws MadaraDeadObjectException
  {
    jni_clear(getCPtr(), clear_variables);
  }

  /**
   * Erases a variable located at key in the map. This also deletes the variable
   * from the knowledge base.
   * @param key  the element key
   **/
  public void erase(java.lang.String key) throws MadaraDeadObjectException
  {
    jni_erase(getCPtr(), key);
  }

  /**
   * Sets the value
   *
   * @param  key  the location in the map
   * @param  value   new value
   */
  public void set(java.lang.String key, java.lang.String value) throws MadaraDeadObjectException
  {
    jni_setString(getCPtr(), key, value);
  }

  /**
   * Sets the value
   *
   * @param  key  the location in the map
   * @param  value   new value
   */
  public void set(java.lang.String key, double value) throws MadaraDeadObjectException
  {
    jni_setDouble(getCPtr(), key, value);
  }

  /**
   * Sets the value
   *
   * @param  key  the location in the map
   * @param  value   new value
   */
  public void set(java.lang.String key, long value) throws MadaraDeadObjectException
  {
    jni_set(getCPtr(), key, 0, value);
  }

  /**
   * Sets the value
   *
   * @param  key  the location in the map
   * @param  value   new value
   */
  public void set(java.lang.String key, KnowledgeRecord value) throws MadaraDeadObjectException
  {
    jni_set(getCPtr(), key, 1, value.getCPtr ());
  }

  /**
   * Sets the name and knowledge base being referred to
   *
   * @param  kb      the knowledge base that contains the name
   * @param  name    the variable name
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
   */
  public void setName(Variables vars, java.lang.String name) throws MadaraDeadObjectException
  {
    jni_setName(getCPtr(), 1, vars.getCPtr (), name);
  }

  /**
   * Sets the settings for updating variables in the Knowledge Base
   *
   * @param  settings  the settings to use for updating the Knowledge Base
   */
  public void setSettings(UpdateSettings settings) throws MadaraDeadObjectException
  {
    jni_setSettings(getCPtr(), settings.getCPtr());
  }

  /**
   * Returns a value at the specified key
   *
   * @param  key  the location in the map
   * @return the value at the index as a knowledge record
   */
  public KnowledgeRecord toRecord(java.lang.String key) throws MadaraDeadObjectException
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

