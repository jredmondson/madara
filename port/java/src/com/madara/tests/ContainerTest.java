/*********************************************************************
 * Usage of this software requires acceptance of the MADARA CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/madara/wiki/CMULicense
 *********************************************************************/

package com.madara.tests;

import com.madara.KnowledgeBase;
import com.madara.KnowledgeRecord;
import com.madara.containers.Vector;
import com.madara.containers.Integer;
import com.madara.containers.Queue;

/**
 * This class is a tester for the com.madara.containers package
 */
public class ContainerTest
{
  public static void testVector()
  {
    KnowledgeBase knowledge = new KnowledgeBase();
    Vector list = new Vector();
    
    boolean error = false;
    
    System.out.println("Testing Vector");
    
    list.setName(knowledge, "device.sensors");
    
    list.resize(4);
    
    System.out.println("  Setting elements");
    
    list.set(0, "first element");
    list.set(1, "second element");
    list.set(2, "third element");
    list.set(3, "fourth element");
    
    System.out.println("0: " + list.get(0).toString());
    System.out.println("1: " + list.get(1).toString());
    System.out.println("2: " + list.get(2).toString());
    System.out.println("3: " + list.get(3).toString());
    
    System.out.println("  Shipping elements to an array");
    
    KnowledgeRecord[] my_array = list.toArray();
    
    System.out.println("  Printing " + my_array.length + " array elements");
    
    for(int i = 0; i < my_array.length; ++i)
    {
      String result = "    ";
      result += i;
      result += ": ";
      result += my_array[i].toString();
      
      System.out.println(result);
    }
    
    if(list.get(0).toString().equals("first element") &&
       list.get(1).toString().equals("second element") &&
       list.get(2).toString().equals("third element") &&
       list.get(3).toString().equals("fourth element"))
    {
      System.out.println("  SUCCESS: Vector set method test");
    }
    else
    {
      System.out.println("  FAIL: Vector set method test");
      error = true;
    }
    
    if(my_array.length == 4 &&
       my_array[0].toString().equals("first element") &&
       my_array[1].toString().equals("second element") &&
       my_array[2].toString().equals("third element") &&
       my_array[3].toString().equals("fourth element"))
    {
      System.out.println("  SUCCESS: Vector toArray test");
    }
    else
    {
      System.out.println("  FAIL: Vector toArray test");
      error = true;
    }
    
    if(error)
      knowledge.print();
  }
  
  public static void testInteger()
  {
    KnowledgeBase knowledge = new KnowledgeBase();
    Integer counter = new Integer();
    
    boolean error = false;
    
    System.out.println("Testing Integer");
    
    counter.setName(knowledge, "counter");
    
    // 0 += 1 == 1
    counter.inc();
    
    // 1 += 10 == 11
    counter.inc(10);
    
    // 11 -= 5 == 6
    counter.dec(5);
    
    // --6 == 5 
    counter.dec();
    
    if(counter.toLong() == 5)
    {
      System.out.println("  SUCCESS: Integer inc/dec test");
    }
    else
    {
      System.out.println("  FAIL: Integer inc/dec test");
      error = true;
    }
    
    if(error)
      knowledge.print();
  }
  
  public static void testQueue()
  {
    KnowledgeBase knowledge = new KnowledgeBase();
    Queue queue = new Queue();
    
    boolean error = false;
    
    System.out.println("Testing Queue");
    
    queue.setName(knowledge, "queue");
    
    queue.resize(10);
    
    queue.enqueue(10);
    queue.enqueue(12.5);
    queue.enqueue("third");
    
    if(queue.count() == 3 && queue.size() == 10)
    {
      System.out.println("  SUCCESS: Queue count and size test");
    }
    else
    {
      System.out.println("  FAIL: Queue count and size test");
      error = true;
    }
    
    KnowledgeRecord value1 = queue.dequeue(true);
    KnowledgeRecord value2 = queue.dequeue(true);
    KnowledgeRecord value3 = queue.dequeue(true);
    
    if(value1.toString().equals("10") &&
       value2.toString().equals("12.5") && value3.toString().equals("third") &&
       queue.count() == 0)
    {
      System.out.println("  SUCCESS: Queue dequeue test");
    }
    else
    {
      System.out.println("  FAIL: Queue dequeue test");
      error = true;
    }
    
    if(error)
      knowledge.print();
  }
  
  public static void main(String...args) throws Exception
  {
    testInteger();
    testVector();
    testQueue();
  }
}
