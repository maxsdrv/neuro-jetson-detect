set(TARGET_NAME rtpserver)
set(CMAKE_CXX_STANDARD 20)

option(ENABLE_FFMPEG "Use FFmpeg library" OFF)
option(ENABLE_GSTREAMER "Use Gstreamer library" ON)

find_package(PkgConfig)
find_package(OpenCV REQUIRED)
pkg_check_modules(Pistache REQUIRED IMPORTED_TARGET libpistache)

if(ENABLE_GSTREAMER)
    set(GSTREAMER_PATH lib/gst_stream/)
    add_definitions(-DENABLE_GSTREAMER)
elseif(ENABLE_FFMPEG)
    set(FFMPEG_PATH lib/ffmpeg_stream/)
    add_definitions(-DENABLE_FFMPEG)
endif()

set(SOURCE
        RequestHandler.hpp
        RequestHandler.cpp
        main.cpp
        StreamWorker.h
        IStreamer.h
        FFmpegAdapter.cpp
        FFmpegAdapter.h
        GstreamerAdapter.cpp
        GstreamerAdapter.h
        StreamServer.cpp
        StreamServer.h
)

add_executable(${TARGET_NAME} ${SOURCE})

target_include_directories(${TARGET_NAME}
        PRIVATE
        ${OPENCV_INCLUDE_DIRS}
)

target_link_libraries(${TARGET_NAME}
        PkgConfig::Pistache
        ${OpenCV_LIBS}
        nlohmann_json::nlohmann_json
)

if(ENABLE_GSTREAMER)
    target_include_directories(${TARGET_NAME}
            PRIVATE
            ${GSTREAMER_PATH}
    )
    target_link_libraries(${TARGET_NAME}
            gst_sender
            gst_receiver
    )
elseif(ENABLE_FFMPEG)
    target_include_directories(${TARGET_NAME}
            PRIVATE
            ${FFMPEG_PATH}
    )
    target_link_libraries(${TARGET_NAME}
            ffmpeg_sender
            ffmpeg_receiver
    )
endif()






