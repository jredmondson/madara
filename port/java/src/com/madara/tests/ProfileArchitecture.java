/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/

package com.madara.tests;

import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.List;

import com.madara.EvalSettings;
import com.madara.KnowledgeBase;
import com.madara.KnowledgeBase.CompiledExpression;
import com.madara.KnowledgeBase.KnowledgeBaseLockedException;
import com.madara.KnowledgeRecord;


public class ProfileArchitecture
{
  private static final String profile_file = "profile_expressions.txt";
  
  private static final List<Test> tests = new ArrayList<Test> ();
  
  private static void warmup (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
  {
    System.out.println ("Warming up compilation and evaluation caches...");
    
    knowledge.compile ("var1 = 10");
    knowledge.compile ("var2 = 15");
    knowledge.compile ("++var3");
    knowledge.compile ("++var4");
    
    for (int x = 0; x < 50000; ++x)
      knowledge.evaluate ("++var2").free ();
    
    for (int x = 0; x < 50000; ++x)
      knowledge.evaluate ("var2 += 1").free ();
  }
  
  private static void compileExpressions (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
  {
    System.out.println ("Compiling all expressions...");
    Timer timer = new Timer ();
    
    for (Test test : tests)
    {
      timer.start ();
      test.expression = knowledge.compile (test.test);
      timer.stop ();
      
      test.compileTime = timer.elapsed ();
    }
  }
  
  private static void evaluateExpressions (KnowledgeBase knowledge) throws KnowledgeBaseLockedException
  {
    System.out.println ("Evaluating all expressions 10,000 times...");
    
    EvalSettings defaultSettings = new EvalSettings (); //Make sure default eval settings are created
    
    int i = 0;
    for (Test test : tests)
    {
      test.maxTime = Long.MIN_VALUE;
      test.minTime = Long.MAX_VALUE;
      
      System.out.println ("  [" + i++ + "] Evaluating " + test.test);
      
      for (int j = 0; j < 100; ++j)
      {
        Timer timer = new Timer ();
        timer.start ();
        KnowledgeRecord kr = knowledge.evaluate (test.expression, defaultSettings);
        timer.stop ();
        kr.free ();
        
        test.maxTime = Math.max (timer.elapsed (), test.maxTime);
        test.minTime = Math.min (timer.elapsed (), test.minTime);
      }
      
      Timer timer = new Timer ();
      timer.start ();
      for (int j = 0; j < 10000; ++j)
      {
        knowledge.evaluate (test.expression, defaultSettings).free ();
      }
      timer.stop ();
      test.averageTime = timer.elapsed () / 10000;
    }
  }
  
  private static void printResults ()
  {
    System.out.println ("\n\n");
    System.out.println (String.format ("%18s | %13s | %13s | %13s | %13s", "Expression", "Compile time", "Min eval time", "Max eval time", "Avg eval time"));
    
    for (Test test : tests)
    {
      System.out.println (String.format ("%18s   %13d   %13d   %13d   %13d", test.test.subSequence (0, Math.min (test.test.length (),18)), test.compileTime, test.minTime, test.maxTime, test.averageTime));
    }
  }
  
  public static void main (String...args) throws Exception
  {
    KnowledgeBase knowledge = new KnowledgeBase ();
    
    BufferedReader br = new BufferedReader (new FileReader (profile_file));
    String line = null;
    while ( (line = br.readLine ()) != null)
      tests.add (new Test (line));
    br.close ();
    
    if (tests.size () > 0)
    {
      warmup (knowledge);
      compileExpressions (knowledge);
      evaluateExpressions (knowledge);
      printResults ();
    }
  }
  
  private static class Test
  {  
    public String test;
    public long maxTime;
    public long minTime;
    public long compileTime;
    public long averageTime;
    public CompiledExpression expression;
  
    public Test (String test)
    {
      this.test = test;
    }
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