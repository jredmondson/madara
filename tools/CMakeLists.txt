
# compile applications/tools

madara_app(karl karl.cpp)
madara_app(stk_inspect stk_inspect.cpp)

install(TARGETS stk_inspect EXPORT madaraTargets
           RUNTIME DESTINATION ${MADARA_RUNTIME_INSTALL_DIR})
install(TARGETS karl EXPORT madaraTargets
           RUNTIME DESTINATION ${MADARA_RUNTIME_INSTALL_DIR})
