cmake_minimum_required(VERSION 3.12)

# Read VERSION.txt and set MADARA_VERSION variables
file(STRINGS "VERSION.txt" MADARA_VERSION)
string(REPLACE "." ";" VERSION_LIST ${MADARA_VERSION})
list(GET VERSION_LIST 0 MADARA_MAJOR_VERSION)
list(GET VERSION_LIST 1 MADARA_MINOR_VERSION)
list(GET VERSION_LIST 2 MADARA_PATCH_VERSION)

# Get helper cmake macros
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(madara_macros)
  
# Define the MADARA project and version
project(MADARA VERSION "${MADARA_VERSION}")

# Provide a lowercase version of project
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

set(MADARA_CMAKE_DIR ${PROJECT_SOURCE_DIR}/cmake CACHE PATH "Location of CMake scripts")
set(MADARA_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include CACHE PATH "Location of MADARA source")
set(MADARA_BIN_DIR ${PROJECT_SOURCE_DIR}/bin CACHE PATH "Location of MADARA scripts and binaries")
set(MADARA_LIB_DIR ${PROJECT_SOURCE_DIR}/lib CACHE PATH "Location of MADARA libraries")

set(CMAKE_DEBUG_POSTFIX d)

########################################
# Package Creation: 
include (${MADARA_CMAKE_DIR}/madara_cpack.cmake)
set (CPACK_PACKAGE_VERSION "${MADARA_VERSION}")
set (CPACK_PACKAGE_VERSION_MAJOR "${MADARA_MAJOR_VERSION}")
set (CPACK_PACKAGE_VERSION_MINOR "${MADARA_MINOR_VERSION}")
set (CPACK_PACKAGE_VERSION_PATCH "${MADARA_PATCH_VERSION}")

if (CPACK_GENERATOR)
  message(STATUS "Found CPack generators: ${CPACK_GENERATOR}")

  configure_file("${MADARA_CMAKE_DIR}/cpack_options.cmake.in" ${MADARA_CPACK_CFG_FILE} @ONLY)
  set(CPACK_PROJECT_CONFIG_FILE ${MADARA_CPACK_CFG_FILE})
  include (CPack)
endif()


# Use GNUInstallDirst to get canonical paths
include(GNUInstallDirs)

set (CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)


# developer's option to cache PKG_CONFIG_PATH and
# LD_LIBRARY_PATH for local installs
if(PKG_CONFIG_PATH)
  set (ENV{PKG_CONFIG_PATH} ${PKG_CONFIG_PATH}:$ENV{PKG_CONFIG_PATH})
endif()
if(LD_LIBRARY_PATH)
  set (ENV{LD_LIBRARY_PATH} ${LD_LIBRARY_PATH}:$ENV{LD_LIBRARY_PATH})
endif()

set (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/madara-${MADARA_VERSION}/madara") 
set (LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
set (BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
    

# Options
option(ANDROID "Build with Android support" OFF)
option(DOCS "Run doxygen or javadocs on code" OFF)
option(KARL "Build with KaRL scripting language support (eval/wait)" ON)
option(LZ4 "Build with LZ4 support" OFF)
option(NDDS "Build with RTI NDDS support" OFF)
option(PYTHON "Build with python support (via Boost.Python)" OFF)
option(SIMTIME "Build with support for simulation/virtual time" ON)
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

if(ANDROID)
  add_definitions(-D_MADARA_ANDROID_ -D__STDC_FORMAT_MACROS)
endif()

if(LZ4)
  add_definitions(-D_USE_LZ4_)
endif()

if(JAVA)
  add_definitions(-D_MADARA_JAVA_)
endif()

if(NOT KARL)
  add_definitions(-D_MADARA_NO_KARL_)
endif()

if(NDDS)
  add_definitions(-D_MADARA_NDDS_)
endif()

if(PYTHON)
  add_definitions(-D_MADARA_PYTHON_CALLBACKS_)
endif()

if(SIMTIME)
  add_definitions(-DMADARA_FEATURE_SIMTIME)
endif()

if(SSL)
  add_definitions(-D_USE_SSL_)
endif()

if(ZMQ)
  add_definitions(-D_MADARA_USING_ZMQ_)
endif()

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
add_subdirectory(tools)

# Build tests
if(TESTS)
  add_subdirectory(tests)
endif()

# Build tutorials
if(TUTORIALS)
  add_subdirectory(tutorials)
endif()

if(DOCS)
  add_subdirectory(docs)
endif()


set(PKG_NAME ${PROJECT_NAME_UPPER})
set(PKG_LIBRARIES madara)
set(PKG_DEPENDS Boost)

install(FILES "${PROJECT_SOURCE_DIR}/VERSION.txt" DESTINATION .)
install(FILES "${PROJECT_SOURCE_DIR}/LICENSE.txt" DESTINATION .)
