

#include "madara/knowledge/FileHeader.h"
#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/logger/GlobalLogger.h"

#include "madara/utility/Utility.h"

#include "madara/filters/ssl/AESBufferFilter.h"
#include "madara/filters/lz4/LZ4BufferFilter.h"
#include "madara/knowledge/containers/Integer.h"
#include "madara/exceptions/MemoryException.h"
#include "madara/exceptions/FilterException.h"
#include "madara/knowledge/CheckpointStreamer.h"
#include "madara/knowledge/CheckpointPlayer.h"
#include "madara/knowledge/Any.h"

#ifdef _USE_CAPNP_
#include "capnfiles/Geo.capnp.h"
#endif // _USE_CAPNP_

#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <string.h>

namespace logger = madara::logger;
namespace knowledge = madara::knowledge;
namespace filters = madara::filters;
namespace utility = madara::utility;
namespace containers = knowledge::containers;

#define BUFFER_SIZE 1000

int madara_fails = 0;

int log_level = 2;

void test_checkpoint_settings(void)
{
  std::cerr << "\n*********** TEST CHECKPOINT SETTINGS *************.\n";

  knowledge::KnowledgeRecord::set_precision(1);

  knowledge::CheckpointSettings settings;
  knowledge::KnowledgeBase saver, loader;

  saver.set("int_var", knowledge::KnowledgeRecord::Integer(15));
  saver.set("str_var", std::string("some string"));
  saver.set("double_var", 3.14159);

  std::vector<knowledge::KnowledgeRecord::Integer> integers;
  std::vector<double> doubles;
  std::string id = "orig_context";

  integers.push_back(10);
  integers.push_back(20);
  integers.push_back(30);

  doubles.push_back(10.1);
  doubles.push_back(20.2);
  doubles.push_back(30.3);
  doubles.push_back(40.4);

  saver.set("int_array", integers);
  saver.set("double_array", doubles);

#ifndef _MADARA_NO_KARL_

  // set the system lamport clock to something arbitrary
  saver.evaluate("#set_clock (2001)");

#endif // end karl support

  settings.filename = "test_context_save_1.kb";
  settings.clear_knowledge = true;

  saver.save_context(settings);

  loader.load_context(settings);

  std::cerr << "Test 1: Loading a context and checking values: ";
  if (loader.get("int_var") ==
          madara::knowledge::KnowledgeRecord::Integer(15) &&
      loader.get("double_var") == 3.14159 &&
      loader.get("str_var") == "some string" &&
      loader.get("int_array").to_string(", ") == "10, 20, 30" &&
      loader.get("double_array").to_string(", ") == "10.1, 20.2, 30.3, 40.4")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL. Knowledge was:\n";
    ++madara_fails;
    loader.print();
  }

  std::cerr << "Test 2: Checking expected settings with last load: ";
  if (settings.initial_lamport_clock == 2001 &&
      settings.last_lamport_clock == 2001)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "  Expected clocks (2001:2001) but got ("
              << settings.initial_lamport_clock << ":"
              << settings.last_lamport_clock << ")\n";
  }

  settings.prefixes.push_back("int");
  settings.initial_timestamp = 1234;
  settings.last_timestamp = 4321;
  settings.override_timestamp = true;
  settings.override_lamport = true;
  settings.initial_lamport_clock = 7;
  settings.last_lamport_clock = 7;
  settings.filename = "test_context_save_2.kb";

  // logger::global_logger->set_level (logger::LOG_MINOR);

  saver.save_context(settings);

  loader.load_context(settings);

  std::cerr << "Test 3: Loading and checking for settings restrictions: ";
  if (loader.get("int_var") ==
          madara::knowledge::KnowledgeRecord::Integer(15) &&
      loader.get("double_var").is_false() && loader.get("str_var").is_false() &&
      loader.get("int_array").to_string(", ") == "10, 20, 30" &&
      loader.get("double_array").is_false() &&
      loader.get("file_var").is_false() && loader.get("extra_var").is_false() &&
      loader.get("additional_var").is_false() &&
      loader.get(".invisible").is_false())
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL. Knowledge was:\n";
    ++madara_fails;
    loader.print();
  }

  std::cerr << "Test 4: Checking expected settings with last load: ";
  if (settings.initial_lamport_clock == 7 && settings.last_lamport_clock == 7 &&
      settings.initial_timestamp == 1234 && settings.last_timestamp == 4321)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
    std::cerr << "  Expected clocks (7:7) but got ("
              << settings.initial_lamport_clock << ":"
              << settings.last_lamport_clock << ")\n";
    std::cerr << "  Expected timestamps (1234:4321) but got ("
              << settings.initial_timestamp << ":" << settings.last_timestamp
              << ")\n";
  }

#ifdef _USE_SSL_

  filters::AESBufferFilter filter;
  filter.generate_key("testPassword#tofile359");

  std::string test_message = "my test message";
  char buffer[200000];
  buffer[63] = 0;

  // copy current message into the buffer
  strncpy((char*)buffer, test_message.c_str(), test_message.size() + 1);

  int encode_length(0), decode_length(0);

  std::cerr << "Test 5.1: buffer = : " << buffer << "\n";

  encode_length = filter.encode(buffer, (int)test_message.size() + 1, 63);

  std::cerr << "Test 5.1: Attempting 256bit AES on simple message: ";

  if (strncmp((char*)buffer, test_message.c_str(), 63) != 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Should be garbled but is the same. " << buffer << "\n";
  }

  std::cerr << "Test 5.2: buffer = : " << buffer << "\n";

  decode_length = filter.decode(buffer, encode_length, 64);

  std::cerr << "Test 5.2: Looking at unencoded message: ";

  if (strncmp((char*)buffer, test_message.c_str(), 63) == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Should be same but is garbled. " << buffer << "\n";
  }

  std::cerr << "Test 5.3: encoding README.md: ";

  test_message =
      utility::file_to_string(utility::expand_envs("$(MADARA_ROOT)/README.md"));
  strncpy((char*)buffer, test_message.c_str(), test_message.size());

  encode_length =
      filter.encode(buffer, (int)test_message.size(), sizeof(buffer));

  utility::write_file("README_encrypted.bin", (void*)buffer, encode_length);

  test_message = utility::file_to_string("README_encrypted.bin");
  memcpy((char*)buffer, test_message.c_str(), test_message.size());

  decode_length = filter.decode(buffer, test_message.size(), sizeof(buffer));

  utility::write_file("README_decrypted.txt", (void*)buffer, decode_length);

  std::cerr << "SUCCESS\n";

  settings.filename = "test_encryption_6.kb";
  settings.buffer_filters.push_back(&filter);
  settings.prefixes.clear();

  saver.save_context(settings);

  loader.load_context(settings);

  std::cerr << "Test 6: Saving and loading with 256 bit AES: ";
  if (loader.get("int_var") ==
          madara::knowledge::KnowledgeRecord::Integer(15) &&
      loader.get("double_var") == 3.14159 &&
      loader.get("str_var") == "some string" &&
      loader.get("int_array").to_string(", ") == "10, 20, 30" &&
      loader.get("double_array").to_string(", ") == "10.1, 20.2, 30.3, 40.4")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 7: Loading without 256 bit AES filter: ";
  settings.filename = "test_encryption_7.kb";

  logger::global_logger->set_level(6);

  saver.save_context(settings);

  settings.buffer_filters.clear();

  try
  {
    loader.load_context(settings);
  }
  catch (madara::exceptions::MadaraException& e)
  {
    // std::cerr << "Caught exception\n";
    std::cerr << e.what() << "\n";
  }

  logger::global_logger->set_level(log_level);

  if (loader.get("int_var").is_false() && loader.get("double_var").is_false() &&
      loader.get("str_var").is_false() && loader.get("int_array").is_false() &&
      loader.get("double_array").is_false() &&
      loader.get("file_var").is_false() && loader.get("extra_var").is_false() &&
      loader.get("additional_var").is_false() &&
      loader.get(".invisible").is_false())
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 8: Saving with prefixes 256 bit AES filter: ";

  settings.filename = "test_encryption_8.kb";
  settings.prefixes.push_back("int");
  settings.buffer_filters.push_back(&filter);

  saver.save_context(settings);

  loader.load_context(settings);

  if (loader.get("int_var") ==
          madara::knowledge::KnowledgeRecord::Integer(15) &&
      loader.get("double_var").is_false() && loader.get("str_var").is_false() &&
      loader.get("int_array").to_string(", ") == "10, 20, 30" &&
      loader.get("double_array").is_false() &&
      loader.get("file_var").is_false() && loader.get("extra_var").is_false() &&
      loader.get("additional_var").is_false() &&
      loader.get(".invisible").is_false())
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

#endif
}

void test_checkpoints_diff(void)
{
  std::cerr << "\n*********** TEST CHECKPOINT DIFFS *************.\n";

  knowledge::KnowledgeRecord::set_precision(1);

  knowledge::VariableReferences current_modifieds;
  knowledge::CheckpointSettings checkpoint_settings;
  knowledge::KnowledgeBase saver, loader;
  knowledge::KnowledgeUpdateSettings track_changes;
  track_changes.track_local_changes = true;
  knowledge::KnowledgeUpdateSettings untrack_changes;
  untrack_changes.track_local_changes = false;

  // make sure all checkpoint containers use checkpoint tracking
  containers::Integer var1(".1", saver, track_changes),
      var2(".2", saver, track_changes), var3(".3", saver, track_changes),
      varglob1("var1", saver, track_changes),
      varglob2("var2", saver, track_changes),
      varglob3("var3", saver, track_changes),
      untracked("var4", saver, untrack_changes);

  // reset the checkpoint on save and on load, clear knowledge
  checkpoint_settings.clear_knowledge = true;
  checkpoint_settings.reset_checkpoint = true;
  checkpoint_settings.filename = "test_checkpoints1.kb";

  std::cerr << "Test 1: Saving .1 and var1: ";

  // modify the two vars with the track_changes update settings
  var1 = 1;
  varglob1 = 1;
  untracked = 4;

  // logger::global_logger->set_level (logger::LOG_MINOR);

  saver.save_checkpoint(checkpoint_settings);

  loader.load_context(checkpoint_settings);

  if (loader.get("var1").to_integer() == 1 &&
      loader.get(".1").to_integer() == 1 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 2: Modify var1 and reload: ";

  varglob1 = 2;
  untracked = 4;

  saver.save_checkpoint(checkpoint_settings);

  loader.load_context(checkpoint_settings);

  if (loader.get("var1").to_integer() == 2 &&
      loader.get(".1").to_integer() == 1 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 3: Change all values and save to new file: ";

  checkpoint_settings.filename = "test_checkpoints2.kb";
  var1 = 1;
  varglob1 = 1;
  var2 = 2;
  varglob2 = 2;
  var3 = 3;
  varglob3 = 3;
  untracked = 4;

  saver.save_checkpoint(checkpoint_settings);

  loader.load_context(checkpoint_settings);

  current_modifieds = saver.save_modifieds();

  if (loader.get("var1").to_integer() == 1 &&
      loader.get(".1").to_integer() == 1 &&
      loader.get("var2").to_integer() == 2 &&
      loader.get(".2").to_integer() == 2 &&
      loader.get("var3").to_integer() == 3 &&
      loader.get(".3").to_integer() == 3 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 4: Increment a variable to each checkpoint: ";

  checkpoint_settings.filename = "test_checkpoints_stack.kb";

  var1 = 1;

  saver.save_checkpoint(checkpoint_settings);

  var1 = 2;

  saver.save_checkpoint(checkpoint_settings);

  var1 = 3;
  varglob1 = 3;

  saver.save_checkpoint(checkpoint_settings);

  var1 = 4;
  varglob1 = 4;

  saver.save_checkpoint(checkpoint_settings);

  var1 = 5;
  varglob1 = 5;

  saver.save_checkpoint(checkpoint_settings);

  var1 = 6;
  varglob1 = 0;

  saver.save_checkpoint(checkpoint_settings);

  loader.load_context(checkpoint_settings);

  if (loader.get(".1").to_integer() == 6 &&
      loader.get("var1").to_integer() == 0 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 5: Loading range 0-0 from stack: ";

  checkpoint_settings.initial_state = 0;
  checkpoint_settings.last_state = 0;

  loader.load_context(checkpoint_settings);

  if (loader.get(".1").to_integer() == 1 && !loader.exists("var1") &&
      !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 6: Loading range 2-2 from stack: ";

  checkpoint_settings.initial_state = 2;
  checkpoint_settings.last_state = 2;

  loader.load_context(checkpoint_settings);

  if (loader.get(".1").to_integer() == 3 &&
      loader.get("var1").to_integer() == 3 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 7: Loading range 2-4 from stack: ";

  checkpoint_settings.initial_state = 2;
  checkpoint_settings.last_state = 4;

  loader.load_context(checkpoint_settings);

  if (loader.get(".1").to_integer() == 5 &&
      loader.get("var1").to_integer() == 5 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 8: Loading range 2-5 from stack: ";

  checkpoint_settings.initial_state = 2;
  checkpoint_settings.last_state = 5;

  loader.load_context(checkpoint_settings);

  logger::global_logger->set_level(logger::LOG_ERROR);

  if (loader.get(".1").to_integer() == 6 &&
      loader.get("var1").to_integer() == 0 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  logger::global_logger->set_level(log_level);
}

void test_compress(void)
{
#ifdef _USE_LZ4_

  filters::LZ4BufferFilter lz4;

  knowledge::CheckpointSettings settings;
  settings.buffer_size = 10000000;
  settings.filename = "buffer_size_test_3.kb";

  std::cerr << "loading from buffer_size_test_3.kb\n";

  knowledge::KnowledgeBase kb;

  kb.load_context(settings);

  std::cerr << "saving to buffer_size_test_3.kb.lz4\n";

  settings.filename = "buffer_size_test_3.kb.lz4";
  settings.buffer_filters.push_back(&lz4);

  kb.save_context(settings);

  std::cerr << "loading from buffer_size_test_3.kb.lz4: ";

  settings.filename = "buffer_size_test_3.kb.lz4";

  kb.load_context(settings);

  if (kb.get("data").size() == 2000000)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    kb.print();
  }
#endif
}

void test_diff_filter_chains(void)
{
  std::cerr << "\n*********** TEST CHECKPOINT DIFF CHAINS *************.\n";

  knowledge::KnowledgeRecord::set_precision(1);

  knowledge::VariableReferences current_modifieds;
  knowledge::CheckpointSettings load_settings, save_settings;
  knowledge::KnowledgeBase saver, loader;
  knowledge::KnowledgeUpdateSettings track_changes;
  track_changes.track_local_changes = true;
  knowledge::KnowledgeUpdateSettings untrack_changes;
  untrack_changes.track_local_changes = false;

#ifdef _USE_LZ4_
  filters::LZ4BufferFilter lz4_save_filter;
  filters::LZ4BufferFilter lz4_load_filter;
  load_settings.buffer_filters.push_back(&lz4_load_filter);
  save_settings.buffer_filters.push_back(&lz4_save_filter);
#endif

#ifdef _USE_SSL_
  filters::AESBufferFilter ssl_save_filter;
  ssl_save_filter.generate_key("temppass");
  filters::AESBufferFilter ssl_load_filter;
  ssl_load_filter.generate_key("temppass");
  load_settings.buffer_filters.push_back(&ssl_load_filter);
  save_settings.buffer_filters.push_back(&ssl_save_filter);
#endif

  // make sure all checkpoint containers use checkpoint tracking
  containers::Integer var1(".1", saver, track_changes),
      var2(".2", saver, track_changes), var3(".3", saver, track_changes),
      varglob1("var1", saver, track_changes),
      varglob2("var2", saver, track_changes),
      varglob3("var3", saver, track_changes),
      untracked("var4", saver, untrack_changes);

  // reset the checkpoint on save and on load, clear knowledge
  load_settings.clear_knowledge = true;
  save_settings.reset_checkpoint = true;
  load_settings.filename = save_settings.filename = "checkpoints1.stk";

  std::cerr << "Test 1: Saving .1 and var1: ";

  // modify the two vars with the track_changes update settings
  var1 = 1;
  varglob1 = 1;
  untracked = 4;

  // logger::global_logger->set_level (logger::LOG_MINOR);

  saver.save_checkpoint(save_settings);

  loader.load_context(load_settings);

  if (loader.get("var1").to_integer() == 1 &&
      loader.get(".1").to_integer() == 1 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 2: Modify var1 and reload: ";

  varglob1 = 2;
  untracked = 4;

  saver.save_checkpoint(save_settings);

  loader.load_context(load_settings);

  if (loader.get("var1").to_integer() == 2 &&
      loader.get(".1").to_integer() == 1 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 3: Change all values and save to new file: ";

  load_settings.filename = save_settings.filename = "checkpoints2.stk";
  var1 = 1;
  varglob1 = 1;
  var2 = 2;
  varglob2 = 2;
  var3 = 3;
  varglob3 = 3;
  untracked = 4;

  saver.save_checkpoint(save_settings);

  loader.load_context(load_settings);

  current_modifieds = saver.save_modifieds();

  if (loader.get("var1").to_integer() == 1 &&
      loader.get(".1").to_integer() == 1 &&
      loader.get("var2").to_integer() == 2 &&
      loader.get(".2").to_integer() == 2 &&
      loader.get("var3").to_integer() == 3 &&
      loader.get(".3").to_integer() == 3 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 4: Increment a variable to each checkpoint: ";

  load_settings.filename = save_settings.filename = "checkpoints.stk";

  var1 = 1;

  saver.save_checkpoint(save_settings);

  var1 = 2;

  saver.save_checkpoint(save_settings);

  var1 = 3;
  varglob1 = 3;

  saver.save_checkpoint(save_settings);

  var1 = 4;
  varglob1 = 4;

  saver.save_checkpoint(save_settings);

  var1 = 5;
  varglob1 = 5;

  saver.save_checkpoint(save_settings);

  var1 = 6;
  varglob1 = 0;

  saver.save_checkpoint(save_settings);

  loader.load_context(load_settings);

  if (loader.get(".1").to_integer() == 6 &&
      loader.get("var1").to_integer() == 0 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 5: Loading range 0-0 from stack: ";

  load_settings.initial_state = 0;
  load_settings.last_state = 0;

  loader.load_context(load_settings);

  if (loader.get(".1").to_integer() == 1 && !loader.exists("var1") &&
      !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 6: Loading range 2-2 from stack: ";

  load_settings.initial_state = 2;
  load_settings.last_state = 2;

  loader.load_context(load_settings);

  if (loader.get(".1").to_integer() == 3 &&
      loader.get("var1").to_integer() == 3 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 7: Loading range 2-4 from stack: ";

  load_settings.initial_state = 2;
  load_settings.last_state = 4;

  loader.load_context(load_settings);

  if (loader.get(".1").to_integer() == 5 &&
      loader.get("var1").to_integer() == 5 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  std::cerr << "Test 8: Loading range 2-5 from stack: ";

  load_settings.initial_state = 2;
  load_settings.last_state = 5;

  loader.load_context(load_settings);

  logger::global_logger->set_level(logger::LOG_ERROR);

  if (loader.get(".1").to_integer() == 6 &&
      loader.get("var1").to_integer() == 0 && !loader.exists("var4"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    ++madara_fails;
    std::cerr << "FAIL. Knowledge was:\n";
    loader.print();
  }

  logger::global_logger->set_level(log_level);
}

void handle_arguments(int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1(argv[i]);
    if (arg1 == "-l" || arg1 == "--level")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer(argv[i + 1]);
        buffer >> log_level;
        logger::global_logger->set_level(log_level);
      }

      ++i;
    }
    else
    {
      madara_logger_ptr_log(logger::global_logger.get(), logger::LOG_ALWAYS,
          "\nProgram summary for %s:\n\n"
          "  Test the checkpointing functionality.\n\n"
          " [-l|--level level]       the logger level (0+, higher is higher "
          "detail)\n"
          "\n",
          argv[0]);
      exit(0);
    }
  }
}

void test_buffer_size(void)
{
  std::cerr << "\n*********** TESTING BUFFER SIZE *************.\n";

  knowledge::KnowledgeBase kb;
  knowledge::CheckpointSettings settings;

  // create 2MB variable to push limits of context settings
  size_t data_size = 2000000;
  unsigned char* data = new unsigned char[data_size];
  kb.set_file("data", data, data_size);

  // configure initial settings
  settings.filename = "buffer_size_test_1.kb";

  std::cerr << "Saving 2MB record with " << settings.buffer_size
            << "B buffer size...\n";

  // Test 1
  std::cerr << "Test 1: save_context: ";

  try
  {
    kb.save_context(settings);
    std::cerr << "FAIL\n";
    madara_fails++;
  }
  catch (const madara::exceptions::MemoryException&)
  {
    std::cerr << "SUCCESS\n";
  }

  // Test 2
  std::cerr << "Test 2: save_checkpoint: ";
  settings.filename = "buffer_size_test_2.kb";

  kb.mark_modified("data");

  try
  {
    kb.save_checkpoint(settings);
    std::cerr << "FAIL\n";
    madara_fails++;
  }
  catch (const madara::exceptions::MemoryException&)
  {
    std::cerr << "SUCCESS\n";
  }

  // Test 3

  settings.buffer_size = 4000000;
  std::cerr << "Saving 2MB record with " << settings.buffer_size
            << "B buffer size...\n";

  std::cerr << "Test 3: save_context: ";
  settings.filename = "buffer_size_test_3.kb";

  kb.save_context(settings);

  std::cerr << "SUCCESS\n";

  // Test 4
  std::cerr << "Test 4: save_checkpoint: ";

  kb.mark_modified("data");

  kb.save_checkpoint(settings);

  std::cerr << "SUCCESS\n";
}

void test_filter_header(void)
{
  std::cerr << "\n*********** TESTING ENCODING FILTER HEADER TO FILE "
               "*************.\n";

  knowledge::KnowledgeBase kb;
  knowledge::CheckpointSettings settings;
  char* buffer = new char[1000000];

  std::string story =
      "Three blind mice accidentally walked into a cat bar.\n"
      "The first mouse went up to the bartender and started\n"
      "to order a drink, but before he could say anything, the\n"
      "bartender, who was a cat, ate him. The second mouse, who\n"
      "was also deaf, walked up to the bar and was promptly also eaten\n"
      "by cats on nearby stools. The third mouse took off his shades,\n"
      "looked into the camera, at the person reading this, and\n"
      "said 'What are you expecting? A joke? This is a test for\n"
      "checkpointing. My two friends would still be alive if you\n"
      "would have stopped reading at the 3rd line. You're a monster!'\n"
      "#schrodinger_mouse_2018.\n";

  kb.set("story", story);

#ifdef _USE_LZ4_
  filters::LZ4BufferFilter lz4_filter;
  settings.buffer_filters.push_back(&lz4_filter);
#endif

#ifdef _USE_SSL_
  filters::AESBufferFilter ssl_filter;
  ssl_filter.generate_key("#schrodinger_mouse_2018");
  settings.buffer_filters.push_back(&ssl_filter);
#endif

#ifdef _USE_LZ4_
  settings.buffer_filters.push_back(&lz4_filter);
#endif

  memcpy(buffer, story.c_str(), story.size());

  std::cerr
      << "  Testing encode and then decode with existing buffer filters...";

  int init_size = (int)story.size();
  logger::global_logger->set_level(4);
  int encoded = settings.encode(buffer, init_size, 1000000);
  int decoded = settings.decode(buffer, encoded, 1000000);
  logger::global_logger->set_level(log_level);

  std::cerr << " init=" << init_size << "encoded=" << encoded
            << " decoded=" << decoded << ": ";

  if (decoded == init_size)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
  }
  buffer[decoded] = 0;

  std::cerr << (char*)(buffer) << "\n";
}

template<typename I = int, typename Iter>
bool is_sequential(Iter first, Iter last, I counter = 0)
{
  while (first != last)
  {
    if (*first != counter)
    {
      return false;
    }
    ++counter;
    ++first;
  }
  return true;
}

void test_streaming()
{
  std::cerr << "\n*********** TESTING STREAMING *************.\n";

  std::cerr << "Attempting to stream to STK file and read back..." << std::endl;

  using madara::knowledge::Any;
#ifdef _USE_CAPNP_
  using madara::knowledge::CapnObject;

  Any::register_type<CapnObject<geo_capn::Point>>("capn_point");

  Any::construct("capn_point");
#endif

  knowledge::CheckpointSettings settings;
  settings.filename = "stream_test.stk";

  knowledge::KnowledgeBase kb;
  kb.attach_streamer(
      utility::mk_unique<knowledge::CheckpointStreamer>(settings, kb));

  utility::sleep(0.5);

  kb.set(".loc1", 2);
  kb.set(".loc2", "foo");

#ifdef _USE_CAPNP_
  capnp::MallocMessageBuilder msg;
  auto builder = msg.initRoot<geo_capn::Point>();
  builder.setX(3);
  builder.setY(6);
  builder.setZ(9);
  kb.emplace_any(".any0", madara::type<CapnObject<geo_capn::Point>>{}, msg);
#endif

  kb.set_history_capacity("hist", 10);
  kb.set("hist", 1);
  kb.set("hist", 2);
  kb.set("hist", 3);

  utility::sleep(0.5);

  kb.set("glob1", 4);
  kb.set("glob2", "bar");
  kb.set("hist", 4);
  kb.set("hist", 5);
  kb.set("hist", 6);

  utility::sleep(0.5);

  kb.attach_streamer(nullptr);

  auto prehist = kb.get_history("hist");
  if (kb.get(".loc1") == 2 && kb.get(".loc2") == "foo" &&
      kb.get("glob1") == 4 && kb.get("glob2") == "bar" && prehist.size() == 6 &&
      is_sequential(prehist.begin(), prehist.end(), 1))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::string dump;
    kb.to_string(dump);
    std::cerr << "FAIL before load:\n" << dump << "\n";
    std::cerr << "    hist:";
    for (const auto& cur : prehist)
    {
      std::cerr << " " << cur;
    }
    std::cerr << std::endl;
    madara_fails++;
    // return;
  }

  knowledge::KnowledgeBase kb2;

  kb2.set_history_capacity("hist", 10);

  kb2.load_context(settings);

  auto hist = kb2.get_history("hist");
  if (kb2.get(".loc1") == 2 && kb2.get(".loc2") == "foo" &&
      kb2.get("glob1") == 4 && kb2.get("glob2") == "bar")
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::string dump;
    kb2.to_string(dump);
    std::cerr << "FAIL :\n" << dump << "\n";
    madara_fails++;
  }

  knowledge::KnowledgeBase kb3;

  kb3.set_history_capacity("hist", 10);

  settings.playback_simtime = true;

  {
    knowledge::CheckpointReader reader(settings);
    for (;;)
    {
      auto next = reader.next();
      if (next.first == "")
      {
        break;
      }

      std::cerr << next.first << ": " << next.second << std::endl;
    }
  }

  knowledge::CheckpointReader reader(settings);
  reader.start();  // Load checkpoint metadata

  // Get time of first checkpoint operation
  uint64_t init_toi = reader.get_checkpoint_settings().initial_timestamp;
  uint64_t final_toi = reader.get_checkpoint_settings().last_timestamp;
  (void)init_toi;
  (void)final_toi;
  knowledge::CheckpointPlayer player(kb3.get_context(), std::move(reader));

  player.play_until(0);  // Stops after first loaded update

  player.start();

  for (int i = 0; i < 10; ++i)
  {
    if (i < 4)
    {
      if (kb3.exists("glob2"))
      {
        std::cerr << "FAIL : glob1 exists before it should\n";
        madara_fails++;
        return;
      }
    }
    if (i > 8)
    {
      if (kb3.get("glob2") != "bar")
      {
        std::cerr << "FAIL : glob1 has wrong value after it should be set\n";
        madara_fails++;
        return;
      }
    }
    std::string dump;
    kb3.to_string(dump);
    std::cerr << utility::get_time() << ": " << dump << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
  }

  auto posthist = kb3.get_history("hist");
  if (kb3.get(".loc1") == 2 && kb3.get(".loc2") == "foo" &&
      kb3.get("glob1") == 4 && kb3.get("glob2") == "bar" &&
      posthist.size() == 6 &&
      is_sequential(posthist.begin(), posthist.end(), 1))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::string dump;
    kb3.to_string(dump);
    std::cerr << "FAIL after playback:\n" << dump << "\n";
    std::cerr << "    hist:";
    for (const auto& cur : posthist)
    {
      std::cerr << " " << cur;
    }
    std::cerr << std::endl;
    madara_fails++;
  }
}

int main(int argc, char* argv[])
{
  handle_arguments(argc, argv);

  // buffer for holding encoding results
  char buffer[BUFFER_SIZE];
  int64_t buffer_remaining;
  std::string key;
  bool header_decoded;

  std::cerr
      << "\n*********** TESTING ENCODING FILE HEADER TO FILE *************.\n";

  // message headers for encoding and decoding
  madara::knowledge::FileHeader source_header;
  madara::knowledge::FileHeader dest_header;

  // Test 1: create the source header
  memset(buffer, 0, BUFFER_SIZE);
  source_header.karl_version += 10;
  source_header.last_timestamp = source_header.initial_timestamp + 100;
  source_header.states += 50;
  source_header.size += 50 * 32;
  buffer_remaining = BUFFER_SIZE;
  strncpy(source_header.file_type, "KaRL1.0\0", 8);
  strncpy(source_header.originator, "localhost:34000\0", 16);

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

  std::cerr << "Test 3: decoded dest_header from buffer. Bytes remaining="
            << buffer_remaining << std::endl;

  header_decoded = dest_header.equals(source_header);

  std::cerr << "Test 4: decoded dest_header is equal to source_header? "
            << (header_decoded ? "true" : "false") << std::endl;

  std::cerr << "\nEncoding results were ";
  if (header_decoded)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  test_checkpoint_settings();

  test_checkpoints_diff();

  test_buffer_size();

  test_compress();

  test_filter_header();

  test_diff_filter_chains();

  logger::global_logger->set_level(log_level);
  test_streaming();

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
