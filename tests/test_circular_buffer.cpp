#include <string>
#include <iostream>

#include "madara/utility/SupportTest.h"
#include "madara/utility/CircularBuffer.h"
#include "test.h"

namespace logger = madara::logger;

using namespace madara;
using namespace knowledge;
using namespace utility;

template<typename T>
std::ostream &operator<<(std::ostream &o, const CircularBuffer<T> &buf)
{
  for (const auto &cur : buf) {
    o << cur << " ";
  }
  return o;
}

void test_circular_int_buffer()
{
  CircularBuffer<int> b(10);

  VAL(b.capacity());

  for (int i = 0; i < 5; ++i) {
    VAL(b.size());
    b.push_back(i);
  }

  std::cout << b << std::endl;

  for (int i = 5; i < 10; ++i) {
    VAL(b.size());
    b.push_back(i);
  }

  std::cout << b << std::endl;

  b.push_back(10);

  std::cout << b << std::endl;

  b.push_back(11);

  std::cout << b << std::endl;

  b.push_back(12);

  std::cout << b << std::endl;

  b.pop_front();
  b.pop_front();

  std::cout << b << std::endl;

  b.push_back(13);

  std::cout << b << std::endl;

  b.push_back(14);

  std::cout << b << std::endl;

  b.pop_back();
  b.pop_back();

  std::cout << b << std::endl;

  b.push_back(15);

  std::cout << b << std::endl;

  for (auto iter = std::reverse_iterator<typename CircularBuffer<int>::iterator>(b.end());
      iter != std::reverse_iterator<typename CircularBuffer<int>::iterator>(b.begin()); ++iter)
  {
    std::cout << *iter << std::endl;
  }
  std::cout << std::endl;
}

int main (int, char **)
{
  madara::logger::global_logger->set_level(9);

  test_circular_int_buffer();

  if (madara_tests_fail_count > 0)
  {
    std::cerr << "OVERALL: FAIL. " << madara_tests_fail_count <<
      " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_tests_fail_count;
}
