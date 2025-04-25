cmake_minimum_required(VERSION 3.10)

project(ShanksUniversity VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Рекурсивно ищем все .cpp, .hpp и .tpp
file(GLOB_RECURSE SOURCE_FILES
        main.cpp
        include/*.hpp
        include/*.tpp
)

include_directories(include)

add_executable(ShanksUniversity ${SOURCE_FILES})

target_compile_options(ShanksUniversity PRIVATE -Wall -Wextra)
