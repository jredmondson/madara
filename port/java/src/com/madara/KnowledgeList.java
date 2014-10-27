/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara;

import java.util.AbstractList;

/**
 * KnowledgeList provides a read-only interface for KnowledgeRecords
 */
public class KnowledgeList extends AbstractList<KnowledgeRecord>
{
  private native void jni_freeKnowledgeList(long[] records, int length);

  private long[] knowledgeRecords;

  public KnowledgeList(long[] records)
  {
    knowledgeRecords = records;
  }

  /**
   * @see java.util.AbstractList#get (int)
   */
  @Override
  public KnowledgeRecord get(int index)
  {
    return KnowledgeRecord.fromPointer(knowledgeRecords[index]);
  }

  /**
   * @see java.util.AbstractCollection#size ()
   */
  @Override
  public int size()
  {
    return knowledgeRecords == null ? 0 : knowledgeRecords.length;
  }

  /**
   * Returns the underlying C++ pointer array (useful for JNI)
   * @return  the C++ pointer array underneath this instance
   **/
  public long[] toPointerArray ()
  {
    return knowledgeRecords;
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be called
   * before an instance of {@link com.madara.KnowledgeList KnowledgeList} gets garbage collected
   */
  public void free()
  {
    if (knowledgeRecords == null)
      return;
    jni_freeKnowledgeList(knowledgeRecords, knowledgeRecords.length);
    knowledgeRecords = null;
  }
}
