cmake_minimum_required(VERSION 3.16)

project(BarCodeX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

qt_standard_project_setup()

qt_add_executable(BarCodeX
    main.cpp
    ui/ui.cpp
    encoder/encoder.cpp
    generator/generator.cpp
    scanner/scanner.cpp
    decoder/decoder.cpp
    camera/camera_scanner.cpp
)

target_link_libraries(BarCodeX PRIVATE Qt6::Widgets Qt6::Multimedia)
set_target_properties(BarCodeX PROPERTIES WIN32_EXECUTABLE ON)

include(CTest)
if(BUILD_TESTING)
    qt_add_executable(CameraScannerTest
        encoder/encoder.cpp
        generator/generator.cpp
        decoder/decoder.cpp
        camera/camera_scanner.cpp
    )
    target_link_libraries(CameraScannerTest PRIVATE Qt6::Multimedia)
    add_test(NAME CameraScannerTest COMMAND CameraScannerTest)
endif()
