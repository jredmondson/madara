#include <iostream>

#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/KnowledgeRecordFilters.h"

/**
 * Function that decrements integers or doubles and returns the results
 * and strips any other value entirely (returns 0)
 **/
madara::KnowledgeRecord
  decrement_primitives (madara::knowledge::FunctionArguments & args,
  madara::knowledge::Variables &)
{
  madara::KnowledgeRecord result;

  if (args.size () > 0)
  {
    if (args[0].type () == madara::KnowledgeRecord::INTEGER ||
        args[0].type () == madara::KnowledgeRecord::DOUBLE)
    {
      result = --args[0];
    }
  }

  return result;
}

int ACE_TMAIN (int, ACE_TCHAR **)
{
  madara::knowledge::KnowledgeRecordFilters filters;

  std::cerr << 
    "Adding decrement_primitives to integer, double, and jpeg types\n";

  filters.add (
    madara::KnowledgeRecord::INTEGER |
    madara::KnowledgeRecord::DOUBLE |
    madara::KnowledgeRecord::IMAGE_JPEG,
    decrement_primitives);

  filters.print_num_filters ();
  
  std::cerr << 
    "Clearing filters from jpeg type\n";

  filters.clear (madara::KnowledgeRecord::IMAGE_JPEG);
  
  filters.print_num_filters ();
  
  std::cerr << 
    "Clearing filters from all types\n";

  filters.clear (madara::KnowledgeRecord::ALL_TYPES);
  
  filters.print_num_filters ();
  
  std::cerr << 
    "Adding a 0 function to all types (should not add)\n";

  filters.add (
    madara::KnowledgeRecord::INTEGER |
    madara::KnowledgeRecord::DOUBLE |
    madara::KnowledgeRecord::IMAGE_JPEG,
    decrement_primitives);
  
  filters.print_num_filters ();
  
  std::cerr << 
    "Adding a decrement_primitives to integer and double types\n";

  filters.add (
    madara::KnowledgeRecord::INTEGER |
    madara::KnowledgeRecord::DOUBLE,
    decrement_primitives);
  
  std::cerr << 
    "Adding a 2nd decrement_primitives to integer type\n";

  filters.add (
    madara::KnowledgeRecord::INTEGER,
    decrement_primitives);
  
  filters.print_num_filters ();
  
  madara::KnowledgeRecord integer_record (
    madara::KnowledgeRecord::Integer (15));
  madara::KnowledgeRecord double_record (17.0);
  madara::KnowledgeRecord string_record ("This string should not change.");

  madara::transport::TransportContext transport_context;

  // Filter the records;
  madara::KnowledgeRecord integer_result = filters.filter (integer_record,
    "", transport_context);
  madara::KnowledgeRecord double_result = filters.filter (double_record,
    "", transport_context);
  madara::KnowledgeRecord string_result = filters.filter (string_record,
    "", transport_context);

  std::cerr << "The result of the filtering was the following:\n";
  std::cerr << "  integer result = " << integer_result << " (";
  if (integer_result == madara::KnowledgeRecord::Integer (12))
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << "  double result = " << double_result << " (";
  if (double_result == 15.0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << "  string result = " << string_result << " (";
  if (string_result == "This string should not change.")
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << 
    "Adding a decrement_primitives to string type (deletes string)\n";

  filters.add (
    madara::KnowledgeRecord::STRING,
    decrement_primitives);
  
  
  string_result = filters.filter (string_record,
    "", transport_context);

  std::cerr << 
    "Running filter on string type (should delete string)\n";
  
  std::cerr << "  string result = " << string_result << " (";
  if (string_result == madara::KnowledgeRecord::Integer (0))
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  return 0;
}
