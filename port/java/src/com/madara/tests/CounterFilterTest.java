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

package com.madara.tests;

import com.madara.KnowledgeBase;
import com.madara.filters.CounterFilter;
import com.madara.threads.BaseThread;
import com.madara.transport.QoSTransportSettings;
import com.madara.transport.TransportType;
import com.madara.threads.Threader;
import java.util.ArrayList;
import java.util.List;

/**
 * This class is a tester for the CounterFilter class
 */
public class CounterFilterTest
{
  static class Sender extends BaseThread
  {
    private com.madara.containers.String payload_ =
      new com.madara.containers.String ();
    private KnowledgeBase knowledge_ = null;
    
    public Sender (KnowledgeBase knowledge, int dataSize)
    {
      java.lang.String payload = new String(
        new char[dataSize]).replace('\0', ' ');
      knowledge_ = knowledge;
      payload_.setName(knowledge, "payload");
      payload_.set(payload);
    }
    
    public void run()
    {
      payload_.modify();
      knowledge_.sendModifieds();
    }
  }
  
  public static void main (String...args) throws InterruptedException, Exception
  {
    int id = 0;
    int dataSize = 1024;
    double activeTime = 10.0;
    
    QoSTransportSettings settings = new QoSTransportSettings();
    List<String> hosts = new ArrayList<String>();
    
    CounterFilter filter = new CounterFilter();
    
    for(int i = 0; i < args.length; ++i)
    {
      if(args[i].equals("-i") || args[i].equals("--id"))
      {
        if (i + 1 < args.length)
        {
          id = Integer.parseInt(args[i + 1]);
        }
        ++i;
      }
      else if(args[i].equals("-s") || args[i].equals("--size"))
      {
        if (i + 1 < args.length)
        {
          dataSize = Integer.parseInt(args[i + 1]);
        }
        ++i;
      }
      else if(args[i].equals("-t") || args[i].equals("--time"))
      {
        if (i + 1 < args.length)
        {
          activeTime = Double.parseDouble(args[i + 1]);
        }
        ++i;
      }
      else if(args[i].equals("-m") || args[i].equals("--multicast"))
      {
        if (i + 1 < args.length)
        {
          hosts.add(args[i + 1]);
          settings.setType(TransportType.MULTICAST_TRANSPORT);
        }
        ++i;
      }
      else if(args[i].equals("-b") || args[i].equals("--broadcast"))
      {
        if (i + 1 < args.length)
        {
          hosts.add(args[i + 1]);
          settings.setType(TransportType.BROADCAST_TRANSPORT);
        }
        ++i;
      }
      else if(args[i].equals("-u") || args[i].equals("--udp"))
      {
        if (i + 1 < args.length)
        {
          hosts.add(args[i + 1]);
          settings.setType(TransportType.UDP_TRANSPORT);
        }
        ++i;
      }
      else
      {
        System.out.println("ERROR: unrecognized parameter:" + args[i]);
        System.out.println("Arguments:");
        System.out.println("  -b|--broadcast:  broadcast host to subscribe to");
        System.out.println("  -i|--id:         the identifier. 0 is publisher.");
        System.out.println("  -m|--multicast:  multicast host to subscribe to");
        System.out.println("  -s|--size: Size of packet to send");
        System.out.println("  -t|--time: Time to stay active");
        System.out.println("  -u|--udp:  udp host to subscribe to. First host");
        System.out.println("             should be self.");
        
        return;
      }
    }
    
    if (hosts.isEmpty())
    {
      hosts.add("239.255.0.1:4150");
      settings.setType(TransportType.MULTICAST_TRANSPORT);
    }
    
    settings.setHosts(hosts.toArray(new String [hosts.size()]));
    
    if (id == 0)
    {
      filter.addSendFilterTo(settings);
    }
    else
    {
      filter.addReceiveFilterTo(settings);
    }
    
    KnowledgeBase knowledge = new KnowledgeBase("", settings);
    
    if (id == 0)
    {
      Threader threader = new Threader(knowledge);
      Sender sender = new Sender(knowledge, dataSize);
      threader.run(0.0, "sender", sender);
      com.madara.util.Utility.sleep (activeTime);
      threader.terminate();
      threader.waitForThreads();
      
      threader.free();
    }
    else
    {
      com.madara.util.Utility.sleep (activeTime);
    }
    
    System.out.println("Test results:");
    System.out.println("  Packet size: " + dataSize);
    System.out.println("  Packets: " + filter.getCount());
    System.out.println("  Elapsed Time (ns): " + filter.getElapsed());
    System.out.println("  Message Throughput (message/s): " +
      filter.getThroughput());
    System.out.println("  Data Throughput (B/s): " +
      filter.getThroughput() * dataSize);
    
    settings.free();
    knowledge.free();
    filter.free();
  }
}
