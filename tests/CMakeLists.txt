
# compile transport tests

madara_test(network_counter_filter transports/network_counter_filter.cpp)
madara_test(network_profiler transports/network_profiler.cpp)
madara_test(test_barrier transports/test_barrier.cpp)
madara_test(test_counter transports/test_counter.cpp)
madara_test(test_file_rebroadcasts transports/test_file_rebroadcasts.cpp)
madara_test(test_history transports/test_history.cpp)
madara_test(test_lamport transports/test_lamport.cpp)
madara_test(test_modifieds transports/test_modifieds.cpp)
madara_test(test_primitive_types transports/test_primitive_types.cpp)
madara_test(test_qos_transport_settings transports/test_qos_transport_settings.cpp)
madara_test(test_rebroadcast_ring transports/test_rebroadcast_ring.cpp)
madara_test(test_shared_memory_push transports/test_shared_memory_push.cpp)
madara_test(test_synchronization transports/test_synchronization.cpp)
madara_test(test_synchronization_three_state transports/test_synchronization_three_state.cpp)
	
madara_test(test_broadcast transports/broadcast/test_broadcast.cpp)
madara_test(test_broadcast_aggregate_filters transports/broadcast/test_broadcast_aggregate_filters.cpp)
madara_test(test_broadcast_filters transports/broadcast/test_broadcast_filters.cpp)
madara_test(test_broadcast_rebroadcast transports/broadcast/test_broadcast_rebroadcast.cpp)
madara_test(test_broadcast_rules transports/broadcast/test_broadcast_rules.cpp)
	
madara_test(test_multicast transports/multicast/test_multicast.cpp)
madara_test(test_multicast_aggregate_filters transports/multicast/test_multicast_aggregate_filters.cpp)
madara_test(test_multicast_close_and_attach transports/multicast/test_multicast_close_and_attach.cpp)
madara_test(test_multicast_filters transports/multicast/test_multicast_filters.cpp)
	
madara_test(test_multicast_mutex transports/multicast/test_multicast_mutex.cpp)


madara_test(test_multicast_peer_discovery transports/multicast/test_multicast_peer_discovery.cpp)
madara_test(test_multicast_rebroadcast transports/multicast/test_multicast_rebroadcast.cpp)
madara_test(test_multicast_rules transports/multicast/test_multicast_rules.cpp)
madara_test(test_multicast_send_list transports/multicast/test_multicast_send_list.cpp)

madara_test(test_registry transports/registry/test_registry.cpp)
	
madara_test(test_udp transports/udp/test_udp.cpp)
madara_test(test_udp_aggregate_filters transports/udp/test_udp_aggregate_filters.cpp)
madara_test(test_udp_filters transports/udp/test_udp_filters.cpp)
madara_test(test_udp_rebroadcast transports/udp/test_udp_rebroadcast.cpp)
madara_test(test_udp_rules transports/udp/test_udp_rules.cpp)
	
if(ZMQ)
  madara_test(test_zmq transports/zmq/test_zmq.cpp)
  madara_test(test_basic_zmq transports/zmq/test_basic_zmq.cpp)
endif()

# compile splice tests

if(SPLICE)
  madara_test(test_splice transports/splice/test_splice.cpp)
  madara_test(test_splice_filters transports/splice/test_splice_filters.cpp)
  madara_test(test_splice_rebroadcast transports/splice/test_splice_rebroadcast.cpp)
  madara_test(test_splice_rules transports/splice/test_splice_rules.cpp)
endif()

# compile ssl tests

if(SSL)
  madara_test(test_aes_256 ssl/test_aes_256.cpp)
  madara_test(test_multicast_ssl transports/multicast/test_multicast_ssl.cpp)
endif()

# compile pugi xml tests

madara_test(test_pugi test_pugi.cpp)

# compile threader tests


madara_test(test_threader_change_hertz threads/test_threader_change_hertz.cpp)
madara_test(test_threader_counter threads/test_threader_counter.cpp)
madara_test(test_threader_hello_world threads/test_threader_hello_world.cpp)
madara_test(test_threader_introspection threads/test_threader_introspection.cpp)
madara_test(test_threader_periodic_counter threads/test_threader_periodic_counter.cpp)
madara_test(test_threader_producer_consumer threads/test_threader_producer_consumer.cpp)
madara_test(test_threader_queue_perf threads/test_threader_queue_perf.cpp)
madara_test(test_threader_wait_for_pause threads/test_threader_wait_for_pause.cpp)

# compile general MADARA tests

madara_test(profile_architecture profile_architecture.cpp)

madara_test(test_arrays test_arrays.cpp)
madara_test(test_bandwidth_monitor test_bandwidth_monitor.cpp)
madara_test(test_basic_reasoning test_basic_reasoning.cpp)
madara_test(test_checkpointing test_checkpointing.cpp)
madara_test(test_circular_buffer test_circular_buffer.cpp)
madara_test(test_context_copy test_context_copy.cpp)
madara_test(test_encoding test_encoding.cpp)
madara_test(test_evaluate test_evaluate.cpp)
madara_test(test_files test_files.cpp)
madara_test(test_filters test_filters.cpp)
madara_test(test_fragmentation test_fragmentation.cpp)
madara_test(test_get_record test_get_record.cpp)
madara_test(test_karl_containers test_karl_containers.cpp)
madara_test(test_karl_exceptions test_karl_exceptions.cpp)
madara_test(test_kb_destructions test_kb_destructions.cpp)
madara_test(test_key_expansion test_key_expansion.cpp)
madara_test(test_knowledge_base test_knowledge_base.cpp)
madara_test(test_knowledge_record test_knowledge_record.cpp)
madara_test(test_logging test_logging.cpp)
madara_test(test_packet_scheduler test_packet_scheduler.cpp)
madara_test(test_periodic_wait test_periodic_wait.cpp)
madara_test(test_prefix_to_map test_prefix_to_map.cpp)
madara_test(test_print_statement test_print_statement.cpp)
madara_test(test_reasoning_throughput test_reasoning_throughput.cpp)
madara_test(test_safe_bool test_safe_bool.cpp)
madara_test(test_save_modifieds test_save_modifieds.cpp)
madara_test(test_shared_record test_shared_record.cpp)
madara_test(test_system_calls test_system_calls.cpp)
madara_test(test_timed_wait test_timed_wait.cpp)
madara_test(test_utility test_utility.cpp)

# compile simtime tests

if(SIMTIME)
  madara_test(test_simtime test_simtime.cpp)
endif()
