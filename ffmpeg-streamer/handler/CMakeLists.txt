project(gst_)
set(TARGET_NAME handler)
set(CMAKE_CXX_STANDARD 20)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

set(SOURCES handler.cpp )

# Add source to this project's executable.
add_executable (${TARGET_NAME} ${SOURCES})


# TODO: Add tests and install targets if needed.
target_link_libraries(${TARGET_NAME}
				PUBLIC 
				sender
				receiver
				PRIVATE
				${OpenCV_LIBS}
				Threads::Threads
)

target_include_directories(${TARGET_NAME} 
							PRIVATE
							${OPENCV_INCLUDE_DIRS}
							${CMAKE_SOURCE_DIR}/sender
							${CMAKE_SOURCE_DIR}/receiver
)