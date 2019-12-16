
aux_source_directory(. pallette_srcs)

add_library(pallette_base ${pallette_srcs})
target_link_libraries(pallette_base pthread rt)

add_subdirectory(http)
add_subdirectory(inspect)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(tests)
endif()