
#include "madara/transport/Fragmentation.h"
#include "madara/transport/MessageHeader.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/utility/Utility.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

#define BUFFER_SIZE 1000
#define LARGE_BUFFER_SIZE 500000

namespace transport = madara::transport;
namespace logger = madara::logger;

int madara_fails = 0;

char chars[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

void handle_arguments(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i) {
    std::string arg1(argv[i]);

    if (arg1 == "-l" || arg1 == "--level") {
      if (i + 1 < argc) {
        int level;
        std::stringstream buffer(argv[i + 1]);
        buffer >> level;

        logger::global_logger->set_level(level);
      }

      ++i;
    } else {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s:\n\n"
          "  Tests the fragment library\n\n"
          " [-l|--level level]       the logger level (0+, higher is higher "
          "detail)\n"
          "\n",
          argv[0]);
      exit(0);
    }
  }
}

void test_frag(void)
{
  uint32_t size = 300000 + transport::MessageHeader::static_encoded_size();

  char* payload = new char[size];
  char* buffer = payload;
  int64_t buffer_remaining = size;
  transport::FragmentMap map;

  transport::MessageHeader header;
  header.size = size;

  buffer = header.write(buffer, buffer_remaining);

  for (int i = 0; i < 300000; ++i) {
    int column = i % 10;
    buffer[i] = chars[column];
  }
  buffer[299999] = 0;

  transport::frag(payload, 50000, map);

  char* result = transport::defrag(map);

  if (header.message_header_test(result)) {
    buffer_remaining = header.encoded_size();
    header.read(result, buffer_remaining);
    buffer_remaining = header.size - header.encoded_size();
    if (buffer_remaining != 300000) {
      std::cerr << "FAIL. defrag: Size is incorrect after defrag.\n";
      ++madara_fails;
    } else {
      bool failed = false;
      for (int i = 0; i < 299999 && !failed; ++i) {
        int column = i % 10;
        if (buffer[i] != chars[column])
          failed = true;
      }

      if (failed) {
        std::cerr << "FAIL. defrag: Size is incorrect after defrag.\n";
        ++madara_fails;
      } else
        std::cerr << "SUCCESS. defrag: Message was defragged correctly.\n";
    }
  } else {
    std::cerr << "FAIL. defrag: Fragments reassembled to bad data.\n";
    ++madara_fails;
  }

  delete[] payload;
  delete[] result;
}

void test_add_frag(void)
{
  uint32_t size = 300000 + transport::MessageHeader::static_encoded_size();

  char* payload = new char[size];
  char* buffer = payload;
  int64_t buffer_remaining = size;
  transport::FragmentMap map;
  transport::OriginatorFragmentMap frag_map;

  transport::MessageHeader header;
  header.size = size;

  buffer = header.write(buffer, buffer_remaining);

  for (int i = 0; i < 300000; ++i) {
    int column = i % 10;
    buffer[i] = chars[column];
  }
  buffer[299999] = 0;

  transport::frag(payload, 50000, map);

  transport::add_fragment("testmachine", 1, 0, map[0], 5, frag_map, true);
  transport::add_fragment("testmachine", 1, 2, map[2], 5, frag_map, true);

  if (transport::exists("testmachine", 1, 0, frag_map))
    std::cerr << "SUCCESS. exists found testmachine:1:0 in frag_map.\n";
  else {
    std::cerr << "FAIL. exists did not find testmachine:1:0 in frag_map.\n";
    ++madara_fails;
  }

  if (transport::exists("testmachine", 1, 1, frag_map)) {
    std::cerr << "FAIL. exists found testmachine:1:1 in frag_map.\n";
    ++madara_fails;
  } else
    std::cerr << "SUCCESS. exists did not find testmachine:1:1 in frag_map.\n";

  transport::add_fragment("testmachine", 1, 6, map[6], 5, frag_map, true);
  transport::add_fragment("testmachine", 1, 6, map[6], 5, frag_map, true);
  transport::add_fragment("testmachine", 1, 1, map[1], 5, frag_map, true);

  if (transport::is_complete("testmachine", 1, frag_map)) {
    std::cerr
        << "FAIL. is_complete is returning true from incomplete packets.\n";
    ++madara_fails;
  }

  transport::add_fragment("testmachine", 1, 3, map[3], 5, frag_map, true);
  transport::add_fragment("testmachine", 1, 5, map[5], 5, frag_map, true);

  char* result =
      transport::add_fragment("testmachine", 1, 4, map[4], 5, frag_map, true);

  if (transport::is_complete("testmachine", 1, frag_map)) {
    std::cerr << "FAIL. is_complete is returning true, even though packet was "
                 "removed.\n";
    ++madara_fails;
  } else {
    std::cerr << "SUCCESS. is_complete is returning false on cleared defragged "
                 "entry.\n";
  }

  if (transport::exists("testmachine", 1, 1, frag_map)) {
    std::cerr << "FAIL. exists found testmachine:1:1 in frag_map.\n";
    ++madara_fails;
  } else
    std::cerr << "SUCCESS. exists did not find testmachine:1:1 in frag_map.\n";

  if (header.message_header_test(result)) {
    buffer_remaining = header.encoded_size();
    header.read(result, buffer_remaining);
    buffer_remaining = header.size - header.encoded_size();
    if (buffer_remaining != 300000) {
      std::cerr << "FAIL. add_fragment: Size is incorrect after defrag.\n";
      ++madara_fails;
    } else {
      bool failed = false;
      for (int i = 0; i < 299999 && !failed; ++i) {
        int column = i % 10;
        if (buffer[i] != chars[column])
          failed = true;
      }

      if (failed) {
        std::cerr << "FAIL. add_fragment: Size is incorrect after defrag.\n";
        ++madara_fails;
      } else
        std::cerr
            << "SUCCESS. add_fragment: Message was defragged correctly.\n";
    }
  } else {
    std::cerr << "FAIL. add_fragment: Fragments reassembled to bad data.\n";
    ++madara_fails;
  }

  transport::add_fragment("testmachine", 2, 0, map[0], 5, frag_map, true);
  transport::add_fragment("testmachine", 2, 2, map[2], 5, frag_map, true);
  transport::add_fragment("testmachine", 2, 6, map[6], 5, frag_map, true);
  transport::add_fragment("testmachine", 2, 6, map[6], 5, frag_map, true);
  transport::add_fragment("testmachine", 2, 1, map[1], 5, frag_map, true);

  if (transport::is_complete("testmachine", 2, frag_map)) {
    std::cerr
        << "FAIL. is_complete is returning true from incomplete packets.\n";
    ++madara_fails;
  }

  transport::add_fragment("testmachine", 2, 3, map[3], 5, frag_map, true);
  transport::add_fragment("testmachine", 2, 5, map[5], 5, frag_map, true);
  result =
      transport::add_fragment("testmachine", 2, 4, map[4], 5, frag_map, false);

  if (transport::is_complete("testmachine", 2, frag_map))
    std::cerr << "SUCCESS. is_complete is returning true on complete packet.\n";
  else {
    ++madara_fails;
  }

  if (result && header.message_header_test(result)) {
    buffer_remaining = header.encoded_size();
    header.read(result, buffer_remaining);
    buffer_remaining = header.size - header.encoded_size();
    if (buffer_remaining != 300000) {
      std::cerr << "FAIL. add_fragment: Size is incorrect after defrag.\n";
      ++madara_fails;
    } else {
      bool failed = false;
      for (int i = 0; i < 299999 && !failed; ++i) {
        int column = i % 10;
        if (buffer[i] != chars[column])
          failed = true;
      }

      if (failed) {
        std::cerr << "FAIL. add_fragment: Size is incorrect after defrag.\n";
        ++madara_fails;
      } else
        std::cerr
            << "SUCCESS. add_fragment: Message was defragged correctly.\n";
    }
  } else {
    std::cerr << "FAIL. add_fragment: Fragments reassembled to bad data.\n";
    ++madara_fails;
  }

  delete[] result;
  delete[] payload;
}

void test_records_frag(void)
{
  uint32_t size = 300000 + sizeof(int64_t) + sizeof(double) +
                  transport::MessageHeader::static_encoded_size();

  char* payload = new char[size];
  char* buffer = payload;
  int64_t buffer_remaining = size;
  transport::FragmentMap map;
  transport::OriginatorFragmentMap frag_map;

  transport::MessageHeader header;
  header.size = size;

  madara::knowledge::KnowledgeMap knowledge, copied_knowledge;
  knowledge["str_var"] =
      madara::knowledge::KnowledgeRecord(std::string(254000, ' '));
  knowledge["int_var"] = madara::knowledge::KnowledgeRecord::Integer(254LL);
  knowledge["double_var"] = madara::knowledge::KnowledgeRecord(5.235);

  header.updates = (uint32_t)knowledge.size();
  header.clock = 10;
  header.ttl = 2;
  memcpy(header.originator, "test_source", strlen("test_source"));

  std::cerr << "Constructing record aggregation...\n";
  buffer = header.write(buffer, buffer_remaining);

  for (madara::knowledge::KnowledgeMap::iterator i = knowledge.begin();
       i != knowledge.end(); ++i) {
    buffer = i->second.write(buffer, i->first, buffer_remaining);
  }

  std::cerr << "Fragmenting the 254KB+ record into 60K packets...\n";

  transport::frag(payload, 60000, map);

  const char* result = 0;

  for (unsigned int i = 0; i < map.size(); ++i) {
    result = transport::add_fragment(
        header.originator, header.clock, i, map[i], 5, frag_map);
  }

  buffer = (char*)result;
  bool knowledge_mismatch = false;

  if (result) {
    buffer_remaining = transport::MessageHeader::get_size(result);
    transport::MessageHeader copied_header;

    std::cerr << "Reading result into a copied header...\n";

    result = copied_header.read(result, buffer_remaining);

    if (copied_header.equals(header)) {
      std::cerr << "SUCCESS: header was decoded properly\n";
    } else {
      std::cerr << "FAIL: header was not decoded properly\n";
      ++madara_fails;
    }

    for (uint32_t i = 0; i < copied_header.updates; ++i) {
      madara::knowledge::KnowledgeRecord temp;
      std::string key;
      result = temp.read(result, key, buffer_remaining);
      copied_knowledge[key] = temp;

      if (copied_knowledge[key] != knowledge[key]) {
        knowledge_mismatch = 1;
        std::cerr << "FAIL: Knowledge mismatch on key " << key << "\n";
        ++madara_fails;
      }
    }

    if (!knowledge_mismatch) {
      std::cerr
          << "SUCCESS: All copied knowledge records match the originals\n";
    } else {
      ++madara_fails;
    }

    delete[] buffer;
  }
  delete[] payload;
}

int main(int argc, char* argv[])
{
  handle_arguments(argc, argv);

  test_frag();
  test_add_frag();
  test_records_frag();

  if (madara_fails > 0) {
    std::cerr << "OVERALL: FAIL. " << madara_fails << " tests failed.\n";
  } else {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_fails;
}
