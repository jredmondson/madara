/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
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

