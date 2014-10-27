/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/
package com.madara.transport.filters;

import com.madara.KnowledgeRecord;
import com.madara.Variables;
import com.madara.KnowledgeList;

public interface RecordFilter
{
  /**
   * Filters a single knowledge update.
   * @param args    {@link java.util.List List&lt;KnowledgeRecord&gt;} of
   *                arguments passed to the function. args[0] is the value,
   *                args[1] is name of record, args[2] is type of operation,
   *                args[3] is send bandwidth used, args[4] is total bandwidth used,
   *                args[5] is the send timestamp, args[6] is current timestamp,
   *                args[7] is the knowledge domain
   * @param variables Local access to evaluate and compile methods
   * @return A {@link com.madara.KnowledgeRecord KnowledgeRecord} containing the result of the function
   */
  public KnowledgeRecord filter(KnowledgeList args, Variables variables);
}

