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

public class LogRecord implements RecordFilter
{
  /**
   * Types of operations
   **/
  public String[] opTypes = {
    "IDLE_OPERATION",
    "SEND_OPERATION",
    "RECEIVE_OPERATION",
    "REBROADCAST_OPERATION"
  };
   
  /**
   * Filters a single knowledge update.
   * @param args    {@link java.util.List List&lt;KnowledgeRecord&gt;} of
   *                arguments passed to the function. args[0] is the value,
   *                args[1] is name of record, args[2] is type of operation,
   *                args[3] is send bandwidth used, args[4] is total bandwidth used,
   *                args[5] is the send timestamp, args[6] is current timestamp,
   *                args[7] is the knowledge domain
   * @param variables Local access to evaluate and compile methods
   * @return the unaltered record
   */
  public KnowledgeRecord filter(KnowledgeList args, Variables variables)
  {
    StringBuffer buffer = new StringBuffer();
    buffer.append("Filter args:\n");
    
    buffer.append("  [0:Record]: ");
    buffer.append(args.get(0).toString());
    buffer.append("\n");
    
    buffer.append("  [1:Record Name]: ");
    buffer.append(args.get(1).toString());
    buffer.append("\n");
    
    buffer.append("  [2:Operation Type]: ");
    
    int index = (int) args.get(2).toLong();
    if (index < opTypes.length)
    {
      buffer.append(opTypes[index]);
    }
    buffer.append("\n");
    
    buffer.append("  [3:Send Bandwidth]: ");
    buffer.append(args.get(3).toString());
    buffer.append(" B/s\n");
    
    buffer.append("  [4:Recv Bandwidth]: ");
    buffer.append(args.get(4).toString());
    buffer.append(" B/s\n");
    
    buffer.append("  [5:Update Time]: ");
    buffer.append(args.get(5).toString());
    buffer.append("\n");
    
    buffer.append("  [6:Current Time]: ");
    buffer.append(args.get(6).toString());
    buffer.append("\n");
    
    buffer.append("  [7:Domain]: ");
    buffer.append(args.get(7).toString());
    buffer.append("\n");
    
    buffer.append("  [8:Originator]: ");
    buffer.append(args.get(8).toString());
    buffer.append("\n");
    
    System.out.println(buffer.toString());
    
    return args.get(0);
  }
}

