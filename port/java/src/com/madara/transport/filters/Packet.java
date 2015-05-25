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
package com.madara.transport.filters;

import com.madara.MadaraJNI;
import com.madara.KnowledgeRecord;

import java.util.AbstractMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public class Packet extends MadaraJNI
{
  private native long jni_get(long cptr, java.lang.String index);
  private native void jni_set(long cptr, java.lang.String index, long record);
  private native java.lang.String[] jni_get_keys(long cptr);
  private native boolean jni_exists(long cptr, java.lang.String index);
  private native void jni_clear(long cptr);
  private native void jni_erase(long cptr, java.lang.String index);

  /**
   * Only constructor comes from the JNI side
   **/
  private Packet()
  {
  }

  /**
   * Creates a {@link com.madara.Variables Variables} from a pointer
   *
   * @param cptr C pointer to a Variables object
   * @return new {@link com.madara.Variables Variables}
   */
  public static Packet fromPointer(long cptr)
  {
    Packet ret = new Packet();
    ret.setCPtr(cptr);
    return ret;
  }
  
  /**
   * Gets the record at the specified index
   * @param  index   the index of the record
   * @return the record at the index
   **/
  public KnowledgeRecord get(java.lang.String index)
  {
    return KnowledgeRecord.fromPointer(jni_get(getCPtr(),index));
  }
     
  /**
   * Clears all records from the packet
   **/
  public void clear()
  {
    jni_clear(getCPtr());
  }
  
  /**
   * Erases the record at the specified index
   * @param  index   the index of the record
   **/
  public void erase(java.lang.String index)
  {
    jni_erase(getCPtr(),index);
  }
  
  /**
   * Checks if there is a record at the specified index
   * @param  index   the index of the record
   * @return true if the index exists in the packet
   **/
  public boolean exists(java.lang.String index)
  {
    return jni_exists(getCPtr(),index);
  }
  
  /**
   * Sets the record at the specified index
   * @param  index   the index of the record
   * @param  record  the record to set at the index
   **/
  public void set(java.lang.String index, KnowledgeRecord record)
  {
    jni_set(getCPtr(), index, record.getCPtr());
  }
   
  /**
   * Gets all keys in the packet
   * @return the array of all keys in the packet
   **/
  public String[] getKeys()
  {
    return jni_get_keys(getCPtr());
  }
}

