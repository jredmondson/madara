
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <iomanip>
#include <algorithm>
#include <chrono>

#include "madara/utility/Utility.h"

#include "madara/utility/Timer.h"
#include "madara/knowledge/FileFragmenter.h"

namespace knowledge = madara::knowledge;
namespace utility = madara::utility;
namespace sc = std::chrono;

int madara_fails = 0;

void handle_arguments(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);

    if (argc > 1)
    {
      std::cout
          << "\nProgram Summary for " << argv[0]
          << ":\n\n"
             "  This stand-alone application tests functions from the utility\n"
             "  functions included in the MADARA distribution.\n"
             "\n";
      exit(0);
    }
  }
}

void test_endian_swap(void)
{
  int64_t ll_value1(1), ll_temp(0), neg_result(0);
  uint64_t ull_value1(1), ull_temp(0);
  int64_t ll_value_neg(-1);

  int32_t l_value1(1), l_temp(0), l_neg_result(0);
  uint32_t ul_value1(1), ul_temp(0);
  int32_t l_value_neg(-1);
  std::cout << "\n********* 64 bit endian swap *************\n\n";
  std::cout << "64 bit endian_swap: Before swap:\n";
  std::cout << "     ll_value1 = " << ll_value1 << "\n";
  std::cout << "    ull_value1 = " << ull_value1 << "\n";
  std::cout << "  ll_value_neg = " << ll_value_neg << "\n";
  std::cout << "       ll_temp = " << ll_temp << "\n";
  std::cout << "      ull_temp = " << ull_temp << "\n";
  std::cout << "    neg_result = " << neg_result << "\n";

  ll_temp = madara::utility::endian_swap(ll_value1);
  ull_temp = madara::utility::endian_swap(ull_value1);
  neg_result = madara::utility::endian_swap(ll_value_neg);

  std::cout << "64 bit endian_swap: After swap to temps:\n";
  std::cout << "     ll_value1 = " << ll_value1 << "\n";
  std::cout << "    ull_value1 = " << ull_value1 << "\n";
  std::cout << "  ll_value_neg = " << ll_value_neg << "\n";
  std::cout << "       ll_temp = " << ll_temp << "\n";
  std::cout << "      ull_temp = " << ull_temp << "\n";
  std::cout << "    neg_result = " << neg_result << "\n";

  ll_temp = madara::utility::endian_swap(ll_temp);
  ull_temp = madara::utility::endian_swap(ull_temp);
  neg_result = madara::utility::endian_swap(neg_result);

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

  l_temp = madara::utility::endian_swap(l_value1);
  ul_temp = madara::utility::endian_swap(ul_value1);
  l_neg_result = madara::utility::endian_swap(l_value_neg);

  std::cout << "32 bit endian_swap: After swap to temps:\n";
  std::cout << "     l_value1 = " << l_value1 << "\n";
  std::cout << "    ul_value1 = " << ul_value1 << "\n";
  std::cout << "  l_value_neg = " << l_value_neg << "\n";
  std::cout << "       l_temp = " << l_temp << "\n";
  std::cout << "      ul_temp = " << ul_temp << "\n";
  std::cout << "  l_neg_result = " << l_neg_result << "\n";

  l_temp = madara::utility::endian_swap(l_temp);
  ul_temp = madara::utility::endian_swap(ul_temp);
  l_neg_result = madara::utility::endian_swap(l_neg_result);

  std::cout << "32 bit endian_swap: After temps swapped themselves:\n";
  std::cout << "     l_value1 = " << l_value1 << "\n";
  std::cout << "    ul_value1 = " << ul_value1 << "\n";
  std::cout << "  l_value_neg = " << l_value_neg << "\n";
  std::cout << "       l_temp = " << l_temp << "\n";
  std::cout << "      ul_temp = " << ul_temp << "\n";
  std::cout << "  l_neg_result = " << l_neg_result << "\n";
}

void test_version(void)
{
  std::string human_readable_version = madara::utility::get_version();
  uint32_t uint_version = madara::utility::get_uint_version();
  std::string converted_version =
      madara::utility::to_string_version(uint_version);

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
  {
    std::cout << "Current version conversion is a FAIL.\n";
    ++madara_fails;
  }
}

void test_heaps(void)
{
  /**            START
   *               8
   *        10           1
   *    3       6     2      5
   **/
  int input[7] = {8, 10, 1, 3, 6, 2, 5};

  std::cerr << "Testing heapify... ";

  madara::utility::heapify(input, 7);

  /**             END
   *               10
   *        8            5
   *   3       6      2      1
   **/

  if (input[0] == 10 && input[1] == 8 && input[2] == 5 && input[3] == 3 &&
      input[4] == 6 && input[5] == 2 && input[6] == 1)
    std::cerr << "SUCCESS\n";
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  madara::utility::heap_sort(input, 7);

  std::cerr << "Testing heap_sort... ";

  /**             END
   *               1
   *        2            3
   *   5       6      8      10
   **/

  if (input[0] == 1 && input[1] == 2 && input[2] == 3 && input[3] == 5 &&
      input[4] == 6 && input[5] == 8 && input[6] == 10)
    std::cerr << "SUCCESS\n";
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
}

void test_ints(void)
{
  std::cerr << "Testing nearest int to 2.25... ";

  if (madara::utility::nearest_int(2.25) == 2)
    std::cerr << "SUCCESS\n";
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  std::cerr << "Testing nearest int to 2.8... ";

  if (madara::utility::nearest_int(2.8) == 3)
    std::cerr << "SUCCESS\n";
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  std::cerr << "Testing nearest int to 16.1... ";

  if (madara::utility::nearest_int(16.1) == 16)
    std::cerr << "SUCCESS\n";
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  std::cerr << "Testing nearest int to 9.9575... ";

  if (madara::utility::nearest_int(9.9575) == 10)
    std::cerr << "SUCCESS\n";
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  std::cerr << "Testing rand_int... ";

  int num_zeroes = 0;
  int num_ones = 0;
  for (int i = 0; i < 100000; ++i)
  {
    if (madara::utility::rand_int(0, 1, true) == 0)
      ++num_zeroes;
    else
      ++num_ones;
  }

  std::cerr << "freq 0=" << num_zeroes << ", freq 1=" << num_ones << "... ";

  if (num_zeroes > 30000 && num_ones > 30000)
    std::cerr << "SUCCESS\n";
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  std::cerr << "Testing rand_int... ";

  num_zeroes = 0;
  num_ones = 0;
  for (int i = 0; i < 100000; ++i)
  {
    if (madara::utility::rand_int(0, 1, false) == 0)
      ++num_zeroes;
    else
      ++num_ones;
  }

  std::cerr << "freq 0=" << num_zeroes << ", freq 1=" << num_ones << "... ";

  if (num_zeroes > 30000 && num_ones > 30000)
    std::cerr << "SUCCESS\n";
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
}

void test_time(void)
{
  std::cerr << "Testing get_time... ";
  int64_t start = madara::utility::get_time();

  for (int i = 0; i < 30000; ++i)
  {
    madara::utility::rand_int(0, 1);
  }

  int64_t end = madara::utility::get_time();

  std::cerr << end - start;

  if (end - start > 0)
    std::cerr << " SUCCESS\n";
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
}

void test_sqrt(void)
{
  // keep track of time
  madara::utility::Timer<std::chrono::steady_clock> timer;

  std::cout << "\n********* Testing sqrt timing *************\n\n";

  timer.start();

  double input = 1000000000;
  for (int i = 0; i < 1000000000; ++i)
  {
    input = sqrt(input);
  }

  timer.stop();
  int measured = (int)timer.duration_ns();
  int average = measured / 1000000000;

  std::cerr << "sqrt: 1,000,000,000 iterations in " << measured
            << " ns. avg=" << average << "ns.\n";
}

void test_sleep(void)
{
  std::cerr << "\n********* Testing sleep *************\n\n";

  std::cerr << std::setprecision(2);
  std::cerr << std::fixed;

  double sleep_time = 1.5, end_time;
  std::cerr << "  Testing sleep time of 1.5s... ";
  end_time = madara::utility::sleep(sleep_time);

  std::cerr << "result=" << end_time;

  if (end_time >= sleep_time)
  {
    std::cerr << "... SUCCESS\n";
  }
  else
  {
    std::cerr << "... FAIL\n";
    ++madara_fails;
  }

  std::cerr << "  Testing sleep time of 2.5s... ";
  end_time = madara::utility::sleep(2.5);

  std::cerr << "result=" << end_time;

  if (end_time >= 2.5)
  {
    std::cerr << "... SUCCESS\n";
  }
  else
  {
    std::cerr << "... FAIL\n";
    ++madara_fails;
  }

  std::cerr << "  Testing sleep time of 2.25s... ";
  end_time = madara::utility::sleep(2.25);

  std::cerr << "result=" << end_time;

  if (end_time >= 2.25)
  {
    std::cerr << "... SUCCESS\n";
  }
  else
  {
    std::cerr << "... FAIL\n";
    ++madara_fails;
  }

#ifdef MADARA_FEATURE_SIMTIME
  std::cerr << "\n********* Testing sim time sleep *************\n\n";

  std::cerr << std::setprecision(2);
  std::cerr << std::fixed;

  auto sim_time_test = [](double sleep_time, double rate) {
    madara::utility::sim_time_notify(-1, rate);
    std::cerr << "  Testing sleep time of " << sleep_time << "s at rate "
              << rate << "... ";
    auto start_actual = sc::system_clock::now();
    double dur = madara::utility::sleep(sleep_time);
    auto end_actual = sc::system_clock::now();
    auto dur_actual =
        sc::duration_cast<sc::milliseconds>(end_actual - start_actual);

    std::cerr << "result=" << dur << " [" << (dur_actual.count() / 1000.0)
              << "]";

    if (dur >= sleep_time)
    {
      std::cerr << "... SUCCESS\n";
    }
    else
    {
      std::cerr << "... FAIL\n";
      ++madara_fails;
    }
  };

  sim_time_test(1.5, 0.25);
  sim_time_test(2.5, 0.5);
  sim_time_test(2.25, 4);

  madara::utility::sim_time_notify(-1, 1);
#endif
}

void test_replace(void)
{
  std::cerr << "Testing replace...\n";

  std::string base("My milkshake brings all the boys to the yard.");
  std::string source(base);

  std::cerr << "  Single char replace... ";
  size_t result = utility::string_replace(source, "i", "u", true);

  if (result == 2 && source == "My mulkshake brungs all the boys to the yard.")
  {
    std::cerr << "SUCCESS";
  }
  else
  {
    std::cerr << "FAIL";
    ++madara_fails;
  }

  std::cerr << "\n";
  std::cerr << "  Word replace... ";

  source = base;

  result =
      utility::string_replace(source, "milkshake brings", "crumpets bring");

  if (result == 1 && source == "My crumpets bring all the boys to the yard.")
  {
    std::cerr << "SUCCESS";
  }
  else
  {
    std::cerr << "FAIL";
    ++madara_fails;
  }
  std::cerr << "\n";

  std::cerr << "  Multi-word replace... ";

  base = "bob is a builder. bob is a boss. bob is the foreman. bob is a hoss.";

  source = base;

  result = utility::string_replace(source, "bob", "jake");

  if (result == 4 && source == "jake is a builder. jake is a boss. jake is the "
                               "foreman. jake is a hoss.")
  {
    std::cerr << "SUCCESS";
  }
  else
  {
    std::cerr << "FAIL";
    ++madara_fails;
  }

  std::cerr << "\n";
  std::cerr << "  Single-word replace... ";

  source = base;

  result = utility::string_replace(source, "bob", "jake", false);

  if (result == 1 && source == "jake is a builder. bob is a boss. bob is the "
                               "foreman. bob is a hoss.")
  {
    std::cerr << "SUCCESS";
  }
  else
  {
    std::cerr << "FAIL";
    ++madara_fails;
  }

  std::cerr << "\n";
}

void test_fragment(const std::string& message,
    knowledge::KnowledgeRecord& record, const char* text_buffer, size_t size)
{
  std::cerr << "  " << message << "... ";

  std::shared_ptr<const std::vector<unsigned char>> binary =
      record.share_binary();

  if (binary->size() != size)
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
  else
  {
    if (memcmp(binary->data(), text_buffer, size) == 0)
    {
      std::cerr << "SUCCESS\n";
    }
    else
    {
      std::cerr << "FAIL\n";
      ++madara_fails;
    }
  }
}

void test_file_crc(void)
{
  std::cerr << "Testing file_crc and file_size...\n";

  std::string filename =
      "$(MADARA_ROOT)/tests/images/manaus_hotel_900x1500.jpg";
  filename = utility::expand_envs(filename);

  std::vector<uint32_t> crcs;

  // check the file in 1KB blocks
  crcs.push_back(utility::file_crc(filename, 512));
  crcs.push_back(utility::file_crc(filename, 1024));
  crcs.push_back(utility::file_crc(filename, 16384));
  crcs.push_back(utility::file_crc(filename, 1000000));
  crcs.push_back(utility::file_crc(filename));

  // read the file in whole and do the crc hash for the full file buffer
  boost::crc_32_type crc_32_hash;
  std::ifstream input(filename, std::ios::in | std::ios::binary);
  std::size_t file_size = utility::file_size(input);

  std::vector<char> buffer(file_size);
  input.read(buffer.data(), file_size);

  crc_32_hash.process_bytes(buffer.data(), file_size);
  crcs.push_back(crc_32_hash.checksum());

  if (crcs[0] != 0 && std::adjacent_find(crcs.begin(), crcs.end(),
                          std::not_equal_to<uint32_t>()) == crcs.end())
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL. crc32 results were:\n";
    for (auto crc : crcs)
    {
      std::cerr << "  " << crc << "\n";
    }
    ++madara_fails;
  }
}

void test_file_fragmenter(void)
{
  std::cerr << "Testing FileFragmenter...\n";

  std::cerr << "  Creating 256KB buffer...\n";

  size_t size = 256000;
  size_t segment_size = 60000;
  char* text_buffer = new char[size];
  std::fill(text_buffer, text_buffer + segment_size, 'a');
  std::fill(text_buffer + segment_size, text_buffer + segment_size * 2, 'b');
  std::fill(
      text_buffer + segment_size * 2, text_buffer + segment_size * 3, 'c');
  std::fill(
      text_buffer + segment_size * 3, text_buffer + segment_size * 4, 'd');
  std::fill(text_buffer + segment_size * 4, text_buffer + size, 'e');

  text_buffer[segment_size - 1] = 0;
  text_buffer[segment_size * 2 - 1] = 0;
  text_buffer[segment_size * 3 - 1] = 0;
  text_buffer[segment_size * 4 - 1] = 0;
  text_buffer[size - 1] = 0;

  std::cerr << "  Splitting 256KB buffer into 5 chunks... ";

  knowledge::FileFragmenter fragmenter(text_buffer, size, segment_size);
  if (fragmenter.records.size() == 5)
  {
    std::cerr << "SUCCESS\n";

    test_fragment("Checking fragment 1 contents", fragmenter.records[0],
        text_buffer, segment_size);
    test_fragment("Checking fragment 2 contents", fragmenter.records[1],
        text_buffer + segment_size, segment_size);
    test_fragment("Checking fragment 3 contents", fragmenter.records[2],
        text_buffer + segment_size * 2, segment_size);
    test_fragment("Checking fragment 4 contents", fragmenter.records[3],
        text_buffer + segment_size * 3, segment_size);
    test_fragment("Checking fragment 5 contents", fragmenter.records[4],
        text_buffer + segment_size * 4, size - segment_size * 4);

    std::cerr << "  Creating knowledge base file fragments... ";

    knowledge::KnowledgeBase kb;
    knowledge::containers::Vector fragments =
        fragmenter.create_vector("record.split", kb);
    knowledge::VariableReferences modifieds;

    if (fragments[0].size() == segment_size &&
        fragments[1].size() == segment_size &&
        fragments[2].size() == segment_size &&
        fragments[3].size() == segment_size &&
        fragments[4].size() == size % segment_size)
    {
      std::cerr << "SUCCESS\n";

      std::cerr << "  Checking modifieds...";
      modifieds = kb.save_modifieds();

      if (modifieds.size() == 0)
      {
        std::cerr << "SUCCESS\n";
      }
      else
      {
        std::cerr << "FAIL. modifieds.size should be 0 but is "
                  << modifieds.size() << "\n";
        ++madara_fails;
        kb.print();
      }

      std::cerr << "  Rebuilding file from fragments... ";

      size_t rebuilt_size = fragmenter.from_kb("record.split", kb);

      if (rebuilt_size == size && fragmenter.file_size == size)
      {
        std::cerr << "SUCCESS\n";
      }
      else
      {
        std::cerr << "FAIL\n";
        ++madara_fails;
        kb.print();
      }

      std::cerr << "  Erasing a fragment and rebuilding... ";
      fragments.set(2, knowledge::KnowledgeRecord());

      rebuilt_size = fragmenter.from_kb("record.split", kb);

      if (rebuilt_size != size && fragmenter.file_size == rebuilt_size)
      {
        std::cerr << "SUCCESS\n";
      }  // end is still fragmented
      else
      {
        std::cerr << "FAIL\n";
        ++madara_fails;
        kb.print();
      }  // end is complete (FAIL)

      std::cerr << "  Checking modifieds with default settings... ";

      fragments = fragmenter.create_vector(
          "record.split", kb, knowledge::KnowledgeUpdateSettings::DEFAULT);
      modifieds = kb.save_modifieds();

      if (modifieds.size() == 6)
      {
        std::cerr << "SUCCESS\n";
      }  // end modifieds == 6
      else
      {
        std::cerr << "FAIL. modifieds.size should be 6 but is "
                  << modifieds.size() << "\n";
        ++madara_fails;
        kb.print();
      }  // end modifieds != 6 (FAIL)

      std::cerr << "  Clearing modifieds...\n";

      kb.clear_modifieds();

      std::cerr << "  Checking modifieds on one vector element... ";

      fragments.modify(2);
      modifieds = kb.save_modifieds();

      if (modifieds.size() == 1)
      {
        std::cerr << "SUCCESS\n";
      }  // end modifieds == 1
      else
      {
        std::cerr << "FAIL. modifieds.size should be 1 but is "
                  << modifieds.size() << "\n";
        ++madara_fails;
        kb.print();
      }  // end modifieds != 1 (FAIL)
    }    // end has 5 fragments of appropriate size
    else
    {
      std::cerr << "FAIL\n";
      ++madara_fails;
      kb.print();
    }  // end does not have appropriate fragments (FAIL)
  }    // end has 5 fragments
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }  // end does not have 5 fragments (FAIL)

  delete[] text_buffer;
}

int main(int argc, char** argv)
{
  handle_arguments(argc, argv);

  test_replace();
  test_sqrt();
  test_version();
  test_endian_swap();
  test_heaps();
  test_time();
  test_ints();
  test_sleep();
  test_file_fragmenter();
  test_file_crc();

  if (madara_fails > 0)
  {
    std::cerr << "OVERALL: FAIL. " << madara_fails << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_fails;
}
