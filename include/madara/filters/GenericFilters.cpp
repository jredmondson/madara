#include "Arguments.h"
#include "GenericFilters.h"
#include "madara/knowledge/Functions.h"
#include "madara/knowledge/ExternFunctionVariables.h"
#include <sstream>

const char * operation_types [] = {
  "IDLE_OPERATION",
  "SENDING_OPERATION",
  "RECEIVING_OPERATION",
  "REBROADCASTING_OPERATION"
};


madara::KnowledgeRecord
madara::filters::discard (knowledge::FunctionArguments & args,
              knowledge::Variables & vars)
{
  // this works because a default record is UNCREATED
  return madara::KnowledgeRecord ();
}

madara::KnowledgeRecord
madara::filters::discard_nonprimitives (knowledge::FunctionArguments & args,
              knowledge::Variables & vars)
{
  madara::KnowledgeRecord result;

  if (args.size () > 0)
  {
    /**
     * Alternatively, we could check type () for each
     * defined type, but this will check that we are not
     * a file type
     **/
    if (!args[0].is_file_type ())
    {
      result = args[0];
    }
  }

  /**
   * At the transport layer, we should check for stripped types
   * and remove them from the list of knowledge records
   **/
  return result;
}


madara::KnowledgeRecord
madara::filters::discard_nonfiles (knowledge::FunctionArguments & args,
              knowledge::Variables & vars)
{
  madara::KnowledgeRecord result;

  if (args.size () > 0)
  {
    /**
     * Alternatively, we could check type () for each
     * defined type, but this will check that we are
     * a file type
     **/
    if (args[0].is_file_type ())
    {
      result = args[0];
    }
  }

  /**
   * At the transport layer, we should check for stripped types
   * and remove them from the list of knowledge records
   **/
  return result;
}

madara::KnowledgeRecord
madara::filters::log_args (knowledge::FunctionArguments & args,
                           knowledge::Variables & vars)
{
  madara::KnowledgeRecord result;

  if (args.size () == madara::filters::TOTAL_ARGUMENTS)
  {
    // return the first arg for further processing by other filters
    result = args[0];

    std::stringstream buffer;
    buffer << "Filter args:\n";
    
    // [0] Record
    buffer << "  [0:Record]:            " ;
    buffer << args[0].to_string ();
    buffer << "\n";
    
    // [1] Record
    buffer << "  [1:Record Name]:       " ;
    buffer << args[1].to_string ();
    buffer << "\n";
    
    // [2] Operation Type
    buffer << "  [2:Operation Type]:    " ;
    buffer << operation_types [args[2].to_integer ()];
    buffer << "\n";
    
    // [3] Send Bandwidth
    buffer << "  [3:Send Bandwidth]:    " ;
    buffer << args[3].to_integer ();
    buffer << " B/s\n";
    
    // [4] Receive Bandwidth
    buffer << "  [4:Receive Bandwidth]: " ;
    buffer << args[4].to_integer ();
    buffer << " B/s\n";
    
    // [5] Update Time
    buffer << "  [5:Update Time]:       " ;
    buffer << args[5].to_integer ();
    buffer << "\n";
    
    // [6] Current Time
    buffer << "  [6:Current Time]:      " ;
    buffer << args[6].to_integer ();
    buffer << "\n";
    
    // [7] Knowledge Domain
    buffer << "  [7:Knowledge Domain]:  " ;
    buffer << args[7].to_string ();
    buffer << "\n";
    
    // [8] Knowledge Domain
    buffer << "  [8:Originator]:        " ;
    buffer << args[8].to_string ();
    buffer << "\n";

    vars.print (buffer.str (), 0);
  }

  return result;
}

void
madara::filters::log_aggregate (
  KnowledgeMap & records,
  const transport::TransportContext & transport_context,
  knowledge::Variables & vars)
{
  std::stringstream buffer;
  buffer << "Aggregate Filter Arguments:\n";
    
  // Operation Type
  buffer << "  Operation Type:    " ;
  buffer << operation_types [transport_context.get_operation ()];
  buffer << "\n";
    
  // Send Bandwidth
  buffer << "  Send Bandwidth:    " ;
  buffer << transport_context.get_send_bandwidth ();
  buffer << " B/s\n";
    
  // Receive Bandwidth
  buffer << "  Receive Bandwidth: " ;
  buffer << transport_context.get_receive_bandwidth ();
  buffer << " B/s\n";
    
  // Update Time
  buffer << "  Update Time:       " ;
  buffer << transport_context.get_message_time ();
  buffer << "\n";
    
  // Current Time
  buffer << "  Current Time:      " ;
  buffer << transport_context.get_current_time ();
  buffer << "\n";
    
  // Knowledge Domain
  buffer << "  Knowledge Domain:  " ;
  buffer << transport_context.get_domain ();
  buffer << "\n";
    
  // Originator
  buffer << "  Originator:        " ;
  buffer << transport_context.get_originator ();
  buffer << "\n";
  
  buffer << "  Updates:\n" ;
  if (records.size () > 0)
  {
    for (KnowledgeMap::const_iterator i = records.begin ();
         i != records.end (); ++i)
    {
      buffer << "    " << i->first << " = ";
      buffer << i->second.to_string () << "\n";
    }
  }
  
  vars.print (buffer.str (), 0);
}
