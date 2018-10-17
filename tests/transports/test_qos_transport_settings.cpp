

#include "madara/knowledge/KnowledgeBase.h"
#include "madara/transport/QoSTransportSettings.h"

#include <iostream>
#include <iomanip>
#include <string>

namespace transport = madara::transport;
typedef transport::QoSTransportSettings QoSTransportSettings;

void test_rebroadcast_settings(void)
{
  madara::transport::QoSTransportSettings settings;

  std::cerr << std::dec
            << "\n***********Testing rebroadcast settings************\n\n";

  unsigned char old_ttl, current_ttl;

  std::cerr << "Setting rebroadcast ttl to 3... ";

  // set the time to live for rebroadcast messages
  old_ttl = 3;
  settings.set_rebroadcast_ttl(old_ttl);
  current_ttl = settings.get_rebroadcast_ttl();

  if(old_ttl == current_ttl && current_ttl == 3)
    std::cerr << "SUCCESS.\n";
  else
    std::cerr << "FAIL.\n";

  std::cerr << "Setting participant rebroadcast ttl to 4... ";

  // set the time to live for participant rebroadcast messages
  old_ttl = settings.get_participant_ttl();
  settings.enable_participant_ttl(4);
  current_ttl = settings.get_participant_ttl();

  if(old_ttl == 0 && current_ttl == 4)
    std::cerr << "SUCCESS.\n";
  else
    std::cerr << "FAIL.\n";
}

void test_peer_list(void)
{
  madara::transport::QoSTransportSettings settings;

  std::cerr << std::dec << "\n***********Testing peer lists************\n\n";

  std::string peer1 = "127.0.0.1:13000";
  std::string peer2 = "127.0.0.1:13001";
  std::string peer3 = "127.0.0.1:13002";
  std::string peer4 = "127.0.0.1:13003";
  std::string peer5 = "127.0.0.1:13004";

  // check the defaults (all peers trusted)
  std::cerr << "Checking default trust:\n";
  std::cerr << "  " << peer1 << "=" << settings.is_trusted(peer1) << "\n";
  std::cerr << "  " << peer2 << "=" << settings.is_trusted(peer2) << "\n";
  std::cerr << "  " << peer3 << "=" << settings.is_trusted(peer3) << "\n";
  std::cerr << "  " << peer4 << "=" << settings.is_trusted(peer4) << "\n";
  std::cerr << "  " << peer5 << "=" << settings.is_trusted(peer5) << "\n";

  std::cerr << "Adding " << peer5 << " and " << peer2
            << " to the banned list:\n";
  settings.add_banned_peer(peer5);
  settings.add_banned_peer(peer2);
  std::cerr << "  " << peer1 << "=" << settings.is_trusted(peer1) << "\n";
  std::cerr << "  " << peer2 << "=" << settings.is_trusted(peer2) << "\n";
  std::cerr << "  " << peer3 << "=" << settings.is_trusted(peer3) << "\n";
  std::cerr << "  " << peer4 << "=" << settings.is_trusted(peer4) << "\n";
  std::cerr << "  " << peer5 << "=" << settings.is_trusted(peer5) << "\n";

  std::cerr << "Removing " << peer2 << " from the banned list:\n";
  settings.remove_banned_peer(peer2);
  std::cerr << "  " << peer1 << "=" << settings.is_trusted(peer1) << "\n";
  std::cerr << "  " << peer2 << "=" << settings.is_trusted(peer2) << "\n";
  std::cerr << "  " << peer3 << "=" << settings.is_trusted(peer3) << "\n";
  std::cerr << "  " << peer4 << "=" << settings.is_trusted(peer4) << "\n";
  std::cerr << "  " << peer5 << "=" << settings.is_trusted(peer5) << "\n";

  std::cerr << "Adding " << peer5 << " to the trusted list:\n";
  settings.add_trusted_peer(peer5);
  std::cerr << "  " << peer1 << "=" << settings.is_trusted(peer1) << "\n";
  std::cerr << "  " << peer2 << "=" << settings.is_trusted(peer2) << "\n";
  std::cerr << "  " << peer3 << "=" << settings.is_trusted(peer3) << "\n";
  std::cerr << "  " << peer4 << "=" << settings.is_trusted(peer4) << "\n";
  std::cerr << "  " << peer5 << "=" << settings.is_trusted(peer5) << "\n";

  std::cerr << "Removing " << peer5 << " from the trusted list:\n";
  settings.remove_trusted_peer(peer5);
  std::cerr << "  " << peer1 << "=" << settings.is_trusted(peer1) << "\n";
  std::cerr << "  " << peer2 << "=" << settings.is_trusted(peer2) << "\n";
  std::cerr << "  " << peer3 << "=" << settings.is_trusted(peer3) << "\n";
  std::cerr << "  " << peer4 << "=" << settings.is_trusted(peer4) << "\n";
  std::cerr << "  " << peer5 << "=" << settings.is_trusted(peer5) << "\n";

  settings.add_banned_peer(peer3);
  settings.add_banned_peer(peer4);

  madara::transport::QoSTransportSettings settings_copy(settings);
  madara::transport::QoSTransportSettings settings_assignment;
  settings_assignment = settings;

  std::cerr << "Testing copy constructors and assignments.\n";
  std::cerr << "  Copy constructor = ";

  if(!settings_copy.is_trusted(peer3) && !settings_copy.is_trusted(peer4) &&
      settings_copy.is_trusted(peer2))
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAILURE\n";

  std::cerr << "  Assignment operator = ";

  if(!settings_assignment.is_trusted(peer3) &&
      !settings_assignment.is_trusted(peer4) &&
      settings_assignment.is_trusted(peer1))
    std::cerr << "SUCCESS\n";
  else
    std::cerr << "FAILURE\n";
}

/**
 * Filter for dropping non-primitives (file records). This
 * can be useful for a rebroadcast filter that does not want
 * to waste bandwidth with large formats.
 **/
madara::knowledge::KnowledgeRecord drop_non_primitives(
    madara::knowledge::FunctionArguments& args, madara::knowledge::Variables&)
{
  madara::knowledge::KnowledgeRecord result;

  if(args.size() > 0)
  {
    /**
     * Alternatively, we could check type () for each
     * defined type, but this will check that we are not
     * a file type
     **/
    if(!args[0].is_file_type())
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
madara::knowledge::KnowledgeRecord increase_clock(
    madara::knowledge::FunctionArguments& args, madara::knowledge::Variables&)
{
  madara::knowledge::KnowledgeRecord result;

  if(args.size() > 0)
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
madara::knowledge::KnowledgeRecord drop_record(
    madara::knowledge::FunctionArguments&, madara::knowledge::Variables&)
{
  return madara::knowledge::KnowledgeRecord();
}

/**
 * No-op filter
 **/
madara::knowledge::KnowledgeRecord no_op(
    madara::knowledge::FunctionArguments& args, madara::knowledge::Variables&)
{
  if(args.size() > 0)
    return args[0];
  else
    return madara::knowledge::KnowledgeRecord();
}

void test_filters(void)
{
  std::cerr << std::dec << "\n***********Testing filters************\n\n";

  madara::transport::QoSTransportSettings settings;

  madara::knowledge::ThreadSafeContext context;
  settings.attach(&context);

  std::cerr << "Adding drop_non_primitives filter to rebroadcast filters.\n";
  settings.add_rebroadcast_filter(
      madara::knowledge::KnowledgeRecord::ALL_TYPES, drop_non_primitives);
  std::cerr << "Printing rebroadcast filters.\n";
  settings.print_num_filters_rebroadcast();

  std::cerr << "Adding increase_clock filter x 2 to rebroadcast filters for "
               "integers.\n";
  std::cerr << "Adding increase_clock filter x 1 to rebroadcast filters for "
               "doubles.\n";
  settings.add_rebroadcast_filter(
      madara::knowledge::KnowledgeRecord::INTEGER |
          madara::knowledge::KnowledgeRecord::DOUBLE,
      drop_non_primitives);
  settings.add_rebroadcast_filter(
      madara::knowledge::KnowledgeRecord::INTEGER, drop_non_primitives);
  std::cerr << "Printing rebroadcast filters.\n";
  settings.print_num_filters_rebroadcast();

  std::cerr << "Clearing filters for all types in rebroadcast filters\n";
  settings.clear_rebroadcast_filters(
      madara::knowledge::KnowledgeRecord::ALL_TYPES);
  std::cerr << "Printing rebroadcast filters.\n";
  settings.print_num_filters_rebroadcast();

  std::cerr << "Adding no_op filter x 2 to send filters for integers.\n";
  std::cerr << "Adding no_op filter x 1 to send filters for doubles.\n";
  settings.add_send_filter(madara::knowledge::KnowledgeRecord::INTEGER |
                               madara::knowledge::KnowledgeRecord::DOUBLE,
      no_op);
  settings.add_send_filter(madara::knowledge::KnowledgeRecord::INTEGER, no_op);
  std::cerr << "Printing send filters.\n";
  settings.print_num_filters_send();

  std::cerr
      << "Adding increase_clock filter x 3 to receive filters for strings.\n";
  std::cerr
      << "Adding increase_clock filter x 2 to receive filters for integers.\n";
  std::cerr
      << "Adding increase_clock filter x 1 to receive filters for doubles.\n";
  std::cerr
      << "Adding drop_record filter x 1 to receive filters for file types.\n";
  settings.add_receive_filter(madara::knowledge::KnowledgeRecord::INTEGER |
                                  madara::knowledge::KnowledgeRecord::DOUBLE |
                                  madara::knowledge::KnowledgeRecord::STRING,
      increase_clock);
  settings.add_receive_filter(madara::knowledge::KnowledgeRecord::INTEGER |
                                  madara::knowledge::KnowledgeRecord::STRING,
      increase_clock);
  settings.add_receive_filter(
      madara::knowledge::KnowledgeRecord::STRING, increase_clock);
  settings.add_receive_filter(
      madara::knowledge::KnowledgeRecord::ALL_FILE_TYPES, drop_record);
  std::cerr << "Printing receive filters.\n";
  settings.print_num_filters_receive();

  madara::knowledge::KnowledgeRecord integer_record(
      madara::knowledge::KnowledgeRecord::Integer(1));
  madara::knowledge::KnowledgeRecord double_record(2.5);
  madara::knowledge::KnowledgeRecord string_record("Extra data");
  madara::knowledge::KnowledgeRecord file_record;

  file_record.read_file(madara::utility::expand_envs(
      "$(MADARA_ROOT)/tests/images/manaus_hotel_100x100.jpg"));

  madara::transport::TransportContext transport_context;

  // Filter the records;
  madara::knowledge::KnowledgeRecord integer_result =
      settings.filter_rebroadcast(integer_record, "", transport_context);
  madara::knowledge::KnowledgeRecord double_result =
      settings.filter_rebroadcast(double_record, "", transport_context);
  madara::knowledge::KnowledgeRecord string_result =
      settings.filter_rebroadcast(string_record, "", transport_context);
  madara::knowledge::KnowledgeRecord file_result =
      settings.filter_rebroadcast(file_record, "", transport_context);

  std::cerr << "The result of the rebroadcast filtering was the following:\n";
  std::cerr << "  integer result = " << integer_result
            << " and clock = " << integer_result.clock << " (";
  if(integer_result == madara::knowledge::KnowledgeRecord::Integer(1) &&
      integer_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";

  std::cerr << "  double result = " << double_result
            << " and clock = " << double_result.clock << " (";
  if(double_result == 2.5 && double_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";

  std::cerr << "  string result = " << string_result
            << " and clock = " << string_result.clock << " (";
  if(string_result == "Extra data" && string_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";

  std::cerr << "  file size = " << file_result.size()
            << " and clock = " << file_result.clock << " (";
  if(file_result.size() > 20000 && file_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";

  // Filter the records;
  integer_result = settings.filter_send(integer_record, "", transport_context);
  double_result = settings.filter_send(double_record, "", transport_context);
  string_result = settings.filter_send(string_record, "", transport_context);
  file_result = settings.filter_send(file_record, "", transport_context);

  std::cerr << "The result of the send filtering was the following:\n";
  std::cerr << "  integer result = " << integer_result
            << " and clock = " << integer_result.clock << " (";
  if(integer_result == madara::knowledge::KnowledgeRecord::Integer(1) &&
      integer_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";

  std::cerr << "  double result = " << double_result
            << " and clock = " << double_result.clock << " (";
  if(double_result == 2.5 && double_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";

  std::cerr << "  string result = " << string_result
            << " and clock = " << string_result.clock << " (";
  if(string_result == "Extra data" && string_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";

  std::cerr << "  file size = " << file_result.size()
            << " and clock = " << file_result.clock << " (";
  if(file_result.size() > 20000 && file_result.clock == 0)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";

  // Filter the records;
  integer_result =
      settings.filter_receive(integer_record, "", transport_context);
  double_result = settings.filter_receive(double_record, "", transport_context);
  string_result = settings.filter_receive(string_record, "", transport_context);
  file_result = settings.filter_receive(file_record, "", transport_context);

  std::cerr << "The result of the receive filtering was the following:\n";
  std::cerr << "  integer result = " << integer_result
            << " and clock = " << integer_result.clock << " (";
  if(integer_result == madara::knowledge::KnowledgeRecord::Integer(1) &&
      integer_result.clock == 2)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";

  std::cerr << "  double result = " << double_result
            << " and clock = " << double_result.clock << " (";
  if(double_result == 2.5 && double_result.clock == 1)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";

  std::cerr << "  string result = " << string_result
            << " and clock = " << string_result.clock << " (";
  if(string_result == "Extra data" && string_result.clock == 3)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";

  std::cerr << "  file size = " << file_result.size()
            << " and status = " << file_result.status() << " (";
  if(file_result.size() == 0 &&
      file_result.status() == madara::knowledge::KnowledgeRecord::UNCREATED)
    std::cerr << "SUCCESS)\n";
  else
    std::cerr << "FAILURE)\n";
}

void test_save_and_load()
{
  QoSTransportSettings source_settings, loaded_settings;

  source_settings.add_banned_peer("banned_website");
  source_settings.add_banned_peer("banned_user");
  source_settings.add_banned_peer("banned_computer");

  source_settings.add_trusted_peer("trusted_website");
  source_settings.add_trusted_peer("trusted_user");
  source_settings.add_trusted_peer("trusted_computer");

  source_settings.set_deadline(10);
  source_settings.delay_launch = true;
  source_settings.write_domain = "my_domain";
  source_settings.fragment_queue_length = 32000;
  source_settings.hosts.push_back("localhost:15000");
  source_settings.hosts.push_back("localhost:15001");
  source_settings.id = 1;
  source_settings.max_fragment_size = 61350;
  source_settings.never_exit = 1;
  source_settings.no_receiving = 1;
  source_settings.no_sending = 1;
  source_settings.on_data_received_logic = "on_data_received_check == 1";
  source_settings.processes = 10;
  source_settings.queue_length = 1500000;
  source_settings.read_threads = 5;
  source_settings.read_thread_hertz = 15000;
  source_settings.reliability = transport::RELIABLE;
  source_settings.send_reduced_message_header = true;
  source_settings.slack_time = 0.2;
  source_settings.type = transport::UDP;

  std::cerr << "Saving QoS Settings...\n";

  source_settings.save("test_qos_settings.kb");

  std::cerr << "Loading QoS Settings...\n";

  loaded_settings.load("test_qos_settings.kb");

  std::cerr << "Checking loaded QoS Settings...\n";

  std::cerr << "  Checking trusted peers... ";
  if(loaded_settings.is_trusted("trusted_website") &&
      loaded_settings.is_trusted("trusted_user") &&
      loaded_settings.is_trusted("trusted_computer"))
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking banned peers... ";
  if(!loaded_settings.is_trusted("banned_website") &&
      !loaded_settings.is_trusted("banned_user") &&
      !loaded_settings.is_trusted("banned_computer"))
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking deadline... ";
  if(loaded_settings.get_deadline() == 10)
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking delay launch... ";
  if(loaded_settings.delay_launch == true)
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking domain... ";
  if(loaded_settings.write_domain == "my_domain")
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking frag queue length... ";
  if(loaded_settings.fragment_queue_length == 32000)
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking hosts... ";
  if(loaded_settings.hosts.size() == 2 &&
      loaded_settings.hosts[0] == "localhost:15000" &&
      loaded_settings.hosts[1] == "localhost:15001")
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking id and processes... ";
  if(loaded_settings.id == 1 && loaded_settings.processes == 10)
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking receive and send suppression... ";
  if(loaded_settings.no_receiving && loaded_settings.no_sending)
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking never exit... ";
  if(loaded_settings.never_exit)
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking read thread settings... ";
  if(loaded_settings.read_threads == 5 &&
      loaded_settings.read_thread_hertz == 15000)
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking queue length and frag size... ";
  if(loaded_settings.max_fragment_size == 61350 &&
      loaded_settings.queue_length == 1500000)
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking reduced header setting... ";
  if(loaded_settings.send_reduced_message_header)
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking type and reliability... ";
  if(loaded_settings.reliability == transport::RELIABLE &&
      loaded_settings.type == transport::UDP)
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }

  std::cerr << "  Checking on data received and slack time... ";
  if(loaded_settings.on_data_received_logic == "on_data_received_check == 1" &&
      loaded_settings.slack_time == 0.2)
  {
    std::cerr << "SUCCESS.\n";
  }
  else
  {
    std::cerr << "FAIL.\n";
  }
}

int main(int, char**)
{
  test_rebroadcast_settings();
  test_peer_list();
  test_filters();
  test_save_and_load();

  return 0;
}
