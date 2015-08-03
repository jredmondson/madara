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
package com.madara.containers;

import com.madara.KnowledgeBase;
import com.madara.UpdateSettings;
import com.madara.Variables;

/**
 * A facade for an integer value within a knowledge base
 **/

public class Integer extends BaseContainer
{	
  private native long jni_Integer();
  private native long jni_Integer(long cptr);
  private static native void jni_freeInteger(long cptr);
  private native void jni_set(long cptr, long value);
  private native java.lang.String jni_getName(long cptr);
  private native void jni_setName(long cptr, long type, long kb, java.lang.String name);
  private native java.lang.String jni_toString(long cptr);
  private native double jni_toDouble(long cptr);
  private native long jni_toLong(long cptr);
  private native long jni_inc(long cptr);
  private native long jni_dec(long cptr);
  private native long jni_incValue(long cptr, long value);
  private native long jni_decValue(long cptr, long value);
  private native void jni_modify(long cptr);
  private native void jni_setSettings(long cptr, long settings);
  private native boolean jni_isTrue(long cptr);
  private native boolean jni_isFalse(long cptr);

  private boolean manageMemory = true;

  /**
   * Default constructor
   **/
  public Integer()
  {
    setCPtr(jni_Integer());
  }

  /**
   * Copy constructor
   * @param input  instance to copy
   **/
  public Integer(Integer input)
  {
    setCPtr(jni_Integer(input.getCPtr()));
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @return a new java instance of the underlying pointer
   */
  public static Integer fromPointer(long cptr)
  {
    Integer ret = new Integer();
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
  public static Integer fromPointer(long cptr, boolean shouldManage)
  {
    Integer ret = new Integer();
    ret.manageMemory=shouldManage;
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Gets the value
   *
   * @return   current value
   */
  public long get()
  {
    return jni_toLong(getCPtr());
  }

  /**
   * Gets the name of the variable
   *
   * @return  name of the variable within the context
   */
  public java.lang.String getName()
  {
    return jni_getName(getCPtr());
  }

  /**
   * Increments the container
   *
   * @return new value of the container
   */
  public long inc()
  {
    return jni_inc(getCPtr());
  }

  /**
   * Increments by a value
   *
   * @param  value   value to increment by
   * @return new value of the container
   */
  public long inc(long value)
  {
    return jni_incValue(getCPtr(), value);
  }

  /**
   * Decrements the container
   *
   * @return new value of the container
   */
  public long dec()
  {
    return jni_dec(getCPtr());
  }

  /**
   * Decrements by a value
   *
   * @param  value   value to increment by
   * @return new value of the container
   */
  public long dec(long value)
  {
    return jni_decValue(getCPtr(), value);
  }

  /**
   * Sets the value
   *
   * @param  value   new value
   */
  public void set(long value)
  {
    jni_set(getCPtr(), value);
  }

  /**
   * Mark the value as modified. The Integer retains the same value
   * but will resend its value as if it had been modified.
   **/
  public void modify()
  {
    jni_modify(getCPtr());
  }
  
  /**
   * Returns true if the container evaluates to true
   * @return true if container has all true values
   **/
  public boolean isTrue()
  {
    return jni_isTrue(getCPtr());
  }
  
  /**
   * Returns true if the container evaluates to false
   * @return true if container has any false values or is uninitialized
   **/
  public boolean isFalse()
  {
    return jni_isFalse(getCPtr());
  }
  
  /**
   * Sets the name and knowledge base being referred to
   *
   * @param  kb      the knowledge base that contains the name
   * @param  name    the variable name
   */
  public void setName(KnowledgeBase kb, java.lang.String name)
  {
    jni_setName(getCPtr(), 0, kb.getCPtr (), name);
  }

  /**
   * Sets the name and knowledge base being referred to
   *
   * @param  vars    the variables facade that contains the name
   * @param  name    the variable name
   */
  public void setName(Variables vars, java.lang.String name)
  {
    jni_setName(getCPtr(), 1, vars.getCPtr (), name);
  }

  /**
   * Sets the settings for updating variables in the Knowledge Base
   *
   * @param  settings  the settings to use for updating the Knowledge Base
   */
  public void setSettings(UpdateSettings settings)
  {
    jni_setSettings(getCPtr(), settings.getCPtr());
  }

  /**
   * Converts the value to a double
   *
   * @return current double value
   */
  public double toDouble()
  {
    return jni_toDouble(getCPtr());
  }

  /**
   * Converts the value to a long
   *
   * @return current long value
   */
  public long toLong()
  {
    return jni_toLong(getCPtr());
  }

  /**
   * Converts the value to a string
   *
   * @return current string value
   */
  @Override
  public java.lang.String toString()
  {
    return jni_toString(getCPtr());
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance gets garbage collected
   */
  public void free()
  {
    if (manageMemory)
    {
      jni_freeInteger(getCPtr());
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

