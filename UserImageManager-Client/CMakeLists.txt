cmake_minimum_required(VERSION 3.8)

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

target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-g> $<$<CONFIG:Release>:-O3>)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND F:/Tools/Qt/6.5.3/mingw_64/bin/windeployqt.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.exe
        COMMENT "MinGW ------ Generating <client> external dependency."
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND F:/Tools/Qt/6.5.3/msvc2019_64/bin/windeployqt.exe ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.exe
        COMMENT "MSVC ------ Generating <client> external dependency."
    )
endif()