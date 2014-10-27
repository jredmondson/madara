package com.madara.googlecode.testing;

/**
 * Conducts series of tests to guage performance of Java.
 *
 * SimpleOperations class uses the high resolution timer available via
 * System.nanoTime () to gauge the performance of reinforcements (i.e. ++j)
 * and guarded reinforcements (i.e. guard => ++j) in Java.
 * @author James Edmondson <james.r.edmondson@vanderbilt.edu>
 */

public class SimpleOperations
{
  /**
   * Test reinforcements
   * @param   iterations  the number of time to repeat the reinforcement
   * @return  number of nanoseconds the iterations took to complete
   */

    public static long test_reinforcements (long iterations)
    {
      // to have uniformity between the tests, we have a counter
      // and an accumulator. The j value is the reinforced value
      long j = 0;

      long start = System.nanoTime ();

      for (long i = 0; i < iterations; ++i)
        ++j;

      long total = System.nanoTime () - start;  
      System.out.println ("\nTotal time for " + iterations + " guarded reinforcements was: " 
                          + total + "ns");    
      System.out.println ("Resulting reinforcement was: " + j);    
 
      return total;         
    }

  /**
   * Test a guarded reinforcement
   * @param iterations
   * @return  number of nanoseconds the iterations took to complete
   */

    public static long test_guarded_reinforcements (boolean guard, long iterations)
    {
      // to have uniformity between the tests, we have a counter
      // and an accumulator. The j value is the reinforced value
      long j = 0;

      long start = System.nanoTime ();

      for (long i = 0; i < iterations; ++i)
        ++j;

      long total = System.nanoTime () - start;    
      System.out.println ("\nTotal time for " + iterations + " guarded reinforcements was: " 
                          + total + "ns");    
      System.out.println ("Resulting reinforcement was: " + j);    

      return total;   
    }

  /**
   * Test reinforcements and inferences with reinforcements to
   * gauge speed differences between C++ and Java
   *
   * @param args   command line arguments passed by the user
   */

    public static void main(String[] args)
    {
      // test 10 million iterations of each type of reinforcement
      long iterations = 10000000;

      // we're just going to run one test
      long r_time = test_reinforcements (iterations);
      long i_time = test_guarded_reinforcements (true, iterations);

      // these were nanosecond timers so, let's see what that means
      // iterations / r_time == iterations / ns
      // iterations / s == iterations / r_time * 1,000,000,000 ns / s

      double r_hz = 1000000000 * iterations / r_time;
      double i_hz = 1000000000 * iterations / i_time;

      System.out.println ("\nReinforcement max hz was: " + r_hz + "/s");      
      System.out.println ("Guarded reinforcement max hz was: " + i_hz + "/s\n");    
    }
}