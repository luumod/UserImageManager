cmake_minimum_required(VERSION 3.10)

project(View VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED Widgets Network)


file(GLOB VIEW_CPP ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB VIEW_H ${CMAKE_CURRENT_LIST_DIR}/*.h)

list(APPEND SRC_FILES ${VIEW_CPP} ${VIEW_H})

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network Util LittleWidget Widget)
 
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})