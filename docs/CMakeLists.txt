if(NOT DOCS)
  return()
endif()

find_package(Doxygen)
find_package(Perl)

if(DOXYGEN_FOUND)

  add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/docs/VERSION.txt
    COMMAND ${PERL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/docs/get_version.pl
  )
  
  add_custom_target(doc
    # Generate the API documentation
    ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/doc/gazebo.dox
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND mv ${CMAKE_BINARY_DIR}/doxygen/latex/refman.pdf
            ${CMAKE_BINARY_DIR}/doxygen/latex/madara-${MADARA_VERSION}.pdf

    COMMENT "Generating API documentation with Doxygen" VERBATIM)
	

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen/html
    DESTINATION docs
    COMPONENT "docs" OPTIONAL
    ${compatible_MESSAGE_NEVER}
  )
endif()

