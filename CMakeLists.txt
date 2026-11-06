cmake_minimum_required(VERSION 3.16)

project(HelloWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(HelloWorld
    main.cpp
    encoder/encoder.cpp
    generator/generator.cpp
    scanner/scanner.cpp
    decoder/decoder.cpp
)

target_link_libraries(HelloWorld PRIVATE Qt6::Widgets)
set_target_properties(HelloWorld PROPERTIES WIN32_EXECUTABLE ON)