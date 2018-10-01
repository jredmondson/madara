

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/transport/MessageHeader.h"
#include "madara/transport/Transport.h"

#include "madara/utility/Utility.h"
#include <stdio.h>
#include <iostream>

#define BUFFER_SIZE 1000
#define LARGE_BUFFER_SIZE 500000

int madara_fails = 0;

void test_primitive_encoding(void)
{
  std::cerr << "\n*************TEST PRIMITIVE ENCODING*****************\n\n";

  // buffer for holding encoding results
  char buffer[BUFFER_SIZE];
  int64_t buffer_remaining;
  std::string key;
  bool header_decoded, string_decoded, int_decoded, double_decoded;

  // message headers for encoding and decoding
  madara::transport::MessageHeader source_header;
  madara::transport::MessageHeader dest_header;

  // knowledge update for encoding and decoding
  madara::knowledge::KnowledgeRecord string_helloworld_source("hello world");
  madara::knowledge::KnowledgeRecord int_source(
      madara::knowledge::KnowledgeRecord::Integer(10));
  madara::knowledge::KnowledgeRecord double_source(5.5);

  madara::knowledge::KnowledgeRecord dest;

  // Test 1: create the source header
  memset(buffer, 0, BUFFER_SIZE);
  source_header.clock = 100;
  buffer_remaining = BUFFER_SIZE;
  strncpy(source_header.domain, "encoding\0", 9);
  strncpy(source_header.madara_id, "KaRL1.0\0", 8);
  strncpy(source_header.originator, "localhost:34000\0", 16);
  source_header.size = source_header.encoded_size();
  source_header.type = madara::transport::MULTIASSIGN;
  source_header.updates = 0;
  source_header.quality = 5;

  std::cerr << "Test 1: encoding source_header to buffer.\n";

  // Test 1: encode source header into the buffer
  source_header.write(buffer, buffer_remaining);

  std::cerr << "Test 1: encoded source_header to buffer. Bytes remaining="
            << buffer_remaining << std::endl;

  // Test 2: decode source header from buffer

  std::cerr << "Test 2: decoding dest_header from buffer.\n";

  buffer_remaining = BUFFER_SIZE - buffer_remaining;
  dest_header.read(buffer, buffer_remaining);
  buffer_remaining = BUFFER_SIZE - buffer_remaining;

  std::cerr << "Test 2: decoded dest_header from buffer. Bytes remaining="
            << buffer_remaining << std::endl;

  header_decoded = dest_header.equals(source_header);

  std::cerr << "Test 2: decoded dest_header is equal to source_header? "
            << (header_decoded ? "true" : "false") << std::endl;

  std::cerr << "Test 3: encoding string_helloworld_source to buffer.\n";

  string_helloworld_source.write(buffer, "message", buffer_remaining);

  std::cerr << "Test 3: encoded string_helloworld_source to buffer. "
            << "Bytes remaining=" << buffer_remaining << std::endl;

  std::cerr << "Test 4: decoding string_helloworld_source from buffer.\n";

  buffer_remaining = BUFFER_SIZE - buffer_remaining;
  dest.read(buffer, key, buffer_remaining);
  buffer_remaining = BUFFER_SIZE - buffer_remaining;

  std::cerr << "Test 4: decoded " << key << "=" << dest.to_string()
            << " from buffer. Bytes remaining=" << buffer_remaining
            << std::endl;

  string_decoded = dest == string_helloworld_source;

  std::cerr << "Test 4: decoded knowledge record is equal to "
            << "string_helloworld_source? "
            << (string_decoded ? "true" : "false") << std::endl;

  std::cerr << "Test 5: encoding int_source to buffer.\n";

  int_source.write(buffer, "message", buffer_remaining);

  std::cerr << "Test 5: encoded int_source to buffer. "
            << "Bytes remaining=" << buffer_remaining << std::endl;

  std::cerr << "Test 6: decoding int_source from buffer.\n";

  buffer_remaining = BUFFER_SIZE - buffer_remaining;
  dest.read(buffer, key, buffer_remaining);
  buffer_remaining = BUFFER_SIZE - buffer_remaining;

  std::cerr << "Test 6: decoded " << key << "=" << dest.to_string()
            << " from buffer. Bytes remaining=" << buffer_remaining
            << std::endl;

  int_decoded = dest == int_source;

  std::cerr << "Test 6: decoded knowledge record is equal to "
            << "int_source? " << (int_decoded ? "true" : "false") << std::endl;

  std::cerr << "Test 7: encoding double_source to buffer.\n";

  double_source.write(buffer, "message", buffer_remaining);

  std::cerr << "Test 7: encoded double_source to buffer. "
            << "Bytes remaining=" << buffer_remaining << std::endl;

  std::cerr << "Test 8: decoding double_source from buffer.\n";

  buffer_remaining = BUFFER_SIZE - buffer_remaining;
  dest.read(buffer, key, buffer_remaining);
  buffer_remaining = BUFFER_SIZE - buffer_remaining;

  std::cerr << "Test 8: decoded " << key << "=" << dest.to_string()
            << " from buffer. Bytes remaining=" << buffer_remaining
            << std::endl;

  double_decoded = dest == double_source;

  std::cerr << "Test 8: decoded knowledge record is equal to "
            << "double_source? " << (double_decoded ? "true" : "false")
            << std::endl;

  std::cerr << "\nRESULT: ";
  if (header_decoded && string_decoded && int_decoded && double_decoded) {
    std::cerr << "SUCCESS\n";
  } else {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
}

void test_image_encoding(void)
{
  std::cerr << "\n\n*************TEST IMAGE ENCODING*****************\n\n";

  char buffer[LARGE_BUFFER_SIZE];
  char* write_buffer;
  const char* read_buffer;
  int64_t buffer_remaining = LARGE_BUFFER_SIZE, buffer_read, buffer_written;
  std::string key;

  // message headers for encoding and decoding
  madara::transport::MessageHeader source_header;
  madara::transport::MessageHeader dest_header;

  // knowledge update for encoding and decoding
  madara::knowledge::KnowledgeRecord string_source("hello world");
  madara::knowledge::KnowledgeRecord int_source(
      madara::knowledge::KnowledgeRecord::Integer(10));
  madara::knowledge::KnowledgeRecord double_source(5.5);
  madara::knowledge::KnowledgeRecord image_source;

  std::string filename = madara::utility::expand_envs(
      "$(MADARA_ROOT)/tests/images/manaus_hotel_225x375.jpg");

  image_source.read_file(filename);

  madara::knowledge::KnowledgeRecord string_dest;
  madara::knowledge::KnowledgeRecord int_dest;
  madara::knowledge::KnowledgeRecord double_dest;
  madara::knowledge::KnowledgeRecord image_dest;

  std::string string_key_read, int_key_read, double_key_read, image_key_read;

  // Test 1: create the source header
  memset(buffer, 0, LARGE_BUFFER_SIZE);
  source_header.clock = 100;
  strncpy(source_header.domain, "encoding\0", 9);
  strncpy(source_header.madara_id, "KaRL1.0\0", 8);
  strncpy(source_header.originator, "localhost:34000\0", 16);
  source_header.size = source_header.encoded_size();
  source_header.type = madara::transport::MULTIASSIGN;
  source_header.updates = 0;
  source_header.quality = 5;

  std::cerr << "Test 1: encoding source_header to buffer.\n";

  // Test 1: encode source header into the buffer
  source_header.write(buffer, buffer_remaining);

  buffer_written = LARGE_BUFFER_SIZE - buffer_remaining;

  std::cerr << "  Testing bytes written (" << buffer_written << ")...";

  if (dest_header.encoded_size() == buffer_written)
    std::cerr << "SUCCESS\n";
  else {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  // Test 2: decode source header from buffer

  std::cerr << "Test 2: decoding from buffer to dest_header.\n";

  buffer_remaining = buffer_written;

  dest_header.read(buffer, buffer_remaining);
  buffer_read = buffer_written - buffer_remaining;

  std::cerr << "  Testing bytes read (" << buffer_read << ")...";

  if (buffer_written == buffer_read)
    std::cerr << "SUCCESS\n";
  else {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  std::cerr << "Test 3: encoding string, image, int, double.\n";

  buffer_remaining = LARGE_BUFFER_SIZE;
  write_buffer = buffer;
  write_buffer = string_source.write(write_buffer, "string", buffer_remaining);
  write_buffer = image_source.write(write_buffer, "image", buffer_remaining);
  write_buffer = int_source.write(write_buffer, "int", buffer_remaining);
  write_buffer = double_source.write(write_buffer, "double", buffer_remaining);

  buffer_written = LARGE_BUFFER_SIZE - buffer_remaining;

  std::cerr << "  Testing bytes written (" << buffer_written << ")...";

  if (string_source.get_encoded_size("string") +
          image_source.get_encoded_size("image") +
          int_source.get_encoded_size("int") +
          double_source.get_encoded_size("double") ==
      buffer_written)
    std::cerr << "SUCCESS\n";
  else {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  std::cerr << "Test 4: decoding string, image, int, double.\n";

  buffer_remaining = buffer_written;

  read_buffer = buffer;
  read_buffer =
      string_dest.read(read_buffer, string_key_read, buffer_remaining);
  read_buffer = image_dest.read(read_buffer, image_key_read, buffer_remaining);
  read_buffer = int_dest.read(read_buffer, int_key_read, buffer_remaining);
  read_buffer =
      double_dest.read(read_buffer, double_key_read, buffer_remaining);
  buffer_read = buffer_written - buffer_remaining;

  std::cerr << "  Checking amount read (" << buffer_read << ")... ";

  if (string_source.get_encoded_size("string") +
          image_source.get_encoded_size("image") +
          int_source.get_encoded_size("int") +
          double_source.get_encoded_size("double") ==
      buffer_read)
    std::cerr << "SUCCESS\n";
  else {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  std::cerr << "  Checking keys read... ";

  if (string_key_read == "string" && image_key_read == "image" &&
      int_key_read == "int" && double_key_read == "double")
    std::cerr << "SUCCESS\n";
  else {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
}

void test_key_id_encoding(void)
{
  std::cerr << "\n*************TEST KEYED ID ENCODING*****************\n\n";

  // buffer for holding encoding results
  char buffer[BUFFER_SIZE];
  int64_t buffer_remaining = BUFFER_SIZE;
  char* current = buffer;
  const char* reader = buffer;
  std::string key;

  enum KeyIds { HELLO_WORLD, CUSTOM_SIZE, CUSTOM_DISTANCE };

  // message headers for encoding and decoding
  madara::transport::MessageHeader source_header;
  madara::transport::MessageHeader dest_header;

  // knowledge update for encoding and decoding
  madara::knowledge::KnowledgeRecord hello_world("hello world");
  madara::knowledge::KnowledgeRecord custom_size(
      madara::knowledge::KnowledgeRecord::Integer(10));
  madara::knowledge::KnowledgeRecord custom_distance(5.5);

  madara::knowledge::KnowledgeRecord dest;
  uint32_t temp_id(0);

  std::cerr << "Writing string, int, and double records to buffer\n";

  // write the records to the buffer
  current = hello_world.write(current, HELLO_WORLD, buffer_remaining);
  current = custom_size.write(current, CUSTOM_SIZE, buffer_remaining);
  current = custom_distance.write(current, CUSTOM_DISTANCE, buffer_remaining);

  buffer_remaining = BUFFER_SIZE - buffer_remaining;

  std::cerr << "Wrote " << buffer_remaining << " bytes to buffer\n";

  std::cerr << "Reading string, int, and double records from buffer\n";

  std::cerr << "  String: ";

  // read the records from the buffer
  reader = dest.read(reader, temp_id, buffer_remaining);

  if (temp_id == HELLO_WORLD && dest.is_string_type() &&
      dest.to_string() == "hello world") {
    std::cerr << "SUCCESS\n";
  } else {
    ++madara_fails;
    std::cerr << "FAIL\n";
    std::cerr << "    id: " << temp_id << "\n";
    std::cerr << "    type: " << dest.type() << "\n";
    std::cerr << "    value: " << dest.to_string() << "\n";
  }

  std::cerr << "  Integer: ";

  // read the records from the buffer
  reader = dest.read(reader, temp_id, buffer_remaining);

  if (temp_id == CUSTOM_SIZE && dest.is_integer_type() &&
      dest.to_integer() == 10) {
    std::cerr << "SUCCESS\n";
  } else {
    ++madara_fails;
    std::cerr << "FAIL\n";
    std::cerr << "    id: " << temp_id << "\n";
    std::cerr << "    type: " << dest.type() << "\n";
    std::cerr << "    value: " << dest.to_string() << "\n";
  }

  std::cerr << "  Double: ";

  // read the records from the buffer
  reader = dest.read(reader, temp_id, buffer_remaining);

  if (temp_id == CUSTOM_DISTANCE && dest.is_double_type() &&
      dest.to_double() == 5.5) {
    std::cerr << "SUCCESS\n";
  } else {
    ++madara_fails;
    std::cerr << "FAIL.\n";
    std::cerr << "    id: " << temp_id << "\n";
    std::cerr << "    type: " << dest.type() << "\n";
    std::cerr << "    value: " << dest.to_string() << "\n";
  }
}

int main(int, char**)
{
  test_image_encoding();
  test_primitive_encoding();
  test_key_id_encoding();

  if (madara_fails > 0) {
    std::cerr << "OVERALL: FAIL. " << madara_fails << " tests failed.\n";
  } else {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_fails;
}
