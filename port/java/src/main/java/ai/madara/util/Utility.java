/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ai.madara.util;

import ai.madara.MadaraJNI;


/**
 * Utility methods provided by MADARA
 */
public class Utility extends MadaraJNI {  
  //Native Methods
  private static native String jni_getVersion();
  private static native long jni_getTime();
  private static native void jni_sleep(double sleepTime);

  /**
   * Returns the current MADARA version
   * @return the version in "major.minor.revision compiled on {date} at {time}"
   *         format
   */
  public static String getVersion()
  {
    return jni_getVersion();
  }
  
  /**
   * Returns the time in nanoseconds
   * @return the current timestamp in nanoseconds
   */
  public static long getTime()
  {
    return jni_getTime();
  }
  
  /**
   * Sleeps for a number of seconds in double format. Partial time (e.g.
   * 4.5s) is encouraged.
   * @param sleepTime  the current timestamp in nanoseconds
   */
  public static void sleep(double sleepTime)
  {
    jni_sleep(sleepTime);
  }
}
