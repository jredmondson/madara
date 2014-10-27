/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
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

