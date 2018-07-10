#!/bin/sh

ZMQ=0

if [ ! -z $1 ] && ( [ "$1" = "--zmq" ] || [ "$1" = "--0mq" ] || 
                    [ "$1" = "zmq" ] ) ; then
  ZMQ=$1
fi

if [ ! -z $1 ] && ( [ "$1" = "-h" ] || [ "$1" = "--help" ] ) ; then
  echo "Usage for $0 <options>:"
  echo ""
  echo "  zmq|--zmq|--0mq: include tests for ZeroMQ"
  echo ""
  exit 0
fi

# Multicast Tests
$MADARA_ROOT/bin/network_profiler -i 0 -s 128 -q 1000000 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000

$MADARA_ROOT/bin/network_profiler -i 0 -s 1024 -q 1000000 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000

$MADARA_ROOT/bin/network_profiler -i 0 -s 8196 -q 1000000 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000

$MADARA_ROOT/bin/network_profiler -i 0 -s 16384 -q 1000000 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000

$MADARA_ROOT/bin/network_profiler -i 0 -s 64000 -q 1000000 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 1000000 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000

# UDP Tests
$MADARA_ROOT/bin/network_profiler -i 0 -s 128 -q 1000000 -u 127.0.0.1:30000 -u 127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 -u 127.0.0.1:30001 -u 127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 1024 -q 1000000 -u 127.0.0.1:30000 -u 127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 -u 127.0.0.1:30001 -u 127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 8196 -q 1000000 -u 127.0.0.1:30000 -u 127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 -u 127.0.0.1:30001 -u 127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 16384 -q 1000000 -u 127.0.0.1:30000 -u 127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 -u 127.0.0.1:30001 -u 127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 64000 -q 1000000 -u 127.0.0.1:30000 -u 127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 -u 127.0.0.1:30001 -u 127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 1000000 -u 127.0.0.1:30000 -u 127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 -u 127.0.0.1:30001 -u 127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 -u 127.0.0.1:30000 -u 127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 -u 127.0.0.1:30001 -u 127.0.0.1:30000

# ZeroMQ TCP Tests
$MADARA_ROOT/bin/network_profiler -i 0 -s 128 -q 1000000 --zmq tcp://127.0.0.1:30000 --zmq tcp://127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 --zmq tcp://127.0.0.1:30001 --zmq tcp://127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 1024 -q 1000000 --zmq tcp://127.0.0.1:30000 --zmq tcp://127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 --zmq tcp://127.0.0.1:30001 --zmq tcp://127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 8196 -q 1000000 --zmq tcp://127.0.0.1:30000 --zmq tcp://127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 --zmq tcp://127.0.0.1:30001 --zmq tcp://127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 16384 -q 1000000 --zmq tcp://127.0.0.1:30000 --zmq tcp://127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 --zmq tcp://127.0.0.1:30001 --zmq tcp://127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 64000 -q 1000000 --zmq tcp://127.0.0.1:30000 --zmq tcp://127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 --zmq tcp://127.0.0.1:30001 --zmq tcp://127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 1000000 --zmq tcp://127.0.0.1:30000 --zmq tcp://127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 --zmq tcp://127.0.0.1:30001 --zmq tcp://127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --zmq tcp://127.0.0.1:30000 --zmq tcp://127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 --zmq tcp://127.0.0.1:30001 --zmq tcp://127.0.0.1:30000

rm /tmp/network_profiler_*

# ZeroMQ IPC Tests
$MADARA_ROOT/bin/network_profiler -i 0 -s 128 -q 1000000 --zmq ipc:///tmp/network_profiler_0_0 --zmq ipc:///tmp/network_profiler_0_1 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 --zmq ipc:///tmp/network_profiler_0_1 --zmq ipc:///tmp/network_profiler_0_0

$MADARA_ROOT/bin/network_profiler -i 0 -s 1024 -q 1000000 --zmq ipc:///tmp/network_profiler_1_0 --zmq ipc:///tmp/network_profiler_1_1 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 --zmq ipc:///tmp/network_profiler_1_1 --zmq ipc:///tmp/network_profiler_1_0

$MADARA_ROOT/bin/network_profiler -i 0 -s 8196 -q 1000000 --zmq ipc:///tmp/network_profiler_2_0 --zmq ipc:///tmp/network_profiler_2_1 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 --zmq ipc:///tmp/network_profiler_2_1 --zmq ipc:///tmp/network_profiler_2_0

$MADARA_ROOT/bin/network_profiler -i 0 -s 16384 -q 1000000 --zmq ipc:///tmp/network_profiler_3_0 --zmq ipc:///tmp/network_profiler_3_1 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 --zmq ipc:///tmp/network_profiler_3_1 --zmq ipc:///tmp/network_profiler_3_0

$MADARA_ROOT/bin/network_profiler -i 0 -s 64000 -q 1000000 --zmq ipc:///tmp/network_profiler_4_0 --zmq ipc:///tmp/network_profiler_4_1 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 --zmq ipc:///tmp/network_profiler_4_1 --zmq ipc:///tmp/network_profiler_4_0

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 1000000 --zmq ipc:///tmp/network_profiler_5_0 --zmq ipc:///tmp/network_profiler_5_1 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 1000000 --zmq ipc:///tmp/network_profiler_5_1 --zmq ipc:///tmp/network_profiler_5_0

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --zmq ipc:///tmp/network_profiler_6_0 --zmq ipc:///tmp/network_profiler_6_1 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 --zmq ipc:///tmp/network_profiler_6_1 --zmq ipc:///tmp/network_profiler_6_0




