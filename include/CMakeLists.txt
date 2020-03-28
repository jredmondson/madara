
# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

cmake_minimum_required(VERSION 3.12)

# Optionally glob, but only for CMake 3.12 or later:
# file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${ModernCMakeExample_SOURCE_DIR}/include/modern/*.hpp")

set(header_path ${MADARA_SOURCE_DIR}/include/madara)

file(GLOB_RECURSE MADARA_SRC_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/include/*.c
    ${PROJECT_SOURCE_DIR}/include/*.cpp 
)

file(GLOB_RECURSE MADARA_HEADER_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/include/*.h
    ${PROJECT_SOURCE_DIR}/include/*.inl
)

# remove all files in the ndds tree if it isn't feature-enabled
if(NOT NDDS)
  set(EXCLUDE_DIR "/ndds/")
  foreach(TMP_PATH ${MADARA_SRC_FILES})
      string(FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
      if(NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
          list(REMOVE_ITEM MADARA_SRC_FILES ${TMP_PATH})
      endif()
  endforeach(TMP_PATH)
  
  foreach(TMP_PATH ${MADARA_HEADER_FILES})
      string(FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
      if(NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
          list(REMOVE_ITEM MADARA_HEADER_FILES ${TMP_PATH})
      endif()
  endforeach(TMP_PATH)
endif()

# remove all files in the splice tree if it isn't feature-enabled
if(NOT SPLICE)
  set(EXCLUDE_DIR "/splice/")
  foreach(TMP_PATH ${MADARA_SRC_FILES})
      string(FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
      if(NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
          list(REMOVE_ITEM MADARA_SRC_FILES ${TMP_PATH})
      endif()
  endforeach(TMP_PATH)
  
  foreach(TMP_PATH ${MADARA_HEADER_FILES})
      string(FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
      if(NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
          list(REMOVE_ITEM MADARA_HEADER_FILES ${TMP_PATH})
      endif()
  endforeach(TMP_PATH)
endif()

# remove all files in the zmq tree if it isn't feature-enabled
if(NOT ZMQ)
  set(EXCLUDE_DIR "/zmq/")
  foreach(TMP_PATH ${MADARA_SRC_FILES})
      string(FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
      if(NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
          list(REMOVE_ITEM MADARA_SRC_FILES ${TMP_PATH})
      endif()
  endforeach(TMP_PATH)
  
  foreach(TMP_PATH ${MADARA_HEADER_FILES})
      string(FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
      if(NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
          list(REMOVE_ITEM MADARA_HEADER_FILES ${TMP_PATH})
      endif()
  endforeach(TMP_PATH)
endif()

# remove all files in the java tree if it isn't feature-enabled
if(NOT JAVA)
  set(EXCLUDE_DIR "/java/")
  foreach(TMP_PATH ${MADARA_SRC_FILES})
      string(FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
      if(NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
          list(REMOVE_ITEM MADARA_SRC_FILES ${TMP_PATH})
      endif()
  endforeach(TMP_PATH)
  
  foreach(TMP_PATH ${MADARA_HEADER_FILES})
      string(FIND ${TMP_PATH} ${EXCLUDE_DIR} EXCLUDE_DIR_FOUND)
      if(NOT ${EXCLUDE_DIR_FOUND} EQUAL -1)
          list(REMOVE_ITEM MADARA_HEADER_FILES ${TMP_PATH})
      endif()
  endforeach(TMP_PATH)
endif()

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         ON) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     ON)

find_package(Boost 1.60.0 REQUIRED)

if(Boost_FOUND)
  
  add_library(madara SHARED
    ${MADARA_HEADER_FILES}
	${MADARA_SRC_FILES}) 
	
  target_include_directories(madara PUBLIC ${Boost_INCLUDE_DIRS})
  
  target_include_directories(madara
    PUBLIC $<BUILD_INTERFACE:${MADARA_SOURCE_DIR}/include/>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
         $<INSTALL_INTERFACE:${MADARA_INC_INSTALL_DIR}>
    PRIVATE ${header_path})
	
  set_target_properties(madara PROPERTIES 
    DEBUG_POSTFIX d)

  target_compile_definitions(madara PUBLIC
                            MADARA=1
                            MADARA_VERSION_MAJOR=${MADARA_VERSION_MAJOR}
                            MADARA_VERSION_MINOR=${MADARA_VERSION_MINOR}
                            MADARA_VERSION_PATCH=${MADARA_VERSION_PATCH})
							
  target_compile_definitions(madara PRIVATE -DMADARA_BUILD_DLL)
  target_compile_definitions(madara PUBLIC -D_WINSOCK_DEPRECATED_NO_WARNINGS -DBOOST_ALL_NO_LIB)
							
  if(madara_ANDROID)
    target_compile_definitions(madara PUBLIC -D_MADARA_ANDROID_ -D__STDC_FORMAT_MACROS)
  endif()

  if(madara_LZ4)
    target_compile_definitions(madara PUBLIC -D_USE_LZ4_)
  endif()

  if(madara_JAVA)
    target_compile_definitions(madara PUBLIC -D_MADARA_JAVA_)
  endif()

  if(NOT madara_KARL)
    target_compile_definitions(madara PUBLIC -D_MADARA_NO_KARL_)
  endif()

  if(madara_NDDS)
    target_compile_definitions(madara PRIVATE -D_MADARA_NDDS_)
  endif()

  if(madara_PYTHON)
    target_compile_definitions(madara PUBLIC -D_MADARA_PYTHON_CALLBACKS_)
  endif()

  if(madara_SIMTIME)
    target_compile_definitions(madara PUBLIC -DMADARA_FEATURE_SIMTIME)
  endif()

  if(madara_SSL)
    target_compile_definitions(madara PUBLIC -D_USE_SSL_)
  endif()

  if(madara_THREADLOCAL)
    target_compile_definitions(madara PUBLIC -DMADARA_THREAD_LOCAL)
  else()
    target_compile_definitions(madara PUBLIC -DMADARA_NO_THREAD_LOCAL)
  endif()

  if(madara_ZMQ)
    target_compile_definitions(madara PRIVATE -D_MADARA_USING_ZMQ_)
  endif()

  # All users of this library will need at least C++11
  target_compile_features(madara PUBLIC cxx_std_14)
  
  if(MSVC)
    target_compile_options(madara PRIVATE /MP)
  endif()
  
  target_link_libraries(madara Boost::boost)

  set_target_properties(madara PROPERTIES
    OUTPUT_NAME "madara-${MADARA_VERSION}"
    POSITION_INDEPENDENT_CODE ON)
									   
  install(TARGETS madara EXPORT madaraTargets
    RUNTIME       DESTINATION ${MADARA_RUNTIME_INSTALL_DIR}
    LIBRARY       DESTINATION ${MADARA_LIBRARY_INSTALL_DIR}
    ARCHIVE       DESTINATION ${MADARA_ARCHIVE_INSTALL_DIR}
    FRAMEWORK     DESTINATION ${MADARA_FRAMEWORK_INSTALL_DIR})

  # Write/install version file
  include(CMakePackageConfigHelpers)
  set(version_file "${CMAKE_CURRENT_BINARY_DIR}/cmake/madara-config-version.cmake")
  write_basic_package_version_file(${version_file}
    VERSION ${MADARA_VERSION}
    COMPATIBILITY AnyNewerVersion)
  
  set_property(TARGET madara PROPERTY VERSION "${MADARA_VERSION}")
  set_property(TARGET madara PROPERTY SOVERSION "${MADARA_VERSION}")

  install(TARGETS madara DESTINATION lib)
  install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/madara" DESTINATION include)
  install(FILES ${version_file} DESTINATION ${MADARA_CMAKE_CONFIG_INSTALL_DIR})	 
endif()
