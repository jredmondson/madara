/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
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
