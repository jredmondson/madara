#include <iostream>

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeRecordFilters.h"
#include "madara/filters/DynamicPredicateFilter.h"
#include "madara/filters/DynamicPrefixFilter.h"
#include "madara/filters/DynamicPrefixIntConvert.h"
#include "madara/filters/DynamicPrefixPrint.h"
#include "madara/filters/PrefixPrint.h"
#include "madara/filters/PrefixIntConvert.h"
#include "madara/filters/FragmentsToFilesFilter.h"
#include "madara/filters/VariableMapFilter.h"
#include "madara/utility/Utility.h"
#include "madara/knowledge/FileFragmenter.h"
#include "madara/knowledge/FileRequester.h"

namespace knowledge = madara::knowledge;
namespace filters = madara::filters;
namespace transport = madara::transport;
namespace logger = madara::logger;
namespace utility = madara::utility;

typedef knowledge::KnowledgeRecord KnowledgeRecord;

int madara_fails = 0;

/**
 * Function that decrements integers or doubles and returns the results
 * and strips any other value entirely (returns 0)
 **/
madara::knowledge::KnowledgeRecord decrement_primitives(
    madara::knowledge::FunctionArguments& args, madara::knowledge::Variables&)
{
  madara::knowledge::KnowledgeRecord result;

  if(args.size() > 0)
  {
    if(args[0].type() == madara::knowledge::KnowledgeRecord::INTEGER ||
        args[0].type() == madara::knowledge::KnowledgeRecord::DOUBLE)
    {
      result = --args[0];
    }
  }

  return result;
}

void test_fragments_to_files_filter(void)
{
  std::cerr << "Testing fragments to files filter and reconstruction...\n";

  std::string filename =
      "$(MADARA_ROOT)/tests/images/manaus_hotel_900x1500.jpg";
  filename = utility::expand_envs(filename);

  uint32_t crc = utility::file_crc(filename);

  knowledge::KnowledgeBase kb;
  madara::knowledge::Variables vars(&kb.get_context());
  knowledge::FileFragmenter fragmenter(filename);
  fragmenter.create_vector(
      "agent.0.sandbox.files.file.images/manaus.jpg.contents", kb);
  kb.set("agent.0.sandbox.files.file.images/manaus.jpg.size",
      utility::file_size(filename));
  kb.set("agent.0.sandbox.files.file.images/manaus.jpg.crc", crc);
  kb.set("a0", 1);
  kb.set("b2", 2);

  knowledge::KnowledgeMap args = kb.to_map("");
  knowledge::KnowledgeMap args_copy = args;
  knowledge::KnowledgeMap args_less_1 = args;

  args_less_1.erase("agent.0.sandbox.files.file.images/manaus.jpg.contents.1");

  knowledge::KnowledgeMap args_less_1_copy = args_less_1;

  // std::cerr << "  crc: " << crc << "\n";
  // std::cerr << "  fragments: " << crc << "\n";
  // for(auto entry : args)
  // {
  //   std::cerr << "    " << entry.first << "=" << entry.second << "\n";
  // }

  filters::FragmentsToFilesFilter filter;
  transport::TransportContext context;

  filter.set_dir_mapping("agent.0.sandbox.files.file", "files");

  filter.filter(args, context, vars);

  std::cerr << "  Testing FragmentsToFilesFilter::filter... ";

  if(utility::file_exists("files/images/manaus.jpg"))
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }

  knowledge::FileRequester requester;
  requester.init("agent.0.sandbox.files.file.images/manaus.jpg",
      "agent.0.sync.sandbox.files.file.images/manaus.jpg",
      "files/images/manaus.jpg", kb);

  requester.clear_fragments();

  remove("files/images/manaus.jpg");

  filter.filter(args_less_1, context, vars);

  std::cerr << "  Testing FileRequester with 1 less fragment... ";

  if(requester.needs_request())
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL. Fragments should be size 1 but is "
              << requester.build_fragment_request().size() << "\n";

    ++madara_fails;
  }

  requester.clear_fragments();

  args = args_copy;

  filter.filter(args, context, vars);

  std::cerr << "  Testing FileRequester with all fragments... ";

  if(!requester.needs_request())
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL. Fragments should be size 0 but is "
              << requester.build_fragment_request().size() << "\n";

    ++madara_fails;
  }

  requester.clear_fragments();

  remove("files/images/manaus.jpg");

  std::cerr << "  Testing build_fragment_request with 0 fragments... ";

  if(requester.build_fragment_request().size() == 9)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL. Fragments should be size 9 but is "
              << requester.build_fragment_request().size() << "\n";

    ++madara_fails;
  }

  std::cerr << "  Testing needs_request with 0 fragments... ";

  if(requester.needs_request())
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL. Fragments should be size 9 but is "
              << requester.build_fragment_request().size() << "\n";

    ++madara_fails;
  }

  requester.max_fragments = 4;

  std::cerr << "  Testing build_fragment_request with max_fragments... ";

  if(requester.build_fragment_request().size() == 4)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL. Fragments should be size 4 but is "
              << requester.build_fragment_request().size() << "\n";

    ++madara_fails;
  }

  std::cerr << "  Testing needs_request with max_fragments... ";

  if(requester.needs_request())
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL. Fragments should be size 4 but is "
              << requester.build_fragment_request().size() << "\n";

    ++madara_fails;
  }
}
void test_dynamic_predicate_filter(void)
{
  madara::knowledge::KnowledgeBase kb;
  madara::knowledge::Variables vars(&kb.get_context());

  // create allowed predicates for agent.0 and agent.1
  kb.evaluate("predicates.allowed.size=2;"
              "predicates.allowed.0='agent.1';"
              "predicates.allowed.1='agent.3*'");

  filters::DynamicPredicateFilter filter;

  knowledge::KnowledgeMap map;
  transport::TransportContext context;

  map["agent.0.field1"] = KnowledgeRecord(1);
  map["agent.0.field2"] = KnowledgeRecord(2);
  map["agent.1"] = KnowledgeRecord(1);
  map["agent.1.field1"] = KnowledgeRecord(1);
  map["agent.1.field2"] = KnowledgeRecord(2);
  map["agent.2.field1"] = KnowledgeRecord(1);
  map["agent.2.field2"] = KnowledgeRecord(2);
  map["agent.3.field1"] = KnowledgeRecord(1);
  map["agent.3.field2"] = KnowledgeRecord(2);

  std::cerr << "Testing dynamic predicates filter: ";

  filter.filter(map, context, vars);

  if(map.size() == 3)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    std::cerr << map.size() << " elements (should be 3):\n";
    for(auto element : map)
    {
      std::cerr << "  " << element.first << "\n";
    }
    ++madara_fails;
  }
}

void test_dynamic_prefix_filter(void)
{
  madara::knowledge::KnowledgeBase kb;
  madara::knowledge::Variables vars(&kb.get_context());

  // create allowed prefixes for agent.0 and agent.1
  kb.evaluate("prefixes.allowed.size=2;"
              "prefixes.allowed.0='agent.1';"
              "prefixes.allowed.1='agent.3'");

  filters::DynamicPrefixFilter filter;

  knowledge::KnowledgeMap map;
  transport::TransportContext context;

  map["agent.0.field1"] = KnowledgeRecord(1);
  map["agent.0.field2"] = KnowledgeRecord(2);
  map["agent.1.field1"] = KnowledgeRecord(1);
  map["agent.1.field2"] = KnowledgeRecord(2);
  map["agent.2.field1"] = KnowledgeRecord(1);
  map["agent.2.field2"] = KnowledgeRecord(2);
  map["agent.3.field1"] = KnowledgeRecord(1);
  map["agent.3.field2"] = KnowledgeRecord(2);

  std::cerr << "Testing dynamic prefix filter: ";

  filter.filter(map, context, vars);

  if(map.size() == 4)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";
    ++madara_fails;
  }
}

void test_dynamic_prefix_int_convert(void)
{
  madara::knowledge::KnowledgeBase kb;
  madara::knowledge::Variables vars(&kb.get_context());

  // create allowed prefixes for agent.0 and agent.1
  kb.evaluate(".prefixes.size=2;"
              ".prefixes.0='agent.1';"
              ".prefixes.1='agent.3'");

  filters::DynamicPrefixIntConvert filter;

  knowledge::KnowledgeMap map;
  transport::TransportContext context;

  map["agent.0.field1"] = "field1";
  map["agent.0.field2"] = "field2";
  ;
  map["agent.1.field1"] = "field1";
  map["agent.1.field2"] = "field2";
  map["agent.2.field1"] = "field1";
  map["agent.2.field2"] = "field2";
  map["agent.3.field1"] = "field1";
  map["agent.3.field2"] = "field2";

  std::cerr << "Testing dynamic prefix int convert: ";

  filter.filter(map, context, vars);

  if(map.size() == 8 && map["agent.0.field1"] == "field1" &&
      map["agent.0.field2"] == "field2" && map["agent.2.field1"] == "field1" &&
      map["agent.2.field2"] == "field2" && map["agent.1.field1"] == 0 &&
      map["agent.1.field2"] == 0 && map["agent.3.field1"] == 0 &&
      map["agent.3.field2"] == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";

    for(auto entry : map)
    {
      std::cerr << "  " << entry.first << "=" << entry.second << "\n";
    }

    ++madara_fails;
  }
}

void test_prefix_int_convert(void)
{
  madara::knowledge::KnowledgeBase kb;
  madara::knowledge::Variables vars(&kb.get_context());

  // create allowed prefixes for agent.0 and agent.1
  std::vector<std::string> prefixes;
  prefixes.push_back("agent.1");
  prefixes.push_back("agent.3");

  filters::PrefixIntConvert filter;

  knowledge::KnowledgeMap map;
  transport::TransportContext context;

  map["agent.0.field1"] = "field1";
  map["agent.0.field2"] = "field2";
  ;
  map["agent.1.field1"] = "field1";
  map["agent.1.field2"] = "field2";
  map["agent.2.field1"] = "field1";
  map["agent.2.field2"] = "field2";
  map["agent.3.field1"] = "field1";
  map["agent.3.field2"] = "field2";

  std::cerr << "Testing dynamic prefix int convert: ";

  filter.filter(map, context, vars);

  if(map.size() == 8 && map["agent.0.field1"] == "field1" &&
      map["agent.0.field2"] == "field2" && map["agent.2.field1"] == "field1" &&
      map["agent.2.field2"] == "field2" && map["agent.1.field1"] == 0 &&
      map["agent.1.field2"] == 0 && map["agent.3.field1"] == 0 &&
      map["agent.3.field2"] == 0)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";

    for(auto entry : map)
    {
      std::cerr << "  " << entry.first << "=" << entry.second << "\n";
    }

    ++madara_fails;
  }
}

void test_print_filter_compile(void)
{
  filters::PrefixPrint static_printer;
  filters::DynamicPrefixPrint dynamic_printer;
}

void test_variable_map_filter(void)
{
  madara::knowledge::KnowledgeBase kb;
  madara::knowledge::Variables vars(&kb.get_context());

  // create allowed prefixes for agent.0 and agent.1
  kb.set(".var1", 1);
  kb.set(".var2", 2);
  kb.set(".var3", 3);
  kb.set(".var4", 4);

  filters::VariableMapFilter filter;
  filter.process_config(".var1 -> agent.0.field1\n"
                        ".var2 -> agent.0.field2\n"
                        ".var3 -> agent.1.field1\n"
                        ".var4 -> agent.1.field2\n");

  knowledge::KnowledgeMap map;
  transport::TransportContext context;

  map["agent.2.field1"] = 5;
  map["agent.2.field2"] = 6;
  map["agent.3.field1"] = 7;
  map["agent.3.field2"] = 8;

  std::cerr << "Testing variable map filter: ";

  filter.filter(map, context, vars);

  if(map.size() == 8 && map["agent.0.field1"] == 1 &&
      map["agent.0.field2"] == 2 && map["agent.1.field1"] == 3 &&
      map["agent.1.field2"] == 4 && map["agent.2.field1"] == 5 &&
      map["agent.2.field2"] == 6 && map["agent.3.field1"] == 7 &&
      map["agent.3.field2"] == 8)
  {
    std::cerr << "SUCCESS\n";
  }
  else
  {
    std::cerr << "FAIL\n";

    for(auto entry : map)
    {
      std::cerr << "  " << entry.first << "=" << entry.second << "\n";
    }

    ++madara_fails;
  }
}

int main(int, char**)
{
  test_dynamic_predicate_filter();
  test_dynamic_prefix_filter();
  test_dynamic_prefix_int_convert();
  test_prefix_int_convert();
  test_print_filter_compile();
  test_variable_map_filter();
  test_fragments_to_files_filter();

  madara::knowledge::KnowledgeRecordFilters filters;

  std::cerr
      << "Adding decrement_primitives to integer, double, and jpeg types\n";

  filters.add(madara::knowledge::KnowledgeRecord::INTEGER |
                  madara::knowledge::KnowledgeRecord::DOUBLE |
                  madara::knowledge::KnowledgeRecord::IMAGE_JPEG,
      decrement_primitives);

  filters.print_num_filters();

  std::cerr << "Clearing filters from jpeg type\n";

  filters.clear(madara::knowledge::KnowledgeRecord::IMAGE_JPEG);

  filters.print_num_filters();

  std::cerr << "Clearing filters from all types\n";

  filters.clear(madara::knowledge::KnowledgeRecord::ALL_TYPES);

  filters.print_num_filters();

  std::cerr << "Adding a 0 function to all types (should not add)\n";

  filters.add(madara::knowledge::KnowledgeRecord::INTEGER |
                  madara::knowledge::KnowledgeRecord::DOUBLE |
                  madara::knowledge::KnowledgeRecord::IMAGE_JPEG,
      decrement_primitives);

  filters.print_num_filters();

  std::cerr << "Adding a decrement_primitives to integer and double types\n";

  filters.add(madara::knowledge::KnowledgeRecord::INTEGER |
                  madara::knowledge::KnowledgeRecord::DOUBLE,
      decrement_primitives);

  std::cerr << "Adding a 2nd decrement_primitives to integer type\n";

  filters.add(
      madara::knowledge::KnowledgeRecord::INTEGER, decrement_primitives);

  filters.print_num_filters();

  madara::knowledge::KnowledgeRecord integer_record(
      madara::knowledge::KnowledgeRecord::Integer(15));
  madara::knowledge::KnowledgeRecord double_record(17.0);
  madara::knowledge::KnowledgeRecord string_record(
      "This string should not change.");

  madara::transport::TransportContext transport_context;

  // Filter the records;
  madara::knowledge::KnowledgeRecord integer_result =
      filters.filter(integer_record, "", transport_context);
  madara::knowledge::KnowledgeRecord double_result =
      filters.filter(double_record, "", transport_context);
  madara::knowledge::KnowledgeRecord string_result =
      filters.filter(string_record, "", transport_context);

  std::cerr << "The result of the filtering was the following:\n";
  std::cerr << "  integer result = " << integer_result << " (";
  if(integer_result == madara::knowledge::KnowledgeRecord::Integer(12))
    std::cerr << "SUCCESS)\n";
  else
  {
    std::cerr << "FAILURE)\n";
    ++madara_fails;
  }

  std::cerr << "  double result = " << double_result << " (";
  if(double_result == 15.0)
    std::cerr << "SUCCESS)\n";
  else
  {
    std::cerr << "FAILURE)\n";
    ++madara_fails;
  }

  std::cerr << "  string result = " << string_result << " (";
  if(string_result == "This string should not change.")
    std::cerr << "SUCCESS)\n";
  else
  {
    std::cerr << "FAILURE)\n";
    ++madara_fails;
  }

  std::cerr
      << "Adding a decrement_primitives to string type (deletes string)\n";

  filters.add(madara::knowledge::KnowledgeRecord::STRING, decrement_primitives);

  string_result = filters.filter(string_record, "", transport_context);

  std::cerr << "Running filter on string type (should delete string)\n";

  std::cerr << "  string result = " << string_result << " (";
  if(string_result == madara::knowledge::KnowledgeRecord::Integer(0))
    std::cerr << "SUCCESS)\n";
  else
  {
    std::cerr << "FAILURE)\n";
    ++madara_fails;
  }

  if(madara_fails > 0)
  {
    std::cerr << "OVERALL: FAIL. " << madara_fails << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return madara_fails;
}
