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
 * Encapsulates settings for an evaluation statement.
 */
public class EvalSettings extends MadaraJNI
{

  //Constructors
  private static native long jni_evalSettings();
  private static native long jni_evalSettings(long oldPtr);

  //Getters/Setters
  private static native void jni_setDelaySendingModifieds(long cptr, boolean delaySendingModifieds);
  private static native boolean jni_getDelaySendingModifieds(long cptr);
  private static native void jni_setPrePrintStatement(long cptr, String prePrintStatement);
  private static native String jni_getPrePrintStatement(long cptr);
  private static native void jni_setPostPrintStatement(long cptr, String prePrintStatement);
  private static native String jni_getPostPrintStatement(long cptr);
  private static native void jni_setAlwaysOverwrite(long cptr, boolean alwaysOverwrite);
  private static native boolean jni_getAlwaysOverwrite(long cptr);
  private static native void jni_setTreatGlobalsAsLocals(long cptr, boolean treatGlobalsAsLocals);
  private static native boolean jni_getTreatGlobalsAsLocals(long cptr);
  private static native void jni_setClockIncrement(long cptr, long defaultClockIncrement);
  private static native long jni_getClockIncrement(long cptr);
  private static native void jni_freeEvalSettings(long cptr);
  public static final EvalSettings DEFAULT_EVAL_SETTINGS = new EvalSettings(jni_evalSettings());

  /**
   * Default constructor
   */
  public EvalSettings()
  {
    setCPtr(jni_evalSettings());
  }

  /**
   * Copy constructor
   *
   * @param input the settings to copy from
   */
  public EvalSettings(EvalSettings input)
  {
    setCPtr(jni_evalSettings(input.getCPtr()));
  }

  /**
   * Constructor to create constants
   *
   * @param cptr Pointer to C++ object
   */
  protected EvalSettings(long cptr)
  {
    setCPtr(cptr);
  }

  /**
   * @param delaySendingModifieds Toggle for sending modifieds in a single update event after each evaluation.
   */
  public void setDelaySendingModifieds(boolean delaySendingModifieds) throws MadaraDeadObjectException
  {
    jni_setDelaySendingModifieds(getCPtr(), delaySendingModifieds);
  }

  /**
   * @return current value of delaySendingModifieds
   */
  public boolean getDelaySendingModifieds() throws MadaraDeadObjectException
  {
    return jni_getDelaySendingModifieds(getCPtr());
  }

  /**
   * @param prePrintStatement Statement to print before evaluations.
   */
  public void setPrePrintStatement(String prePrintStatement) throws MadaraDeadObjectException
  {
    jni_setPrePrintStatement(getCPtr(), prePrintStatement);
  }

  /**
   * @return current value of prePrintStatement
   */
  public String getPrePrintStatement() throws MadaraDeadObjectException
  {
    return jni_getPrePrintStatement(getCPtr());
  }

  /**
   * @param postPrintStatement Statement to print after evaluations.
   */
  public void setPostPrintStatement(String postPrintStatement) throws MadaraDeadObjectException
  {
    jni_setPostPrintStatement(getCPtr(), postPrintStatement);
  }

  /**
   * @return current value of getPostPrintStatement
   */
  public String getPostPrintStatement() throws MadaraDeadObjectException
  {
    return jni_getPostPrintStatement(getCPtr());
  }

  /**
   * @param alwaysOverwrite Toggle for always overwriting records, regardless of quality, clock values, etc.
   */
  public void setAlwaysOverwrite(boolean alwaysOverwrite) throws MadaraDeadObjectException
  {
    jni_setAlwaysOverwrite(getCPtr(), alwaysOverwrite);
  }

  /**
   * @return current value of alwaysOverwrite
   */
  public boolean getAlwaysOverwrite() throws MadaraDeadObjectException
  {
    return jni_getAlwaysOverwrite(getCPtr());
  }

  /**
   * @param treatGlobalsAsLocals Toggle whether updates to global variables are
   * treated as local variables and not marked as modified to the transport.
   */
  public void setTreatGlobalsAsLocals(boolean treatGlobalsAsLocals) throws MadaraDeadObjectException
  {
    jni_setTreatGlobalsAsLocals(getCPtr(), treatGlobalsAsLocals);
  }

  /**
   * @return current value of treatGlobalsAsLocals
   */
  public boolean getTreatGlobalsAsLocals() throws MadaraDeadObjectException
  {
    return jni_getTreatGlobalsAsLocals(getCPtr());
  }

  /**
   * @param defaultClockIncrement Default clock increment.
   */
  public void setDefaultClockIncrement(long defaultClockIncrement) throws MadaraDeadObjectException
  {
    jni_setClockIncrement(getCPtr(), defaultClockIncrement);
  }

  /**
   * @return get the default clock increment
   */
  public long getDefaultClockIncrement() throws MadaraDeadObjectException
  {
    return jni_getClockIncrement(getCPtr());
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be called
   * before an instance of EvalSettings gets garbage collected
   */
  public void free()
  {
    jni_freeEvalSettings(getCPtr());
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
}

