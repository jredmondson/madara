#include <iostream>

#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/knowledge_engine/Knowledge_Record_Filters.h"

/**
 * Function that decrements integers or doubles and returns the results
 * and strips any other value entirely (returns 0)
 **/
Madara::Knowledge_Record
  decrement_primitives (Madara::Knowledge_Engine::Function_Arguments & args,
  Madara::Knowledge_Engine::Variables & vars)
{
  Madara::Knowledge_Record result;

  if (args.size () > 0)
  {
    if (args[0].type () == Madara::Knowledge_Record::INTEGER ||
        args[0].type () == Madara::Knowledge_Record::DOUBLE)
    {
      result = --args[0];
    }
  }

  return result;
}

int ACE_TMAIN (int argc, ACE_TCHAR * argv[])
{
  Madara::Knowledge_Engine::Knowledge_Record_Filters filters;

  std::cerr << 
    "Adding decrement_primitives to integer, double, and jpeg types\n";

  filters.add (
    Madara::Knowledge_Record::INTEGER |
    Madara::Knowledge_Record::DOUBLE |
    Madara::Knowledge_Record::IMAGE_JPEG,
    decrement_primitives);

  filters.print_num_filters ();
  
  std::cerr << 
    "Clearing filters from jpeg type\n";

  filters.clear (Madara::Knowledge_Record::IMAGE_JPEG);
  
  filters.print_num_filters ();
  
  std::cerr << 
    "Clearing filters from all types\n";

  filters.clear (Madara::Knowledge_Record::ALL_TYPES);
  
  filters.print_num_filters ();
  
  std::cerr << 
    "Adding a 0 function to all types (should not add)\n";

  filters.add (
    Madara::Knowledge_Record::INTEGER |
    Madara::Knowledge_Record::DOUBLE |
    Madara::Knowledge_Record::IMAGE_JPEG,
    decrement_primitives);
  
  filters.print_num_filters ();
  
  std::cerr << 
    "Adding a decrement_primitives to integer and double types\n";

  filters.add (
    Madara::Knowledge_Record::INTEGER |
    Madara::Knowledge_Record::DOUBLE,
    decrement_primitives);
  
  std::cerr << 
    "Adding a 2nd decrement_primitives to integer type\n";

  filters.add (
    Madara::Knowledge_Record::INTEGER,
    decrement_primitives);
  
  filters.print_num_filters ();
  
  Madara::Knowledge_Record integer_record (
    Madara::Knowledge_Record::Integer (15));
  Madara::Knowledge_Record double_record (17.0);
  Madara::Knowledge_Record string_record ("This string should not change.");

  Madara::Transport::Transport_Context transport_context;

  // Filter the records;
  Madara::Knowledge_Record integer_result = filters.filter (integer_record,
    "", transport_context);
  Madara::Knowledge_Record double_result = filters.filter (double_record,
    "", transport_context);
  Madara::Knowledge_Record string_result = filters.filter (string_record,
    "", transport_context);

  std::cerr << "The result of the filtering was the following:\n";
  std::cerr << "  integer result = " << integer_result << " (";
  if (integer_result == Madara::Knowledge_Record::Integer (13))
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << "  double result = " << double_result << " (";
  if (double_result == 16.0)
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
    Madara::Knowledge_Record::STRING,
    decrement_primitives);
  
  
  string_result = filters.filter (string_record,
    "", transport_context);

  std::cerr << 
    "Running filter on string type (should delete string)\n";
  
  std::cerr << "  string result = " << string_result << " (";
  if (string_result == Madara::Knowledge_Record::Integer (0))
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  return 0;
}
