cmake_minimum_required(VERSION 3.18)
project(square)

file(GLOB_RECURSE HEADERS *.h)

set(SOURCES square.c)
add_subdirectory(src)

set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} m)

enable_testing()

add_test(BasicTest ${PROJECT_NAME} 5)
set_tests_properties(BasicTest
    PROPERTIES PASS_REGULAR_EXPRESSION "5*5 = 25")

add_test(UsageTest ${PROJECT_NAME})
set_tests_properties(UsageTest
    PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*")

