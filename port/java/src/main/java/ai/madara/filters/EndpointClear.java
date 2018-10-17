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
package ai.madara.filters;

import ai.madara.MadaraJNI;
import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.transport.QoSTransportSettings;

/**
 * A facade for the C++ Counter_Filter aggregate filter
 **/

public class EndpointClear extends MadaraJNI
{
  private native long jni_EndpointClear();
  private native void jni_addReceiveFilterTo(long cptr, long qosCptr);
  private native void jni_addSendFilterTo(long cptr, long qosCptr);
  private native void jni_addRebroadcastFilterTo(long cptr, long qosCptr);
  private native void jni_setPrefix(long cptr, String prefix);
  private static native void jni_freeEndpointClear(long cptr);

  private boolean manageMemory = true;

  /**
   * Default constructor
   **/
  public EndpointClear()
  {
    setCPtr(jni_EndpointClear());
  }

  /**
   * Sets prefix where domain members are kept
   * @param  prefix   the new variable prefix
   */
  void setPrefix(String prefix) throws MadaraDeadObjectException
  {
    jni_setPrefix(getCPtr(), prefix);
  }

  /**
   * Adds the filter as a receive filter to transport settings
   * @param  settings the QoS Transport settings to add the filter to
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public void addReceiveFilterTo(QoSTransportSettings settings) throws MadaraDeadObjectException
  {
    jni_addReceiveFilterTo(getCPtr(), settings.getCPtr());
  }

  /**
   * Adds the filter as a receive filter to transport settings
   * @param  settings the QoS Transport settings to add the filter to
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public void addSendFilterTo(QoSTransportSettings settings) throws MadaraDeadObjectException
  {
    jni_addSendFilterTo(getCPtr(), settings.getCPtr());
  }

  /**
   * Adds the filter as a receive filter to transport settings
   * @param  settings the QoS Transport settings to add the filter to
 * @throws MadaraDeadObjectException throws exception if object is already released 
   */
  public void addRebroadcastFilterTo(QoSTransportSettings settings) throws MadaraDeadObjectException
  {
    jni_addRebroadcastFilterTo(getCPtr(), settings.getCPtr());
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance gets garbage collected
   */
  public void free()
  {
    if(manageMemory)
    {
      jni_freeEndpointClear(getCPtr());
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

