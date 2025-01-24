cmake_minimum_required(VERSION 3.10)

project(SWidget VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS REQUIRED Widgets Svg Network)

add_subdirectory(LittleWidget)
add_subdirectory(SItemDelegate)
add_subdirectory(View)
add_subdirectory(Widget)
file(GLOB SWIDGET_SITEMDELEGATE_CPP "${CMAKE_CURRENT_LIST_DIR}/SItemDelegate/*.cpp")
file(GLOB SWIDGET_SLITTLEWIDGET_CPP "${CMAKE_CURRENT_LIST_DIR}/LittleWidget/*.cpp")
file(GLOB SWIDGET_VIEW_H "${CMAKE_CURRENT_LIST_DIR}/View/*.h")
file(GLOB SWIDGET_WIDGET_CPP "${CMAKE_CURRENT_LIST_DIR}/Widget/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SWIDGET_SLITTLEWIDGET_CPP} ${SWIDGET_SITEMDELEGATE_CPP} ${SWIDGET_VIEW_H} ${SWIDGET_WIDGET_CPP})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_CURRENT_LIST_DIR}/SItemDelegate/ ${CMAKE_CURRENT_LIST_DIR}/LittleWidget/ ${CMAKE_CURRENT_LIST_DIR}/View/ ${CMAKE_CURRENT_LIST_DIR}/Widget/)

target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Widgets Util Qt6::Svg Qt6::Network LittleWidget SItemDelegate View Widget)
