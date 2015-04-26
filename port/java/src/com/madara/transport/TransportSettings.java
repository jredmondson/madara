/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara.transport;

import com.madara.MadaraJNI;

public class TransportSettings extends MadaraJNI
{
  private native long jni_Settings();
  private native long jni_Settings(long cptr);
  private static native void jni_freeSettings(long cptr);

  public TransportSettings()
  {
    setCPtr(jni_Settings());
  }

  public TransportSettings(TransportSettings transportSettings)
  {
    setCPtr(jni_Settings(transportSettings.getCPtr()));
  }

  private native void jni_setDomains(long cptr, String domains);
  private native String jni_getDomains(long cptr);
  private native void jni_setQueueLength(long cptr, int queue_length);
  private native int jni_getQueueLength(long cptr);
  private native void jni_setDeadline(long cptr, int deadline);
  private native int jni_getDeadline(long cptr);
  private native void jni_setType(long cptr, int type);
  private native int jni_getType(long cptr);
  private native void jni_setReliability(long cptr, int reliability);
  private native int jni_getReliability(long cptr);
  private native void jni_setId(long cptr, int id);
  private native int jni_getId(long cptr);
  private native void jni_setProcesses(long cptr, int id);
  private native int jni_getProcesses(long cptr);
  private native void jni_setOnDataReceivedLogic(long cptr, String onDataReceivedLogic);
  private native String jni_getOnDataReceivedLogic(long cptr);
  private native void jni_setHosts(long cptr, String[] hosts);
  private native String[] jni_getHosts(long cptr);


  public void setDomains(String domains)
  {
    jni_setDomains(getCPtr(), domains);
  }

  public String getDomains()
  {
    return jni_getDomains(getCPtr());
  }

  public void setQueueLength(int queueLength)
  {
    jni_setQueueLength(getCPtr(), queueLength);
  }

  public int getQueueLength()
  {
    return jni_getQueueLength(getCPtr());
  }

  public void setDeadline(int deadline)
  {
    jni_setDeadline(getCPtr(), deadline);
  }

  public int getDeadline()
  {
    return jni_getDeadline(getCPtr());
  }

  public void setType(TransportType transport)
  {
    jni_setType(getCPtr(), transport.value());
  }

  public TransportType getType()
  {
    return TransportType.getType(jni_getType(getCPtr()));
  }

  public void setReliability(TransportReliability r)
  {
    jni_setReliability(getCPtr(), r.value());
  }

  public TransportReliability getReliability()
  {
    return TransportReliability.getReliability(jni_getReliability(getCPtr()));
  }

  public void setId(int id)
  {
    jni_setId(getCPtr(), id);
  }

  public int getId()
  {
    return jni_getId(getCPtr());
  }

  public void setProcesses(int processes)
  {
    jni_setProcesses(getCPtr(), processes);
  }

  public int getProcesses()
  {
    return jni_getProcesses(getCPtr());
  }

  public void setOnDataReceivedLogic(String onDataReceivedLogic)
  {
    jni_setOnDataReceivedLogic(getCPtr(), onDataReceivedLogic);
  }

  public String getOnDataReceivedLogic()
  {
    return jni_getOnDataReceivedLogic(getCPtr());
  }

  public void setHosts(String[] hosts)
  {
    jni_setHosts(getCPtr(), hosts);
  }

  public String[] getHosts()
  {
    return jni_getHosts(getCPtr());
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be called
   * before an instance of WaitSettings gets garbage collected
   */
  public void free()
  {
    jni_freeSettings(getCPtr());
    setCPtr(0);
  }
}

