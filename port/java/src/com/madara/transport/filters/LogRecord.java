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

