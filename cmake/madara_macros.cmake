
macro(madara_app _NAME _FILE)
  add_executable(${_NAME} ${_FILE})
  target_link_libraries(${_NAME} PRIVATE madara)
  install(TARGETS ${_NAME} EXPORT madaraTargets
           RUNTIME DESTINATION ${GAMS_RUNTIME_INSTALL_DIR})

endmacro()

macro(madara_test _NAME _FILE)
  add_executable(${_NAME} ${_FILE})
  target_link_libraries(${_NAME} PRIVATE madara)
  install(TARGETS ${_NAME} EXPORT madaraTargets
           RUNTIME DESTINATION ${GAMS_RUNTIME_INSTALL_DIR})
endmacro()
