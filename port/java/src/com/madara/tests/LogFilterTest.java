/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/

package com.madara.tests;

import com.madara.KnowledgeBase;
import com.madara.KnowledgeRecord;
import com.madara.KnowledgeType;
import com.madara.EvalSettings;
import com.madara.containers.Vector;
import com.madara.transport.filters.LogAggregate;
import com.madara.transport.filters.LogRecord;
import com.madara.transport.QoSTransportSettings;
import com.madara.transport.TransportType;

/**
 * This class is a tester for the com.madara.containers package
 */
public class LogFilterTest
{
  public static void main (String...args) throws InterruptedException, Exception
  {
    QoSTransportSettings settings = new QoSTransportSettings();
    settings.setHosts(new String[]{"239.255.0.1:4150"});
    settings.setType(TransportType.MULTICAST_TRANSPORT);
    
    System.out.println ("Adding individual record log filter to receive.");
    settings.addReceiveFilter(KnowledgeType.ALL_TYPES, new LogRecord());
    settings.addReceiveFilter(new LogAggregate());
    System.out.println ("Adding individual record log filter to send.");
    settings.addSendFilter(KnowledgeType.ALL_TYPES, new LogRecord());
    settings.addSendFilter(new LogAggregate());
    
    KnowledgeBase knowledge = new KnowledgeBase("", settings);
    
    System.out.println ("Beginning to loop.");
    for (int i = 0; i < 60; ++i)
    {
      System.out.println ("Sending an update.");
      knowledge.set(".base", i);
      knowledge.evaluate(".i[0->10)(updates.{.i}=.base * 5 + .i)");
      
      java.lang.Thread.sleep(1000);
    }
    
    // print all knowledge
    knowledge.print();
    
    knowledge.print("And we're done.\n");
  }
}
