/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara;

/**
 * Type of {@link com.madara.KnowledgeRecord KnowledgeRecord}
 */
public enum KnowledgeType
{
  //These are defined in Knowledge_Record.h
  UNINITIALIZED(0),
  INTEGER(1),
  STRING(2),
  DOUBLE(4),
  FLOAT(4),
  UNKNOWN_FILE_TYPE(8),
  XML(16),
  TEXT_FILE(32),
  INTEGER_ARRAY(64),
  DOUBLE_ARRAY(128),
  IMAGE_JPEG(256),
  ALL_PRIMITIVE_TYPES(INTEGER.num | STRING.num | DOUBLE.num | INTEGER_ARRAY.num | DOUBLE_ARRAY.num),
  ALL_FILE_TYPES(UNKNOWN_FILE_TYPE.num | XML.num | TEXT_FILE.num | IMAGE_JPEG.num),
  ALL_TYPES(ALL_PRIMITIVE_TYPES.num | ALL_FILE_TYPES.num);

  private int num;

  private KnowledgeType(int num)
  {
    this.num = num;
  }

  /**
   * @return int value of this {@link com.madara.KnowledgeType KnowledgeType}
   */
  public int value()
  {
    return num;
  }

  /**
   * Converts an int to a {@link com.madara.KnowledgeType KnowledgeType}
   *
   * @param val value to convert
   * @return {@link com.madara.KnowledgeType KnowledgeType} or null if the int is invalid
   */
  public static KnowledgeType getType(int val)
  {
    for (KnowledgeType t : values())
    {
      if (t.value() == val)
        return t;
    }
    return null;
  }
}

