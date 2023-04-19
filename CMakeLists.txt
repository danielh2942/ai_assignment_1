cmake_minimum_required(VERSION 3.25)
project(assignment1)

add_executable(assignment1 main.cpp)
add_executable(assignment1_2 partb.cpp)

set_property(TARGET assignment1 PROPERTY CXX_STANDARD 17)
set_property(TARGET assignment1_2 PROPERTY CXX_STANDARD 17)
