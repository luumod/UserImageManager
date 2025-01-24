cmake_minimum_required(VERSION 3.10)

project(LittleWidget VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED Widgets Svg)

file(GLOB LITTLEWIDGET_CPP ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB LITTLEWIDGET_H ${CMAKE_CURRENT_LIST_DIR}/*.h)

list(APPEND SRC_FILES ${LITTLEWIDGET_CPP} ${LITTLEWIDGET_H})

# 构建为静态库文件
add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Widgets Qt6::Svg Util)

# 添加头文件的包含路径，public表示其他目标也能使用该路径
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

