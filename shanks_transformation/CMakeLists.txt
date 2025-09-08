cmake_minimum_required(VERSION 3.11)
project(shanks_transformation LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Executable (main program)
add_executable(shanks_transformation main.cpp)
target_include_directories(shanks_transformation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Pybind11 module (Python entrypoint)
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)
pybind11_add_module(pyshanks py.cpp)
target_include_directories(pyshanks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
