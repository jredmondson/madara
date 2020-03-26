
# compile applications/tools

madara_app(karl karl.cpp)
madara_app(stk_inspect stk_inspect.cpp)

install(TARGETS karl DESTINATION bin)
install(TARGETS stk_inspect DESTINATION bin)
