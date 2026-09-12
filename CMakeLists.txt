cmake_minimum_required(VERSION 3.20)
project(SMMRX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(SMMRX WIN32 main.cpp)

add_custom_command(TARGET SMMRX POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:SMMRX>/assets
)

target_link_libraries(SMMRX PRIVATE raylib)

if (WIN32)
    target_link_libraries(SMMRX PRIVATE winmm)
    target_link_options(SMMRX PRIVATE -static-libgcc -static-libstdc++)
endif()
