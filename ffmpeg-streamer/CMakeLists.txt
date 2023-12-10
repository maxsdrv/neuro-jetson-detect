cmake_minimum_required (VERSION 3.16)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ffmpeg_streamer" LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # For static and import libraries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # For shared libraries

# Include sub-projects.
add_subdirectory ("sender")
add_subdirectory ("receiver")
#add_subdirectory ("handler")
add_subdirectory ("rtp-server")