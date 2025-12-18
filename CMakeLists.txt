cmake_minimum_required(VERSION 3.10)

project(HW7_VAR6
    VERSION 1.0.0
    DESCRIPTION "Лабораторная работа №7: Асинхронное программирование"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_executable(HW7_VAR6 
    main.cpp
    objects/npc/npc.cpp
    objects/dragon/dragon.cpp
    objects/princess/princess.cpp
    objects/knight/knight.cpp
)

target_include_directories(HW7_VAR6 PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/objects/npc
    ${CMAKE_CURRENT_SOURCE_DIR}/objects/dragon
    ${CMAKE_CURRENT_SOURCE_DIR}/objects/princess
    ${CMAKE_CURRENT_SOURCE_DIR}/objects/knight
)

target_link_libraries(HW7_VAR6 PRIVATE Threads::Threads)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(HW7_VAR6 PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()


# GoogleTest

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

set(INSTALL_GTEST OFF CACHE BOOL "Disable installation of googletest" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(gtests 
    tests.cpp
    objects/npc/npc.cpp
    objects/dragon/dragon.cpp
    objects/princess/princess.cpp
    objects/knight/knight.cpp
)

target_include_directories(gtests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/objects/npc
    ${CMAKE_CURRENT_SOURCE_DIR}/objects/dragon
    ${CMAKE_CURRENT_SOURCE_DIR}/objects/princess
    ${CMAKE_CURRENT_SOURCE_DIR}/objects/knight
)

target_link_libraries(gtests PRIVATE GTest::gtest_main)
target_link_libraries(gtests PRIVATE Threads::Threads)


include(GoogleTest)
gtest_discover_tests(gtests)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(gtests PRIVATE -Wall -Wextra -Wpedantic)
endif()
