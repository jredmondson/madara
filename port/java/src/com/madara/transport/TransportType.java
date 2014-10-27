/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara.transport;

public enum TransportType
{
  NO_TRANSPORT(0),
  SPLICE_TRANSPORT(1),
  NDSS_TRANSPORT(2),
  TCP_TRANSPORT(3),
  UDP_TRANSPORT(4),
  MULTICAST_TRANSPORT(5),
  BROADCAST_TRANSPORT(6),
  INCONSISTENT_TRANSPORT(100);

  private int num;

  private TransportType(int num)
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

  public static TransportType getType(int val)
  {
    for (TransportType t : values())
    {
      if (t.value() == val)
        return t;
    }
    return null;
  }
}

