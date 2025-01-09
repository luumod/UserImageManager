cmake_minimum_required(VERSION 3.10)
project(Util VERSION 1.0 LANGUAGES CXX)

set(cmake_automoc ON)
set(cmake_autouic ON)
set(cmake_autorcc ON)

set(CMAKE_CXX_STANDARD 17)
set(cmake_cxx_standard_required ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Network)

file(GLOB Util_SERVER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB Util_SERVER_HEARDERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${PROJECT_NAME} ${Util_SERVER_SOURCES})

# 直接访问文件
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Sql Qt6::Network)