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
package ai.madara.transport;

import ai.madara.knowledge.KnowledgeMap;
import ai.madara.knowledge.KnowledgeRecord;
import ai.madara.MadaraJNI;

public class TransportContext extends MadaraJNI
{
  private static native void jni_addRecord(long cptr, String key, long record);
  private static native void jni_clearRecords(long cptr);
  private static native long jni_getCurrentTime(long cptr);
  private static native String jni_getDomain(long cptr);
  private static native long jni_getMessageTime(long cptr);
  private static native long jni_getOperation(long cptr);
  private static native String jni_getOriginator(long cptr);
  private static native long jni_getReceiveBandwidth(long cptr);
  private static native void jni_getRecords(long cptr, MapReturn ret);
  private static native long jni_getSendBandwidth(long cptr);

  private KnowledgeMap currentMap = null;

  private TransportContext(long cptr)
  {
    setCPtr(cptr);
  }

  public static TransportContext fromPointer(long cptr)
  {
    return new TransportContext(cptr);
  }

  /**
   * Adds a record to the list that should be appended to send or rebroadcast.
   * @param key key of the record to add
   * @param record the record to add
   */
  public void addRecord(String key, KnowledgeRecord record)
  {
    KnowledgeRecord value = record;
    if (value != null && !value.isNew())
      value = record.clone();
    jni_addRecord(getCPtr(), key, record == null ? 0 : record.getCPtr());
  }

  /**
   * Clears records added through filtering operations.
   */
  public void clearRecords()
  {
    jni_clearRecords(getCPtr());
  }

  /**
   * Gets the current timestamp
   * @return the current time stamp
   */
  public long getCurrentTime()
  {
    return jni_getCurrentTime(getCPtr());
  }

  /**
   * Gets the current network domain
   * @return the network domain
   */
  public String getDomain()
  {
    return jni_getDomain(getCPtr());
  }

  /**
   * Gets the message timestamp
   * @return the message timestamp
   */
  public long getMessageTime()
  {
    return jni_getMessageTime(getCPtr());
  }

  /**
   * Get operation that the context is performing
   * @return operation type that is being performed
   */
  public long getOperation()
  {
    return jni_getOperation(getCPtr());
  }

  /**
   * Returns the current message originator
   * @return the current message originator
   */
  public String getOriginator()
  {
    return jni_getOriginator(getCPtr());
  }

  /**
   * Gets the receive bandwidth in bytes per second
   * @return the receive bandwidth in bytes per second
   */
  public long getReceiveBandwidth()
  {
    return jni_getReceiveBandwidth(getCPtr());
  }

  /**
   * Returns the additional records stored in the context. Do not free the resulting map
   * @return the additional records stored in the context
   */
  private KnowledgeMap getRecords()
  {
    MapReturn jniRet = new MapReturn();
    jni_getRecords(getCPtr(), jniRet);
    System.err.println("Got the records: " + jniRet.keys.length);
//    return new KnowledgeMap(jniRet.keys, jniRet.vals);
    return null;
  }

  /**
   * Gets the send/rebroadcast bandwidth in bytes per second
   * @return the send/rebroadcast bandwidth in bytes per second
   */
  public long getSendBandwidth()
  {
    return jni_getSendBandwidth(getCPtr());
  }

  private static class MapReturn
  {
    public String[] keys;
    public long[] vals;
  }
}

