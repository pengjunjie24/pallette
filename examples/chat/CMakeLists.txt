add_executable(ChatClient ChatClient.cpp)
target_link_libraries(ChatClient pallette_base)

add_executable(ChatServer ChatServer.cpp)
target_link_libraries(ChatServer pallette_base)


