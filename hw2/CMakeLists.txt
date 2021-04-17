cmake_minimum_required(VERSION 3.0.0)
project(Timer VERSION 0.1.0)

find_package(Threads REQUIRED)


include(CTest)
enable_testing()

add_executable(Timer main.cpp ITimer.hpp)
target_link_libraries(Timer pthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
