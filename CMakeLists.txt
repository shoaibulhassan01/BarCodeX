cmake_minimum_required(VERSION 3.16)

project(DukaanOS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

qt_standard_project_setup()

qt_add_executable(DukaanOS
    main.cpp
    ui/ui.cpp
    BarCode/encoder/encoder.cpp
    BarCode/generator/generator.cpp
    BarCode/scanner/scanner.cpp
    BarCode/decoder/decoder.cpp
    BarCode/camera/camera_scanner.cpp
)

target_include_directories(DukaanOS PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/BarCode")
target_link_libraries(DukaanOS PRIVATE Qt6::Widgets Qt6::Multimedia)
set_target_properties(DukaanOS PROPERTIES WIN32_EXECUTABLE ON)

include(CTest)
if(BUILD_TESTING)
    qt_add_executable(CameraScannerTest
        BarCode/encoder/encoder.cpp
        BarCode/generator/generator.cpp
        BarCode/decoder/decoder.cpp
        BarCode/camera/camera_scanner.cpp
    )
    target_include_directories(CameraScannerTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/BarCode")
    target_link_libraries(CameraScannerTest PRIVATE Qt6::Multimedia)
    add_test(NAME CameraScannerTest COMMAND CameraScannerTest)
endif()
