cmake_minimum_required(VERSION 3.16)

project(BarCodeX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

qt_standard_project_setup()

qt_add_executable(BarCodeX
    main.cpp
    ui/ui.cpp
    ui/pos/pos_ui.cpp
    POS/common/id_generator.cpp
    POS/common/record_codec.cpp
    POS/user/user_repository.cpp
    POS/user/auth_service.cpp
    POS/product/product_repository.cpp
    POS/product/product_service.cpp
    POS/scanner/pos_barcode_scanner.cpp
    POS/order/order_repository.cpp
    POS/order/order_service.cpp
    POS/bill/bill_repository.cpp
    POS/bill/bill_service.cpp
    BarCode/encoder/encoder.cpp
    BarCode/generator/generator.cpp
    BarCode/scanner/scanner.cpp
    BarCode/decoder/decoder.cpp
    BarCode/camera/camera_scanner.cpp
)

target_include_directories(BarCodeX PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/BarCode")
target_link_libraries(BarCodeX PRIVATE Qt6::Widgets Qt6::Multimedia)
set_target_properties(BarCodeX PROPERTIES WIN32_EXECUTABLE ON)

include(CTest)
if(BUILD_TESTING)
    qt_add_executable(PosStarterTest
        POS/tests/pos_starter_test.cpp
        POS/common/record_codec.cpp
        POS/product/product_repository.cpp
        POS/product/product_service.cpp
        POS/bill/bill_service.cpp
    )
    target_link_libraries(PosStarterTest PRIVATE Qt6::Core)
    add_test(NAME PosStarterTest COMMAND PosStarterTest)

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
