#include <iostream>

#include "madara/knowledge/Knowledge_Record.h"
#include "madara/knowledge/Knowledge_Record_Filters.h"

/**
 * Function that decrements integers or doubles and returns the results
 * and strips any other value entirely (returns 0)
 **/
madara::Knowledge_Record
  decrement_primitives (madara::knowledge::Function_Arguments & args,
  madara::knowledge::Variables &)
{
  madara::Knowledge_Record result;

  if (args.size () > 0)
  {
    if (args[0].type () == madara::Knowledge_Record::INTEGER ||
        args[0].type () == madara::Knowledge_Record::DOUBLE)
    {
      result = --args[0];
    }
  }

  return result;
}

int ACE_TMAIN (int, ACE_TCHAR **)
{
  madara::knowledge::Knowledge_Record_Filters filters;

  std::cerr << 
    "Adding decrement_primitives to integer, double, and jpeg types\n";

  filters.add (
    madara::Knowledge_Record::INTEGER |
    madara::Knowledge_Record::DOUBLE |
    madara::Knowledge_Record::IMAGE_JPEG,
    decrement_primitives);

  filters.print_num_filters ();
  
  std::cerr << 
    "Clearing filters from jpeg type\n";

  filters.clear (madara::Knowledge_Record::IMAGE_JPEG);
  
  filters.print_num_filters ();
  
  std::cerr << 
    "Clearing filters from all types\n";

  filters.clear (madara::Knowledge_Record::ALL_TYPES);
  
  filters.print_num_filters ();
  
  std::cerr << 
    "Adding a 0 function to all types (should not add)\n";

  filters.add (
    madara::Knowledge_Record::INTEGER |
    madara::Knowledge_Record::DOUBLE |
    madara::Knowledge_Record::IMAGE_JPEG,
    decrement_primitives);
  
  filters.print_num_filters ();
  
  std::cerr << 
    "Adding a decrement_primitives to integer and double types\n";

  filters.add (
    madara::Knowledge_Record::INTEGER |
    madara::Knowledge_Record::DOUBLE,
    decrement_primitives);
  
  std::cerr << 
    "Adding a 2nd decrement_primitives to integer type\n";

  filters.add (
    madara::Knowledge_Record::INTEGER,
    decrement_primitives);
  
  filters.print_num_filters ();
  
  madara::Knowledge_Record integer_record (
    madara::Knowledge_Record::Integer (15));
  madara::Knowledge_Record double_record (17.0);
  madara::Knowledge_Record string_record ("This string should not change.");

  madara::transport::Transport_Context transport_context;

  // Filter the records;
  madara::Knowledge_Record integer_result = filters.filter (integer_record,
    "", transport_context);
  madara::Knowledge_Record double_result = filters.filter (double_record,
    "", transport_context);
  madara::Knowledge_Record string_result = filters.filter (string_record,
    "", transport_context);

  std::cerr << "The result of the filtering was the following:\n";
  std::cerr << "  integer result = " << integer_result << " (";
  if (integer_result == madara::Knowledge_Record::Integer (12))
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
    madara::Knowledge_Record::STRING,
    decrement_primitives);
  
  
  string_result = filters.filter (string_record,
    "", transport_context);

  std::cerr << 
    "Running filter on string type (should delete string)\n";
  
  std::cerr << "  string result = " << string_result << " (";
  if (string_result == madara::Knowledge_Record::Integer (0))
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  return 0;
}
