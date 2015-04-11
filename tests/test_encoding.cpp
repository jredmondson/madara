

#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/transport/Message_Header.h"
#include "madara/transport/Transport.h"
#include "madara/utility/Log_Macros.h"
#include "madara/utility/Utility.h"
#include <stdio.h>
#include <iostream>

#define BUFFER_SIZE    1000
#define LARGE_BUFFER_SIZE 500000

void test_primitive_encoding (void)
{
  std::cerr << "\n*************TEST PRIMITIVE ENCODING*****************\n\n";

  // buffer for holding encoding results
  char buffer[BUFFER_SIZE];
  int64_t buffer_remaining;
  std::string key;
  bool header_decoded, string_decoded, int_decoded, double_decoded;


  // message headers for encoding and decoding
  Madara::Transport::Message_Header source_header;
  Madara::Transport::Message_Header dest_header;

  // knowledge update for encoding and decoding
  Madara::Knowledge_Record string_helloworld_source ("hello world");
  Madara::Knowledge_Record int_source (Madara::Knowledge_Record::Integer (10));
  Madara::Knowledge_Record double_source (5.5);
  
  Madara::Knowledge_Record dest;

  // Test 1: create the source header
  memset (buffer, 0, BUFFER_SIZE);
  source_header.clock = 100;
  buffer_remaining = BUFFER_SIZE;
  strncpy (source_header.domain, "encoding\0", 9);
  strncpy (source_header.madara_id, "KaRL1.0\0", 8);
  strncpy (source_header.originator, "localhost:34000\0", 16);
  source_header.size = source_header.encoded_size ();
  source_header.type = Madara::Transport::MULTIASSIGN;
  source_header.updates = 0;
  source_header.quality = 5;
  
  std::cerr << "Test 1: encoding source_header to buffer.\n";

  // Test 1: encode source header into the buffer
  source_header.write (buffer, buffer_remaining);
  
  std::cerr << "Test 1: encoded source_header to buffer. Bytes remaining=" << 
    buffer_remaining << std::endl;

  // Test 2: decode source header from buffer
  
  std::cerr << "Test 2: decoding dest_header from buffer.\n";

  buffer_remaining = BUFFER_SIZE - buffer_remaining;
  dest_header.read (buffer, buffer_remaining);
  buffer_remaining = BUFFER_SIZE - buffer_remaining;
  
  std::cerr << "Test 2: decoded dest_header from buffer. Bytes remaining=" << 
    buffer_remaining << std::endl;
  
  header_decoded = dest_header.equals (source_header);

  std::cerr << "Test 2: decoded dest_header is equal to source_header? " <<
    (header_decoded ? "true" : "false") << std::endl;
  
  std::cerr << "Test 3: encoding string_helloworld_source to buffer.\n";

  string_helloworld_source.write (buffer, "message", buffer_remaining);
  
  std::cerr << "Test 3: encoded string_helloworld_source to buffer. " <<
    "Bytes remaining=" << 
    buffer_remaining << std::endl;
  
  std::cerr << "Test 4: decoding string_helloworld_source from buffer.\n";

  buffer_remaining = BUFFER_SIZE - buffer_remaining;
  dest.read (buffer, key, buffer_remaining);
  buffer_remaining = BUFFER_SIZE - buffer_remaining;
  
  std::cerr << "Test 4: decoded " << key << "=" <<
    dest.to_string () << " from buffer. Bytes remaining=" << 
    buffer_remaining << std::endl;
  
  string_decoded = dest == string_helloworld_source;

  std::cerr << "Test 4: decoded knowledge record is equal to " <<
    "string_helloworld_source? " <<
    (string_decoded ? "true" : "false") <<
    std::endl;
  
  std::cerr << "Test 5: encoding int_source to buffer.\n";

  int_source.write (buffer, "message", buffer_remaining);
  
  std::cerr << "Test 5: encoded int_source to buffer. " <<
    "Bytes remaining=" << 
    buffer_remaining << std::endl;
  
  std::cerr << "Test 6: decoding int_source from buffer.\n";

  buffer_remaining = BUFFER_SIZE - buffer_remaining;
  dest.read (buffer, key, buffer_remaining);
  buffer_remaining = BUFFER_SIZE - buffer_remaining;
  
  std::cerr << "Test 6: decoded " << key << "=" <<
    dest.to_string () << " from buffer. Bytes remaining=" << 
    buffer_remaining << std::endl;
  
  int_decoded = dest == int_source;

  std::cerr << "Test 6: decoded knowledge record is equal to " <<
    "int_source? " <<
    (int_decoded ? "true" : "false") <<
    std::endl;
  
  std::cerr << "Test 7: encoding double_source to buffer.\n";

  double_source.write (buffer, "message", buffer_remaining);
  
  std::cerr << "Test 7: encoded double_source to buffer. " <<
    "Bytes remaining=" << 
    buffer_remaining << std::endl;
  
  std::cerr << "Test 8: decoding double_source from buffer.\n";

  buffer_remaining = BUFFER_SIZE - buffer_remaining;
  dest.read (buffer, key, buffer_remaining);
  buffer_remaining = BUFFER_SIZE - buffer_remaining;
  
  std::cerr << "Test 8: decoded " << key << "=" <<
    dest.to_string () << " from buffer. Bytes remaining=" << 
    buffer_remaining << std::endl;
  
  double_decoded = dest == double_source;

  std::cerr << "Test 8: decoded knowledge record is equal to " <<
    "double_source? " <<
    (double_decoded ? "true" : "false") <<
    std::endl;
  

  std::cerr << "\nRESULT: " << 
    (header_decoded && string_decoded && int_decoded && double_decoded ?
    "SUCCESS\n" : "FAIL\n");
}

void test_image_encoding (void)
{
  std::cerr << "\n\n*************TEST IMAGE ENCODING*****************\n\n";

  char buffer[LARGE_BUFFER_SIZE];
  char * write_buffer;
  const char * read_buffer;
  int64_t buffer_remaining = LARGE_BUFFER_SIZE, buffer_read, buffer_written;
  std::string key;

  // message headers for encoding and decoding
  Madara::Transport::Message_Header source_header;
  Madara::Transport::Message_Header dest_header;

  // knowledge update for encoding and decoding
  Madara::Knowledge_Record string_source ("hello world");
  Madara::Knowledge_Record int_source (Madara::Knowledge_Record::Integer (10));
  Madara::Knowledge_Record double_source (5.5);
  Madara::Knowledge_Record image_source;

  std::string filename =
    Madara::Utility::expand_envs (
      "$(MADARA_ROOT)/tests/images/manaus_hotel_225x375.jpg");

  image_source.read_file (filename);

  Madara::Knowledge_Record string_dest;
  Madara::Knowledge_Record int_dest;
  Madara::Knowledge_Record double_dest;
  Madara::Knowledge_Record image_dest;

  std::string string_key_read, int_key_read, double_key_read, image_key_read;

  // Test 1: create the source header
  memset (buffer, 0, LARGE_BUFFER_SIZE);
  source_header.clock = 100;
  strncpy (source_header.domain, "encoding\0", 9);
  strncpy (source_header.madara_id, "KaRL1.0\0", 8);
  strncpy (source_header.originator, "localhost:34000\0", 16);
  source_header.size = source_header.encoded_size ();
  source_header.type = Madara::Transport::MULTIASSIGN;
  source_header.updates = 0;
  source_header.quality = 5;
  
  std::cerr << "Test 1: encoding source_header to buffer.\n";

  // Test 1: encode source header into the buffer
  source_header.write (buffer, buffer_remaining);
  
  buffer_written = LARGE_BUFFER_SIZE - buffer_remaining;

  std::cerr << "  Testing bytes written (" << 
    buffer_written << ")..." ;
  
  if (dest_header.encoded_size () == buffer_written)
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
  
  // Test 2: decode source header from buffer
  
  std::cerr << "Test 2: decoding from buffer to dest_header.\n";

  buffer_remaining = buffer_written;

  dest_header.read (buffer, buffer_remaining);
  buffer_read = buffer_written - buffer_remaining;
  
  std::cerr << "  Testing bytes read (" << 
    buffer_read << ")..." ;
  
  if (buffer_written == buffer_read)
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
  
  std::cerr << "Test 3: encoding string, image, int, double.\n";

  buffer_remaining = LARGE_BUFFER_SIZE;
  write_buffer = buffer;
  write_buffer = string_source.write (write_buffer, "string", buffer_remaining);
  write_buffer = image_source.write (write_buffer, "image", buffer_remaining);
  write_buffer = int_source.write (write_buffer, "int", buffer_remaining);
  write_buffer = double_source.write (write_buffer, "double", buffer_remaining);
  
  buffer_written = LARGE_BUFFER_SIZE - buffer_remaining;

  std::cerr << "  Testing bytes written (" << 
    buffer_written << ")..." ;
  
  if (string_source.get_encoded_size ("string") +
      image_source.get_encoded_size ("image") +
      int_source.get_encoded_size ("int") +
      double_source.get_encoded_size ("double") == buffer_written)
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
  
  std::cerr << "Test 4: decoding string, image, int, double.\n";

  buffer_remaining = buffer_written;
  
  read_buffer = buffer;
  read_buffer = string_dest.read (read_buffer, string_key_read, buffer_remaining);
  read_buffer = image_dest.read (read_buffer, image_key_read, buffer_remaining);
  read_buffer = int_dest.read (read_buffer, int_key_read, buffer_remaining);
  read_buffer = double_dest.read (read_buffer, double_key_read, buffer_remaining);
  buffer_read = buffer_written - buffer_remaining;
  
  std::cerr << "  Checking amount read (" <<
    buffer_read << ")... ";

  if (string_source.get_encoded_size ("string") +
      image_source.get_encoded_size ("image") +
      int_source.get_encoded_size ("int") +
      double_source.get_encoded_size ("double") == buffer_read)
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
  
  std::cerr << "  Checking keys read... ";
  
  if (string_key_read == "string" &&
      image_key_read == "image" &&
      int_key_read == "int" &&
      double_key_read == "double")
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAIL\n";
  
  
}

int main (int, char **)
{
  test_image_encoding ();
  //test_primitive_encoding ();
  return 0;
}
