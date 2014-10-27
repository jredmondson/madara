/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara;

/**
 * Interface for defining a Java method to be executed by MADARA
 */
public interface MadaraFunction
{
  /**
   * Java implementation of a MADARA function. <b>DO NOT</b> invoke methods on an instance of
   * {@link com.madara.KnowledgeBase KnowledgeBase} in this method
   *
   * @param args    {@link java.util.List List&lt;KnowledgeRecord&gt;} of arguments passed to the function
   * @param variables Local access to evaluate and compile methods
   * @return A {@link com.madara.KnowledgeRecord KnowledgeRecord} containing the result of the function
   */
  public KnowledgeRecord execute(KnowledgeList args, Variables variables);
}

