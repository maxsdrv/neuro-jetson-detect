set(TARGET_NAME rtpserver)
set(CMAKE_CXX_STANDARD 20)

find_package(PkgConfig)
pkg_check_modules(Pistache REQUIRED IMPORTED_TARGET libpistache)

set(SOURCE
        RTPServer.hpp
        RTPServer.cpp
        main.cpp
)

add_executable(${TARGET_NAME} ${SOURCE})

target_include_directories(${TARGET_NAME}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/sender
        ${CMAKE_SOURCE_DIR}/receiver
)

target_link_libraries(${TARGET_NAME}
        PkgConfig::Pistache
        sender
        receiver
)


