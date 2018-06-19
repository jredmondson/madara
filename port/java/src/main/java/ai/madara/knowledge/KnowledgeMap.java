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
package ai.madara.knowledge;

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
   * before an instance of {@link ai.madara.knowledge.KnowledgeMap KnowledgeMap} gets garbage collected
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

