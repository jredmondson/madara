import madara

# zmq
# $MADARA_ROOT/bin/mfs -0f $MADARA_ROOT/examples/settings/mfs_sandboxes.mf --zmq 127.0.0.1:40001 --zmq 127.0.0.1:40000 --zmq 127.0.0.1:40002 -q 12000000
# $MADARA_ROOT/bin/karl -y 0.1 -ky -t 15 --zmq 127.0.0.1:40000 --zmq 127.0.0.1:40001 --zmq 127.0.0.1:40002 ".var='agent.0.sync.sandbox.files.file.samples/chapter6.mp3';{.var}=1"

# multicast
# $MADARA_ROOT/bin/mfs -0f $MADARA_ROOT/examples/settings/mfs_sandboxes.mf -m 239.255.0.1:4150 -q 12000000 -esb 500000
# $MADARA_ROOT/bin/karl -y 0.1 -ky -t 15 -m 239.255.0.1:4150 ".var='agent.0.sync.sandbox.files.file.samples/chapter6.mp3';{.var}=1"


import sys
import time
import madara.knowledge as engine
import madara.transport as transport
import madara.filters as filters

# create transport settings for a multicast transport
settings = transport.QoSTransportSettings()
#settings.hosts.append("239.255.0.1:4150")
#settings.type = transport.TransportTypes.MULTICAST
settings.type = transport.TransportTypes.UDP
#settings.hosts.append("127.0.0.1:40002")
settings.hosts.append("127.0.0.1:40001")
settings.hosts.append("127.0.0.1:40000")
#settings.type = transport.TransportTypes.ZMQ
settings.queue_length = 12000000

# create a filter 
filter = filters.FragmentsToFilesFilter()
filter.set_dir_mapping("agent.0.sandbox.files.file", "files")

settings.add_receive_filter(filter)

# create a knowledge base with the multicast transport settings
kb = engine.KnowledgeBase("agent1", settings)

sleep_time = 180
filename = "ros2gams_bigbag.sbz"

if len(sys.argv) > 1:
  filename = sys.argv[1]
  print ("Changing file name to ", filename)

if len(sys.argv) > 2:
  sleep_time = float (sys.argv[2])
  print ("Changing sleep time to ", sleep_time)

print ("keys will be read from ", "agent.0.sandbox.files.file.", filename)

requester = madara.knowledge.FileRequester ()
requester.init ("agent.0.sandbox.files.file." + filename,
  "agent.0.sync.sandbox.files.file." + filename, "files/" + filename, kb, 500)

print ("Beginning requests for ", filename)

print ("Fragments are currently: ", madara.to_pylongs (requester.build_fragment_request ()))

requester.needs_request ()
time.sleep (3)

while requester.needs_request ():
  print ("Percent complete is ", requester.get_percent_complete (), "Frag list is ", madara.to_pylongs (requester.build_fragment_request ()))
  kb.send_modifieds ()
  time.sleep (1)
