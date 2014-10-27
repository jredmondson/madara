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

