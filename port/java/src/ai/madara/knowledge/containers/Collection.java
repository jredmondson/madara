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
package ai.madara.knowledge.containers;

import ai.madara.knowledge.UpdateSettings;

/**
 * A facade for a collection of knowledge base containers
 **/

public class Collection extends BaseContainer
{	
  private native long jni_Collection();
  private native long jni_Collection(long cptr);
  private static native void jni_freeCollection(long cptr);
  private native java.lang.String jni_getDebugInfo(long cptr);
  private native void jni_modify(long cptr);
  private native void jni_setSettings(long cptr, long settings);
  private native void jni_addBarrier(long cptr, long container);
  private native void jni_addCounter(long cptr, long container);
  private native void jni_addDouble(long cptr, long container);
  private native void jni_addDoubleVector(long cptr, long container);
  private native void jni_addFlexMap(long cptr, long container);
  private native void jni_addInteger(long cptr, long container);
  private native void jni_addIntegerVector(long cptr, long container);
  private native void jni_addMap(long cptr, long container);
  private native void jni_addNativeDoubleVector(long cptr, long container);
  private native void jni_addNativeIntegerVector(long cptr, long container);
  private native void jni_addString(long cptr, long container);
  private native void jni_addStringVector(long cptr, long container);
  private native void jni_addVector(long cptr, long container);
  private native boolean jni_isTrue(long cptr);
  private native boolean jni_isFalse(long cptr);

  private boolean manageMemory = true;

  /**
   * Default constructor
   **/
  public Collection()
  {
    setCPtr(jni_Collection());
  }

  /**
   * Copy constructor
   * @param input  instance to copy
   **/
  public Collection(Collection input)
  {
    setCPtr(jni_Collection(input.getCPtr()));
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @return a new java instance of the underlying pointer
   */
  public static Collection fromPointer(long cptr)
  {
    Collection ret = new Collection();
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
  public static Collection fromPointer(long cptr, boolean shouldManage)
  {
    Collection ret = new Collection();
    ret.manageMemory=shouldManage;
    ret.setCPtr(cptr);
    return ret;
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
   * Mark the value as modified. The Collection retains the same value
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
   * Reads all debug info for the collection into a string
   *
   * @return aggregate debug information for the collection
   */
  @Override
  public java.lang.String toString()
  {
    return jni_getDebugInfo(getCPtr());
  }

  /**
   * Adds a Barrier container to the collection
   * @param container the container to add to the collection
   */
  public void add(Barrier container)
  {
    jni_addBarrier(getCPtr(), container.getCPtr());
  }
  
  /**
   * Adds a Counter container to the collection
   * @param container the container to add to the collection
   */
  public void add(Counter container)
  {
    jni_addCounter(getCPtr(), container.getCPtr());
  }
  
  /**
   * Adds a Double container to the collection
   * @param container the container to add to the collection
   */
  public void add(Double container)
  {
    jni_addDouble(getCPtr(), container.getCPtr());
  }
  
  /**
   * Adds a DoubleVector container to the collection
   * @param container the container to add to the collection
   */
  public void add(DoubleVector container)
  {
    jni_addDoubleVector(getCPtr(), container.getCPtr());
  }
  
  /**
   * Adds a FlexMap container to the collection
   * @param container the container to add to the collection
   */
  public void add(FlexMap container)
  {
    jni_addFlexMap(getCPtr(), container.getCPtr());
  }
  
  /**
   * Adds a Integer container to the collection
   * @param container the container to add to the collection
   */
  public void add(Integer container)
  {
    jni_addInteger(getCPtr(), container.getCPtr());
  }
    
  /**
   * Adds a IntegerVector container to the collection
   * @param container the container to add to the collection
   */
  public void add(IntegerVector container)
  {
    jni_addIntegerVector(getCPtr(), container.getCPtr());
  }
    
  /**
   * Adds a Map container to the collection
   * @param container the container to add to the collection
   */
  public void add(Map container)
  {
    jni_addMap(getCPtr(), container.getCPtr());
  }
    
  /**
   * Adds a NativeDoubleVector container to the collection
   * @param container the container to add to the collection
   */
  public void add(NativeDoubleVector container)
  {
    jni_addNativeDoubleVector(getCPtr(), container.getCPtr());
  }
    
  /**
   * Adds a NativeIntegerVector container to the collection
   * @param container the container to add to the collection
   */
  public void add(NativeIntegerVector container)
  {
    jni_addNativeIntegerVector(getCPtr(), container.getCPtr());
  }
    
  /**
   * Adds a String container to the collection
   * @param container the container to add to the collection
   */
  public void add(String container)
  {
    jni_addString(getCPtr(), container.getCPtr());
  }
    
  /**
   * Adds a StringVector container to the collection
   * @param container the container to add to the collection
   */
  public void add(StringVector container)
  {
    jni_addStringVector(getCPtr(), container.getCPtr());
  }
    
  /**
   * Adds a Vector container to the collection
   * @param container the container to add to the collection
   */
  public void add(Vector container)
  {
    jni_addVector(getCPtr(), container.getCPtr());
  }
  
  /**
   * Reads all debug info for the collection into a string
   *
   * @return aggregate debug information for the collection
   */
  public java.lang.String getDebugInfo()
  {
    return jni_getDebugInfo(getCPtr());
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance gets garbage collected
   */
  public void free()
  {
    if (manageMemory)
    {
      jni_freeCollection(getCPtr());
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

