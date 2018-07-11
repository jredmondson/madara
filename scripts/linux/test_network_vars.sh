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
$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 10 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 100 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 1000 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 10000 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000

# UDP Tests
$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 10 -u 127.0.0.1:30000 -u 127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 -u 127.0.0.1:30001 -u 127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 100 -u 127.0.0.1:30000 -u 127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 -u 127.0.0.1:30001 -u 127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 1000 -u 127.0.0.1:30000 -u 127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 -u 127.0.0.1:30001 -u 127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 10000 -u 127.0.0.1:30000 -u 127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 -u 127.0.0.1:30001 -u 127.0.0.1:30000

# ZeroMQ TCP Tests

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 10 --zmq tcp://127.0.0.1:30000 --zmq tcp://127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 --zmq tcp://127.0.0.1:30001 --zmq tcp://127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 100 --zmq tcp://127.0.0.1:30000 --zmq tcp://127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 --zmq tcp://127.0.0.1:30001 --zmq tcp://127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 1000 --zmq tcp://127.0.0.1:30000 --zmq tcp://127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 --zmq tcp://127.0.0.1:30001 --zmq tcp://127.0.0.1:30000

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 10000 --zmq tcp://127.0.0.1:30000 --zmq tcp://127.0.0.1:30001 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 --zmq tcp://127.0.0.1:30001 --zmq tcp://127.0.0.1:30000

rm /tmp/network_profiler_*

# ZeroMQ IPC Tests

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 10 --zmq ipc:///tmp/network_profiler_6_0 --zmq ipc:///tmp/network_profiler_6_1 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 --zmq ipc:///tmp/network_profiler_6_1 --zmq ipc:///tmp/network_profiler_6_0

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 100 --zmq ipc:///tmp/network_profiler_6_0 --zmq ipc:///tmp/network_profiler_6_1 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 --zmq ipc:///tmp/network_profiler_6_1 --zmq ipc:///tmp/network_profiler_6_0

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 1000 --zmq ipc:///tmp/network_profiler_6_0 --zmq ipc:///tmp/network_profiler_6_1 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 --zmq ipc:///tmp/network_profiler_6_1 --zmq ipc:///tmp/network_profiler_6_0

$MADARA_ROOT/bin/network_profiler -i 0 -s 500000 -q 50000000 --num-vars 10000 --zmq ipc:///tmp/network_profiler_6_0 --zmq ipc:///tmp/network_profiler_6_1 &
$MADARA_ROOT/bin/network_profiler -i 1 -q 50000000 --zmq ipc:///tmp/network_profiler_6_1 --zmq ipc:///tmp/network_profiler_6_0




