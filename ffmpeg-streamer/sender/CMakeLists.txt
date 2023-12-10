
set(TARGET_NAME sender)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)
find_package(ZLIB REQUIRED)

if (ZLIB_FOUND)
	include_directories(${ZLIB_INCLUDE_DIRS})
endif()


set(SOURCES src/FFMpegStreamer.h
		src/FFMpegStreamer.cpp
        src/TypeStream.cpp
        src/TypeStream.h
)

# ffmpeg
message("Searching FFMPEG library")
if (MSVC)
	if (DEFINED ffmpeg_DIR)
		message("You specified ffmpeg_DIR variable. Its value: " ${ffmpeg_DIR} ". Using it for processing_daemon")
	else()
		message("ffmpeg_DIR variable is not defined.")
		if (DEFINED ENV{ffmpeg_DIR})
			message("ffmpeg_DIR environment variable defined instead. Its value: " $ENV{ffmpeg_DIR} ". Using it for processing daemon")
			set(ffmpeg_DIR $ENV{ffmpeg_DIR})
		else()
			message("Neither ffmpeg_DIR environment variable nor CMake one defined. Using system location for find ffmpeg_DIR")
		endif()
	endif()
	include_directories(${ffmpeg_DIR}/include)
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${ffmpeg_DIR})
endif()

find_library(ffmpeg_avformat_LIB avformat)
find_library(ffmpeg_avcodec_LIB avcodec)
find_library(ffmpeg_swscale_LIB swscale)
find_library(ffmpeg_avutil_LIB avutil)

message("FFMPEG avformat library location: " ${ffmpeg_avformat_LIB})
message("FFMPEG avcodec library location: " ${ffmpeg_avcodec_LIB})
message("FFMPEG swscale library location: " ${ffmpeg_swscale_LIB})
message("FFMPEG avutil library location: " ${ffmpeg_avutil_LIB})

set(REQUIRED_LIBRARIES ${REQUIRED_LIBRARIES} ${ffmpeg_avformat_LIB} ${ffmpeg_avcodec_LIB} ${ffmpeg_swscale_LIB} ${ffmpeg_avutil_LIB})


# Add source to this project's executable.
add_library (${TARGET_NAME} ${SOURCES})


# TODO: Add tests and install targets if needed.
target_link_libraries(${TARGET_NAME}
				PUBLIC
				${REQUIRED_LIBRARIES}
				${ZLIB_LIBRARIES}
				PRIVATE
				${OpenCV_LIBS}
)

target_include_directories(${TARGET_NAME} 
							PUBLIC
							${CMAKE_CURRENT_SOURCE_DIR}/src
							PRIVATE
							${OPENCV_INCLUDE_DIRS}
)