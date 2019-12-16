
aux_source_directory(. inspect_src)

add_library(pallette_inspect ${inspect_src})
target_link_libraries(pallette_inspect pallette_base pallette_http)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
add_executable(InspectorTest tests/InspectorTest.cpp)
target_link_libraries(InspectorTest pallette_inspect)
endif()