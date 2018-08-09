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

import java.nio.ByteBuffer;

import ai.madara.MadaraJNI;
import ai.madara.knowledge.KnowledgeRecord;

import org.capnproto.StructReader;
import org.capnproto.SegmentBuilder;
import org.capnproto.StructBuilder;
import org.capnproto.StructFactory;
import org.capnproto.MessageReader;

/**
 * This class refers to a C++ object which supports Any. It does not own any
 * object, and thus does not require calling free() on it.
 */
public class AnyRef
{
  protected long handler_ = 0;
  protected long data_ = 0;

  protected AnyRef(long handler, long data)
  {
    handler_ = handler;
    data_ = data;
  }

  protected static void err(String error) throws BadAnyAccess
  {
    if (error != null) {
      throw new BadAnyAccess(error);
    }
  }

  protected static void err_unchecked(String error)
  {
    if (error != null) {
      throw new RuntimeException(error);
    }
  }

  /**
   * Return true if this Any does not hold any object. False otherwise.
   **/
  public boolean empty()
  {
    return data_ == 0 || handler_ == 0;
  }

  /**
   * Create an AnyRef object refering to the same this already refers to.
   * This does not extend the lifetime of the underlying C++ object. If the
   * Any (C++ or Java) holding the reference object is destroyed, this AnyRef
   * will be left dangling.
   **/
  public AnyRef ref()
  {
    return new AnyRef(handler_, data_);
  }

  private static native String jni_field(String name, long handler,
    long data, long[] out);

  /**
   * Create an AnyRef refering to the named field within this AnyRef.
   * This does not extend the lifetime of the underlying C++ object. If the
   * Any (C++ or Java) holding the reference object is destroyed, this AnyRef
   * will be left dangling.
   *
   * @throws BadAnyAccess if the object does not have the given field
   **/
  public AnyRef ref(String name) throws BadAnyAccess
  {
    long[] out = new long[2];
    err(jni_field(name, handler_, data_, out));
    return new AnyRef(out[0], out[1]);
  }

  private static native String jni_index(String key, long handler,
    long data, long[] out);

  /**
   * Create an AnyRef refering to the named key of a held map-like object.
   * This does not extend the lifetime of the underlying C++ object. If the
   * Any (C++ or Java) holding the reference object is destroyed, this AnyRef
   * will be left dangling.
   *
   * This method will create a key of the given name if one does not exist.
   **/
  public AnyRef at(String key)
  {
    long[] out = new long[2];
    err_unchecked(jni_index(key, handler_, data_, out));
    return new AnyRef(out[0], out[1]);
  }

  private static native String jni_index(long index, long handler,
    long data, long[] out);

  /**
   * Create an AnyRef refering to the index key of a held vector-like object.
   * This does not extend the lifetime of the underlying C++ object. If the
   * Any (C++ or Java) holding the reference object is destroyed, this AnyRef
   * will be left dangling.
   *
   * For resizable types, they are resized to hold the index. If nonresizable,
   * will throw BadAnyAccess if it supports checked access. Otherwise,
   * out-of-range indexes are undefined behavior.
   *
   * @throws RuntimeException if underlying object supports checked access,
   * and is not resizable, and an out-of-range index is given.
   **/
  public AnyRef at(long index)
  {
    long[] out = new long[2];
    err_unchecked(jni_index(index, handler_, data_, out));
    return new AnyRef(out[0], out[1]);
  }

  private static native String jni_assign(long lhs_handler, long lhs_data,
    long rhs_handler, long rhs_data);

  /**
   * Assign to the referenced object using the given AnyRef. This operation
   * cannot change the type of the object, and will throw an exception if the
   * given value does not match, or cannot be converted, to the current type.
   *
   * @param value the value to set with
   **/
  public void assign(AnyRef value) throws BadAnyAccess
  {
    err(jni_assign(handler_, data_, value.handler_, value.data_));
  }

  private static native String jni_assign_record(
      long lhs_handler, long lhs_data, long record_ptr);

  /**
   * Assign to the referenced object using the given KnowledgeRecord. This
   * operation cannot change the type of the object, and will throw an
   * exception if the given record does not cannot contain a matching type,
   * or cannot be converted to the current type.
   *
   * @param value the value to set with
   **/
  public void assign(KnowledgeRecord value) throws BadAnyAccess
  {
    err(jni_assign_record(handler_, data_, value.getCPtr()));
  }

  private static native String jni_assign(long lhs_handler, long lhs_data,
    long value);

  /**
   * Assign to the referenced object using the given integer. This operation
   * cannot change the type of the object, and will throw an exception if the
   * given value does not match, or cannot be converted, to the current type.
   *
   * @param value the value to set with
   **/
  public void assign(long value) throws BadAnyAccess
  {
    err(jni_assign(handler_, data_, value));
  }

  private static native String jni_assign(long lhs_handler, long lhs_data,
    double value);

  /**
   * Assign to the referenced object using the given double. This operation
   * cannot change the type of the object, and will throw an exception if the
   * given value does not match, or cannot be converted, to the current type.
   *
   * @param value the value to set with
   **/
  public void assign(double value) throws BadAnyAccess
  {
    err(jni_assign(handler_, data_, value));
  }

  private static native String jni_assign(long lhs_handler, long lhs_data,
    String value);

  /**
   * Assign to the referenced object using the given string. This operation
   * cannot change the type of the object, and will throw an exception if the
   * given value does not match, or cannot be converted, to the current type.
   *
   * @param value the value to set with
   **/
  public void assign(String value) throws BadAnyAccess
  {
    err(jni_assign(handler_, data_, value));
  }

  private static native String jni_assign(long lhs_handler, long lhs_data,
    long[] value);

  /**
   * Assign to the referenced object with the a integer array. This operation
   * cannot change the type of the object, and will throw an exception if the
   * given value does not match, or cannot be converted, to the current type.
   *
   * @param value the value to set with
   **/
  public void assign(long[] value) throws BadAnyAccess
  {
    err(jni_assign(handler_, data_, value));
  }

  private static native String jni_assign(long lhs_handler, long lhs_data,
    double[] value);

  /**
   * Assign to the referenced object with the a double array. This operation
   * cannot change the type of the object, and will throw an exception if the
   * given value does not match, or cannot be converted, to the current type.
   *
   * @param value the value to set with
   **/
  public void assign(double[] value) throws BadAnyAccess
  {
    err(jni_assign(handler_, data_, value));
  }

  private static native String jni_to_record(long handler, long data,
    long[] out);

  /**
   * Create a KnowledgeRecord holding a copy of the data this AnyRef references.
   * This will store it as a native type (long, double, long[], double[], or
   * String) if possible. Otherwise, the record will hold a clone of this Any.
   *
   * @return the new KnowledgeRecord
   **/
  public KnowledgeRecord toRecord()
  {
    long[] out = new long[1];
    err_unchecked(jni_to_record(handler_, data_, out));
    return KnowledgeRecord.fromPointer(out[0]);
  }

  private static native String jni_to(long handler, long data, long[] out);

  /**
   * Get the value of this Any as a 64-bit integer.
   *
   * @return the value
   **/
  public long toInteger()
  {
    long[] out = new long[1];
    err_unchecked(jni_to(handler_, data_, out));
    return out[0];
  }

  private static native String jni_to(long handler, long data, double[] out);

  /**
   * Get the value of this Any as a double.
   *
   * @return the value
   **/
  public double toDouble()
  {
    double[] out = new double[1];
    err_unchecked(jni_to(handler_, data_, out));
    return out[0];
  }

  private static native String jni_to(long handler, long data, String[] out);

  /**
   * Get the value of this Any as a String.
   *
   * @return the value
   **/
  @Override
  public String toString()
  {
    String[] out = new String[1];
    String e = jni_to(handler_, data_, out);
    if (e != null) {
      return "[toString error: " + e + "]";
    }
    return out[0];
  }

  private static native String jni_to(long handler, long data, long[][] out);

  /**
   * Get the value of this Any as an array of 64-bit integers.
   *
   * @return the value
   **/
  public long[] toIntegers()
  {
    long[][] out = new long[1][];
    err_unchecked(jni_to(handler_, data_, out));
    return out[0];
  }

  private static native String jni_to(long handler, long data, double[][] out);

  /**
   * Get the value of this Any as an array of doubles.
   *
   * @return the value
   **/
  public double[] toDoubles()
  {
    double[][] out = new double[1][];
    err_unchecked(jni_to(handler_, data_, out));
    return out[0];
  }

  private static native String jni_size(long handler, long data, long[] out);

  /**
   * Get the size of the held object, as determined by its size() method.
   *
   * @throws BadAnyAccess if the held object does not support size()
   **/
  public long size() throws BadAnyAccess
  {
    long[] out = new long[1];
    err(jni_size(handler_, data_, out));
    return out[0];
  }

  private static native String jni_list_fields(long handler, long data, String[][] out);

  /**
   * Get a list of field names for a type supporting fields.
   **/
  public String[] listFields()
  {
    String[][] out = new String[1][];
    err_unchecked(jni_list_fields(handler_, data_, out));
    return out[0];
  }

  private static native String jni_get_tag(long handler, long data, String[] out);

  /**
   * Get the registered tag of the held object. Will return null if the held
   * type is not registered with a tag.
   **/
  public String getTag()
  {
    String[] out = new String[1];
    err_unchecked(jni_get_tag(handler_, data_, out));
    return out[0];
  }

  protected static java.util.Map<String, StructFactory> registered_classes =
    new java.util.HashMap<String, StructFactory>();
  protected static java.util.Map<String, String> registered_factories =
    new java.util.HashMap<String, String>();

  protected void print_hex(byte[] data)
  {
    for (byte cur : data) {
      System.err.print(String.format("%02x", cur));
    }
  }

  protected static native String jni_register_tag(String tag);

  /**
   * Register a Java type with the Any system. This will allow the given
   * factory to be used to build and read types with the given tag. This
   * function is not thread-safe, and should be called as early as possible,
   * before any KnowledgeBases are in use.
   **/
  public static
    <Reader extends StructReader,
     Builder extends StructBuilder,
     Factory extends StructFactory<Builder, Reader>>
    void registerClass(String tag, Factory factory)
  {
    registered_classes.put(tag, factory);
    registered_factories.put(factory.getClass().getName(), tag);
    err_unchecked(jni_register_tag(tag));
  }

  protected static native String jni_reader(long handler, long data, byte[][] out);

  /**
   * Get a Cap'n Proto reader for the held data. Will throw if the held data
   * is not a Cap'n Proto message, or if it doesn't match the type of the
   * given factory.
   **/
  public <T> T reader(org.capnproto.FromPointerReader<T> factory)
    throws BadAnyAccess
  {
    byte[][] out = new byte[1][];

    String factory_name = factory.getClass().getName();
    String tag = registered_factories.get(factory_name);
    String my_tag = getTag();

    if (my_tag == null) {
      throw new BadAnyAccess("Any is holding an unregistered type");
    } else if (tag == null) {
      throw new BadAnyAccess("Cap'n Proto message type not registered");
    } else if (!tag.equals(my_tag)) {
      throw new BadAnyAccess("Mismatched tags: expected " + my_tag +
          " requested reader for tag " + tag);
    }

    err(jni_reader(handler_, data_, out));
    ByteBuffer[] msg = new ByteBuffer[1];
    msg[0] = ByteBuffer.wrap(out[0]);
    try {
      MessageReader message = org.capnproto.Serialize.read(msg[0]);
      return message.getRoot(factory);
    } catch (java.io.IOException e) {
      throw new BadAnyAccess("Bad IO: " + e);
    }
  }

  // Registration functions for common C++ types. Other types must be registered
  // from C++ code, potentially using System.loadLibrary.

  /** Register 32-bit integer with given tag */
  public static native void registerInt(String tag);
  /** Register 64-bit integer with given tag */
  public static native void registerLong(String tag);
  /** Register 16-bit integer with given tag */
  public static native void registerShort(String tag);
  /** Register the C char type with given tag */
  public static native void registerChar(String tag);
  /** Register unsigned 32-bit integer with given tag */
  public static native void registerUInt(String tag);
  /** Register unsigned 64-bit integer with given tag */
  public static native void registerULong(String tag);
  /** Register unsigned 16-bit integer with given tag */
  public static native void registerUShort(String tag);
  /** Register the C unsigned char type with given tag */
  public static native void registerUChar(String tag);
  /** Register the C signed char type with given tag */
  public static native void registerSChar(String tag);
  /** Register double type with given tag */
  public static native void registerDouble(String tag);
  /** Register float type with given tag */
  public static native void registerFloat(String tag);
  /** Register C++ std::string type with given tag */
  public static native void registerString(String tag);
  /** Register C++ std::vector<int32_t> type with given tag */
  public static native void registerIntVector(String tag);
  /** Register C++ std::vector<int64_t> type with given tag */
  public static native void registerLongVector(String tag);
  /** Register C++ std::vector<int16_t> type with given tag */
  public static native void registerShortVector(String tag);
  /** Register C++ std::vector<char> type with given tag */
  public static native void registerCharVector(String tag);
  /** Register C++ std::vector<uint32_t> type with given tag */
  public static native void registerUIntVector(String tag);
  /** Register C++ std::vector<uint64_t> type with given tag */
  public static native void registerULongVector(String tag);
  /** Register C++ std::vector<uint16_t> type with given tag */
  public static native void registerUShortVector(String tag);
  /** Register C++ std::vector<unsigned char> type with given tag */
  public static native void registerUCharVector(String tag);
  /** Register C++ std::vector<signed char> type with given tag */
  public static native void registerSCharVector(String tag);
  /** Register C++ std::vector<double> type with given tag */
  public static native void registerDoubleVector(String tag);
  /** Register C++ std::vector<float> type with given tag */
  public static native void registerFloatVector(String tag);
  /** Register C++ std::vector<std::string> type with given tag */
  public static native void registerStringVector(String tag);
  /** Register C++ std::map<std::string, std::string> type with given tag */
  public static native void registerStringToStringMap(String tag);
  /** Register C++ std::map<std::string, int64_t> type with given tag */
  public static native void registerStringToLongMap(String tag);
  /** Register C++ std::map<std::string, double> type with given tag */
  public static native void registerStringToDoubleMap(String tag);
}
