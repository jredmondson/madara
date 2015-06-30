
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <math.h>
#include <iomanip>
#include <algorithm>

#include "madara/utility/Utility.h"

#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_Thread.h"
#include "ace/Sched_Params.h"

namespace utility = Madara::Utility;

void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);
    
    if (argc > 1)
    {
      std::cout <<
        "\nProgram Summary for " << argv[0] << ":\n\n" \
        "  This stand-alone application tests functions from the utility\n"
        "  functions included in the MADARA distribution.\n"
        "\n";
      exit (0);
    }
  }
}

void test_endian_swap (void)
{
  int64_t ll_value1 (1), ll_temp (0), neg_result (0);
  uint64_t ull_value1 (1), ull_temp (0);
  int64_t ll_value_neg (-1);
  
  int32_t l_value1 (1), l_temp (0), l_neg_result (0);
  uint32_t ul_value1 (1), ul_temp (0);
  int32_t l_value_neg (-1);
  std::cout << "\n********* 64 bit endian swap *************\n\n";
  std::cout << "64 bit endian_swap: Before swap:\n";
  std::cout << "     ll_value1 = " << ll_value1 << "\n";
  std::cout << "    ull_value1 = " << ull_value1 << "\n";
  std::cout << "  ll_value_neg = " << ll_value_neg << "\n";
  std::cout << "       ll_temp = " << ll_temp << "\n";
  std::cout << "      ull_temp = " << ull_temp << "\n";
  std::cout << "    neg_result = " << neg_result << "\n";

  ll_temp = Madara::Utility::endian_swap (ll_value1);
  ull_temp = Madara::Utility::endian_swap (ull_value1);
  neg_result = Madara::Utility::endian_swap (ll_value_neg);
  
  std::cout << "64 bit endian_swap: After swap to temps:\n";
  std::cout << "     ll_value1 = " << ll_value1 << "\n";
  std::cout << "    ull_value1 = " << ull_value1 << "\n";
  std::cout << "  ll_value_neg = " << ll_value_neg << "\n";
  std::cout << "       ll_temp = " << ll_temp << "\n";
  std::cout << "      ull_temp = " << ull_temp << "\n";
  std::cout << "    neg_result = " << neg_result << "\n";
  
  ll_temp = Madara::Utility::endian_swap (ll_temp);
  ull_temp = Madara::Utility::endian_swap (ull_temp);
  neg_result = Madara::Utility::endian_swap (neg_result);
  
  std::cout << "64 bit endian_swap: After temps swapped themselves:\n";
  std::cout << "     ll_value1 = " << ll_value1 << "\n";
  std::cout << "    ull_value1 = " << ull_value1 << "\n";
  std::cout << "  ll_value_neg = " << ll_value_neg << "\n";
  std::cout << "       ll_temp = " << ll_temp << "\n";
  std::cout << "      ull_temp = " << ull_temp << "\n";
  std::cout << "    neg_result = " << neg_result << "\n";
  
  std::cout << "\n********* 32 bit endian swap *************\n\n";
  
  std::cout << "32 bit endian_swap: Before swap:\n";
  std::cout << "     l_value1 = " << l_value1 << "\n";
  std::cout << "    ul_value1 = " << ul_value1 << "\n";
  std::cout << "  l_value_neg = " << l_value_neg << "\n";
  std::cout << "       l_temp = " << l_temp << "\n";
  std::cout << "      ul_temp = " << ul_temp << "\n";
  std::cout << "  l_neg_result = " << l_neg_result << "\n";

  l_temp = Madara::Utility::endian_swap (l_value1);
  ul_temp = Madara::Utility::endian_swap (ul_value1);
  l_neg_result = Madara::Utility::endian_swap (l_value_neg);
  
  std::cout << "32 bit endian_swap: After swap to temps:\n";
  std::cout << "     l_value1 = " << l_value1 << "\n";
  std::cout << "    ul_value1 = " << ul_value1 << "\n";
  std::cout << "  l_value_neg = " << l_value_neg << "\n";
  std::cout << "       l_temp = " << l_temp << "\n";
  std::cout << "      ul_temp = " << ul_temp << "\n";
  std::cout << "  l_neg_result = " << l_neg_result << "\n";
  
  l_temp = Madara::Utility::endian_swap (l_temp);
  ul_temp = Madara::Utility::endian_swap (ul_temp);
  l_neg_result = Madara::Utility::endian_swap (l_neg_result);
  
  std::cout << "32 bit endian_swap: After temps swapped themselves:\n";
  std::cout << "     l_value1 = " << l_value1 << "\n";
  std::cout << "    ul_value1 = " << ul_value1 << "\n";
  std::cout << "  l_value_neg = " << l_value_neg << "\n";
  std::cout << "       l_temp = " << l_temp << "\n";
  std::cout << "      ul_temp = " << ul_temp << "\n";
  std::cout << "  l_neg_result = " << l_neg_result << "\n";
}

void
test_version (void)
{
  std::string human_readable_version = Madara::Utility::get_version ();
  uint32_t uint_version = Madara::Utility::get_uint_version ();
  std::string converted_version =
    Madara::Utility::to_string_version (uint_version);
  
  std::cout << "\n********* Testing version functions *************\n\n";
  std::cout << "Version in VERSION.txt is " << human_readable_version << ".\n";
  std::cout << "Version from get_uint_version is " << uint_version << ".\n";
  std::cout << "Converted uint_version is " << converted_version << ".\n";

  std::stringstream buffer;
  std::string version;
  buffer << human_readable_version;
  buffer >> version;

  if (version == converted_version)
    std::cout << "Current version conversion is a SUCCESS.\n";
  else
    std::cout << "Current version conversion is a FAIL.\n";
}

void test_heaps (void)
{
  /**            START
   *               8
   *        10           1
   *    3       6     2      5
   **/
  int input [7] = {8, 10, 1, 3, 6, 2, 5};

  std::cerr << "Testing heapify... ";

  Madara::Utility::heapify (input, 7);

  /**             END
   *               10
   *        8            5
   *   3       6      2      1
   **/

  if (input[0] == 10 && input[1] == 8 && input[2] == 5 &&
      input[3] == 3 && input[4] == 6 && input[5] == 2 &&
      input[6] == 1)
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
  
  Madara::Utility::heap_sort (input, 7);

  std::cerr << "Testing heap_sort... ";
  
  /**             END
   *               1
   *        2            3
   *   5       6      8      10
   **/

  if (input[0] == 1 && input[1] == 2 && input[2] == 3 &&
      input[3] == 5 && input[4] == 6 && input[5] == 8 &&
      input[6] == 10)
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
  
}

void test_ints (void)
{
  std::cerr << "Testing nearest int to 2.25... ";

  if (Madara::Utility::nearest_int (2.25) == 2)
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
  
  std::cerr << "Testing nearest int to 2.8... ";

  if (Madara::Utility::nearest_int (2.8) == 3)
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
  
  std::cerr << "Testing nearest int to 16.1... ";

  if (Madara::Utility::nearest_int (16.1) == 16)
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
  
  std::cerr << "Testing nearest int to 9.9575... ";

  if (Madara::Utility::nearest_int (9.9575) == 10)
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
  
  std::cerr << "Testing rand_int... ";

  int num_zeroes = 0;
  int num_ones = 0;
  for (int i = 0; i < 100000; ++i)
  {
    if (Madara::Utility::rand_int (0, 1, true) == 0)
      ++num_zeroes;
    else
      ++num_ones;
  }

  std::cerr << "freq 0=" << num_zeroes << ", freq 1=" << num_ones << "... ";

  if (num_zeroes > 30000 && num_ones > 30000)
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
  
  std::cerr << "Testing rand_int... ";

  num_zeroes = 0;
  num_ones = 0;
  for (int i = 0; i < 100000; ++i)
  {
    if (Madara::Utility::rand_int (0, 1, false) == 0)
      ++num_zeroes;
    else
      ++num_ones;
  }

  std::cerr << "freq 0=" << num_zeroes << ", freq 1=" << num_ones << "... ";

  if (num_zeroes > 30000 && num_ones > 30000)
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
}

void test_time (void)
{
  std::cerr << "Testing get_time... ";
  int64_t start = Madara::Utility::get_time ();
  
  for (int i = 0; i < 30000; ++i)
  {
    Madara::Utility::rand_int (0, 1);
  }

  int64_t end = Madara::Utility::get_time ();

  std::cerr << end - start;

  if (end - start > 0)
    std::cerr << " SUCCESS\n";
  else
    std::cerr << " FAIL\n";
}

void test_sqrt (void)
{
  // keep track of time
  ACE_hrtime_t measured;
  ACE_High_Res_Timer timer;
  int64_t elapsed_ns;
  
  std::cout << "\n********* Testing sqrt timing *************\n\n";

  timer.start ();

  double input = 1000000000;
  for (int i = 0; i < 1000000000; ++i)
  {
    sqrt (input);
  }

  timer.stop ();

  timer.elapsed_time (measured);

  elapsed_ns = measured / 1000000000;

  std::cerr << "sqrt: 1,000,000,000 iterations in " << measured <<
    " ns. avg=" << elapsed_ns << "ns.\n";
}

void test_sleep (void)
{
  std::cerr << "\n********* Testing sleep *************\n\n";

  std::cerr << std::setprecision (2);
  std::cerr << std::fixed;

  double sleep_time = 1.5, end_time;
  std::cerr << "  Testing sleep time of 1.5s... ";
  end_time = Madara::Utility::sleep (sleep_time);

  std::cerr << "result=" << end_time;

  if (end_time >= sleep_time)
  {
    std::cerr << "... SUCCESS\n";
  }
  else
  {
    std::cerr << "... FAIL\n";
  }
  
  std::cerr << "  Testing sleep time of 2.5s... ";
  end_time = Madara::Utility::sleep (2.5);

  std::cerr << "result=" << end_time;

  if (end_time >= 2.5)
  {
    std::cerr << "... SUCCESS\n";
  }
  else
  {
    std::cerr << "... FAIL\n";
  }
  
  std::cerr << "  Testing sleep time of 2.25s... ";
  end_time = Madara::Utility::sleep (2.25);

  std::cerr << "result=" << end_time;

  if (end_time >= 2.25)
  {
    std::cerr << "... SUCCESS\n";
  }
  else
  {
    std::cerr << "... FAIL\n";
  }
}

void test_replace (void)
{
  std::cerr << "Testing replace...\n";

  std::string base ("My milkshake brings all the boys to the yard.");
  std::string source (base);
  
  std::cerr << "  Single char replace... ";
  size_t result = utility::string_replace (source, "i", "u", true);

  if (result == 2 && source == "My mulkshake brungs all the boys to the yard.")
  {
    std::cerr << "SUCCESS";
  }
  else
  {
    std::cerr << "FAIL";
  }

  std::cerr << "\n";
  std::cerr << "  Word replace... ";

  source = base;

  result = utility::string_replace (source,
    "milkshake brings", "crumpets bring");
  
  if (result == 1 && source == "My crumpets bring all the boys to the yard.")
  {
    std::cerr << "SUCCESS";
  }
  else
  {
    std::cerr << "FAIL";
  }
  std::cerr << "\n";

  std::cerr << "  Multi-word replace... ";

  base = "bob is a builder. bob is a boss. bob is the foreman. bob is a hoss.";

  source = base;
  
  result = utility::string_replace (source,
    "bob", "jake");
  
  if (result == 4 &&
    source == "jake is a builder. jake is a boss. jake is the foreman. jake is a hoss.")
  {
    std::cerr << "SUCCESS";
  }
  else
  {
    std::cerr << "FAIL";
  }

  std::cerr << "\n";
  std::cerr << "  Single-word replace... ";

  
  source = base;
  
  result = utility::string_replace (source,
    "bob", "jake", false);
  
  if (result == 1 &&
    source == "jake is a builder. bob is a boss. bob is the foreman. bob is a hoss.")
  {
    std::cerr << "SUCCESS";
  }
  else
  {
    std::cerr << "FAIL";
  }

  std::cerr << "\n";

}

int main (int argc, char ** argv)
{
  handle_arguments (argc, argv);
  
  test_replace ();
  test_sqrt ();
  test_version ();
  test_endian_swap ();
  test_heaps ();
  test_time ();
  test_ints ();
  test_sleep ();

  return 0;
}
