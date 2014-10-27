/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara;

import java.util.AbstractMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public class KnowledgeMap extends AbstractMap<String, KnowledgeRecord>
{

  private native void jni_freeKnowledgeMap(long[] ptrs, int length);

  private Set<Map.Entry<String, KnowledgeRecord>> mySet;
  private final boolean freeable;

  public KnowledgeMap(String[] keys, long[] vals)
  {
    this(keys, vals, true);
  }

  public KnowledgeMap(String[] keys, long[] vals, boolean freeable)
  {
    this.freeable = freeable;

    if (keys == null || vals == null || keys.length != vals.length)
      return;

    mySet = new HashSet<Map.Entry<String, KnowledgeRecord>>();

    for (int x = 0; x < keys.length; x++)
    {
      mySet.add(new KnowledgeMapEntry(keys[x], vals[x], freeable));
    }
  }

  /**
   * @see java.util.AbstractMap#entrySet ()
   */
  @Override
  public Set<Map.Entry<String, KnowledgeRecord>> entrySet()
  {
    return mySet;
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be called
   * before an instance of {@link com.madara.KnowledgeMap KnowledgeMap} gets garbage collected
   */
  public void free()
  {
    if (!freeable)
      return;
    long[] ptrs = new long[mySet == null ? 0 : mySet.size()];
    int pos = 0;
    for (Map.Entry<String, KnowledgeRecord> entry : mySet)
    {
      ptrs[pos++] = entry.getValue().getCPtr();
    }

    jni_freeKnowledgeMap(ptrs, ptrs.length);

    mySet = null;

  }

  private static class KnowledgeMapEntry implements Map.Entry<String, KnowledgeRecord>
  {

    private String key;
    private KnowledgeRecord record;

    private KnowledgeMapEntry(String key, long val, boolean isNew)
    {
      this.key = key;
      record = KnowledgeRecord.fromPointer(val, isNew);
    }

    /**
     * @see java.util.Map.Entry#getKey ()
     */
    public String getKey()
    {
      return key;
    }

    /**
     * @see java.util.Map.Entry#getValue ()
     */
    public KnowledgeRecord getValue()
    {
      return record;
    }

    /**
     * @see java.util.Map.Entry#setValue (java.lang.Object)
     */
    public KnowledgeRecord setValue(KnowledgeRecord value)
    {
      throw new UnsupportedOperationException("This map does not allow modification");
    }
  }
}

