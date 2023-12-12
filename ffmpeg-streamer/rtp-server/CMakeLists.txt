set(TARGET_NAME rtpserver)
set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig)
find_package(OpenCV REQUIRED)
pkg_check_modules(Pistache REQUIRED IMPORTED_TARGET libpistache)

set(SOURCE
        RTPServer.hpp
        RTPServer.cpp
        main.cpp
        StreamWorker.cpp
        StreamWorker.h
)

add_executable(${TARGET_NAME} ${SOURCE})

target_include_directories(${TARGET_NAME}
        PRIVATE
        ${OPENCV_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/sender
        ${CMAKE_SOURCE_DIR}/receiver
)

target_link_libraries(${TARGET_NAME}
        PkgConfig::Pistache
        ${OpenCV_LIBS}
        sender
        receiver
        nlohmann_json::nlohmann_json
)


