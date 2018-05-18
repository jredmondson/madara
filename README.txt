0. ABOUT MADARA

  The Multi-Agent Distributed Adaptive Research Allocation (MADARA) suite is a
  collection of middleware and tools for real-time system developers. Included
  in MADARA is a state-of-the-art knowledge and reasoning engine (KaRL) that
  is capable of evaluating over 5 million knowledge operations per second on
  a single core, a deployment and automated testing suite (KATS) that can
  perform distributed testing with microsecond precision, two APIs for
  instrumenting Android smartphones (MAML and MAAL), and a suite of heuristics
  and genetic algorithms for approximating certain types of constraint
  satisfaction problems within milliseconds.

  These tools are released under a BSD license which should be included in
  this directory. If you have any questions, comments, or concerns, please
  feel free to contact our development team. You can either email me directly
  at the email address below, or you can file an issue on our project site,
  the url of which is also included below.

  Thanks,

  James Edmondson (jedmondson@gmail.com)
  MADARA Project (http://madara.googlecode.com)

1. GETTING STARTED WITH YOUR FIRST PROJECT

  If you've run the Windows installer, or you've followed the installation
  instructions for Windows or Linux installations, you already have everything
  you need to run MAML, MAAL, and the blackbox testing portions of KATS. But
  there's so much more to MADARA!

  Let's get you into a MADARA KaRL "Hello World" knowledge and reasoning
  application!

  1.a. FOR THOSE WHO USED THE WINDOWS INSTALLER

    If you've used the Windows installer, we've included ACE and Open Splice
    DDS inside of the MADARA installation directories. This should make
    setting up your project a bit easier. If you're copy of Visual Studio
    was running during the install process, try closing it and restarting
    it to reload environment variables. Then try doing the following:

    1.) Create a new Win32 Console Application with "HelloWorld" as the
        Name. Press OK and then Press Next.
    2.) Uncheck "Precompiled header" and then check "Empty Project". Press
        the Finish button. This should create a new Project called
        "HelloWorld"
    3.) Right click on "Source Files" and click "Add New Item". Select a
        new CPP file, name it "HelloWorld" and copy and paste the following:


#include "madara/knowledge/Knowledge_Base.h"

int main (int argc, char * argv[])
{
  madara::knowledge::Knowledge_Base knowledge;

  knowledge.evaluate ("++hello");
  knowledge.evaluate ("++world");

  knowledge.print ("hello={hello} world={world}\n");

  return 0;
}


    4.) Now, you should be in debug solution mode, which is perfect for early
        development. Let's go ahead and setup the include and library dirs to
        reference MADARA and ACE.
    5.) Right click your project "HelloWorld" and go to Properties. Click
        C/C++ under the Configuration Properties and then add
        $(MADARA_ROOT)/include to the Additional Include Directories.
    6.) ACE uses a bunch of stdlib functions which cause Visual Studio
        to issue a lot of warnings. Let's suppress those with some
        preprocessor definitions. Expand the C/C++->Preprocessor tab and and
        add the following to "Preprocessor Definitions" (click the little
        "..." button to the right on that line to open up an editor).

        _CRT_NONSTDC_NO_WARNINGS
        _CRT_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_DEPRECATE

    7.) Click Linker->General and add $(MADARA_ROOT)\lib to Additional Library
        Directories
    8.) Click Linker->Input and add "MADARAd.lib, ACEd.lib" to Additional
        Dependencies.
    9.) Now, switch to Release Mode (the dropdown next to the one that says
        "Win32") and repeat steps 5-7. Go to Linker->Input and add
        "MADARA.lib, ACE.lib" to Additional Dependencies.
    10.) You've now configured your project for both Debug and Release modes
        in Visual Studio. Go ahead and build your project by pressing F7.


  1.b. FOR THOSE WHO BUILT MADARA AND INSTALLED ACE 6.0.1 AND OPENSPLICE 5.4.1
       ON WINDOWS


    1.) Create a new Win32 Console Application with "HelloWorld" as the
        Name. Press OK and then Press Next.
    2.) Uncheck "Precompiled header" and then check "Empty Project". Press
        the Finish button. This should create a new Project called
        "HelloWorld"
    3.) Right click on "Source Files" and click "Add New Item". Select a
        new CPP file, name it "HelloWorld" and copy and paste the following:


#include "madara/knowledge/Knowledge_Base.h"

int ACE_TMAIN (int argc, char * argv[])
{
  madara::knowledge::Knowledge_Base knowledge;

  knowledge.evaluate ("++hello");
  knowledge.evaluate ("++world");

  knowledge.print ("hello={hello} world={world}\n");

  return 0;
}


    4.) Now, you should be in debug solution mode, which is perfect for early
        development. Let's go ahead and setup the include and library dirs to
        reference MADARA and ACE.
    5.) Right click your project "HelloWorld" and go to Properties. Click
        C/C++ under the Configuration Properties and then add
        "$(MADARA_ROOT)/include, $(ACE_ROOT)" to the Additional Include
        Directories.
    6.) ACE uses a bunch of stdlib functions which cause Visual Studio
        to issue a lot of warnings. Let's suppress those with some
        preprocessor definitions. Expand the C/C++->Preprocessor tab and and
        add the following to "Preprocessor Definitions" (click the little
        "..." button to the right on that line to open up an editor).

        _CRT_NONSTDC_NO_WARNINGS
        _CRT_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_DEPRECATE

    7.) Click Linker->General and add $(MADARA_ROOT)\lib, $(ACE_ROOT)\lib
        to Additional Library Directories
    8.) Click Linker->Input and add "MADARAd.lib, ACEd.lib" to Additional
        Dependencies.
    9.) Now, switch to Release Mode (the dropdown next to the one that says
        "Win32") and repeat steps 5-7. Go to Linker->Input and add
        "MADARA.lib, ACE.lib" to Additional Dependencies.
    10.) You've now configured your project for both Debug and Release modes
        in Visual Studio. Go ahead and build your project by pressing F7.


  1.c. FOR THOSE WHO BUILT MADARA AND INSTALLED ACE 6.0.1 AND OPENSPLICE 5.4.1
       ON LINUX

    1.) Make sure you've set your environment variables as indicated in the
        Installation Wiki. Most notably, ensure that your LD_LIBRARY_PATH
        now includes $MADARA_ROOT/lib and $ACE_ROOT/lib.
    2.) Create a file called HelloWorld.cpp and copy and paste the following:

#include "madara/knowledge/Knowledge_Base.h"

int ACE_TMAIN (int argc, char * argv[])
{
  madara::knowledge::Knowledge_Base knowledge;

  knowledge.evaluate ("++hello");
  knowledge.evaluate ("++world");

  knowledge.print ("hello={hello} world={world}\n");

  return 0;
}

    3.) Use your favorite compiler (e.g., g++) and add an include for
        $MADARA_ROOT/include and $ACE_ROOT.

  2. HOW FAST CAN KARL RUN ON MY COMPUTER?

    We include several tests in the $MADARA_ROOT/bin directory that you
    can use to test KaRL on your system. To run the throughput tests for
    KaRL on your system, try the following:

      test_reasoning_throughput

    If that doesn't work, try the following:

      $MADARA_ROOT/bin/test_reasoning_throughput (on Linux)

      or

      %MADARA_ROOT%/bin/test_reasoning_throughput (on Windows)

   3. HOW DO I START DISSEMINATING KNOWLEDGE OVER A NETWORK

     We include several different transports including IP multicast, IP
     broadcast, Prism Tech Open Splice DDS, and RTI DDS. Tests for IP-based
     protocols can be found inside of tests/transports. To get started, try
     looking at test_primitive_types.cpp, which shows how to modify strings,
     integers, and doubles and read them across the network using IP multicast.

     Other tests that may be interesting are test_synchronization, which forms
     a self-healing network of processes using Dijkstra's N-state self-
     stabilizing algorithm, and test_synchronization_three_state, which uses
     Dijkstra's three-state self-stabilizing algorithm.


