/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
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

