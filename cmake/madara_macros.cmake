
macro(madara_app _NAME _FILE)
  add_executable(${_NAME} ${_FILE})
  target_link_libraries(${_NAME} PRIVATE madara)
endmacro()

macro(madara_test _NAME _FILE)
  add_executable(${_NAME} ${_FILE})
  target_link_libraries(${_NAME} PRIVATE madara)
endmacro()
