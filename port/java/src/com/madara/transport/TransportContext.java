/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara.transport;

import com.madara.KnowledgeMap;
import com.madara.KnowledgeRecord;
import com.madara.MadaraJNI;

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

