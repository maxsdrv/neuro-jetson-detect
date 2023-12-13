project(gst_)
set(TARGET_NAME gst_handler)
set(CMAKE_CXX_STANDARD 20)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(GSTREAMER_APP REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)
find_package(Threads REQUIRED)


set(SOURCES
		handler.cpp
		../meta/meta_api.h
)

# Add source to this project's executable.
add_executable (${TARGET_NAME} ${SOURCES})


#link_directories(${CMAKE_SOURCE_DIR}/libs/elements/cmake-build-debug/)
# TODO: Add tests and install targets if needed.
target_link_libraries(${TARGET_NAME}
				PUBLIC 
				PkgConfig::GSTREAMER PkgConfig::GSTREAMER_APP
				gst_sender
				gst_receiver
				PRIVATE
				${OpenCV_LIBS}
				Threads::Threads
)

target_include_directories(${TARGET_NAME} 
							PRIVATE
							${OPENCV_INCLUDE_DIRS}
							${CMAKE_SOURCE_DIR}/gstreamer_sender
							${CMAKE_SOURCE_DIR}/gstreamer_receiver
							#${CMAKE_SOURCE_DIR}/libs/elements/
)