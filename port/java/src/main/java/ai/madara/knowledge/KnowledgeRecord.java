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
package ai.madara.knowledge;

import ai.madara.MadaraJNI;
import ai.madara.exceptions.MadaraDeadObjectException;

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

  //Checks
  private native boolean jni_isValid(long cptr);

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
  public KnowledgeRecord(long lng) throws MadaraDeadObjectException
  {
    isNew = true;
    setCPtr(jni_KnowledgeRecord(lng));
  }


  /**
   * Constructor for string values
   *
   * @param str value to set
   */
  public KnowledgeRecord(String str) throws MadaraDeadObjectException
  {
    isNew = true;
    setCPtr(jni_KnowledgeRecord(str));
  }


  /**
   * Constructor for double values
   *
   * @param dbl value to set
   */
  public KnowledgeRecord(double dbl) throws MadaraDeadObjectException
  {
    isNew = true;
    setCPtr(jni_KnowledgeRecord(dbl));
  }

  /**
   * Constructor for double[] values
   *
   * @param dbls value to set
   */
  public KnowledgeRecord(double[] dbls) throws MadaraDeadObjectException
  {
    isNew = true;
    setCPtr(jni_KnowledgeRecord(dbls));
  }

  /**
   * Constructor for long[] values
   *
   * @param longs value to set
   */
  public KnowledgeRecord(long[] longs) throws MadaraDeadObjectException
  {
    isNew = true;
    setCPtr(jni_KnowledgeRecord(longs));
  }


  /**
   * Checks if the record is valid or uncreated
   *
   * @return true if record has a value, false otherwise.
   */
  public boolean isValid() throws MadaraDeadObjectException
  {
    return jni_isValid(getCPtr());
  }

  /**
   * Converts the value to a long
   *
   * @return current long value
   */
  public long toLong() throws MadaraDeadObjectException
  {
    return jni_toLongValue(getCPtr());
  }


  /**
   * Converts the value to a float/double
   *
   * @return current double value
   */
  public double toDouble() throws MadaraDeadObjectException
  {
    return jni_toDoubleValue(getCPtr());
  }

  /**
   * Converts the value to a double array
   *
   * @return current array values
   */
  public double[] toDoubleArray() throws MadaraDeadObjectException
  {
    return jni_toDoubleArray(getCPtr());
  }

  /**
   * Converts the value to a long array
   *
   * @return current array values
   */
  public long[] toLongArray() throws MadaraDeadObjectException
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
   * @return the {@link ai.madara.knowledge.KnowledgeType KnowledgeType} of the value
   */
  public KnowledgeType getType() throws MadaraDeadObjectException
  {
    return KnowledgeType.getType(jni_getType(getCPtr()));
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be called
   * before an instance of KnowledgeRecord gets garbage collected
   */
  public void free() throws MadaraDeadObjectException
  {
    jni_freeKnowledgeRecord(getCPtr());
    setCPtr(0);
  }

  /**
   * Cleans up underlying C resources
   * @throws Throwable necessary for override but unused
   */
  @Override
  protected void finalize() throws Throwable
  {
    try {
      free();
    } catch (Throwable t) {
      throw t;
    } finally {
      super.finalize();
    }
  }

  /**
   * Creates a {@link ai.madara.knowledge.KnowledgeRecord KnowledgeRecord} from a pointer
   *
   * @param cptr C pointer to a KnowledgeRecord object
   * @return new KnowledgeRecord
   */
  public static KnowledgeRecord fromPointer(long cptr)
  {
    return fromPointer(cptr, true);
  }

  /**
   * Creates a {@link ai.madara.knowledge.KnowledgeRecord KnowledgeRecord} from a pointer
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
