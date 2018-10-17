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

package ai.madara.tests;

import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.knowledge.containers.Integer;

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
      try
	{
		while(counter.inc() < target);
	} catch (MadaraDeadObjectException e)
	{
		// TODO Auto-generated catch block
		e.printStackTrace();
	}
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
      while(counter.incrementAndGet() < target);
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
      while(incAndGet() < target);
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
      while(run)
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
