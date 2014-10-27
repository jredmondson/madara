/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara.transport;

/**
 * @author jdroot
 */
public enum TransportReliability
{
  BEST_EFFORT(0),
  RELIABLE(1);

  private int num;

  private TransportReliability(int num)
  {
    this.num = num;
  }

  /**
   * Get the integer value of this enum
   *
   * @return value of the enum
   */
  public int value()
  {
    return num;
  }

  public static TransportReliability getReliability(int val)
  {
    for (TransportReliability t : values())
    {
      if (t.value() == val)
        return t;
    }
    return null;
  }
}

