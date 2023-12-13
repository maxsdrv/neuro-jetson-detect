cmake_minimum_required(VERSION 3.16)
project(plugin_payloader)

#set(CMAKE_CXX_STANDARD 20)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(PkgConfig REQUIRED)
pkg_search_module(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(GSTREAMER_APP REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)
pkg_search_module(GSTREAMER_RTP REQUIRED IMPORTED_TARGET gstreamer-rtp-1.0>=1.4)
find_package(Threads REQUIRED)


set(SOURCES
        gstrtploaderplugin.c
		gstrtploaderplugin.h
)

add_library(${PROJECT_NAME} SHARED
		${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
	PkgConfig::GSTREAMER PkgConfig::GSTREAMER_APP PkgConfig::GSTREAMER_RTP
)



