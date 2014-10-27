/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara;


import com.madara.transport.filters.AggregateFilter;
import com.madara.transport.TransportContext;
import com.madara.transport.filters.RecordFilter;

/**
 * Abstract class that insures loading of libMADARA.so and holds the C pointer
 */
public abstract class MadaraJNI
{
  static
  {
    System.loadLibrary("ACE");
    System.loadLibrary("MADARA");
  }

  /**
   * C pointer to an object
   */
  private long cptr = 0;

  /**
   * Set the C pointer to the object
   *
   * @param cptr C Pointer
   */
  protected void setCPtr(long cptr)
  {
    this.cptr = cptr;
  }

  /**
   * @return The C pointer of this object for passing to JNI functions
   */
  public long getCPtr()
  {
    return cptr;
  }

  /**
   * @return &lt;ClassName&gt;[&lt;C Pointer&gt;]
   * @see java.lang.Object#toString ()
   */
  public String toString()
  {
    return getClass().getName() + "[" + cptr + "]";
  }
}

