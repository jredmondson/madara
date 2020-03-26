
# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

cmake_minimum_required(VERSION 3.12)

# Optionally glob, but only for CMake 3.12 or later:
# file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${ModernCMakeExample_SOURCE_DIR}/include/modern/*.hpp")

file(GLOB_RECURSE MADARA_SRC_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/include/*.c
    ${PROJECT_SOURCE_DIR}/include/*.cpp 
)

file(GLOB_RECURSE MADARA_HEADER_FILES CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/include/*.h
    ${PROJECT_SOURCE_DIR}/*.inl
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

# remove all files in the splice tree if it isn't feature-enabled
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


# Make an automatic library - will be static or dynamic based on user setting


# We need this directory, and users of our library will need it too

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         ON) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     ON)
find_package(Boost 1.73.0) # REQUIRED COMPONENTS filesystem) 

if(Boost_FOUND)
  
  add_library(madara SHARED
    ${MADARA_HEADER_FILES}
	${MADARA_SRC_FILES}) 
  #target_link_libraries(MADARA_LIBRARY Boost::filesystem)
  target_include_directories(madara PUBLIC ${Boost_INCLUDE_DIRS})
  target_include_directories(madara PUBLIC ${PROJECT_SOURCE_DIR}/include)
  target_compile_definitions(madara PRIVATE -DMADARA_BUILD_DLL)
  target_compile_definitions(madara PUBLIC -D_WINSOCK_DEPRECATED_NO_WARNINGS -DBOOST_ALL_NO_LIB)

  # All users of this library will need at least C++11
  target_compile_features(madara PUBLIC cxx_std_14)
  target_compile_options(madara PRIVATE /MP)
  target_link_libraries(madara ${Boost_LIBRARIES})


endif()

# IDEs should put the headers in a nice place
source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${MADARA_HEADER_FILES})

