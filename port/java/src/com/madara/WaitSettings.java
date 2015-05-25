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
package com.madara;

/**
 * Encapsulates settings for a wait statement.
 */
public class WaitSettings extends EvalSettings
{
  //Constructors
  private static native long jni_waitSettings();
  private static native long jni_waitSettings(long oldPtr);

  //Setters/Getters
  private static native void jni_setPollFrequency(long cptr, double freq);
  private static native double jni_getPollFrequency(long cptr);
  private static native void jni_setMaxWaitTime(long cptr, double maxWaitTime);
  private static native double jni_getMaxWaitTime(long cptr);
  private static native void jni_freeWaitSettings(long cptr);

  public static WaitSettings DEFAULT_WAIT_SETTINGS = new WaitSettings(jni_waitSettings());

  /**
   * Default constructor
   */
  public WaitSettings()
  {
    setCPtr(jni_waitSettings());
  }

  /**
   * Copy constructor
   *
   * @param waitSettings settings to be copied
   */
  public WaitSettings(WaitSettings waitSettings)
  {
    setCPtr(jni_waitSettings(waitSettings.getCPtr()));
  }


  /**
   * C pointer constructor
   *
   * @param cptr the C ptr to be inherited
   */
  protected WaitSettings(long cptr)
  {
    super(cptr);
  }


  /**
   * @param freq Frequency to poll an expression for truth.
   */
  public void setPollFrequency(double freq)
  {
    jni_setPollFrequency(getCPtr(), freq);
  }


  /**
   * @return current poll frequency
   */
  public double getPollFrequency()
  {
    return jni_getPollFrequency(getCPtr());
  }


  /**
   * @param maxWaitTime Maximum time to wait for an expression to become true.
   */
  public void setMaxWaitTime(double maxWaitTime)
  {
    jni_setMaxWaitTime(getCPtr(), maxWaitTime);
  }

  /**
   * @return current max wait time
   */
  public double getMaxWaitTime()
  {
    return jni_getMaxWaitTime(getCPtr());
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be called
   * before an instance of WaitSettings gets garbage collected
   */
  public void free()
  {
    jni_freeWaitSettings(getCPtr());
    setCPtr(0);
  }
}

