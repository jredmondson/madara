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
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * This class is a tester for Java locks versus MADARA locks
 */
public class JavaLockPerformance
{
  static long target = 10000000;
  static long numThreads = 10;
    
  static class MadaraCounter implements Runnable
  {
    /**
     * A thread-safe counter 
     **/
    public Integer counter;

    public MadaraCounter(Integer counter)
    {
      this.counter = counter;
    }

    @Override
    public void run()
    {
      while (counter.inc() < target);
    }
  }
  
  public static class AtomicCounter implements Runnable
  {
    private final AtomicLong counter;

    public AtomicCounter(AtomicLong counter)
    {
      this.counter = counter;
    }

    @Override
    public void run()
    {
      while (counter.incrementAndGet() < target);
    }
  }
    
  public static class SynchronizedCounter implements Runnable
  {
    public static long counter = 0;

    public SynchronizedCounter()
    {
    }

    public synchronized long incAndGet()
    {
      return ++counter;
    }
    
    @Override
    public void run()
    {
      while (incAndGet() < target);
    }
  }
       
  public static class LockCounter implements Runnable
  {
    public static long counter = 0;
    public static Lock lock = new ReentrantLock();

    public LockCounter()
    {
    }
    
    @Override
    public void run()
    {
      boolean run = true;
      while (run)
      {
			  lock.lock();
			  try {
				  run = ++counter < target;
			  } finally {
				  lock.unlock();
			  }
		  }
    }
  }
   
  public static void testMadaraCounter() throws InterruptedException, Exception
  {
    KnowledgeBase knowledge = new KnowledgeBase();
    
    Integer counter = new Integer();
    counter.setName(knowledge, ".counter");
    
    Thread t1 = new Thread(new MadaraCounter(counter));
    Thread t2 = new Thread(new MadaraCounter(counter));
    Thread t3 = new Thread(new MadaraCounter(counter));
    Thread t4 = new Thread(new MadaraCounter(counter));
    Thread t5 = new Thread(new MadaraCounter(counter));
    Thread t6 = new Thread(new MadaraCounter(counter));
    Thread t7 = new Thread(new MadaraCounter(counter));
    Thread t8 = new Thread(new MadaraCounter(counter));
    Thread t9 = new Thread(new MadaraCounter(counter));
    Thread t10 = new Thread(new MadaraCounter(counter));
    
    knowledge.evaluateNoReturn(".start_time = #get_time()");

    t1.start();
    t2.start();
    t3.start();
    t4.start();
    t5.start();
    t6.start();
    t7.start();
    t8.start();
    t9.start();
    t10.start();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
    t10.join();
    
    knowledge.evaluateNoReturn(".end_time = #get_time();" +
      ".total_time = .end_time - .start_time;" +
      ".total_time_in_seconds = #double(.total_time) / 1000000000");

    knowledge.set(".num_threads", numThreads);
    knowledge.set(".target", target);
    
    knowledge.evaluateNoReturn(".avg_hertz = .counter / .total_time_in_seconds");
    knowledge.evaluateNoReturn(".avg_hertz_per_thread = .avg_hertz / .num_threads");

    knowledge.print("MADARA:\n");
    knowledge.print("  Time: {.total_time_in_seconds} s\n");
    knowledge.print("  Hz: {.avg_hertz}\n  Thread Hz: {.avg_hertz_per_thread}\n");
    
    // free the underlying C++ heap
    counter.free();
    knowledge.free();
  }
     
  public static void testAtomicCounter() throws InterruptedException, Exception
  {
    KnowledgeBase knowledge = new KnowledgeBase();
    AtomicLong counter = new AtomicLong(0);
    
    Thread t1 = new Thread(new AtomicCounter(counter));
    Thread t2 = new Thread(new AtomicCounter(counter));
    Thread t3 = new Thread(new AtomicCounter(counter));
    Thread t4 = new Thread(new AtomicCounter(counter));
    Thread t5 = new Thread(new AtomicCounter(counter));
    Thread t6 = new Thread(new AtomicCounter(counter));
    Thread t7 = new Thread(new AtomicCounter(counter));
    Thread t8 = new Thread(new AtomicCounter(counter));
    Thread t9 = new Thread(new AtomicCounter(counter));
    Thread t10 = new Thread(new AtomicCounter(counter));
    
    knowledge.evaluateNoReturn(".start_time = #get_time()");

    t1.start();
    t2.start();
    t3.start();
    t4.start();
    t5.start();
    t6.start();
    t7.start();
    t8.start();
    t9.start();
    t10.start();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
    t10.join();
    
    
    knowledge.evaluateNoReturn(".end_time = #get_time();" +
      ".total_time = .end_time - .start_time;" +
      ".total_time_in_seconds = #double(.total_time) / 1000000000");

    knowledge.set(".num_threads", numThreads);
    knowledge.set(".target", target);
    knowledge.set(".counter", counter.get());
    
    knowledge.evaluateNoReturn(".avg_hertz = .counter / .total_time_in_seconds");
    knowledge.evaluateNoReturn(".avg_hertz_per_thread = .avg_hertz / .num_threads");

    knowledge.print("ATOMIC:\n");
    knowledge.print("  Time: {.total_time_in_seconds} s\n");
    knowledge.print("  Hz: {.avg_hertz}\n  Thread Hz: {.avg_hertz_per_thread}\n");
    
    // free the underlying C++ heap
    knowledge.free();
  }
     
  public static void testSynchronizedCounter() throws InterruptedException, Exception
  {
    KnowledgeBase knowledge = new KnowledgeBase();
    
    Thread t1 = new Thread(new SynchronizedCounter());
    Thread t2 = new Thread(new SynchronizedCounter());
    Thread t3 = new Thread(new SynchronizedCounter());
    Thread t4 = new Thread(new SynchronizedCounter());
    Thread t5 = new Thread(new SynchronizedCounter());
    Thread t6 = new Thread(new SynchronizedCounter());
    Thread t7 = new Thread(new SynchronizedCounter());
    Thread t8 = new Thread(new SynchronizedCounter());
    Thread t9 = new Thread(new SynchronizedCounter());
    Thread t10 = new Thread(new SynchronizedCounter());
    
    knowledge.evaluateNoReturn(".start_time = #get_time()");

    t1.start();
    t2.start();
    t3.start();
    t4.start();
    t5.start();
    t6.start();
    t7.start();
    t8.start();
    t9.start();
    t10.start();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
    t10.join();
    
    knowledge.evaluateNoReturn(".end_time = #get_time();" +
      ".total_time = .end_time - .start_time;" +
      ".total_time_in_seconds = #double(.total_time) / 1000000000");

    knowledge.set(".num_threads", numThreads);
    knowledge.set(".target", target);
    knowledge.set(".counter", SynchronizedCounter.counter);
    
    knowledge.evaluateNoReturn(".avg_hertz = .counter / .total_time_in_seconds");
    knowledge.evaluateNoReturn(".avg_hertz_per_thread = .avg_hertz / .num_threads");

    knowledge.print("SYNCHRONIZE:\n");
    knowledge.print("  Time: {.total_time_in_seconds} s\n");
    knowledge.print("  Hz: {.avg_hertz}\n  Thread Hz: {.avg_hertz_per_thread}\n");
    
    // free the underlying C++ heap
    knowledge.free();
  }
  
       
  public static void testLockedCounter() throws InterruptedException, Exception
  {
    KnowledgeBase knowledge = new KnowledgeBase();
    
    Thread t1 = new Thread(new LockCounter());
    Thread t2 = new Thread(new LockCounter());
    Thread t3 = new Thread(new LockCounter());
    Thread t4 = new Thread(new LockCounter());
    Thread t5 = new Thread(new LockCounter());
    Thread t6 = new Thread(new LockCounter());
    Thread t7 = new Thread(new LockCounter());
    Thread t8 = new Thread(new LockCounter());
    Thread t9 = new Thread(new LockCounter());
    Thread t10 = new Thread(new LockCounter());
    
    knowledge.evaluateNoReturn(".start_time = #get_time()");

    t1.start();
    t2.start();
    t3.start();
    t4.start();
    t5.start();
    t6.start();
    t7.start();
    t8.start();
    t9.start();
    t10.start();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
    t10.join();
    
    knowledge.evaluateNoReturn(".end_time = #get_time();" +
      ".total_time = .end_time - .start_time;" +
      ".total_time_in_seconds = #double(.total_time) / 1000000000");

    knowledge.set(".num_threads", numThreads);
    knowledge.set(".target", target);
    knowledge.set(".counter", SynchronizedCounter.counter);
    
    knowledge.evaluateNoReturn(".avg_hertz = .counter / .total_time_in_seconds");
    knowledge.evaluateNoReturn(".avg_hertz_per_thread = .avg_hertz / .num_threads");

    knowledge.print("LOCK:\n");
    knowledge.print("  Time: {.total_time_in_seconds} s\n");
    knowledge.print("  Hz: {.avg_hertz}\n  Thread Hz: {.avg_hertz_per_thread}\n");
    
    // free the underlying C++ heap
    knowledge.free();
  }
  
  public static void main(String...args) throws InterruptedException, Exception
  {
    System.out.println("Testing mutexing for counters");
    System.out.println(" Target: " + target);
    System.out.println(" Threads: " + numThreads);
    testMadaraCounter();
    testAtomicCounter();
    testSynchronizedCounter();
    testLockedCounter();
  }
}
