cmake_minimum_required(VERSION 3.15)

# set the project name and version
project(OLC_SORT)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(sort sort.cpp sortwindow.cpp subwindow.cpp 
				bubble.cpp insertion.cpp quick.cpp
				selection.cpp shell.cpp)