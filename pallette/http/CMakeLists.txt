
aux_source_directory(. http_src)

add_library(pallette_http ${http_src})
target_link_libraries(pallette_http pallette_base)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
add_executable(HttpServerTest tests/HttpServerTest.cpp)
target_link_libraries(HttpServerTest pallette_http)
endif()