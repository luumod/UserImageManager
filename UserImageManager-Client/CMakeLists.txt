﻿cmake_minimum_required(VERSION 3.8)

project(UserImageManager_Client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../bin)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

file(GLOB USERPRIVILEGE_CLIENT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp "ResourceClient.qrc")

add_subdirectory(LoginRegister)
add_subdirectory(SubPage)

add_executable(${PROJECT_NAME} ${USERPRIVILEGE_CLIENT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network LoginRegister SWidget SubPage)