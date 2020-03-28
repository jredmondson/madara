
# compile applications/tools

madara_app(karl karl.cpp)
madara_app(madara_registry madara_registry.cpp)
madara_app(madara_version madara_version.cpp)
madara_app(stk_inspect stk_inspect.cpp)

install(TARGETS karl EXPORT madaraTargets
           RUNTIME DESTINATION ${MADARA_RUNTIME_INSTALL_DIR})
install(TARGETS madara_registry EXPORT madaraTargets
           RUNTIME DESTINATION ${MADARA_RUNTIME_INSTALL_DIR})
install(TARGETS madara_version EXPORT madaraTargets
           RUNTIME DESTINATION ${MADARA_RUNTIME_INSTALL_DIR})
install(TARGETS stk_inspect EXPORT madaraTargets
           RUNTIME DESTINATION ${MADARA_RUNTIME_INSTALL_DIR})
           