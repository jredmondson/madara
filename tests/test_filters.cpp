#include <iostream>

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeRecordFilters.h"

int num_fails = 0;

/**
 * Function that decrements integers or doubles and returns the results
 * and strips any other value entirely (returns 0)
 **/
madara::knowledge::KnowledgeRecord
  decrement_primitives (madara::knowledge::FunctionArguments & args,
  madara::knowledge::Variables &)
{
  madara::knowledge::KnowledgeRecord result;

  if (args.size () > 0)
  {
    if (args[0].type () == madara::knowledge::KnowledgeRecord::INTEGER ||
        args[0].type () == madara::knowledge::KnowledgeRecord::DOUBLE)
    {
      result = --args[0];
    }
  }

  return result;
}

int main (int, char **)
{
  madara::knowledge::KnowledgeRecordFilters filters;

  std::cerr << 
    "Adding decrement_primitives to integer, double, and jpeg types\n";

  filters.add (
    madara::knowledge::KnowledgeRecord::INTEGER |
    madara::knowledge::KnowledgeRecord::DOUBLE |
    madara::knowledge::KnowledgeRecord::IMAGE_JPEG,
    decrement_primitives);

  filters.print_num_filters ();
  
  std::cerr << 
    "Clearing filters from jpeg type\n";

  filters.clear (madara::knowledge::KnowledgeRecord::IMAGE_JPEG);
  
  filters.print_num_filters ();
  
  std::cerr << 
    "Clearing filters from all types\n";

  filters.clear (madara::knowledge::KnowledgeRecord::ALL_TYPES);
  
  filters.print_num_filters ();
  
  std::cerr << 
    "Adding a 0 function to all types (should not add)\n";

  filters.add (
    madara::knowledge::KnowledgeRecord::INTEGER |
    madara::knowledge::KnowledgeRecord::DOUBLE |
    madara::knowledge::KnowledgeRecord::IMAGE_JPEG,
    decrement_primitives);
  
  filters.print_num_filters ();
  
  std::cerr << 
    "Adding a decrement_primitives to integer and double types\n";

  filters.add (
    madara::knowledge::KnowledgeRecord::INTEGER |
    madara::knowledge::KnowledgeRecord::DOUBLE,
    decrement_primitives);
  
  std::cerr << 
    "Adding a 2nd decrement_primitives to integer type\n";

  filters.add (
    madara::knowledge::KnowledgeRecord::INTEGER,
    decrement_primitives);
  
  filters.print_num_filters ();
  
  madara::knowledge::KnowledgeRecord integer_record (
    madara::knowledge::KnowledgeRecord::Integer (15));
  madara::knowledge::KnowledgeRecord double_record (17.0);
  madara::knowledge::KnowledgeRecord string_record ("This string should not change.");

  madara::transport::TransportContext transport_context;

  // Filter the records;
  madara::knowledge::KnowledgeRecord integer_result = filters.filter (integer_record,
    "", transport_context);
  madara::knowledge::KnowledgeRecord double_result = filters.filter (double_record,
    "", transport_context);
  madara::knowledge::KnowledgeRecord string_result = filters.filter (string_record,
    "", transport_context);

  std::cerr << "The result of the filtering was the following:\n";
  std::cerr << "  integer result = " << integer_result << " (";
  if (integer_result == madara::knowledge::KnowledgeRecord::Integer (12))
    std::cerr << "SUCCESS)\n";
  else
  {
    std::cerr << "FAILURE)\n"; ++num_fails;
  }

  std::cerr << "  double result = " << double_result << " (";
  if (double_result == 15.0)
    std::cerr << "SUCCESS)\n";
  else
  {
    std::cerr << "FAILURE)\n"; ++num_fails;
  }
  
  std::cerr << "  string result = " << string_result << " (";
  if (string_result == "This string should not change.")
    std::cerr << "SUCCESS)\n";
  else
  {
    std::cerr << "FAILURE)\n"; ++num_fails;
  }
  
  std::cerr << 
    "Adding a decrement_primitives to string type (deletes string)\n";

  filters.add (
    madara::knowledge::KnowledgeRecord::STRING,
    decrement_primitives);
  
  
  string_result = filters.filter (string_record,
    "", transport_context);

  std::cerr << 
    "Running filter on string type (should delete string)\n";
  
  std::cerr << "  string result = " << string_result << " (";
  if (string_result == madara::knowledge::KnowledgeRecord::Integer (0))
    std::cerr << "SUCCESS)\n";
  else
  {
    std::cerr << "FAILURE)\n"; ++num_fails;
  }
  
  if (num_fails > 0)
  {
    std::cerr << "OVERALL: FAIL. " << num_fails << " tests failed.\n";
  }
  else
  {
    std::cerr << "OVERALL: SUCCESS.\n";
  }

  return num_fails;
}
