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

/**
 * Type of {@link ai.madara.knowledge.KnowledgeRecord KnowledgeRecord}
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
   * @return int value of this {@link ai.madara.knowledge.KnowledgeType KnowledgeType}
   */
  public int value()
  {
    return num;
  }

  /**
   * Converts an int to a {@link ai.madara.knowledge.KnowledgeType KnowledgeType}
   *
   * @param val value to convert
   * @return {@link ai.madara.knowledge.KnowledgeType KnowledgeType} or null if the int is invalid
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

