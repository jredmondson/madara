Author: James Edmondson <jedmondson@gmail.com>

This directory contains example programs that focus on teaching the basics
of using MADARA to build distributed reasoning agents and programs. You can
build these applications by enabling the tutorials feature via the following
command:

======= BUILDING TUTORIALS ON WINDOWS =======
Windows (Visual Studio 2010):

cd %MADARA_ROOT%
mwc.pl -type vc10 -features tutorials=1 MADARA.mwc
(open the MADARA.sln file in the directory and compile with Visual Studio)


======= BUILDING TUTORIALS ON LINUX =======
Linux (g++)

cd $MADARA_ROOT
mwc.pl -type gnuace -features tutorials=1 MADARA.mwc
make -j4 tutorials=1 tests=1

======== Tutorial Descriptions ==========

The tutorials are listed in recommended order of investigation, starting from
the most basic to the most advanced.

1.) hello_world/hello_world.cpp

The Hello World application prompts a user for their name and then replies
"Hello, {name}" back to them.

2.) local_reasoning/bad_career_counsellor.cpp

The Bad Career Counsellor application asks for the user's height and then tries
to make a prediction of what career they should pursue based on three people
that he knows about (Danny DeVito, Napoleon, and Lebron James)

3.) system_primitives/integer_types.cpp

The integer types tutorial looks at using multi-valued integer logics to create
a simple, self-interested agent. The example specificially creates a logical
sequence of KaRL logics for reinforcement learning, including rewards and
punishments.

4.) system_primitives/fractions.cpp

The fractions application models the state of a quantum variable, where truth
is determined on a range from zero (false) to one (true). Everything in between
is a shade of truth. This is a good starting point for users who wish to do
fuzzy logic and other fraction-based applications with the KaRL engine.

5.) system_primitives/functions.cpp

The MADARA KaRL engine provides mechanisms for defining user functions. In this
tutorial, we demonstrate how to define both external C++ functions and MADARA
KaRL logics that can be called from within a KaRL interpreted logic.

6.) system_primitives/files.cpp

The MADARA KaRL engine supports files as first class entities, and this tutorial
demonstrates the basics of reading a file and querying its type and size.

7.) system_calls/system_calls.cpp

A very simple application that is used for general purpose help when working
with MADARA system calls (functions that start with # and cannot be overridden
by users). When tutorials are compiled into MADARA (as shown earlier in this
file), users can call $MADARA_ROOT/bin/system_calls to see a listing of all of
the system calls which can be used in MADARA to work with files, return
substrings, change logging levels, and various other tools.

8.) system_calls/controlling_types.cpp

The controlling types tutorial walks the user through type casting values
into any supported type in the system. Additionally, double precision is
covered, which can be helpful when displaying floating point types. Basics
of logging levels are also covered in this tutorial.

9.) distributed_knowledge/counting_agents.cpp

The MADARA KaRL engine supports a variety of transports for ensuring
consistency across an agent reasoning network. Because the engine is
decentralized and meant for continuous real-time systems that do not
require explicit, blocking reliable transports, it does not support
a blocking transactional variable that would allow each agent to execute
"++.count" to find a total count (this would instead result in a .count
that could range from 1 to N). In this example, we showcase
how to count the number of agents that are online on the network using
the MADARA KaRL engine.

10.) distributed_knowledge/packet_aggregation.cpp

The MADARA KaRL engine aggregates all changes made in an evaluation of a logic
and then sends those changes in bulk to ensure consistency in knowledge. This
feature is completely asynchronous and enforces ordering via Lamport Clocks
instead of transactions (which are blocking, often centralized, and slow). In
this tutorial, we highlight how to create a simple tracking application that
follows agents around a grid with four zones. The agents stay within the grid,
change vectors and can potentially query other agents (this last part is left
as an exercise).

11.) advanced/bandwidth_filtering_send_limit.cpp

The MADARA KaRL engine includes some advanced features like bandwidth filtering
to stop bursty clients from overwhelming the agent network. In this tutorial,
we examine the QoS_Transport_Settings class and use the set_send_bandwidth_limit
function to drop any packet after we hit a send bandwidth limmit.

12.) advanced/bandwidth_filtering_total_limit.cpp

The MADARA KaRL engine includes some advanced features like bandwidth filtering
to stop bursty clients from overwhelming the agent network. In this tutorial,
we examine the QoS_Transport_Settings class and use the set_send_bandwidth_limit
function to drop any packet after we hit a receive bandwidth limmit.

13.) advanced/bandwidth_filtering.cpp

The MADARA KaRL engine includes some advanced features like bandwidth filtering
to stop bursty clients from overwhelming the agent network. In this tutorial,
we examine the QoS_Transport_Settings class and use send and receive filters
to monitor and respond to a bursty client and slow it down at the sender side.
Also included in this tutorial are custom filter creation, compiled expressions,
variable references, and other features that enable fast execution to meet
real-time goals.

14.) advanced/deadline_filtering.cpp

The MADARA KaRL engine includes some advanced features like deadline filtering
to stop bursty clients from overwhelming the agent network. In this tutorial,
we examine the QoS_Transport_Settings class and use send and receive filters
enforce deadlines.

15.) timed_executor/adding_events.cpp

With version 1.5.3, the KaRL engine includes support for timed event processing.
Timed events are KaRL logics that can be queued into the system and executed by
time-sensitive threads with a certain delay, period, and cancellation semantics.
In this tutorial, we create a simple program that queues two timed events,
cancel one of them and runs until a certain condition is met.

======== Additional example programs ==========

Inside of $MADARA_ROOT/tests, there are several interesting applications that
will prove useful in understanding the capabilities, speed, and potential of
the MADARA KaRL engine.


1.) $MADARA_ROOT/tests/profile_architecture.cpp

This test executes commands in tests/settings/profile_expressions.txt multiple
times and guages their execution time in nanoseconds. This file showcases
what operations are expensive (e.g., printing, variable expansion, reading
and writing files, etc.) and can be useful for evaluating timing values for
your own logics (you can add them to profile_expressions.txt and run
$MADARA_ROOT/bin/profile_architecture to see the timing values)

2.) $MADARA_ROOT/tests/test_system_calls.cpp

Unlike the tutorial application which prints the help for all system calls,
the test_system_calls.cpp file is used by MADARA developers to test most
of the system calls before release to make sure they are working. This is
a good reference for MADARA user applications to see if you are using a
system call in the manner it was intended.

3.) $MADARA_ROOT/tests/test_swarm_area_coverage.cpp

This monolithic file contains everything you need to get started with your
own self-interested agent. This particular one contains functions and
scaffold code for an agent solving an area coverage problem and may be
copied and tweaked to your liking (according to the BSD license we have
provided with this code repository, which can be found in the root
directory).

4.) $MADARA_ROOT/tests/transports/test_synchronization.cpp

This distributed test showcases how to use MADARA to establish a
Dijkstra self-stabilizing ring of K-state machines. This is useful for
synchronizing agents, entering critical sections, and self-healing
systems. In the same directory is an implementation of Dijkstra's
3-state self-stabilizing machine which is also uses a distributed KaRL
logic.

5.) $MADARA_ROOT/tests/test_files.cpp

This application creates a file sharing server with KaRL. The agent
that starts with id 0 shares his collection of XML, Text, and image
files with all other agents on the network using a multicast transport.
This is a good starter application for understanding global variables
and files.