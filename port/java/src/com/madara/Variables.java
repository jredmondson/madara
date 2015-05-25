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
package com.madara;

import com.madara.KnowledgeBase.CompiledExpression;

/**
 * Provides an interface for external functions into the MADARA KaRL variable settings.
 */
public class Variables extends MadaraJNI
{
  private native long jni_evaluate(long cptr, long expression, long evalSettings);
  private native long jni_compile(long cptr, String expression);
  private native long jni_get(long cptr, String name);
  private native void jni_set(long cptr, String name, long record);

  /**
   * {@link com.madara.Variables Variables} should only be created from a pointer
   */
  private Variables()
  {

  }

  /**
   * Creates a {@link com.madara.Variables Variables} from a pointer
   *
   * @param cptr C pointer to a Variables object
   * @return new {@link com.madara.Variables Variables}
   */
  public static Variables fromPointer(long cptr)
  {
    Variables ret = new Variables();
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Evaluates an expression.
   * The returned KnowledgeRecord should either be freed  ({@link com.madara.KnowledgeRecord#free () KnowledgeRecord.free ()}) or returned
   *
   * @param expression KaRL expression to evaluate  (result of {@link #compile (String)})
   * @return value of expression
   */
  public KnowledgeRecord evaluate(CompiledExpression expression)
  {
    return evaluate(expression, EvalSettings.DEFAULT_EVAL_SETTINGS);
  }

  /**
   * Evaluates an expression.
   * The returned KnowledgeRecord should either be freed  ({@link com.madara.KnowledgeRecord#free () KnowledgeRecord.free ()}) or returned
   *
   * @param expression   KaRL expression to evaluate  (result of {@link #compile (String)})
   * @param evalSettings Settings for evaluating and printing
   * @return value of expression
   */
  public KnowledgeRecord evaluate(CompiledExpression expression, EvalSettings evalSettings)
  {
    return KnowledgeRecord.fromPointer(jni_evaluate(getCPtr(), expression.getCPtr(), evalSettings.getCPtr()));
  }

  /**
   * Compiles a KaRL expression into an expression tree.
   *
   * @param expression expression to compile
   * @return {@link com.madara.KnowledgeBase.CompiledExpression CompiledExpression}: compiled, optimized expression tree
   */
  public CompiledExpression compile(String expression)
  {
    return new CompiledExpression(jni_compile(getCPtr(), expression));
  }

  /**
   * Retrieves a knowledge value.
   *
   * @param name knowledge name
   * @return value of knowledge
   */
  public KnowledgeRecord get(String name)
  {
    return KnowledgeRecord.fromPointer(jni_get(getCPtr(), name));
  }


  /**
   * Sets a knowledge value to a specified value.
   *
   * @param name   knowledge name
   * @param record value to set
   */
  public void set(String name, KnowledgeRecord record)
  {
    jni_set(getCPtr(), name, record.getCPtr());
  }


  /**
   * Sets a knowledge value to a specified value.
   *
   * @param name  knowledge name
   * @param value value to set
   */
  public void set(String name, long value)
  {
    KnowledgeRecord kr = new KnowledgeRecord(value);
    set(name, kr);
    kr.free();
  }

  /**
   * Sets a knowledge value to a specified value.
   *
   * @param name  knowledge name
   * @param value value to set
   */
  public void set(String name, double value)
  {
    KnowledgeRecord kr = new KnowledgeRecord(value);
    set(name, kr);
    kr.free();
  }

  /**
   * Sets a knowledge value to a specified value.
   *
   * @param name  knowledge name
   * @param value value to set
   */
  public void set(String name, String value)
  {
    KnowledgeRecord kr = new KnowledgeRecord(value);
    set(name, kr);
    kr.free();
  }
}

