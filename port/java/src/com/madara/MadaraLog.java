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
 * Holder for static method to set global debug level.
 * MadaraLog extends MadaraJNI to insure the library is loaded
 * prior to invocation of {@link #setLogLevel (com.madara.MadaraLog.MadaraLogLevel) setLogLevel}
 */
public class MadaraLog extends MadaraJNI
{
  private static native void jni_setLogLevel(int debug);
  private static native int jni_getLogLevel();

  /**
   * Sets the global debug level
   *
   * @param debug {@link com.madara.MadaraLog.MadaraLogLevel MadaraLogLevel} to set
   */
  public static void setLogLevel(MadaraLogLevel debug)
  {
    jni_setLogLevel(debug.value());
  }

  /**
   * Gets the global debug level
   *
   * @return  the log level used by MADARA
   */
  public static int getLogLevel()
  {
    return jni_getLogLevel();
  }

  /**
   * private constructor because we do not need any thing but static here
   */
  private MadaraLog()
  {

  }

  /**
   * Enumeration of valid debug levels
   */
  public static enum MadaraLogLevel
  {
    MADARA_LOG_EMERGENCY(0),
    MADARA_LOG_TERMINAL_ERROR(1),
    MADARA_LOG_NONFATAL_ERROR(2),
    MADARA_LOG_ERROR(3),
    MADARA_LOG_WARNING(4),
    MADARA_LOG_MAJOR_EVENT(5),
    MADARA_LOG_MINOR_EVENT(6),
    MADARA_LOG_EVENT_TRACE(7),
    MADARA_LOG_MAJOR_DEBUG_INFO(8),
    MADARA_LOG_TRACE(9),
    MADARA_LOG_DETAILED_TRACE(10);

    private int num;

    private MadaraLogLevel(int num)
    {
      this.num = num;
    }

    /**
     * Get the integer value of this {@link com.madara.MadaraLog.MadaraLogLevel MadaraLogLevel}
     *
     * @return int value of this {@link com.madara.MadaraLog.MadaraLogLevel MadaraLogLevel}
     */
    public int value()
    {
      return num;
    }


    /**
     * Converts an int to a {@link com.madara.MadaraLog.MadaraLogLevel MadaraLogLevel}
     *
     * @param val value to convert
     * @return {@link com.madara.MadaraLog.MadaraLogLevel MadaraLogLevel} or null if the int is invalid
     */
    public static MadaraLogLevel getDebugLevel(int val)
    {
      for (MadaraLogLevel t : values())
      {
        if (t.value() == val)
          return t;
      }
      return null;
    }
  }
}

