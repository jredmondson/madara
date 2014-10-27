INTRODUCTION

This folder contains the Python port for MADARA. These instructions will
explain how to install and use the Python port.

AUTHOR(s)

James Edmondson <jedmondson@gmail.com>

PREREQUISITES

1.) Python

  1.a) Obtain python and any dev libraries (Linux users require this step)
  1.b) Set environment variables
    
       PYTHON_ROOT    The location of your Python install folder
       PYTHON_VERSION The major and minor version number (e.g. 33 for 3.3)
    
       PYTHON_VERSION is used to locate the python{PYTHON_VERSION).lib
       On Linux, this is likely to be something like 3.3 or 2.7 (see
       /usr/lib/libpython* for information). On Windows, this is likely
       to be something like 33 or 27.

2.) Boost.Python

  2.a) Set environment variables
    
       BOOST_ROOT      The location of your Boost install folder
       BOOST_VERSION   The version number (e.g. 1_55)
         
       BOOST_VERSION is used to locate the boost.python library.
       On Linux, this is likely to be something like py32 or py33 (see
       /usr/lib/libboost* for information). On Windows, this is likely
       to be something like 1_32 or 1_33. If installing from a package
       manager into /usr/lib, BOOST_ROOT and PYTHON_ROOT are
       unnecessary on Linux.
    
  2.b) Setup your user-config.jam file in your home directory
    
       Example on Windows in C:\Users\<username>, using VS 2010
       and Python version 3.3 (32 bit)
         
         using msvc : 10.0 ;
         using python
           : 3.3
           : C:/Python33/32bit
         ;
         
  2.c) Compile the Boost.Python Library
 
       1. cd to BOOST_ROOT
       2. Run bootstrap in the local directory
       3. Run "b2 link=shared threading=multi stage"
       4. You can also run b2 with static linking if you want a static lib
         
3.) Enable the python feature in your MADARA install

    LINUX:
    
      cd $MADARA_ROOT
      mwc.pl -type gnuace -features python=1 MADARA.mwc
      make tests=1 python=1
      
    WINDOWS:
    
      cd %MADARA_ROOT%
      mwc.pl -type vc10 -features python=1 MADARA.mwc
      *open Visual Studio solution*
 
4.) Integrate the generated madara module into your Python environment

  4.a) On Windows, in %MADARA_ROOT%/lib, rename the generated madara.pyd.dll
       file to madara.pyd. If you have statically compiled, rename
       madara.pyd.lib to madara.pyd. On Linux, your module should be
       generated as madara.so, which is built for Python and should not
       be used interchangeably with MADARA.so (the C++ shared library)

  4.b) Set env var PYTHONPATH to include the following directories:
  
       $MADARA_ROOT/lib
       $BOOST_ROOT/stage/lib
       
  4.c) Type the following from a new command prompt (have to refresh the
       PYTHONPATH variable):
       
       $> python
       >>> help ('modules')
       
       Check the printout to see if "madara" is listed in the available modules
       
  4.d) Import the module and use the available classes
  
       $> python
       >>> import madara
       >>> knowledge = madara.Knowledge_Engine.Knowledge_Base ()
       >>> knowledge.print ()
       
       
