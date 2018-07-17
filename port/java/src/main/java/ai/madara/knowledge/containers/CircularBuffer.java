/*********************************************************************
 * Copyright (c) 2013-2018 Carnegie Mellon University. All Rights Reserved.
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

import ai.madara.MadaraJNI;
import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.knowledge.KnowledgeRecord;
import ai.madara.knowledge.UpdateSettings;
import ai.madara.knowledge.Variables;

/**
 * A facade for a dynamically typed vector within a knowledge base.
 **/

public class CircularBuffer extends MadaraJNI
{
  private native long jni_CircularBuffer();
  private native long jni_CircularBuffer(long cptr);
  private static native void jni_freeCircularBuffer(long cptr);
  private native void jni_addRecord(long cptr, long record);
  private native void jni_addRecordVector(long cptr, long[] recordVector);
  private native long jni_getRecord(long cptr);
  private native Object[] jni_getEarliestRecordVector(long cptr, int count);
  private native Object[] jni_getLatestRecordVector(long cptr, int count);
  private native long jni_inspectRecord(long cptr, int position);
  private native Object[] jni_inspectRecordVector(
    long cptr, int position, int count);
  private native java.lang.String jni_getName(long cptr);
  private native void jni_setName(long cptr, long type, long kb,
    java.lang.String name);
  private native long jni_size(long cptr);
  private native void jni_clear(long cptr);
  private native long jni_count(long cptr);
  private native void jni_resize(long cptr, long length);
  private native void jni_setSettings(long cptr, long settings);

  private boolean manageMemory = true;

  /**
   * Default constructor
   **/
  public CircularBuffer()
  {
    setCPtr(jni_CircularBuffer());
  }

  /**
   * Copy constructor
   * @param input  instance to copy
   **/
  public CircularBuffer(CircularBuffer input)
  {
    setCPtr(jni_CircularBuffer(input.getCPtr()));
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @return a new java instance of the underlying pointer
   */
  public static CircularBuffer fromPointer(long cptr)
  {
    CircularBuffer ret = new CircularBuffer();
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
  public static CircularBuffer fromPointer(long cptr, boolean shouldManage)
  {
    CircularBuffer ret = new CircularBuffer();
    ret.manageMemory=shouldManage;
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Inspects the record at the specified position
   *
   * @param  position   position in the queue to inspect
   * @return   the record at the position (uncreated record
   *           if position is inaccessible)
   */
  public KnowledgeRecord inspect(int position) throws MadaraDeadObjectException
  {
    return KnowledgeRecord.fromPointer(jni_inspectRecord(getCPtr(), position));
  }

  /**
   * Inspects the record at the specified position
   *
   * @param  position   position in the queue to inspect
   * @param  count      max number of elements to return
   * @return   the record at the position (uncreated record
   *           if position is inaccessible)
   */
  public KnowledgeRecord [] inspect(int position, int count)
    throws MadaraDeadObjectException
  {
    Object[] objs = jni_inspectRecordVector(getCPtr(), position, count);
    KnowledgeRecord[] records = new KnowledgeRecord[objs.length];
    for (int i = 0; i < objs.length; ++i)
    {
      records[i] = (KnowledgeRecord)objs[i];
    }
    return records;
  }

  /**
   * Attempts to dequeue a record
   * @return  next record in the queue. An uncreated record if empty and
   * waitForRecord is false.
   */
  public KnowledgeRecord get() throws MadaraDeadObjectException
  {
    return KnowledgeRecord.fromPointer(jni_getRecord(getCPtr()));
  }

  /**
   * Gets earliest records in the buffer up to a certain count
   *
   * @param  count      max number of elements to return
   * @return   the record at the position (uncreated record
   *           if position is inaccessible)
   */
  public KnowledgeRecord [] get_earliest(int count)
    throws MadaraDeadObjectException
  {
    Object[] objs = jni_getEarliestRecordVector(getCPtr(), count);
    KnowledgeRecord[] records = new KnowledgeRecord[objs.length];
    for (int i = 0; i < objs.length; ++i)
    {
      records[i] = (KnowledgeRecord)objs[i];
    }
    return records;
  }

  /**
   * Gets latest records in the buffer up to a certain count
   *
   * @param  count      max number of elements to return
   * @return   the record at the position (uncreated record
   *           if position is inaccessible)
   */
  public KnowledgeRecord [] get_latest(int count)
    throws MadaraDeadObjectException
  {
    Object[] objs = jni_getLatestRecordVector(getCPtr(), count);
    KnowledgeRecord[] records = new KnowledgeRecord[objs.length];
    for (int i = 0; i < objs.length; ++i)
    {
      records[i] = (KnowledgeRecord)objs[i];
    }
    return records;
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
   * Resizes the vector
   *
   * @param  length   new number of elements of the vector
   */
  public void resize (long length) throws MadaraDeadObjectException
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
  public void add(KnowledgeRecord record) throws MadaraDeadObjectException
  {
    jni_addRecord(getCPtr(), record.getCPtr());
  }

  /**
   * Attempts to enqueue a double
   *
   * @param  value  the new value to place on the queue
   * @return true if the queue now contains the record. False is returned if
   *         there was not enough room in the queue.
   *
   */
  public void add(KnowledgeRecord[] records)
    throws MadaraDeadObjectException
  {
    long [] record_ptrs = new long [records.length];

    for (int i = 0; i < records.length; ++i)
    {
      record_ptrs[i] = records[i].getCPtr();
    }

    jni_addRecordVector(getCPtr(), record_ptrs);
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
   * Returns the maximum size of the queue
   *
   * @return  the maximum number of records in the queue
   */
  public long size() throws MadaraDeadObjectException
  {
    return jni_size(getCPtr());
  }

  /**
   * Returns the number of records currently in the queue
   *
   * @return  the number of elements in the queue
   */
  public long count() throws MadaraDeadObjectException
  {
    return jni_count(getCPtr());
  }

  /**
   * Clears the buffer of all records and resets index
   */
  public void clear()
  {
    jni_clear(getCPtr());
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance gets garbage collected
   */
  public void free()
  {
    if (manageMemory)
    {
      jni_freeCircularBuffer(getCPtr());
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

