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

import ai.madara.MadaraJNI;
import ai.madara.knowledge.KnowledgeType;
import ai.madara.knowledge.MadaraFunction;
import ai.madara.transport.filters.AggregateFilter;
import ai.madara.transport.filters.RecordFilter;
import ai.madara.filters.BufferFilter;

public class QoSTransportSettings extends TransportSettings {

  private native long jni_QoSTransportSettings();

  private native long jni_QoSTransportSettings(long cptr);

  private static native void jni_freeQoSTransportSettings(long cptr);

  private native void jni_addBufferFilter(long cptr, long filter);
  
  private native void jni_addBufferFilterObj(long cptr, BufferFilter filter);
  
  private native void jni_clearBufferFilters(long cptr);
  
  private native int jni_getNumberOfBufferFilters(long cptr);
  
  private native void jni_addRebroadcastRecordFilter(long cptr, int type, RecordFilter filter);

  private native void jni_addRebroadcastAggregateFilter(long cptr, AggregateFilter filter);

  private native void jni_addSendRecordFilter(long cptr, int type, RecordFilter filter);

  private native void jni_addSendAggregateFilter(long cptr, AggregateFilter filter);

  private native void jni_addReceiveRecordFilter(long cptr, int type, RecordFilter filter);

  private native void jni_addReceiveAggregateFilter(long cptr, AggregateFilter filter);

  private native void jni_setRebroadcastTtl(long cptr, int ttl);

  private native int jni_getRebroadcastTtl(long cptr);

  private native void jni_enableParticipantTtl(long cptr, int ttl);

  private native int jni_getParticipantTtl(long cptr);

  private native void jni_saveQoS(long cptr, java.lang.String filenmae);

  private native void jni_loadQoS(long cptr, java.lang.String filename);

  private native void jni_setSendBandwidthLimit(long cptr, int limit);

  private native int jni_getSendBandwidthLimit(long cptr);

  private native void jni_setTotalBandwidthLimit(long cptr, int limit);

  private native int jni_getTotalBandwidthLimit(long cptr);

  private native void jni_setDeadline(long cptr, int deadline);

  private native int jni_getDeadline(long cptr);

  private native void jni_addTrustedPeer(long cptr, java.lang.String host);

  private native void jni_addBannedPeer(long cptr, java.lang.String host);

  private native void jni_updateDropRate(long cptr, double percentage, int type, int burstamount);

  public QoSTransportSettings()
  {
    setCPtr(jni_QoSTransportSettings());
  }

  public QoSTransportSettings(QoSTransportSettings transportSettings)
  {
    setCPtr(jni_QoSTransportSettings(transportSettings.getCPtr()));
  }

  /**
   * Adds a BufferFilter to the filter chain. Buffer filters are applied just
   * after all other filters on send and before all other filters on receive.
   *
   * @param filter a filter to encode and decode buffers
   */
  public void addFilter(BufferFilter filter)
  {
    if (filter instanceof MadaraJNI)
    {
      MadaraJNI superClass = (MadaraJNI)filter;
      jni_addBufferFilter(getCPtr(), superClass.getCPtr());
    }
    else
    {
      jni_addBufferFilterObj(getCPtr(), filter);
    }
  }

  /**
   * Clears the list of buffer filters
   */
  public void clearBufferFilters()
  {
    jni_clearBufferFilters(getCPtr());
  }

  /**
   * Adds a filter that will be applied to certain types after receiving and
   * before rebroadcasting, if time-to-live is greater than 0
   *
   * @param type the types to add the filter to
   * @param filter Madara callback function
   */
  public void addRebroadcastFilter(KnowledgeType type, RecordFilter filter)
  {
    jni_addRebroadcastRecordFilter(getCPtr(), type.value(), filter);
  }

  /**
   * Adds an aggregate update filter that will be applied before rebroadcasting,
   * after individual record filters.
   *
   * @param filter Madara callback function
   */
  public void addRebroadcastFilter(AggregateFilter filter)
  {
    jni_addRebroadcastAggregateFilter(getCPtr(), filter);
  }

  /**
   * Adds a filter that will be applied to certain types before sending
   *
   * @param type the types to add the filter to
   * @param filter Madara callback function
   */
  public void addSendFilter(KnowledgeType type, RecordFilter filter)
  {
    jni_addSendRecordFilter(getCPtr(), type.value(), filter);
  }

  /**
   * Adds an aggregate update filter that will be applied before sending, after
   * individual record filters.
   *
   * @param filter Madara callback function
   */
  public void addSendFilter(AggregateFilter filter)
  {
    jni_addSendAggregateFilter(getCPtr(), filter);
  }

  /**
   * Adds an aggregate filter for a map of variables to values before applying
   * updates to the Knowledge_Base
   *
   * @param type the types to add the filter to
   * @param filter Madara callback function
   */
  public void addReceiveFilter(KnowledgeType type, RecordFilter filter)
  {
    jni_addReceiveRecordFilter(getCPtr(), type.value(), filter);
  }

  /**
   * Adds an aggregate update filter that will be applied after receiving, after
   * individual record filters.
   *
   * @param filter Madara callback function
   */
  public void addReceiveFilter(AggregateFilter filter)
  {
    jni_addReceiveAggregateFilter(getCPtr(), filter);
  }

  /**
   * Sets the rebroadcast time-to-live for all sent packets
   *
   * @param ttl the time-to-live
   */
  public void setRebroadcastTtl(int ttl)
  {
    jni_setRebroadcastTtl(getCPtr(), ttl);
  }

  /**
   * Gets the rebroadcast time-to-live for all sent packets
   *
   * @return the rebroadcast time-to-live
   */
  public int getRebroadcastTtl()
  {
    return jni_getRebroadcastTtl(getCPtr());
  }

  /**
   * Enables participation in rebroadcasts up to a certain ttl value
   *
   * @param ttl the time-to-live
   */
  public void enableParticipantTtl(int ttl)
  {
    jni_enableParticipantTtl(getCPtr(), ttl);
  }

  /**
   * Gets the rebroadcast time-to-live for all rebroadcasted packets
   *
   * @return the rebroadcast time-to-live
   */
  public int getParticipantTtl()
  {
    return jni_getParticipantTtl(getCPtr());
  }

  /**
   * Sets the send bandwidth limit
   *
   * @param limit the bandwidth limit for sending packets
   */
  public void setSendBandwidthLimit(int limit)
  {
    jni_setSendBandwidthLimit(getCPtr(), limit);
  }

  /**
   * Gets the send bandwidth limit
   *
   * @return the bandwidth limit for sending packets
   */
  public int getSendBandwidthLimit()
  {
    return jni_getSendBandwidthLimit(getCPtr());
  }

  /**
   * Sets the total bandwidth limit
   *
   * @param limit the bandwidth limit for all packets
   */
  public void setTotalBandwidthLimit(int limit)
  {
    jni_setTotalBandwidthLimit(getCPtr(), limit);
  }

  /**
   * Gets the total bandwidth limit
   *
   * @return the bandwidth limit for all packets
   */
  public int getTotalBandwidthLimit()
  {
    return jni_getTotalBandwidthLimit(getCPtr());
  }

  /**
   * Sets the deadline (seconds)
   *
   * @param deadline the maximum lifetime for all packets in seconds
   */
  public void setDeadline(int deadline)
  {
    jni_setDeadline(getCPtr(), deadline);
  }

  /**
   * Gets the deadline (seconds)
   *
   * @return the maximum lifetime for all packets in seconds
   */
  public int getDeadline()
  {
    return jni_getDeadline(getCPtr());
  }

  /**
   * Adds a trusted peer
   *
   * @param host the peer to add to the trusted list
   *
   */
  void addTrustedPeer(java.lang.String host)
  {
    jni_addTrustedPeer(getCPtr(), host);
  }

  /**
   * Adds a banned peer
   *
   * @param host the peer to add to the banned list
   *
   */
  void addBannedPeer(java.lang.String host)
  {
    jni_addBannedPeer(getCPtr(), host);
  }

  /**
   * Updates drop rate
   *
   * @param percentage the percentage of drops to enforce
   * @param type the type of drop policy to use
   * @param burstamount the amount of bursts of drops to enforce
   *
   */
  void updateDropRate(double percentage, DropType type, int burstamount)
  {
    jni_updateDropRate(getCPtr(), percentage, type.value(), burstamount);
  }

  /**
   * Saves the transport settings as a KnowledgeBase to a file
   *
   * @param filename the file to save the knowledge base to
   *
   **/
  @Override
  public void save(String filename)
  {
    jni_saveQoS(getCPtr(), filename);
  }

  /**
   * Loads the transport settings from a KnowledgeBase context file
   *
   * @param filename the file to save the knowledge base to
   *
   **/
  @Override
  public void load(String filename)
  {
    jni_loadQoS(getCPtr(), filename);
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance of WaitSettings gets garbage collected
   */
  public void free()
  {
    jni_freeQoSTransportSettings(getCPtr());
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
