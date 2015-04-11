

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/transport/QoS_Transport_Settings.h"
#include "madara/utility/Log_Macros.h"
#include <iostream>
#include <iomanip>
#include <string>

void test_rebroadcast_settings (void)
{ 
  Madara::Transport::QoS_Transport_Settings settings;
  
  std::cerr << std::dec <<
    "\n***********Testing rebroadcast settings************\n\n";

  unsigned char old_ttl, current_ttl;

  std::cerr << "Setting rebroadcast ttl to 3... ";

  // set the time to live for rebroadcast messages
  old_ttl = 3;
  settings.set_rebroadcast_ttl (old_ttl);
  current_ttl = settings.get_rebroadcast_ttl ();

  if (old_ttl == current_ttl && current_ttl == 3)
    std::cerr << "SUCCESS.\n";
  else
    std::cerr << "FAIL.\n";
  
  std::cerr << "Setting participant rebroadcast ttl to 4... ";

  // set the time to live for participant rebroadcast messages
  old_ttl = settings.get_participant_ttl ();
  settings.enable_participant_ttl (4);
  current_ttl = settings.get_participant_ttl ();

  if (old_ttl == 0 && current_ttl == 4)
    std::cerr << "SUCCESS.\n";
  else
    std::cerr << "FAIL.\n";

}

void test_peer_list (void)
{
  Madara::Transport::QoS_Transport_Settings settings;
  
  std::cerr << std::dec <<
    "\n***********Testing peer lists************\n\n";

  std::string peer1 = "127.0.0.1:13000";
  std::string peer2 = "127.0.0.1:13001";
  std::string peer3 = "127.0.0.1:13002";
  std::string peer4 = "127.0.0.1:13003";
  std::string peer5 = "127.0.0.1:13004";

  // check the defaults (all peers trusted)
  std::cerr << "Checking default trust:\n";
  std::cerr << "  " << peer1 << "=" << settings.is_trusted (peer1) << "\n";
  std::cerr << "  " << peer2 << "=" << settings.is_trusted (peer2) << "\n";
  std::cerr << "  " << peer3 << "=" << settings.is_trusted (peer3) << "\n";
  std::cerr << "  " << peer4 << "=" << settings.is_trusted (peer4) << "\n";
  std::cerr << "  " << peer5 << "=" << settings.is_trusted (peer5) << "\n";
  
  std::cerr << "Adding " << peer5 << " and " << peer2 <<
    " to the banned list:\n";
  settings.add_banned_peer (peer5);
  settings.add_banned_peer (peer2);
  std::cerr << "  " << peer1 << "=" << settings.is_trusted (peer1) << "\n";
  std::cerr << "  " << peer2 << "=" << settings.is_trusted (peer2) << "\n";
  std::cerr << "  " << peer3 << "=" << settings.is_trusted (peer3) << "\n";
  std::cerr << "  " << peer4 << "=" << settings.is_trusted (peer4) << "\n";
  std::cerr << "  " << peer5 << "=" << settings.is_trusted (peer5) << "\n";
  
  std::cerr << "Removing " << peer2 <<
    " from the banned list:\n";
  settings.remove_banned_peer (peer2);
  std::cerr << "  " << peer1 << "=" << settings.is_trusted (peer1) << "\n";
  std::cerr << "  " << peer2 << "=" << settings.is_trusted (peer2) << "\n";
  std::cerr << "  " << peer3 << "=" << settings.is_trusted (peer3) << "\n";
  std::cerr << "  " << peer4 << "=" << settings.is_trusted (peer4) << "\n";
  std::cerr << "  " << peer5 << "=" << settings.is_trusted (peer5) << "\n";
  
  std::cerr << "Adding " << peer5 <<
    " to the trusted list:\n";
  settings.add_trusted_peer (peer5);
  std::cerr << "  " << peer1 << "=" << settings.is_trusted (peer1) << "\n";
  std::cerr << "  " << peer2 << "=" << settings.is_trusted (peer2) << "\n";
  std::cerr << "  " << peer3 << "=" << settings.is_trusted (peer3) << "\n";
  std::cerr << "  " << peer4 << "=" << settings.is_trusted (peer4) << "\n";
  std::cerr << "  " << peer5 << "=" << settings.is_trusted (peer5) << "\n";
  
  std::cerr << "Removing " << peer5 <<
    " from the trusted list:\n";
  settings.remove_trusted_peer (peer5);
  std::cerr << "  " << peer1 << "=" << settings.is_trusted (peer1) << "\n";
  std::cerr << "  " << peer2 << "=" << settings.is_trusted (peer2) << "\n";
  std::cerr << "  " << peer3 << "=" << settings.is_trusted (peer3) << "\n";
  std::cerr << "  " << peer4 << "=" << settings.is_trusted (peer4) << "\n";
  std::cerr << "  " << peer5 << "=" << settings.is_trusted (peer5) << "\n";


  settings.add_banned_peer (peer3);
  settings.add_banned_peer (peer4);

  Madara::Transport::QoS_Transport_Settings settings_copy (settings);
  Madara::Transport::QoS_Transport_Settings settings_assignment;
  settings_assignment = settings;

  std::cerr << "Testing copy constructors and assignments.\n";
  std::cerr << "  Copy constructor = ";

  if (!settings_copy.is_trusted (peer3) &&
      !settings_copy.is_trusted (peer4) &&
      settings_copy.is_trusted (peer2))
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAILURE\n";
  
  std::cerr << "  Assignment operator = ";

  if (!settings_assignment.is_trusted (peer3) &&
      !settings_assignment.is_trusted (peer4) &&
      settings_assignment.is_trusted (peer1))
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAILURE\n";
}

/**
 * Filter for dropping non-primitives (file records). This
 * can be useful for a rebroadcast filter that does not want
 * to waste bandwidth with large formats.
 **/
Madara::Knowledge_Record
drop_non_primitives (
  Madara::Knowledge_Engine::Function_Arguments & args,
  Madara::Knowledge_Engine::Variables &)
{
  Madara::Knowledge_Record result;

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

/**
 * Filter for increasing the clock of the record
 **/
Madara::Knowledge_Record
increase_clock (
  Madara::Knowledge_Engine::Function_Arguments & args,
  Madara::Knowledge_Engine::Variables &)
{
  Madara::Knowledge_Record result;

  if (args.size () > 0)
  {
    result = args[0];
    ++result.clock;
  }

  return result;
}

/**
 * Filter for dropping a record completely (we don't do
 * anything with the argument)
 **/
Madara::Knowledge_Record
drop_record (
  Madara::Knowledge_Engine::Function_Arguments &,
  Madara::Knowledge_Engine::Variables &)
{
  return Madara::Knowledge_Record ();
}

/**
 * No-op filter
 **/
Madara::Knowledge_Record
no_op (
  Madara::Knowledge_Engine::Function_Arguments & args,
  Madara::Knowledge_Engine::Variables &)
{
  if (args.size () > 0)
    return args[0];
  else
    return Madara::Knowledge_Record ();
}


void test_filters (void)
{
  std::cerr << std::dec << "\n***********Testing filters************\n\n";

  Madara::Transport::QoS_Transport_Settings settings;
  
  Madara::Knowledge_Engine::Thread_Safe_Context context;
  settings.attach (&context);

  std::cerr <<
    "Adding drop_non_primitives filter to rebroadcast filters.\n";
  settings.add_rebroadcast_filter (
    Madara::Knowledge_Record::ALL_TYPES, drop_non_primitives);
  std::cerr << "Printing rebroadcast filters.\n";
  settings.print_num_filters_rebroadcast ();

  std::cerr << 
    "Adding increase_clock filter x 2 to rebroadcast filters for integers.\n";
  std::cerr << 
    "Adding increase_clock filter x 1 to rebroadcast filters for doubles.\n";
  settings.add_rebroadcast_filter (Madara::Knowledge_Record::INTEGER
    | Madara::Knowledge_Record::DOUBLE, drop_non_primitives);
  settings.add_rebroadcast_filter (
    Madara::Knowledge_Record::INTEGER, drop_non_primitives);
  std::cerr << "Printing rebroadcast filters.\n";
  settings.print_num_filters_rebroadcast ();

  std::cerr << "Clearing filters for all types in rebroadcast filters\n";
  settings.clear_rebroadcast_filters (Madara::Knowledge_Record::ALL_TYPES);
  std::cerr << "Printing rebroadcast filters.\n";
  settings.print_num_filters_rebroadcast ();
  
  std::cerr << "Adding no_op filter x 2 to send filters for integers.\n";
  std::cerr << "Adding no_op filter x 1 to send filters for doubles.\n";
  settings.add_send_filter (Madara::Knowledge_Record::INTEGER
    | Madara::Knowledge_Record::DOUBLE, no_op);
  settings.add_send_filter (Madara::Knowledge_Record::INTEGER, no_op);
  std::cerr << "Printing send filters.\n";
  settings.print_num_filters_send ();
  
  std::cerr <<
    "Adding increase_clock filter x 3 to receive filters for strings.\n";
  std::cerr <<
    "Adding increase_clock filter x 2 to receive filters for integers.\n";
  std::cerr <<
    "Adding increase_clock filter x 1 to receive filters for doubles.\n";
  std::cerr <<
    "Adding drop_record filter x 1 to receive filters for file types.\n";
  settings.add_receive_filter (Madara::Knowledge_Record::INTEGER
    | Madara::Knowledge_Record::DOUBLE | Madara::Knowledge_Record::STRING,
    increase_clock);
  settings.add_receive_filter (
    Madara::Knowledge_Record::INTEGER | Madara::Knowledge_Record::STRING,
    increase_clock);
  settings.add_receive_filter (
    Madara::Knowledge_Record::STRING, increase_clock);
  settings.add_receive_filter (
    Madara::Knowledge_Record::ALL_FILE_TYPES, drop_record);
  std::cerr << "Printing receive filters.\n";
  settings.print_num_filters_receive ();

  
  Madara::Knowledge_Record integer_record (
    Madara::Knowledge_Record::Integer (1));
  Madara::Knowledge_Record double_record (2.5);
  Madara::Knowledge_Record string_record ("Extra data");
  Madara::Knowledge_Record file_record;

  file_record.read_file (Madara::Utility::expand_envs (
    "$(MADARA_ROOT)/tests/images/manaus_hotel_100x100.jpg"));

  Madara::Transport::Transport_Context transport_context;

  // Filter the records;
  Madara::Knowledge_Record integer_result =
    settings.filter_rebroadcast (integer_record, "", transport_context);
  Madara::Knowledge_Record double_result =
    settings.filter_rebroadcast (double_record, "", transport_context);
  Madara::Knowledge_Record string_result =
    settings.filter_rebroadcast (string_record, "", transport_context);
  Madara::Knowledge_Record file_result =
    settings.filter_rebroadcast (file_record, "", transport_context);
  
  std::cerr << "The result of the rebroadcast filtering was the following:\n";
  std::cerr << "  integer result = " << integer_result << 
               " and clock = " << integer_result.clock << " (";
  if (integer_result == Madara::Knowledge_Record::Integer (1) &&
      integer_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << "  double result = " << double_result << 
               " and clock = " << double_result.clock << " (";
  if (double_result == 2.5 &&
      double_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << "  string result = " << string_result << 
               " and clock = " << string_result.clock << " (";
  if (string_result == "Extra data" &&
      string_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << "  file size = " << file_result.size () << 
               " and clock = " << file_result.clock << " (";
  if (file_result.size () > 20000 &&
      file_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  // Filter the records;
  integer_result =
    settings.filter_send (integer_record, "", transport_context);
  double_result =
    settings.filter_send (double_record, "", transport_context);
  string_result =
    settings.filter_send (string_record, "", transport_context);
  file_result =
    settings.filter_send (file_record, "", transport_context);
  
  std::cerr << "The result of the send filtering was the following:\n";
  std::cerr << "  integer result = " << integer_result << 
               " and clock = " << integer_result.clock << " (";
  if (integer_result == Madara::Knowledge_Record::Integer (1) &&
      integer_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << "  double result = " << double_result << 
               " and clock = " << double_result.clock << " (";
  if (double_result == 2.5 &&
      double_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << "  string result = " << string_result << 
               " and clock = " << string_result.clock << " (";
  if (string_result == "Extra data" &&
      string_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << "  file size = " << file_result.size () << 
               " and clock = " << file_result.clock << " (";
  if (file_result.size () > 20000 &&
      file_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  // Filter the records;
  integer_result =
    settings.filter_receive (integer_record, "", transport_context);
  double_result =
    settings.filter_receive (double_record, "", transport_context);
  string_result =
    settings.filter_receive (string_record, "", transport_context);
  file_result =
    settings.filter_receive (file_record, "", transport_context);
  
  std::cerr << "The result of the receive filtering was the following:\n";
  std::cerr << "  integer result = " << integer_result << 
               " and clock = " << integer_result.clock << " (";
  if (integer_result == Madara::Knowledge_Record::Integer (1) &&
      integer_result.clock == 2)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << "  double result = " << double_result << 
               " and clock = " << double_result.clock << " (";
  if (double_result == 2.5 &&
      double_result.clock == 1)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << "  string result = " << string_result << 
               " and clock = " << string_result.clock << " (";
  if (string_result == "Extra data" &&
      string_result.clock == 3)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
  std::cerr << "  file size = " << file_result.size () << 
               " and status = " << file_result.status () << " (";
  if (file_result.size () == 0 &&
      file_result.status () == Madara::Knowledge_Record::UNINITIALIZED)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
  
}

int main (int, char **)
{
  test_rebroadcast_settings ();
  test_peer_list ();
  test_filters ();

  return 0;
}
