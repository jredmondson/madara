/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
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

