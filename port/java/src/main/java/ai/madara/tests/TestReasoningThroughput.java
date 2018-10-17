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

import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.List;

import ai.madara.exceptions.MadaraDeadObjectException;
import ai.madara.knowledge.KnowledgeBase;
import ai.madara.knowledge.KnowledgeBase.CompiledExpression;
import ai.madara.knowledge.KnowledgeBase.KnowledgeBaseLockedException;
import ai.madara.knowledge.KnowledgeRecord;

public class TestReasoningThroughput
{

  private static final double GHZ = 1000000000;
  private static final double MHZ = 1000000;
  private static final double KHZ = 1000;


  private static final long RUNCOUNT = 10;
  private static final long ITERATIONS = 100000;
  private static final long EVALUATIONS = RUNCOUNT * ITERATIONS;

  private static List<Test> tests = new ArrayList<Test> ();

  private static abstract class Test
  {
    public long elapsed = 0;
    public long hertz = 0;

    public abstract long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException;
    public abstract String name ();
  }


  private static class SimpleReinforcement extends Test
  {
    @Override
    public String name ()
    {
      return "KaRL: Simple Reinforcements";
    }
    @Override
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException
    {
      knowledge.clear ();
      Timer timer = new Timer ();

      timer.start ();
      for(int x = 0; x < ITERATIONS; ++x)
      {
        knowledge.evaluateNoReturn ("++.var1");
      }
      timer.stop ();

      KnowledgeRecord kr = knowledge.get (".var1");
      print (timer.elapsed (), kr, name ());
      kr.free ();

      return timer.elapsed ();
    }
  }

  private static class LargeReinforcement extends Test
  {
    @Override
    public String name ()
    {
      return "KaRL: Large Reinforcements";
    }
    @Override
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException
    {
      knowledge.clear ();
      Timer timer = new Timer ();

      long maxSize = ITERATIONS > 1000 ? 1000 : ITERATIONS;
      long actualIterations = ITERATIONS > 1000 ? ITERATIONS / 1000 : 1;

      StringBuilder sb = new StringBuilder ();
      for(int x = 0; x < maxSize; x++)
        sb.append ("++.var1;");

      final String eval = sb.toString ();

      timer.start ();
      for(int x = 0; x < actualIterations; ++x)
      {
        knowledge.evaluateNoReturn (eval);
      }
      timer.stop ();

      KnowledgeRecord kr = knowledge.get (".var1");
      print (timer.elapsed (), kr, name ());
      kr.free ();

      return timer.elapsed ();
    }
  }

  private static class SimpleInference extends Test
  {
    @Override
    public String name ()
    {
      return "KaRL: Simple Inference    ";
    }
    @Override
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException
    {
      knowledge.clear ();
      Timer timer = new Timer ();

      timer.start ();
      for(int x = 0; x < ITERATIONS; ++x)
      {
        knowledge.evaluateNoReturn ("1 => ++.var1");
      }
      timer.stop ();

      KnowledgeRecord kr = knowledge.get (".var1");
      print (timer.elapsed (), kr, name ());
      kr.free ();

      return timer.elapsed ();
    }
  }

  private static class LargeInference extends Test
  {
    @Override
    public String name ()
    {
      return "KaRL: Large Inference     ";
    }
    @Override
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException
    {
      knowledge.clear ();
      Timer timer = new Timer ();

      long maxSize = ITERATIONS > 1000 ? 1000 : ITERATIONS;
      long actualIterations = ITERATIONS > 1000 ? ITERATIONS / 1000 : 1;

      StringBuilder sb = new StringBuilder ();
      for(int x = 0; x < maxSize; x++)
        sb.append ("1 => ++.var1;");

      final String eval = sb.toString ();

      timer.start ();
      for(int x = 0; x < actualIterations; ++x)
      {
        knowledge.evaluateNoReturn (eval);
      }
      timer.stop ();

      KnowledgeRecord kr = knowledge.get (".var1");
      print (timer.elapsed (), kr, name ());
      kr.free ();

      return timer.elapsed ();
    }
  }

  private static class CompiledSR extends Test
  {
    @Override
    public String name ()
    {
      return "KaRL: Compiled SR         ";
    }
    @Override
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException
    {
      knowledge.clear ();
      Timer timer = new Timer ();

      CompiledExpression ce = knowledge.compile ("++.var1");

      timer.start ();
      for(int x = 0; x < ITERATIONS; ++x)
      {
        knowledge.evaluateNoReturn (ce);
      }
      timer.stop ();

      ce.free ();

      KnowledgeRecord kr = knowledge.get (".var1");
      print (timer.elapsed (), kr, name ());
      kr.free ();

      return timer.elapsed ();
    }
  }

  private static class CompiledLR extends Test
  {
    @Override
    public String name ()
    {
      return "KaRL: Compiled LR         ";
    }
    @Override
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException
    {
      knowledge.clear ();
      Timer timer = new Timer ();

      long maxSize = ITERATIONS > 1000 ? 1000 : ITERATIONS;
      long actualIterations = ITERATIONS > 1000 ? ITERATIONS / 1000 : 1;

      StringBuilder sb = new StringBuilder ();
      for(int x = 0; x < maxSize; x++)
        sb.append ("++.var1;");

      CompiledExpression ce = knowledge.compile (sb.toString ());

      timer.start ();
      for(int x = 0; x < actualIterations; ++x)
      {
        knowledge.evaluateNoReturn (ce);
      }
      timer.stop ();

      ce.free ();

      KnowledgeRecord kr = knowledge.get (".var1");
      print (timer.elapsed (), kr, name ());
      kr.free ();

      return timer.elapsed ();
    }
  }

  private static class CompiledSI extends Test
  {
    @Override
    public String name ()
    {
      return "KaRL: Compiled SI        ";
    }
    @Override
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException
    {
      knowledge.clear ();
      Timer timer = new Timer ();

      CompiledExpression ce = knowledge.compile ("1 => ++.var1");

      timer.start ();
      for(int x = 0; x < ITERATIONS; ++x)
      {
        knowledge.evaluateNoReturn (ce);
      }
      timer.stop ();

      ce.free ();

      KnowledgeRecord kr = knowledge.get (".var1");
      print (timer.elapsed (), kr, name ());
      kr.free ();

      return timer.elapsed ();
    }
  }

  private static class CompiledLI extends Test
  {
    @Override
    public String name ()
    {
      return "KaRL: Compiled LI         ";
    }
    @Override
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException
    {
      knowledge.clear ();
      Timer timer = new Timer ();

      long maxSize = ITERATIONS > 1000 ? 1000 : ITERATIONS;
      long actualIterations = ITERATIONS > 1000 ? ITERATIONS / 1000 : 1;

      StringBuilder sb = new StringBuilder ();
      for(int x = 0; x < maxSize; x++)
        sb.append ("1 => ++.var1;");

      CompiledExpression ce = knowledge.compile (sb.toString ());

      timer.start ();
      for(int x = 0; x < actualIterations; ++x)
      {
        knowledge.evaluateNoReturn (ce);
      }
      timer.stop ();

      ce.free ();

      KnowledgeRecord kr = knowledge.get (".var1");
      print (timer.elapsed (), kr, name ());
      kr.free ();

      return timer.elapsed ();
    }
  }

  private static class LoopedSR extends Test
  {
    @Override
    public String name ()
    {
      return "KaRL: Looped SR         ";
    }
    @Override
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException
    {
      knowledge.clear ();
      Timer timer = new Timer ();


      knowledge.set (".iterations", ITERATIONS);

      CompiledExpression ce = knowledge.compile (".var2[.iterations) (++.var1)");

      timer.start ();
      knowledge.evaluateNoReturn (ce);
      timer.stop ();

      ce.free ();

      KnowledgeRecord kr = knowledge.get (".var1");
      print (timer.elapsed (), kr, name ());
      kr.free ();

      return timer.elapsed ();
    }
  }

  private static class OptimalLoop extends Test
  {
    @Override
    public String name ()
    {
      return "KaRL: Optimal Loop      ";
    }
    @Override
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException
    {
      knowledge.clear ();
      Timer timer = new Timer ();


      knowledge.set (".iterations", ITERATIONS);

      CompiledExpression ce = knowledge.compile (".var2[.iterations)");

      timer.start ();
      knowledge.evaluateNoReturn (ce);
      timer.stop ();

      ce.free ();

      KnowledgeRecord kr = knowledge.get (".var1");
      print (timer.elapsed (), kr, name ());
      kr.free ();

      return timer.elapsed ();
    }
  }

  private static class LoopedSI extends Test
  {
    @Override
    public String name ()
    {
      return "KaRL: Looped SI         ";
    }
    @Override
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException
    {
      knowledge.clear ();
      Timer timer = new Timer ();


      knowledge.set (".iterations", ITERATIONS);

      CompiledExpression ce = knowledge.compile (".var2[.iterations) (1 => ++.var1)");

      timer.start ();
      knowledge.evaluateNoReturn (ce);
      timer.stop ();

      ce.free ();

      KnowledgeRecord kr = knowledge.get (".var1");
      print (timer.elapsed (), kr, name ());
      kr.free ();

      return timer.elapsed ();
    }
  }

  private static class OptimalReinforcement extends Test
  {

    @Override
    public String name ()
    {
      return "Java: Optimal Reinforcement";
    }

    @Override
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException, MadaraDeadObjectException
    {
      knowledge.clear ();
      Timer timer = new Timer ();

      int var1 = 0;
      timer.start ();
      for(int x = 0; x < ITERATIONS; ++x)
        ++var1;
      timer.stop ();

      print (timer.elapsed (), "" + var1, name ());

      return timer.elapsed ();
    }
  }

  public static String formatNumber (long num)
  {
    return NumberFormat.getInstance ().format (num);
  }

  public static void print (long time, KnowledgeRecord value, String type)
  {
    print (time, value.toString (), type);
  }

  public static void print (long time, String value, String type)
  {
    System.out.println (type);
    System.out.println ("  ops=" + formatNumber (ITERATIONS) + ", time=" + formatNumber (time) + " ns, value=" + value);
  }


  public static String toLegibleHertz (final long hertz)
  {
    double freq = (double) hertz / GHZ;
    if(freq >= 1)
      return "" + (Math.round (freq * 100.0) / 100.0) + " ghz";

    freq = (double) hertz / MHZ;
    if(freq >= 1)
      return "" + (Math.round (freq * 100.0) / 100.0) + " mhz";

    freq = (double) hertz / KHZ;
    if(freq >= 1)
      return "" + (Math.round (freq * 100.0) / 100.0) + " khz";

    freq = (double) hertz;

    return "" + (Math.round (freq * 100.0) / 100.0) + " hz";
  }

  public static void main (String...args) throws KnowledgeBaseLockedException, MadaraDeadObjectException
  {
    KnowledgeBase knowledge = new KnowledgeBase ();

    tests.add (new SimpleReinforcement ());
    tests.add (new LargeReinforcement ());
    tests.add (new SimpleInference ());
    tests.add (new LargeInference ());
    tests.add (new CompiledSR ());
    tests.add (new CompiledLR ());
    tests.add (new CompiledSI ());
    tests.add (new CompiledLI ());
    tests.add (new LoopedSR ());
    tests.add (new OptimalLoop ());
    tests.add (new LoopedSI ());
    tests.add (new OptimalReinforcement ());

    for(int x = 0; x < RUNCOUNT; ++x)
    {
      for(Test test : tests)
      {
        test.elapsed += test.test (knowledge);
      }
    }

    for(Test test : tests)
    {
      if(test.elapsed == 0)
        test.elapsed = 1;

      test.hertz = (1000000000 * EVALUATIONS) / test.elapsed;
    }

    System.out.println ("\n\nTotal time taken for each Test with " + ITERATIONS + " iterations * " + RUNCOUNT + " tests was:");
    System.out.println ("=========================================================================");

    for(Test test : tests)
    {
      System.out.println (String.format (" %s\t\t%30s ns", test.name (), formatNumber (test.elapsed)));
    }
    System.out.println ("=========================================================================");

    System.out.println ("\n\nAverage time taken per rule evaluation was:");
    System.out.println ("=========================================================================");

    for(Test test : tests)
    {
      System.out.println (String.format (" %s\t\t%30s ns", test.name (), formatNumber (test.elapsed / EVALUATIONS)));
    }
    System.out.println ("=========================================================================");

    System.out.println ("\n\nHertz for each Test with " + ITERATIONS + " iterations * " + RUNCOUNT + " tests was:");
    System.out.println ("=========================================================================");

    for(Test test : tests)
    {
      System.out.println (String.format (" %s\t\t%33s", test.name (), toLegibleHertz (test.hertz)));
    }
    System.out.println ("=========================================================================");

  }

  private static class Timer
  {

    private long start = 0;
    private long stop = 0;

    public void start ()
    {
      start = System.nanoTime ();
    }

    public void stop ()
    {
      stop = System.nanoTime ();
    }

    public long elapsed ()
    {
      return stop - start;
    }
  }

}
