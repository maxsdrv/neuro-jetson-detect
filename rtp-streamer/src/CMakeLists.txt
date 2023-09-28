cmake_minimum_required(VERSION 3.16)

project(nvidia-gstreamer)


set(CMAKE_CUDA_COMPILER /usr/local/cuda-10.2/bin/nvcc)

enable_language(CUDA)

set(CMAKE_CUDA_STANDARD 11)

unset(CMAKE_CUDA_ARCHITECTURES CACHE)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCV REQUIRED)
find_package(CUDAToolkit REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(GSTREAMER_APP REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)

set(JETSON_INCLUDE_DIRS "/usr/local/include/jetson-utils")

set(SOURCES 
			source/rtp_server.h
			source/rtp_server.cpp
			source/main.cpp
)
set(CUDA_SOURCE 
	cuda_source/test.cuh
	cuda_source/test.cu
)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} ${SOURCES} ${CUDA_SOURCE})


# TODO: Add tests and install targets if needed.
target_link_libraries(${PROJECT_NAME}
				PUBLIC 
				PkgConfig::GSTREAMER PkgConfig::GSTREAMER_APP
				PRIVATE 
				CUDA::cudart
				CUDA::cuda_driver
				${OpenCV_LIBS}
				jetson-utils
)

target_include_directories(${PROJECT_NAME} 
							PUBLIC
							${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
							${CMAKE_CURRENT_SOURCE_DIR}/source
							${CMAKE_CURRENT_SOURCE_DIR}/cuda_source
							PRIVATE
							${OPENCV_INCLUDE_DIRS}
							${JETSON_INCLUDE_DIRS}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_SEPARABLE_COMPILATION ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_STANDARD 14)


