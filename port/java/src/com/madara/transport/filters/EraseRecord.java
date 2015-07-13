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
import com.madara.EvalSettings;
import com.madara.KnowledgeBase;
import com.madara.KnowledgeType;
import com.madara.transport.filters.LogAggregate;
import com.madara.transport.filters.LogRecord;
import com.madara.transport.QoSTransportSettings;
import com.madara.transport.TransportType;

public class EraseRecord implements RecordFilter
{
  /**
   * Removes the record from the packet
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
    return new KnowledgeRecord();
  }
  
  public static void main (String [] args) throws InterruptedException, Exception
  {
    QoSTransportSettings settings = new QoSTransportSettings();
    settings.setHosts(new String[]{"239.255.0.1:4150"});
    settings.setType(TransportType.MULTICAST_TRANSPORT);
    
    System.out.println ("Adding individual erase record filter to receive.");
    settings.addReceiveFilter(KnowledgeType.ALL_TYPES, new LogRecord());
    settings.addReceiveFilter(KnowledgeType.INTEGER, new EraseRecord());
    settings.addReceiveFilter(new LogAggregate());
    System.out.println ("Adding individual erase record filter to send.");
    settings.addSendFilter(KnowledgeType.ALL_TYPES, new LogRecord());
    settings.addSendFilter(KnowledgeType.INTEGER, new EraseRecord());
    settings.addSendFilter(new LogAggregate());
    
    KnowledgeBase knowledge = new KnowledgeBase("", settings);
    
    EvalSettings delaySending = new EvalSettings();
    delaySending.setDelaySendingModifieds(true);
    
    System.out.println ("Beginning to loop.");
    for(int i = 0; i < 10; ++i)
    {
      System.out.println("Sending an update.");
      
      if(args.length == 1 && args[0].equals("1"))
      {
        System.out.println("Agent is James Bond. Writing info.");
        knowledge.set("name1", "James Bond", delaySending);
        knowledge.set("age1", 45, delaySending);
        knowledge.set("weight1", 190.0);
        //knowledge.evaluate("name0=\"James Bond\"; age0=45; weight0=190.0");
      }
      else
      {
        System.out.println("Agent is Alfred Bond. Writing info.");
        knowledge.set("name0", "Alfred Bond", delaySending);
        knowledge.set("age0", 20, delaySending);
        knowledge.set("weight0", 220.0);
        //knowledge.evaluate("name0=\"Alfred Bond\"; age0=20; weight0=220.0");
      }
      
      java.lang.Thread.sleep(1000);
    }
    
    // print all knowledge
    knowledge.print();
    
    if(args.length == 1 && args[0].equals("1"))
    {
      if(knowledge.exists("age0"))
      {
        System.out.println("FAIL: we received age0."); 
      }
      else if(!knowledge.exists("name0"))
      {
        System.out.println("FAIL: did not receive info from agent 0.");
      } 
      else
      {
        System.out.println("SUCCESS: we did not receive age0."); 
      }
    }
    else
    {
      if(knowledge.exists("age1"))
      {
        System.out.println("FAIL: we received age0."); 
      }
      else if(!knowledge.exists("name1"))
      {
        System.out.println("FAIL: did not receive info from agent 1.");
      } 
      else
      {
        System.out.println("SUCCESS: we did not receive age1."); 
      }
    }
  }
}

