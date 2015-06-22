

#include "madara/knowledge_engine/File_Header.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/logger/Global_Logger.h"

#include "madara/utility/Utility.h"

#include <stdio.h>
#include <iostream>

namespace logger = Madara::Logger;

#define BUFFER_SIZE    1000

void test_checkpointing (void)
{
  std::cerr << "\n*********** CHECKPOINTING TO FILE *************.\n";

  std::cerr << "Test 1: creating a 50k knowledge record.\n";

  Madara::Knowledge_Engine::Thread_Safe_Context context;
  Madara::Knowledge_Engine::Thread_Safe_Context context_copy;
  context.read_file ("file_var", Madara::Utility::expand_envs (
      "$(MADARA_ROOT)/tests/images/manaus_hotel_225x375.jpg"));

  context.set ("int_var", Madara::Knowledge_Record::Integer (15));
  context.set ("str_var", std::string ("some string"));
  context.set ("double_var", 3.14159);

  std::vector <Madara::Knowledge_Record::Integer> integers;
  std::vector <double> doubles;
  std::string id = "orig_context";

  integers.push_back (10);
  integers.push_back (20);
  integers.push_back (30);

  doubles.push_back (10.1);
  doubles.push_back (20.2);
  doubles.push_back (30.3);
  doubles.push_back (40.4);

  context.set ("int_array", integers);
  context.set ("double_array", doubles);

  context.print (0);
  
  std::cerr << "Test 1: saving 50k knowledge record to test1_orig.kkb.\n";

  context.save_as_karl ("test1_orig.karl");
  context.save_context ("test1_orig.kkb", "test_context");

  std::cerr << "Test 2: loading test1_orig.kkb.\n";

  context_copy.clear ();
  context_copy.load_context ("test1_orig.kkb", id);

  std::cerr << "Test 2: loaded context " << id << "\n";

  context.print (0);
  
  std::cerr << "Test 2: Results were ";
  if (context_copy.get ("int_var") == Madara::Knowledge_Record::Integer (15)
      && context_copy.get ("double_var") == 3.14159
      && context_copy.get ("str_var") == "some string"
      && context_copy.get ("int_array").to_string (", ") == "10, 20, 30"
      && context_copy.get ("double_array").to_string (", ") ==
          "10.1, 20.2, 30.3, 40.4"
      && context_copy.get ("file_var").size () > 40000)
  {
    std::cerr << "SUCCESS\n\n";
  }
  else
    std::cerr << "FAIL\n\n";

  std::cerr << "Test 3: reading 500 KB file to force buffer resize\n";

  context_copy.read_file ("file_var", Madara::Utility::expand_envs (
      "$(MADARA_ROOT)/tests/images/manaus_hotel_900x1500.jpg"));
  
  context_copy.print (0);
  
  std::cerr << "Test 3: saving context\n";

  context_copy.save_as_karl ("test3_bigger.karl");
  context_copy.save_context ("test3_bigger.kkb", id);
  
  std::cerr << "Test 3: loading context\n";
  
  context_copy.clear ();
  context_copy.load_context ("test3_bigger.kkb", id);
  
  std::cerr << "Test 3: Results were ";
  if (context_copy.get ("int_var") == Madara::Knowledge_Record::Integer (15)
      && context_copy.get ("double_var") == 3.14159
      && context_copy.get ("str_var") == "some string"
      && context_copy.get ("int_array").to_string (", ") == "10, 20, 30"
      && context_copy.get ("double_array").to_string (", ") ==
          "10.1, 20.2, 30.3, 40.4"
      && context_copy.get ("file_var").size () > 500000)
  {
    std::cerr << "SUCCESS\n\n";
  }
  else
    std::cerr << "FAIL\n\n";

  context_copy.print (0);
  
  std::cerr << "Test 3: saving context\n";

  context_copy.save_as_karl ("test4_resave.karl");
  context_copy.save_context ("test4_resave.kkb", id);
  
  std::cerr << "Test 4: loading context into a knowledge base\n";
  
  Madara::Knowledge_Engine::Knowledge_Base knowledge;
  knowledge.load_context ("test4_resave.kkb", false);
  
  std::cerr << "Test 4: Results were ";
  if (knowledge.get ("int_var") == Madara::Knowledge_Record::Integer (15)
      && knowledge.get ("double_var") == 3.14159
      && knowledge.get ("str_var") == "some string"
      && knowledge.get ("int_array").to_string (", ") == "10, 20, 30"
      && knowledge.get ("double_array").to_string (", ") ==
          "10.1, 20.2, 30.3, 40.4"
      && knowledge.get ("file_var").size () > 500000)
  {
    std::cerr << "SUCCESS\n\n";
  }
  else
    std::cerr << "FAIL\n\n";
  
  knowledge.print ();
  
  std::cerr << "Test 5: Adding extra_var to context and saving\n";

  context_copy.save_as_karl ("test4_bigger.karl");
  context_copy.save_context ("test4_bigger.kkb", "Extra Var Context");

  context_copy.set ("extra_var", 5.0,
    Madara::Knowledge_Engine::Knowledge_Update_Settings (false, false, false, false));

  context_copy.save_checkpoint ("test4_bigger.kkb", "Extra Var Context");
  
  std::cerr << "Test 5: Loading new context\n";
  
  knowledge.clear ();
  knowledge.load_context ("test4_bigger.kkb", true);

  std::cerr << "Test 5: Results were ";
  if (knowledge.get ("int_var") == Madara::Knowledge_Record::Integer (15)
      && knowledge.get ("double_var") == 3.14159
      && knowledge.get ("str_var") == "some string"
      && knowledge.get ("int_array").to_string (", ") == "10, 20, 30"
      && knowledge.get ("double_array").to_string (", ") ==
          "10.1, 20.2, 30.3, 40.4"
      && knowledge.get ("file_var").size () > 500000
      && knowledge.get ("extra_var") == 5.0)
  {
    std::cerr << "SUCCESS\n\n";
  }
  else
    std::cerr << "FAIL\n\n";
  
  knowledge.print ();

  knowledge.set ("additional_var", 6.0,
    Madara::Knowledge_Engine::Eval_Settings (true, false, false, false, false));
  
  knowledge.save_checkpoint ("test4_bigger.kkb", "Additional Var Context");
  
  std::cerr << "Test 6: Loading new context\n";
  
  knowledge.clear ();
  knowledge.load_context ("test4_bigger.kkb", true);

  std::cerr << "Test 6: Results were ";
  if (knowledge.get ("int_var") == Madara::Knowledge_Record::Integer (15)
      && knowledge.get ("double_var") == 3.14159
      && knowledge.get ("str_var") == "some string"
      && knowledge.get ("int_array").to_string (", ") == "10, 20, 30"
      && knowledge.get ("double_array").to_string (", ") ==
          "10.1, 20.2, 30.3, 40.4"
      && knowledge.get ("file_var").size () > 500000
      && knowledge.get ("extra_var") == 5.0
      && knowledge.get ("additional_var") == 6.0)
  {
    std::cerr << "SUCCESS\n\n";
  }
  else
    std::cerr << "FAIL\n\n";
  
  knowledge.print ();

  
  std::cerr << "Test 7: Adding local variable without track local changes\n";
  knowledge.set (".invisible", "tee hee");
  
  std::cerr << "Test 7: Printing .invisible value\n";
  knowledge.print (".invisible = {.invisible}\n");
  knowledge.save_checkpoint ("test4_bigger.kkb", "Invisible Var Context");
  
  std::cerr << "Test 7: Loading checkpoint\n";
  knowledge.clear ();
  knowledge.load_context ("test4_bigger.kkb", true);
  
  std::cerr << "Test 7: Printing .invisible value (should be 0)\n";
  knowledge.print (".invisible = {.invisible}\n");
  
  std::cerr << "Test 7: Results were ";
  if (knowledge.get ("int_var") == Madara::Knowledge_Record::Integer (15)
      && knowledge.get ("double_var") == 3.14159
      && knowledge.get ("str_var") == "some string"
      && knowledge.get ("int_array").to_string (", ") == "10, 20, 30"
      && knowledge.get ("double_array").to_string (", ") ==
          "10.1, 20.2, 30.3, 40.4"
      && knowledge.get ("file_var").size () > 500000
      && knowledge.get ("extra_var") == 5.0
      && knowledge.get ("additional_var") == 6.0
      && knowledge.get (".invisible").status ()
           == Madara::Knowledge_Record::UNCREATED)
  {
    std::cerr << "SUCCESS\n\n";
  }
  else
    std::cerr << "FAIL\n\n";
  
  
  std::cerr << "Test 8: Adding local variable with track local changes\n";
  knowledge.set (".invisible", "tee hee",
    Madara::Knowledge_Engine::Eval_Settings (false, false, false, false, false, true));
  
  
  std::cerr << "Test 8: Printing .invisible value\n";
  knowledge.print (".invisible = {.invisible}\n");
  knowledge.save_as_karl ("test4_bigger.karl");
  knowledge.save_checkpoint ("test4_bigger.kkb", "Invisible Var Context");
  
  std::cerr << "Test 8: Loading checkpoint\n";
  knowledge.clear ();
  knowledge.load_context ("test4_bigger.kkb", true);
  
  std::cerr << "Test 8: Printing .invisible value (should be 'tee hee')\n";
  knowledge.print (".invisible = {.invisible}\n");
  std::cerr << "Test 8: Results were ";
  if (knowledge.get ("int_var") == Madara::Knowledge_Record::Integer (15)
      && knowledge.get ("double_var") == 3.14159
      && knowledge.get ("str_var") == "some string"
      && knowledge.get ("int_array").to_string (", ") == "10, 20, 30"
      && knowledge.get ("double_array").to_string (", ") ==
          "10.1, 20.2, 30.3, 40.4"
      && knowledge.get ("file_var").size () > 500000
      && knowledge.get ("extra_var") == 5.0
      && knowledge.get ("additional_var") == 6.0
      && knowledge.get (".invisible") == "tee hee")
  {
    std::cerr << "SUCCESS\n\n";
  }
  else
    std::cerr << "FAIL\n\n";
  
}

void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);
    if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        int level;
        std::stringstream buffer (argv[i + 1]);
        buffer >> level;
        logger::global_logger->set_level (level);
      }

      ++i;
    }
    else
    {
      logger::global_logger->log (logger::LOG_ALWAYS,
        "\nProgram summary for %s:\n\n" \
        "  Test the checkpointing functionality.\n\n" \
        " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
        "\n",
        argv[0]);
      exit (0);
    }
  }
}

int main (int argc, char * argv[])
{
  handle_arguments (argc, argv);

  // buffer for holding encoding results
  char buffer[BUFFER_SIZE];
  int64_t buffer_remaining;
  std::string key;
  bool header_decoded;
  
  std::cerr <<
    "\n*********** TESTING ENCODING FILE HEADER TO FILE *************.\n";


  // message headers for encoding and decoding
  Madara::Knowledge_Engine::File_Header source_header;
  Madara::Knowledge_Engine::File_Header dest_header;

  // Test 1: create the source header
  memset (buffer, 0, BUFFER_SIZE);
  source_header.karl_version += 10;
  source_header.last_timestamp = source_header.initial_timestamp + 100;
  source_header.states += 50;
  source_header.size += 50 * 32;
  buffer_remaining = BUFFER_SIZE;
  strncpy (source_header.file_type, "KaRL1.0\0", 8);
  strncpy (source_header.originator, "localhost:34000\0", 16);
  
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
  
  std::cerr << "Test 3: decoded dest_header from buffer. Bytes remaining=" << 
    buffer_remaining << std::endl;
  
  header_decoded = dest_header.equals (source_header);

  std::cerr << "Test 4: decoded dest_header is equal to source_header? " <<
    (header_decoded ? "true" : "false") << std::endl;
  
  std::cerr << "\nEncoding results were " << 
    (header_decoded ?
    "SUCCESS\n" : "FAIL\n");
  
  test_checkpointing ();



  return 0;
}
