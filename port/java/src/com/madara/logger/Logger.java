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
package com.madara.logger;

import com.madara.MadaraJNI;

/**
 * An extensible logger used for printing to files, terminals and system logs
 **/

public class Logger extends MadaraJNI
{	
  private native long jni_Logger();
  private static native void jni_freeLogger(long cptr);
  private native void jni_setLevel(long cptr, int level);
  private native int jni_getLevel(long cptr);
  private native java.lang.String jni_getTag(long cptr);
  private native void jni_setTag(long cptr, java.lang.String tag);
  private native void jni_addTerm(long cptr);
  private native void jni_addSyslog(long cptr);
  private native void jni_clear(long cptr);
  private native void jni_addFile(long cptr, java.lang.String filename);
  private native void jni_log(long cptr, int level, java.lang.String message);
  private native void jni_setTimestampFormat(long cptr, java.lang.String format);

  private boolean manageMemory = true;

  /**
   * Default constructor
   **/
  public Logger()
  {
    setCPtr(jni_Logger());
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @return a new java instance of the underlying pointer
   */
  public static Logger fromPointer(long cptr)
  {
    Logger ret = new Logger();
    ret.manageMemory = true;
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @param shouldManage  if true, manage the pointer
   * @return a new java instance of the underlying pointer
   */
  public static Logger fromPointer(long cptr, boolean shouldManage)
  {
    Logger ret = new Logger();
    ret.manageMemory=shouldManage;
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Gets the tag used in system logging
   *
   * @return   current tag for system logging
   */
  public java.lang.String getTag()
  {
    return jni_getTag(getCPtr());
  }

  /**
   * Gets the logging level, e.g., where 0 is EMERGENCY and 6 is DETAILED
   *
   * @return  the current logging level
   */
  public int getLevel()
  {
    return jni_getLevel(getCPtr());
  }

  /**
   * Sets the tag used for system logging
   * @param  tag   the tag to be used for system logging
   */
  public void setTag(java.lang.String tag)
  {
    jni_setTag(getCPtr(), tag);
  }

  /**
   * Sets the logging level
   *
   * @param  level   level of message severity to print to log targets
   */
  public void setLevel(int level)
  {
    jni_setLevel(getCPtr(), level);
  }

  /**
   * Clears all logging targets
   */
  public void clear()
  {
    jni_clear(getCPtr());
  }

  /**
   * Adds the terminal to logging outputs (turned on by default)
   */
  public void addTerm()
  {
    jni_addTerm(getCPtr());
  }

  /**
   * Adds the system logger to logging outputs
   */
  public void addSyslog()
  {
    jni_addSyslog(getCPtr());
  }

  /**
   * Adds a file to logging outputs
   * @param  filename   the name of a file to add to logging targets
   */
  public void addFile(java.lang.String filename)
  {
    jni_addFile(getCPtr(), filename);
  }

  /**
   * Logs a message at the specified level
   * @param  level   the logging severity level (0 is high)
   * @param  message the message to send to all logging targets
   */
  public void log(int level, java.lang.String message)
  {
    jni_log(getCPtr(), level, message);
  }

  /**
   * Sets timestamp format. Uses 
   * <a href="http://www.cplusplus.com/reference/ctime/strftime/">strftime</a>
   * for formatting time.
   * @param  format  the format of the timestamp. See C++
   *                 strftime definition for common usage.
   **/
  public void setTimestampFormat(java.lang.String format)
  {
    jni_setTimestampFormat(getCPtr(), format);
  }
  
  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance gets garbage collected
   */
  public void free()
  {
    if (manageMemory)
    {
      jni_freeLogger(getCPtr());
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

