# now run functionality unit tests 
echo "Testing basic functionality..."
$MADARA_ROOT/bin/test_any
$MADARA_ROOT/bin/test_bandwidth_monitor
$MADARA_ROOT/bin/test_basic_reasoning
$MADARA_ROOT/bin/test_checkpointing
$MADARA_ROOT/bin/test_circular_buffer
$MADARA_ROOT/bin/test_context_copy
$MADARA_ROOT/bin/test_encoding
$MADARA_ROOT/bin/test_filters
$MADARA_ROOT/bin/test_fragmentation
$MADARA_ROOT/bin/test_karl_containers
$MADARA_ROOT/bin/test_karl_exceptions
$MADARA_ROOT/bin/test_kb_destructions
$MADARA_ROOT/bin/test_key_expansion
$MADARA_ROOT/bin/test_packet_scheduler
$MADARA_ROOT/bin/test_periodic_wait
$MADARA_ROOT/bin/test_prefix_to_map
$MADARA_ROOT/bin/test_print_statement
$MADARA_ROOT/bin/test_save_modifieds
$MADARA_ROOT/bin/test_shared_record
if [[ "$SIMTIME" == "simtime" ]]; then $MADARA_ROOT/bin/test_simtime ; fi
$MADARA_ROOT/bin/test_system_calls
$MADARA_ROOT/bin/test_timed_wait
$MADARA_ROOT/bin/test_utility
$MADARA_ROOT/bin/test_rcw_tracked
$MADARA_ROOT/bin/test_rcw_transaction
$MADARA_ROOT/bin/test_rcw_prodcon
$MADARA_ROOT/bin/test_rcw_custom
$MADARA_ROOT/bin/test_threader_change_hertz
$MADARA_ROOT/bin/test_knowledge_record
$MADARA_ROOT/bin/test_knowledge_base
# performance test (useful to see if we've regressed in performance)
echo "Testing reasoning throughput..."
$MADARA_ROOT/bin/test_reasoning_throughput
# test UDP Multicast networking performance and connectivity for 60s
echo "Testing multicast performance..."
$MADARA_ROOT/bin/network_profiler -i 0 -s 1024 & $MADARA_ROOT/bin/network_profiler -i 1
# test UDP Unicast networking performance and connectivity for 60s
echo "Testing unicast performance..."
$MADARA_ROOT/bin/network_profiler -i 0 -s 1024 -u 127.0.0.1:30000 -u 127.0.0.1:30001 & $MADARA_ROOT/bin/network_profiler -i 1 -u 127.0.0.1:30001
echo "Testing unicast lamport consistency..."
$MADARA_ROOT/bin/test_lamport -i 0 -u 127.0.0.1:30000 -u 127.0.0.1:30001 & $MADARA_ROOT/bin/test_lamport -i 1 -u 127.0.0.1:30001
