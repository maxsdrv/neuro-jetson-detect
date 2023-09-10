cmake_minimum_required(VERSION 3.16)

project(main_window)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

find_package(Qt6 COMPONENTS Core Widgets Quick REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(GSTREAMER_APP REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)


link_directories(${CMAKE_SOURCE_DIR}/lib)

set(SOURCES
main.cpp
src/GWindow.cpp
src/gsthandler.cpp
src/wgstsenderwrapper.cpp
src/wgstreceiverwrapper.cpp
)

set(HEADERS
src/GWindow.h
src/gsthandler.h
src/wgstsenderwrapper.h
src/wgstreceiverwrapper.h
)

set(RESOURCES
resources.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
${OPENCV_INCLUDE_DIRS}
${CMAKE_CURRENT_SOURCE_DIR}/src
${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

target_link_libraries(${PROJECT_NAME}
Qt6::Core Qt6::Widgets Qt6::Quick
${OpenCV_LIBS}
PkgConfig::GSTREAMER PkgConfig::GSTREAMER_APP
gst_receiver gst_sender)

