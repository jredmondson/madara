#include <vector>
#include <string>
#include <string.h>
#include <iostream>

#include "madara/utility/Utility.h"
#include "madara/filters/ssl/AESBufferFilter.h"

namespace filters = madara::filters;
namespace utility = madara::utility;

int main(int, char**)
{
  filters::AESBufferFilter filter, filter2;
  filter.generate_key("testPassword#214");

  std::vector<std::string> messages;
  messages.push_back("Hello world!");
  messages.push_back("We are checking AES 256 bit encryption!");
  messages.push_back("If you can read this, then the test is a SUCCESS!");

  std::vector<std::string> results;
  char buffer[256];
  memset(buffer, 1, 256);

  std::cout << "Testing encode/decode from single buffer filter\n";
  for (size_t i = 0; i < messages.size(); ++i)
  {
    int encode_length(0);

    // copy current message into the buffer
    utility::strncpy_safe((char*)buffer, messages[i].c_str(), messages[i].size() + 1);

    // encode and then decode the buffer
    encode_length = filter.encode(buffer, (int)messages[i].size() + 1, 256);
    filter.decode(buffer, encode_length, 256);

    // add decoded message to results
    results.push_back((char*)buffer);

    std::cout << "  Encoding test " << i << " is a ";
    if (messages[i] == results[i])
    {
      std::cout << "SUCCESS.";
    }
    else
    {
      std::cout << "FAIL.";
    }
    std::cout << " Message was \"";
    std::cout << results[i];
    std::cout << "\"\n";

    memset(buffer, 1, 256);
  }

  utility::sleep(2.0);
  results.clear();

  filter2.generate_key("testPassword#214");

  std::cout << "Testing encode/decode from paired buffer filter\n";
  for (size_t i = 0; i < messages.size(); ++i)
  {
    int encode_length(0);

    // copy current message into the buffer
    utility::strncpy_safe((char*)buffer, messages[i].c_str(), messages[i].size() + 1);

    // encode and then decode the buffer
    encode_length = filter.encode(buffer, (int)messages[i].size() + 1, 256);
    filter2.decode(buffer, encode_length, 256);

    // add decoded message to results
    results.push_back((char*)buffer);

    std::cout << "  Encoding test " << i << " is a ";
    if (messages[i] == results[i])
    {
      std::cout << "SUCCESS.";
    }
    else
    {
      std::cout << "FAIL.";
    }
    std::cout << " Message was \"";
    std::cout << results[i];
    std::cout << "\"\n";

    memset(buffer, 1, 256);
  }

  return 0;
}
