cmake_minimum_required(VERSION 3.12)

# Read VERSION.txt and set MADARA_VERSION variables
file(STRINGS "VERSION.txt" MADARA_VERSION)
string(REPLACE "." ";" VERSION_LIST ${MADARA_VERSION})
list(GET VERSION_LIST 0 MADARA_MAJOR_VERSION)
list(GET VERSION_LIST 1 MADARA_MINOR_VERSION)
list(GET VERSION_LIST 2 MADARA_PATCH_VERSION)

# Define the MADARA project and version
project(MADARA VERSION "${MADARA_VERSION}")

# Provide a lowercase version of project
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

set(MADARA_CMAKE_DIR ${PROJECT_SOURCE_DIR}/cmake CACHE PATH "Location of CMake scripts")
set(MADARA_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include CACHE PATH "Location of MADARA source")
set(MADARA_BIN_DIR ${PROJECT_SOURCE_DIR}/bin CACHE PATH "Location of MADARA scripts and binaries")
set(MADARA_LIB_DIR ${PROJECT_SOURCE_DIR}/lib CACHE PATH "Location of MADARA libraries")


# Options
option(ANDROID "Build with Android support" OFF)
option(DOCS "Run doxygen or javadocs on code" OFF)
option(KARL "Build with KaRL scripting language support (eval/wait)" ON)
option(LZ4 "Build with LZ4 support" OFF)
option(NDDS "Build with RTI NDDS support" OFF)
option(PYTHON "Build with python support (via Boost.Python)" OFF)
option(SIMTIME "Build with support for simulation/virtual time" OFF)
option(SPLICE "Build with PrismTech OpenSplice DDS support" OFF)
option(SSL "Build with SSL support" OFF)
option(TESTS "Build tests" OFF)
option(THREADLOCAL "Build with support for threadlocal variables" OFF)
option(TUTORIALS "Build tutorials" OFF)
option(XML "Build with XML library support" OFF)
option(ZMQ "Build with ZMQ/0MQ support" OFF)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_custom_command(
  OUTPUT ${MADARA_INCLUDE_DIR}/Version.h
  COMMAND ${MADARA_BIN_DIR}/generate_version_include.pl
)

if(THREADLOCAL)
  add_definitions(-DMADARA_THREAD_LOCAL)
else()
  add_definitions(-DMADARA_NO_THREAD_LOCAL)
endif()

# disable certain warnings that will occur in VS
if(MSVC)
  add_compile_options(/wd4005)
  add_compile_options(/wd4251)
  add_compile_options(/wd4275)
  add_compile_options(/wd4244)
  add_compile_options(/wd4297)
  add_compile_options(/wd4996)
  add_compile_options(/MP)
  
  # boost on windows is built into a stage/lib directory,
  # so let's help the Find utility find Boost
  set(ENV{Boost_LIBRARY_DIR} ENV{BOOST_ROOT}/stage/lib)
  
# make linux/mac compilation stricter
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()
	
# Build the main library
add_subdirectory(include)

# Build tests
if(TESTS)
  add_subdirectory(tests)
endif()

# Build tutorials
if(TUTORIALS)
  add_subdirectory(tutorials)
endif()
