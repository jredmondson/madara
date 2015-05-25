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
import com.madara.KnowledgeRecord;
import com.madara.MadaraLog;
import com.madara.MadaraLog.MadaraLogLevel;
import com.madara.threads.BaseThread;
import com.madara.threads.Threader;
import com.madara.containers.Queue;
import com.madara.containers.Integer;

import java.util.Random;
import java.lang.System;

// a counter thread that works with other counters
public class ProducerConsumerQueueTest
{
  static class Producer extends BaseThread
  {
    // Producer's initialization method
    public void init(KnowledgeBase context)
    {
      generator = new Random(System.currentTimeMillis());
      
      queue = new Queue();
      queue.setName(context, ".jobs");
      queue.resize(-1);
    }

    // Producer's run method.
    public void run()
    {
      // generate a new job until terminated
      long job = (long)generator.nextInt(4);
      queue.enqueue(job);
    }
    
    // Producer's cleanup method
    public void cleanup()
    {
      queue.free();
    }
    
    Random generator;
    Queue queue;
  }

  static class Consumer extends BaseThread
  {
    // Consumer's initialization method
    public void init(KnowledgeBase context)
    {
      queue = new Queue();
      queue.setName(context, ".jobs");
      queue.resize(-1);
      
      jobsCompleted = new Integer();
      jobsCompleted.setName(context, ".jobsCompleted");
    }

    // Consumer's run method.
    public void run()
    {
      KnowledgeRecord job = queue.dequeue(false);
      
      if(job.isValid())
      {
        long value = job.toLong();
      
        if(value == 0)
        {
          System.out.println(jobsCompleted.get() + ": Checking News");
        }
        else if(value == 1)
        {
          System.out.println(jobsCompleted.get() + ": Checking Stocks");
        }
        else if(value == 2)
        {
          System.out.println(jobsCompleted.get() + ": Checking Email");
        }
        else if(value == 3)
        {
          System.out.println(jobsCompleted.get() + ": Checking Schedule");
        }
        else
        {
          System.out.println(jobsCompleted.get() + ": Unknown Job Type");
        }
        
        jobsCompleted.inc();
      }
      
      // clean up the removed job
      job.free();
    }
    
    // Consumer's cleanup method
    public void cleanup()
    {
      // clean up the private copies of our queue and counter
      queue.free();
      jobsCompleted.free();
    }
    
    Queue queue;
    Integer jobsCompleted;
  }

  
  public static void main(String...args) throws InterruptedException, Exception
  {
    KnowledgeBase knowledge = new KnowledgeBase();
    
    Threader threader = new Threader(knowledge);
    Integer jobsCompleted = new Integer();
    jobsCompleted.setName(knowledge, ".jobsCompleted");
    
    Queue jobs = new Queue();
    jobs.setName(knowledge, ".jobs");
    jobs.resize(100);
    
    // run the two counter threads, one at 20hz and one at 40hz
    threader.run(20.0, "producer", new Producer());
    threader.run(40.0, "consumer", new Consumer());

    while(jobsCompleted.get() < 100)
    {
      java.lang.Thread.sleep(1000);
    }
    
    // terminate all threads
    threader.terminate();
    
    // wait for all threads to finish
    threader.waitForThreads();
    
    threader.free();
    knowledge.free();
  }
}
