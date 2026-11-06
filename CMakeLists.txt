cmake_minimum_required(VERSION 3.21)

project(BarCodeX VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(barcodex
    src/main.cpp
)

target_link_libraries(barcodex PRIVATE Qt6::Widgets)
