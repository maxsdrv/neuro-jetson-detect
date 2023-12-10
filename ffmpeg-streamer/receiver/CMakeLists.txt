
set(TARGET_NAME receiver)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)

set(SOURCES src/FFMpegReceiver.h
		src/FFMpegReceiver.cpp
)

# Add source to this project's executable.
add_library (${TARGET_NAME} ${SOURCES})


# TODO: Add tests and install targets if needed.
target_link_libraries(${TARGET_NAME}
				PRIVATE
				${OpenCV_LIBS}
)

target_include_directories(${TARGET_NAME} 
							PUBLIC
							${CMAKE_CURRENT_SOURCE_DIR}/src
							PRIVATE
							${OPENCV_INCLUDE_DIRS}
)