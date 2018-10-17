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

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.knowledge.containers.Integer;
import ai.madara.threads.BaseThread;
import ai.madara.threads.Threader;

/**
 * This class is a tester for the ai.madara.knowledge.containers package
 */
public class ThreadedCounterBlaster extends BaseThread
{
  /**
   * A thread-safe counter
   **/
  public Integer counter;
  static long target = 100000000;

  /**
   * Initialize the counter variable
 *  @throws MadaraDeadObjectException throws exception if object is already released
   **/
  public void init(KnowledgeBase knowledge) throws MadaraDeadObjectException
  {
    counter = new Integer();
    counter.setName(knowledge, ".counter");
  }

  /**
   * Executes the main thread logic.
 *  @throws MadaraDeadObjectException throws exception if object is already released
   **/
  public void run() throws MadaraDeadObjectException
  {
    // increment the counter until it is at least equal to the value
    long value = counter.inc();
    while(value < target)
      value = counter.inc();
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
    Integer counter = new Integer();
    counter.setName(knowledge, ".counter");

    long numThreads = 1;

    if(args.length > 0)
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

      if(args.length > 1)
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
      }
    }

    // create a threader for running threads
    Threader threader = new Threader(knowledge);

    for(int i = 0; i < numThreads; ++i)
    {
      threader.run("thread" + i, new ThreadedCounterBlaster());
    }
    // start 4 threads in a paused state
    //threader.run("thread1", new ThreadedCounterBlaster());

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
