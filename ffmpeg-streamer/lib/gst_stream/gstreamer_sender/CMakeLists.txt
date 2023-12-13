
set(TARGET_NAME gst_sender)
set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(GSTREAMER_APP REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)
pkg_search_module(GSTREAMER_RTP REQUIRED IMPORTED_TARGET gstreamer-rtp-1.0>=1.4)

set(SOURCES source/gst_sender.h
			source/gst_sender.cpp
			source/rtp_sender.h
			source/rtp_sender.cpp
)
set(META_API_SOURCE_DIR
	../meta/
)

# Add source to this project's executable.
add_library (${TARGET_NAME} ${SOURCES})


# TODO: Add tests and install targets if needed.
target_link_libraries(${TARGET_NAME}
				PUBLIC 
				PkgConfig::GSTREAMER PkgConfig::GSTREAMER_APP
				PkgConfig::GSTREAMER_RTP
				PRIVATE
				${OpenCV_LIBS}
)

target_include_directories(${TARGET_NAME} 
							PUBLIC
							${CMAKE_CURRENT_SOURCE_DIR}/source
							PRIVATE
							${OPENCV_INCLUDE_DIRS}
							${META_API_SOURCE_DIR}
)