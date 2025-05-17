cmake_minimum_required(VERSION 3.10)

project(ShanksUniversity VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------
# Подключаем pybind11
# ----------------------------
# Попробуем найти установленный пакет
find_package(pybind11 CONFIG QUIET)

if (NOT pybind11_FOUND)
    # Если не нашли, подтянем из Git
    include(FetchContent)
    FetchContent_Declare(
            pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11.git
            GIT_TAG        v2.11.1   # можно указать актуальный
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# ----------------------------
# Список исходников
# ----------------------------
file(GLOB_RECURSE SOURCE_FILES
        bindings.cpp
        main.cpp
        include/*.h
        include/*.hpp
        include/*.tpp
)

include_directories(include)

# ----------------------------
# Исполняемый файл
# ----------------------------
add_executable(ShanksUniversity ${SOURCE_FILES})
target_compile_options(ShanksUniversity PRIVATE -Wall -Wextra)

# ----------------------------
# Python-модуль
# ----------------------------
pybind11_add_module(series_module bindings.cpp)

# Если нужны дополнительные include’ы (например, include/)
target_include_directories(series_module PRIVATE include)

# Если есть зависимости, линковка и т. п.
# target_link_libraries(series_module PRIVATE ...)

# Убираем префикс «lib» на Windows/Linux
set_target_properties(series_module PROPERTIES PREFIX "")
