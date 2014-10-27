/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara;


/**
 * This class encapsulates an entry in a KnowledgeBase.
 */
public class KnowledgeRecord extends MadaraJNI
{
  //Constructors
  private native long jni_KnowledgeRecord();
  private native long jni_KnowledgeRecordDeep(long ptr);
  private native long jni_KnowledgeRecord(String str);
  private native long jni_KnowledgeRecord(double str);
  private native long jni_KnowledgeRecord(long str);
  private static native long jni_KnowledgeRecord(double[] dbls);
  private static native long jni_KnowledgeRecord(long[] longs);

  //Getters
  private native long jni_toLongValue(long cptr);
  private native String jni_toStringValue(long cptr);
  private native double jni_toDoubleValue(long cptr);
  private static native double[] jni_toDoubleArray(long cptr);
  private static native long[] jni_toLongArray(long cptr);
  private native int jni_getType(long cptr);

  //Free
  private native void jni_freeKnowledgeRecord(long cptr);

  private final boolean isNew;

  /**
   * No reason to ever create this without a pointer
   */
  private KnowledgeRecord(boolean isNew)
  {
    this.isNew = isNew;
  }

  /**
   * Default constructor
   **/
  public KnowledgeRecord()
  {
    isNew = true;
    setCPtr(jni_KnowledgeRecord());
  }
  
  /**
   * Constructor for long/integer values
   *
   * @param lng value to set
   */
  public KnowledgeRecord(long lng)
  {
    isNew = true;
    setCPtr(jni_KnowledgeRecord(lng));
  }


  /**
   * Constructor for string values
   *
   * @param str value to set
   */
  public KnowledgeRecord(String str)
  {
    isNew = true;
    setCPtr(jni_KnowledgeRecord(str));
  }


  /**
   * Constructor for double values
   *
   * @param dbl value to set
   */
  public KnowledgeRecord(double dbl)
  {
    isNew = true;
    setCPtr(jni_KnowledgeRecord(dbl));
  }

  /**
   * Constructor for double[] values
   *
   * @param dbls value to set
   */
  public KnowledgeRecord(double[] dbls)
  {
    isNew = true;
    setCPtr(jni_KnowledgeRecord(dbls));
  }

  /**
   * Constructor for long[] values
   *
   * @param longs value to set
   */
  public KnowledgeRecord(long[] longs)
  {
    isNew = true;
    setCPtr(jni_KnowledgeRecord(longs));
  }


  /**
   * Converts the value to a long
   *
   * @return current long value
   */
  public long toLong()
  {
    return jni_toLongValue(getCPtr());
  }


  /**
   * Converts the value to a float/double
   *
   * @return current double value
   */
  public double toDouble()
  {
    return jni_toDoubleValue(getCPtr());
  }

  /**
   * Converts the value to a double array
   *
   * @return current array values
   */
  public double[] toDoubleArray()
  {
    return jni_toDoubleArray(getCPtr());
  }

  /**
   * Converts the value to a long array
   *
   * @return current array values
   */
  public long[] toLongArray()
  {
    return jni_toLongArray(getCPtr());
  }


  /**
   * Converts the value to a String
   *
   * @return current string value
   */
  public String toString()
  {
    return jni_toStringValue(getCPtr());
  }


  /**
   * @return the {@link com.madara.KnowledgeType KnowledgeType} of the value
   */
  public KnowledgeType getType()
  {
    return KnowledgeType.getType(jni_getType(getCPtr()));
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be called
   * before an instance of KnowledgeRecord gets garbage collected
   */
  public void free()
  {
    jni_freeKnowledgeRecord(getCPtr());
  }

  /**
   * Creates a {@link com.madara.KnowledgeRecord KnowledgeRecord} from a pointer
   *
   * @param cptr C pointer to a KnowledgeRecord object
   * @return new KnowledgeRecord
   */
  public static KnowledgeRecord fromPointer(long cptr)
  {
    return fromPointer(cptr, true);
  }

  /**
   * Creates a {@link com.madara.KnowledgeRecord KnowledgeRecord} from a pointer
   *
   * @param cptr C pointer to a KnowledgeRecord object
   * @param isNew  indicates if the record is meant to be managed
   * @return new KnowledgeRecord
   */
  public static KnowledgeRecord fromPointer(long cptr, boolean isNew)
  {
    KnowledgeRecord ret = new KnowledgeRecord(isNew);
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Will return true if this object was malloced
   * @return true if the object must manage itself, false if not
   */
  public boolean isNew()
  {
    return isNew;
  }

  public KnowledgeRecord clone()
  {
    KnowledgeRecord ret = new KnowledgeRecord(true);
    ret.setCPtr(jni_KnowledgeRecordDeep(this.getCPtr()));
    return ret;
  }
}
