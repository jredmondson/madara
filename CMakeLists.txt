
cmake_minimum_required(VERSION 3.10)

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

########################################
# Package Creation: 
include(${MADARA_CMAKE_DIR}/madara_cpack.cmake)
set(CPACK_PACKAGE_VERSION "${MADARA_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MADARA_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${MADARA_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${MADARA_PATCH_VERSION}")
set(CPACK_INSTALL_CMAKE_PROJECTS
  "debug;madara;ALL;/"
  "release;madara;ALL;/"
)

if(CPACK_GENERATOR)
  message(STATUS "Found CPack generators: ${CPACK_GENERATOR}")

  configure_file("${MADARA_CMAKE_DIR}/cpack_options.cmake.in" ${MADARA_CPACK_CFG_FILE} @ONLY)
  set(CPACK_PROJECT_CONFIG_FILE ${MADARA_CPACK_CFG_FILE})
  include(CPack)
endif()

set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)


# developer's option to cache PKG_CONFIG_PATH and
# LD_LIBRARY_PATH for local installs
if(PKG_CONFIG_PATH)
  set(ENV{PKG_CONFIG_PATH} ${PKG_CONFIG_PATH}:$ENV{PKG_CONFIG_PATH})
endif()
if(LD_LIBRARY_PATH)
  set(ENV{LD_LIBRARY_PATH} ${LD_LIBRARY_PATH}:$ENV{LD_LIBRARY_PATH})
endif()

set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/madara-${MADARA_VERSION}/madara") 
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
set(BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})
    
# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# disable certain warnings that will occur in VS
if(MSVC)

  set(MADARA_INC_INSTALL_DIR "include")
  set(MADARA_RUNTIME_INSTALL_DIR   "bin")
  set(MADARA_LIBRARY_INSTALL_DIR   "bin")
  set(MADARA_ARCHIVE_INSTALL_DIR   "lib")
  set(MADARA_FRAMEWORK_INSTALL_DIR "bin")

  set(MADARA_CMAKE_CONFIG_INSTALL_DIR "share/madara/cmake")
  set(MADARA_ADDITIONAL_FILES_INSTALL_DIR "share/madara")

  add_compile_options(/wd4005)
  add_compile_options(/wd4251)
  add_compile_options(/wd4275)
  add_compile_options(/wd4244)
  add_compile_options(/wd4297)
  add_compile_options(/wd4996)
  add_compile_options(/MP)
  
# make linux/mac compilation stricter
elseif(UNIX)
  include(GNUInstallDirs)

  set(MADARA_INC_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/madara")
  set(MADARA_RUNTIME_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
  set(MADARA_LIBRARY_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
  set(MADARA_ARCHIVE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
  set(MADARA_FRAMEWORK_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")

  set(MADARA_CMAKE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/madara/cmake")
  set(MADARA_ADDITIONAL_FILES_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/madara")

  set(MADARA_RUNTIME_INSTALL_DIR "bin")
  set(MADARA_INC_INSTALL_DIR "include") # header filestree

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()
	
include(cmake/configuration.cmake)

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
set(PKG_DEPENDS Boost::boost)

# install readme and license
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt" "${CMAKE_CURRENT_SOURCE_DIR}/VERSION.txt" "${CMAKE_CURRENT_SOURCE_DIR}/README.md" DESTINATION ${MADARA_ADDITIONAL_FILES_INSTALL_DIR})

install(EXPORT madaraTargets NAMESPACE madara:: 
  DESTINATION ${MADARA_CMAKE_CONFIG_INSTALL_DIR}
  FILE madara-config.cmake)
  