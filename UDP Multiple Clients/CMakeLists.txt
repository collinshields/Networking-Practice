cmake_minimum_required(VERSION 3.15)
project(UDP_Multiple_Clients LANGUAGES C CXX)

# Server and client are built as separate executables to avoid multiple main() definitions
set(SERVER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp")
set(CLIENT_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp")

add_executable(UDP_Multiple_Clients_Server ${SERVER_SRC})
add_executable(UDP_Multiple_Clients_Client ${CLIENT_SRC})

# Include the project's src directory for headers
target_include_directories(UDP_Multiple_Clients_Server PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(UDP_Multiple_Clients_Client PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Use C++17
target_compile_features(UDP_Multiple_Clients_Server PRIVATE cxx_std_17)
target_compile_features(UDP_Multiple_Clients_Client PRIVATE cxx_std_17)

# Link Winsock on Windows and silence CRT deprecation warnings for MSVC
if (WIN32)
    target_link_libraries(UDP_Multiple_Clients_Server PRIVATE Ws2_32)
    target_link_libraries(UDP_Multiple_Clients_Client PRIVATE Ws2_32)

    if (MSVC)
        target_compile_definitions(UDP_Multiple_Clients_Server PRIVATE _CRT_SECURE_NO_WARNINGS)
        target_compile_definitions(UDP_Multiple_Clients_Client PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
endif()