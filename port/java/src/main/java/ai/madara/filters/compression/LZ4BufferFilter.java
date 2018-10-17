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
package ai.madara.filters.compression;

import ai.madara.MadaraJNI;
import ai.madara.exceptions.FeatureNotImplementedException;
import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.filters.BufferFilter;

public class LZ4BufferFilter extends MadaraJNI implements BufferFilter
{
  private native long jni_Lz4BufferFilter();
  private native long jni_Lz4BufferFilter(long cptr);
  private static native void jni_freeLz4BufferFilter(long cptr);
  private native long jni_encode(long cptr, byte[] buffer, long size,
    long maxSize);
  private native long jni_decode(long cptr, byte[] buffer, long size,
    long maxSize);

  /**
   * Let the transport layer manage this
   */
  private boolean manageMemory = false;

  /**
   * Default constructor
   **/
  public LZ4BufferFilter() throws FeatureNotImplementedException
  {
    setCPtr(jni_Lz4BufferFilter());
  }

  /**
   * Copy constructor
   * @param input  instance to copy
   **/
  public LZ4BufferFilter(LZ4BufferFilter input) throws FeatureNotImplementedException
  {
    setCPtr(jni_Lz4BufferFilter(input.getCPtr()));
  }

  /**
   * Encodes a buffer
   * @param buffer  a map of all variable names to values
   * @param size    the initial size of the buffer
   * @param maxSize the maximum size of the buffer
   * @return the new size of the buffer contents
   **/
  @Override
  public long encode(byte[] buffer, long size, long maxSize) throws MadaraDeadObjectException
  {
    return jni_encode(getCPtr(), buffer, size, maxSize);
  }

  /**
   * Decodes a buffer
   * @param buffer  a map of all variable names to values
   * @param size    the initial size of the buffer
   * @param maxSize the maximum size of the buffer
   * @return the new size of the buffer contents
   **/
  @Override
  public long decode(byte[] buffer, long size, long maxSize) throws MadaraDeadObjectException
  {
    return jni_decode(getCPtr(), buffer, size, maxSize);
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance gets garbage collected
   */
  public void free()
  {
    if(manageMemory)
    {
      jni_freeLz4BufferFilter(getCPtr());
      setCPtr(0);
    }
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
}

