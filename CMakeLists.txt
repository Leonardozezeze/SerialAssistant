cmake_minimum_required(VERSION 3.16)
project(Qt6Template VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)

add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    LabeledComboBox.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::SerialPort)
