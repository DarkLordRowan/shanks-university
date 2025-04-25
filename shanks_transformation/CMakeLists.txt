cmake_minimum_required(VERSION 3.10)

# Указываем имя проекта
project(ShanksUniversity VERSION 1.0 LANGUAGES CXX)

# Устанавливаем стандарт C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES main.cpp
        include/algorithm/chang_wynn_algorithm.hpp
        include/algorithm/brezinski_theta_algorithm.tpp)

# Указываем каталог с заголовочными файлами
include_directories(include)

# Создаём исполнимый файл
add_executable(ShanksUniversity ${SOURCE_FILES})

# Указываем дополнительные флаги компилятора (по желанию)
target_compile_options(ShanksUniversity PRIVATE -Wall -Wextra)
