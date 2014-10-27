/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/

package com.madara.tests;

import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.List;

import com.madara.KnowledgeBase;
import com.madara.KnowledgeBase.CompiledExpression;
import com.madara.KnowledgeBase.KnowledgeBaseLockedException;
import com.madara.KnowledgeRecord;

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
    
    public abstract long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException;
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
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
    {
      knowledge.clear ();
      Timer timer = new Timer ();
      
      timer.start ();
      for (int x = 0; x < ITERATIONS; ++x)
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
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
    {
      knowledge.clear ();
      Timer timer = new Timer ();
      
      long maxSize = ITERATIONS > 1000 ? 1000 : ITERATIONS;
      long actualIterations = ITERATIONS > 1000 ? ITERATIONS / 1000 : 1;
      
      StringBuilder sb = new StringBuilder ();
      for (int x = 0; x < maxSize; x++)
        sb.append ("++.var1;");
      
      final String eval = sb.toString ();
      
      timer.start ();
      for (int x = 0; x < actualIterations; ++x)
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
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
    {
      knowledge.clear ();
      Timer timer = new Timer ();
      
      timer.start ();
      for (int x = 0; x < ITERATIONS; ++x)
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
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
    {
      knowledge.clear ();
      Timer timer = new Timer ();
      
      long maxSize = ITERATIONS > 1000 ? 1000 : ITERATIONS;
      long actualIterations = ITERATIONS > 1000 ? ITERATIONS / 1000 : 1;
      
      StringBuilder sb = new StringBuilder ();
      for (int x = 0; x < maxSize; x++)
        sb.append ("1 => ++.var1;");
      
      final String eval = sb.toString ();
      
      timer.start ();
      for (int x = 0; x < actualIterations; ++x)
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
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
    {
      knowledge.clear ();
      Timer timer = new Timer ();
      
      CompiledExpression ce = knowledge.compile ("++.var1");
      
      timer.start ();
      for (int x = 0; x < ITERATIONS; ++x)
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
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
    {
      knowledge.clear ();
      Timer timer = new Timer ();
      
      long maxSize = ITERATIONS > 1000 ? 1000 : ITERATIONS;
      long actualIterations = ITERATIONS > 1000 ? ITERATIONS / 1000 : 1;
      
      StringBuilder sb = new StringBuilder ();
      for (int x = 0; x < maxSize; x++)
        sb.append ("++.var1;");
      
      CompiledExpression ce = knowledge.compile (sb.toString ());
      
      timer.start ();
      for (int x = 0; x < actualIterations; ++x)
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
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
    {
      knowledge.clear ();
      Timer timer = new Timer ();
      
      CompiledExpression ce = knowledge.compile ("1 => ++.var1");
      
      timer.start ();
      for (int x = 0; x < ITERATIONS; ++x)
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
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
    {
      knowledge.clear ();
      Timer timer = new Timer ();
      
      long maxSize = ITERATIONS > 1000 ? 1000 : ITERATIONS;
      long actualIterations = ITERATIONS > 1000 ? ITERATIONS / 1000 : 1;
      
      StringBuilder sb = new StringBuilder ();
      for (int x = 0; x < maxSize; x++)
        sb.append ("1 => ++.var1;");
      
      CompiledExpression ce = knowledge.compile (sb.toString ());
      
      timer.start ();
      for (int x = 0; x < actualIterations; ++x)
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
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
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
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
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
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
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
    public long test (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
    {
      knowledge.clear ();
      Timer timer = new Timer ();
      
      int var1 = 0;
      timer.start ();
      for (int x = 0; x < ITERATIONS; ++x)
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
    if (freq >= 1)
      return "" + (Math.round (freq * 100.0) / 100.0) + " ghz";
    
    freq = (double) hertz / MHZ;
    if (freq >= 1)
      return "" + (Math.round (freq * 100.0) / 100.0) + " mhz";
    
    freq = (double) hertz / KHZ;
    if (freq >= 1)
      return "" + (Math.round (freq * 100.0) / 100.0) + " khz";
    
    freq = (double) hertz;
    
    return "" + (Math.round (freq * 100.0) / 100.0) + " hz";
  }
  
  public static void main (String...args) throws KnowledgeBaseLockedException
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
    
    for (int x = 0; x < RUNCOUNT; ++x)
    {
      for (Test test : tests)
      {
        test.elapsed += test.test (knowledge);
      }
    }
    
    for (Test test : tests)
    {
      if (test.elapsed == 0)
        test.elapsed = 1;
      
      test.hertz = (1000000000 * EVALUATIONS) / test.elapsed;
    }
    
    System.out.println ("\n\nTotal time taken for each Test with " + ITERATIONS + " iterations * " + RUNCOUNT + " tests was:");
    System.out.println ("=========================================================================");
    
    for (Test test : tests)
    {
      System.out.println (String.format (" %s\t\t%30s ns", test.name (), formatNumber (test.elapsed)));
    }
    System.out.println ("=========================================================================");
    
    System.out.println ("\n\nAverage time taken per rule evaluation was:");
    System.out.println ("=========================================================================");
    
    for (Test test : tests)
    {
      System.out.println (String.format (" %s\t\t%30s ns", test.name (), formatNumber (test.elapsed / EVALUATIONS)));
    }
    System.out.println ("=========================================================================");
    
    System.out.println ("\n\nHertz for each Test with " + ITERATIONS + " iterations * " + RUNCOUNT + " tests was:");
    System.out.println ("=========================================================================");
    
    for (Test test : tests)
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
