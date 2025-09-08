cmake_minimum_required(VERSION 3.11)
project(shanks_transformation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(shanks_transformation_lib STATIC
    main.cpp          # add any additional .cpp/.c files here
)

target_include_directories(shanks_transformation_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(shanks_transformation main.cpp)

target_link_libraries(shanks_transformation PRIVATE shanks_transformation_lib)

target_include_directories(shanks_transformation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(pyshanks py.cpp)

target_link_libraries(pyshanks PRIVATE shanks_transformation_lib)

target_include_directories(pyshanks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
