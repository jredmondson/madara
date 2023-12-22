

if(madara_SSL)
  find_package(OpenSSL REQUIRED)
endif()


macro(madara_app _NAME _FILE)
  add_executable(${_NAME} ${_FILE})
  target_link_libraries(${_NAME} PRIVATE madara)

  if(madara_SSL)
    target_link_libraries(${_NAME} PRIVATE OpenSSL::Crypto)
  endif()
  
  install(TARGETS ${_NAME} EXPORT madaraTargets
           RUNTIME DESTINATION ${MADARA_RUNTIME_INSTALL_DIR})

endmacro()

macro(madara_test _NAME _FILE)
  add_executable(${_NAME} ${_FILE})
  target_link_libraries(${_NAME} PRIVATE madara)

  if(madara_SSL)
    target_link_libraries(${_NAME} PRIVATE OpenSSL::Crypto)
  endif()
  
  install(TARGETS ${_NAME} EXPORT madaraTargets
           RUNTIME DESTINATION ${MADARA_RUNTIME_INSTALL_DIR})
endmacro()

macro(madara_repo_test _NAME _FILE)
  add_executable(${_NAME} ${_FILE})
  target_link_libraries(${_NAME} PRIVATE madara)

  if(madara_SSL)
    target_link_libraries(${_NAME} PRIVATE OpenSSL::Crypto)
  endif()
  
  add_test(NAME ${_NAME}
         COMMAND ${_NAME})
  install(TARGETS ${_NAME} EXPORT madaraTargets
           RUNTIME DESTINATION ${MADARA_RUNTIME_INSTALL_DIR})
endmacro()
