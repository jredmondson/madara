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
import com.madara.logger.GlobalLogger;
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
    String curHost = "";
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
      else if(args[i].equals("-f") || args[i].equals("--log-file"))
      {
        if (i + 1 < args.length)
        {
          GlobalLogger.addFile(args[i + 1]);
        }
        ++i;
      }
      else if(args[i].equals("-o") || args[i].equals("--host"))
      {
        if (i + 1 < args.length)
        {
          curHost = args[i + 1];
        }
        ++i;
      }
      else if(args[i].equals("-l") || args[i].equals("--log-level"))
      {
        if (i + 1 < args.length)
        {
          int logLevel  = Integer.parseInt(args[i + 1]);
          GlobalLogger.setLevel(logLevel);
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
      else if(args[i].equals("-q") || args[i].equals("--queue-length"))
      {
        if (i + 1 < args.length)
        {
          int length  = Integer.parseInt(args[i + 1]);
          settings.setQueueLength(length);
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
        GlobalLogger.log(0,"ERROR: unrecognized parameter:" + args[i]);
        GlobalLogger.log(0,"Arguments:");
        GlobalLogger.log(0,"  -b|--broadcast:  broadcast host to subscribe to");
        GlobalLogger.log(0,"  -f|--log-file:   log to a file as well");
        GlobalLogger.log(0,"  -i|--id:         the identifier. 0 is publisher.");
        GlobalLogger.log(0,"  -l|--log-level:  log level to use.");
        GlobalLogger.log(0,"  -m|--multicast:  multicast host to subscribe to");
        GlobalLogger.log(0,"  -o|--host:       this device's unique hostname");
        GlobalLogger.log(0,"  -q|--queue-length: queue length use for send/rcv");
        GlobalLogger.log(0,"  -s|--size: Size of packet to send");
        GlobalLogger.log(0,"  -t|--time: Time to stay active");
        GlobalLogger.log(0,"  -u|--udp:  udp host to subscribe to. First host");
        GlobalLogger.log(0,"             should be self.");
        
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
    
    KnowledgeBase knowledge = new KnowledgeBase(curHost, settings);
    
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
    
    GlobalLogger.log(0,"Test results:");
    GlobalLogger.log(0,"  Packet size: " + dataSize);
    GlobalLogger.log(0,"  Packets: " + filter.getCount());
    GlobalLogger.log(0,"  Elapsed Time (s): " + filter.getElapsed());
    GlobalLogger.log(0,"  Message Throughput (message/s): " +
      filter.getThroughput());
    GlobalLogger.log(0,"  Data Throughput (B/s): " +
      filter.getThroughput() * dataSize);
    
    settings.free();
    knowledge.free();
    filter.free();
  }
}
