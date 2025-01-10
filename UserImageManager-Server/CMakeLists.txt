cmake_minimum_required(VERSION 3.10)
project(UserImageManager_Server VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(cmake_autouic ON)
set(cmake_autorcc ON)

set(CMAKE_CXX_STANDARD 17)
set(cmake_cxx_standard_required ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../bin)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Sql HttpServer)

file(GLOB USERPRIVILEGE_SERVER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB USERPRIVILEGE_SERVER_HEARDERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME} ${USERPRIVILEGE_SERVER_SOURCES} ${USERPRIVILEGE_SERVER_HEARDERS})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core Qt6::Sql Qt6::HttpServer Util SJwt QsLog)