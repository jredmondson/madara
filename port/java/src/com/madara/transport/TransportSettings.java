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
package com.madara.transport;

import com.madara.MadaraJNI;

public class TransportSettings extends MadaraJNI {

  private native long jni_Settings();

  private native long jni_Settings(long cptr);

  private static native void jni_freeSettings(long cptr);

  public TransportSettings() {
    setCPtr(jni_Settings());
  }

  public TransportSettings(TransportSettings transportSettings) {
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

  private native void jni_save(long cptr, java.lang.String filenmae);

  private native void jni_load(long cptr, java.lang.String filename);

  /**
   * Saves the transport settings as a KnowledgeBase to a file
   *
   * @param filename the file to save the knowledge base to
   *
   */
  public void save(String filename) {
    jni_save(getCPtr(), filename);
  }

  /**
   * Loads the transport settings from a KnowledgeBase context file
   *
   * @param filename the file to save the knowledge base to
   *
   */
  public void load(String filename) {
    jni_load(getCPtr(), filename);
  }

  public void setDomains(String domains) {
    jni_setDomains(getCPtr(), domains);
  }

  public String getDomains() {
    return jni_getDomains(getCPtr());
  }

  public void setQueueLength(int queueLength) {
    jni_setQueueLength(getCPtr(), queueLength);
  }

  public int getQueueLength() {
    return jni_getQueueLength(getCPtr());
  }

  public void setDeadline(int deadline) {
    jni_setDeadline(getCPtr(), deadline);
  }

  public int getDeadline() {
    return jni_getDeadline(getCPtr());
  }

  public void setType(TransportType transport) {
    jni_setType(getCPtr(), transport.value());
  }

  public TransportType getType() {
    return TransportType.getType(jni_getType(getCPtr()));
  }

  public void setReliability(TransportReliability r) {
    jni_setReliability(getCPtr(), r.value());
  }

  public TransportReliability getReliability() {
    return TransportReliability.getReliability(jni_getReliability(getCPtr()));
  }

  public void setId(int id) {
    jni_setId(getCPtr(), id);
  }

  public int getId() {
    return jni_getId(getCPtr());
  }

  public void setProcesses(int processes) {
    jni_setProcesses(getCPtr(), processes);
  }

  public int getProcesses() {
    return jni_getProcesses(getCPtr());
  }

  public void setOnDataReceivedLogic(String onDataReceivedLogic) {
    jni_setOnDataReceivedLogic(getCPtr(), onDataReceivedLogic);
  }

  public String getOnDataReceivedLogic() {
    return jni_getOnDataReceivedLogic(getCPtr());
  }

  public void setHosts(String[] hosts) {
    jni_setHosts(getCPtr(), hosts);
  }

  public String[] getHosts() {
    return jni_getHosts(getCPtr());
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance of WaitSettings gets garbage collected
   */
  public void free() {
    jni_freeSettings(getCPtr());
    setCPtr(0);
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
