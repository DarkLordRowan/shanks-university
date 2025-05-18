cmake_minimum_required(VERSION 3.10...3.24)
project(ShanksUniversity VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем pybind11
find_package(pybind11 CONFIG QUIET)
if (NOT pybind11_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11.git
            GIT_TAG        v2.11.1
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# Python-модуль series_module
pybind11_add_module(series_module bindings.cpp)
target_include_directories(series_module PRIVATE include)

# Убираем префикс «lib»
set_target_properties(series_module PROPERTIES PREFIX "")

# Выводим модуль в папку backend/
set_target_properties(series_module PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../backend"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../backend"
)
