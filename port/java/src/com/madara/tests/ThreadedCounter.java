/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/

package com.madara.tests;

import com.madara.KnowledgeBase;
import com.madara.MadaraLog;
import com.madara.MadaraLog.MadaraLogLevel;
import com.madara.threads.BaseThread;
import com.madara.threads.Threader;
import com.madara.containers.Integer;

/**
 * This class is a tester for the com.madara.containers package
 */
public class ThreadedCounter extends BaseThread
{
  /**
   * A thread-safe counter 
   **/
  public Integer counter;
  static long target = 100000000;
  
  /**
   * Initialize the counter variable
   **/
  public void init(KnowledgeBase knowledge)
  {
    counter = new Integer();
    counter.setName(knowledge, ".counter");
  }
  
  /**
   * Executes the main thread logic.
   **/
  public void run()
  {
    // increment the counter
    long value = counter.inc();
    
    //System.out.println ("Counter value is " + value);
  }
  
  /**
   * Clean up the counter residue
   **/
  public void cleanup()
  {
    // free the underlying C++ heap
    counter.free();
  }
  
  public static void main(String...args) throws InterruptedException, Exception
  {
    KnowledgeBase knowledge = new KnowledgeBase();
    //MadaraLog.setLogLevel(MadaraLogLevel.MADARA_LOG_MAJOR_EVENT);
    Integer counter = new Integer();
    counter.setName(knowledge, ".counter");
    
    long numThreads = 1;
    double hertz = 300000;
  
    if (args.length > 0)
    {
      // first arg is the number of threads
      try
      {
        numThreads = java.lang.Long.parseLong(args[0]);
      }
      catch (NumberFormatException e)
      {
        System.err.println("Argument for numThreads " + args[0] +
          " must be an integer.");
        
        System.exit(1);
      }
      
      if (args.length > 1)
      {
        // second arg is the target
        try
        {
          target = java.lang.Long.parseLong(args[1]);
        }
        catch (NumberFormatException e)
        {
          System.err.println("Argument for target " + args[1] +
            " must be an integer.");
        
          System.exit(1);
        }
        
        if (args.length > 2)
        {
          // second arg is the target
          try
          {
            hertz = java.lang.Double.parseDouble(args[2]);
          }
          catch (NumberFormatException e)
          {
            System.err.println("Argument for hertz " + args[2] +
              " must be a double.");
        
            System.exit(1);
          }
        }
      }
    }
    
    // create a threader for running threads
    Threader threader = new Threader(knowledge);

    for (int i = 0; i < numThreads; ++i)
    {
      threader.startPaused(hertz, "thread" + i, new ThreadedCounter()); 
    }
    // start 4 threads in a paused state
    //threader.startPaused(1000000.0, "thread1", new ThreadedCounter());
    
    knowledge.evaluateNoReturn(".start_time = #get_time()");

    // resume all threads
    threader.resume();
    
    while(counter.get() < target)
    {
      // sleep for a second before checking again
      java.lang.Thread.sleep(250);
    }
    
    knowledge.evaluateNoReturn(".end_time = #get_time();" +
      ".total_time = .end_time - .start_time;" +
      ".total_time_in_seconds = #double(.total_time) / 1000000000");

    knowledge.set(".num_threads", numThreads);
    knowledge.set(".target", target);
    
    knowledge.evaluateNoReturn(".avg_hertz = .counter / .total_time_in_seconds");
    knowledge.evaluateNoReturn(".avg_hertz_per_thread = .avg_hertz / .num_threads");

    // resume all threads
    threader.terminate();
    
    // print all knowledge
    knowledge.print();
    knowledge.print("And we're done.\n");
    
    // wait for all threads to finish
    threader.waitForThreads();
    
    // free the underlying C++ heap
    threader.free();
    counter.free();
    knowledge.free();
  }
}
