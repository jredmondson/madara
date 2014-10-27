/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara.transport;

public enum DropType
{
  PACKET_DROP_DETERMINISTIC(0),
  PACKET_DROP_PROBABLISTIC(1);

  private int num;

  private DropType(int num)
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

  public static DropType getType(int val)
  {
    for (DropType t : values())
    {
      if (t.value() == val)
        return t;
    }
    return null;
  }
}

