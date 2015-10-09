
/**
 * @file integer_types.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a simple application that demonstrates
 * working with various simple data types that are supported as
 * first class entities within the knowledge engine.
 **/

#include "madara/knowledge_engine/KnowledgeBase.h"
#include <iostream>
#include <string>

int main (int argc, char * argv[])
{
  // Create a knowledge base
  Madara::KnowledgeEngine::KnowledgeBase knowledge;
  
  /**
   * The default type in MADARA is an integer. All variables
   * start with a value of 0, false in the knowledge engine.
   * The following statement prints out a variable called
   * money, which should be zero by default.
   **/
  knowledge.print ("Money: ${.money}\n");

  /**
   * Integer types support most of the operations you would expect,
   * including +, -, +=, -=, *=, /, %, and a host of other functions.
   * Preincrement and decrement are also supported. Let's use one of
   * these to increase the amount of money.
   **/
  knowledge.print ("  Incrementing money...\n");
  knowledge.evaluate ("++.money");
  knowledge.print ("Money: ${.money}\n");
  
  /**
   * At this point, money is now 1. That's not a lot of money. Wait,
   * we're getting ahead of ourself. We want the knowledge engine to
   * understand that 1 money is not satisfactory. As an agent, it should
   * try acquiring more money. Let's arbitrarily increase the amount of
   * money by 500.
   **/
  knowledge.print ("  Bank error in your favor...\n");
  knowledge.evaluate (".money += 500");
  knowledge.print ("Money: ${.money}\n");

  /**
   * If only we could all print money like this! An agent must live within
   * a society of other agents, and a society always has rules. In this agent
   * society, an agent wins the internet if it has more than 1,000,000 money.
   * Let's check this society rule against our money.
   **/
  knowledge.print ("  Assessing our ability to win the internet...");
  knowledge.evaluate (
    /**
     * Quite literally, if money is greater than 1,000,000, print
     * "You won the internet." Otherwise, print "You did not win the internet"
     **/
    "(.money > 1000000 => #print (' You won the internet.\n'))"
    "||"
    "#print (' You did not win the internet.\n')"
    );
  knowledge.print ("Money: ${.money}\n");
  
  /**
   * The agent becomes frustrated and asks Bill Gates for information on how
   * to become successful. Bill recommends collecting a 200 dollar fee from
   * each of the 310M citizens in the U.S. and an average of 25 dollars from
   * the other 6.7B people on the planet. The agent happily releases a new
   * piece of software, for which each person on the planet pays a fee.
   **/
  knowledge.print ("  Incorporating Bill Gates' ideas...\n");
  knowledge.print ("  Selling licenses to software...\n");
  knowledge.print ("    310M U.S. customers at 200 per customer...\n");
  knowledge.evaluate (".money += 200 * 310000000; ++.gates_advice_taken");
  knowledge.print ("    6.7B international customers at 25 per customer...\n");
  knowledge.evaluate (".money += 25 * 6700000000; ++.gates_advice_taken");

  /**
   * Run the same money check to see if we have won the internet
   **/
  knowledge.print ("  Assessing our ability to win the internet...");
  knowledge.evaluate (
    /**
     * Quite literally, if money is greater than 1,000,000, print
     * "You won the internet." Otherwise, print "You did not win the internet"
     **/
    "(.money > 1000000 => #print ('  You won the internet.\n'))"
    "||"
    "#print ('  You did not win the internet.\n')"
  );
  
  /**
   * For every action, there is a reaction. The agent forgot to mention to
   * mention to Bill Gates that he would be sharing his fortune-making advice
   * with everyone on the internet. Let's apply societal rules once more.
   *
   * If we've taken more than one piece of Bill's advice (and we've used two),
   * and we have more money than him, he'll take us to court. If we have less
   * than him but more than enough to win the internet, then he'll issue a
   * public rebuke of the money-making policy.
   **/
  knowledge.print ("  Assessing our friendship with Bill Gates...");
  knowledge.evaluate (
    // if we have more wealth than Gates and took his advice, expect litigation
    "(.money > 67000000000 && .gates_advice_taken => .litigation = 1)"

    // or
    "||"
    
    /**
     * if we have enough to win the internet and took his advice, expect
     * a public rebuke
     **/
    "(.money > 1000000 && .gates_advice_taken => .public_rebuke = 1)"
    
    // or
    "||"
    /**
     * if we have enough to win the internet and took his advice, expect
     * a public rebuke
     **/
    "(.money > 1000000 => .gates_is_a_friend = 1)"

    // perform both the preceding expression and a subsequent one
    ";"

    ".litigation => #print (' Lawsuit incoming.\n');"
    ".public_rebuke => #print (' Press release incoming.\n');"
    ".gates_is_a_friend => #print (' Party at Gates Mansion.\n')"
  );
  knowledge.print ("Money: ${.money}\n");

  /**
   * The agent is only interested in winning the internet. The rest of the
   * money he has made can go to charity, which should result in more
   * happines, less lawsuits, and more parties at Gates Mansion.
   **/
  knowledge.print ("  Donating excess money to charity...\n");
  knowledge.evaluate (
    /**
     * To appease Bill Gates, the agent donates all of his money but
     * 1,000,001 dollars, just enough to win the internet. He also feels
     * like he is no longer following Bill Gates' advice because the
     * advice specifically dealt with increasing personal wealth to
     * absurd levels.
     **/
    ".donations = .money - 1000001;"
    ".money -= .donations;"
    ".gates_advice_taken -= 2;"
    ".litigation = .public_rebuke = .gates_is_a_friend = 0"
    );
  knowledge.print ("  Assessing our ability to win the internet... ");
  knowledge.evaluate (
    /**
     * Quite literally, if money is greater than 1,000,000, print
     * "You won the internet." Otherwise, print "You did not win the internet"
     **/
    "(.money > 1000000 => #print ('  You won the internet.\n'))"
    "||"
    "#print ('  You did not win the internet.\n')"
    );
  
  knowledge.print ("  Assessing our friendship with Bill Gates...");
  knowledge.evaluate (
    // if we have more wealth than Gates and took his advice, expect litigation
    "(.money > 67000000000 && .gates_advice_taken => .litigation = 1) ||"

    /**
     * if we have enough to win the internet and took his advice, expect
     * a public rebuke
     **/
     "(.money > 1000000 && .gates_advice_taken => .public_rebuke = 1) ||"
    
    /**
     * if we have enough to win the internet and took his advice, expect
     * a public rebuke
     **/
     "(.money > 1000000 => .gates_is_a_friend = 1);"


    ".litigation => #print (' Lawsuit incoming.\n');"
    ".public_rebuke => #print (' Press release incoming.\n');"
    ".gates_is_a_friend => #print (' Party at Gates Mansion.\n')"
  );
  knowledge.print ("Money: ${.money}\n");
  
  knowledge.print ();

  /**
   * This tutorial has showcased a rather silly example, but it is enough
   * information to implement your own reinforcement learning. Reinforcement
   * learning is a continuous process involving multi-valued logic which
   * attempts to satisfy internals rules according to rewards and penalties.
   * To make your own reinforcement learning agent, think of formulating a
   * problem like this:
   *
   * 1) Do I have sensors? What are they returning? What is the utility of
   *    a sensor returning something good? What happens if the sensor is
   *    wrong? Does it decrease overall good (i.e., should I give myself
   *    a penalty to indicate that I'm not contributing as much as I should
   *    toward being a part of the agent community?)
   * 2) What is the goal of the agent community? Are they collaborative or
   *    competitive? This will help establish rewards and penalties.
   * 3) Generally, the more positive the variable, the higher the utility.
   *    However, as we can see in the above example, too much hoarding of
   *    utility could be a bad thing. Consider an agent community that
   *    governs roads. One road agent collects 90% of the tolls and keeps
   *    all of the money to himself for maintenance. This means the other
   *    roads all become neglected and potentially impassible, resulting
   *    in overall city throughput of vehicles to go down. The reinforcement
   *    learning should be tweaked to have either a maximum cash-on-hand
   *    before dumping it into the public coffers for road maintenance or
   *    use KaRL to announce each agent's cash-on-hand and when some
   *    threshold or percentage of overall wealth is met, dump excess
   *    into the road agent who needs the cash the most. In this way,
   *    you can create a harmonious agent system based on reinforcement
   *    learning. See the tutorials in distributed_knowledge for more
   *    information on how you can update each agent using variable mutations.
   **/

  return 0;
}
