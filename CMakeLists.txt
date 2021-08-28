cmake_minimum_required(VERSION 3.8)
project(CamH264CvMatStreaming_Cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()

if (WIN32)
	set(CMAKE_MODULE_PATH "$ENV{OPENCV_DIR};${CMAKE_MODULE_PATH}") #apend opencv cmake module
	set(CMAKE_MODULE_PATH "$ENV{FFMPEG_DIR};${CMAKE_MODULE_PATH}") #apend ffmpeg cmake module
endif()

find_package(OpenCV REQUIRED)
find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL AVDEVICE REQUIRED)

include_directories("${OpenCV_INCLUDE_DIRS}")
include_directories("${FFMPEG_INCLUDE_DIRS}")

link_libraries("${OpenCV_LIBS}")
link_libraries("${FFMPEG_LIBRARIES}")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/StreamingAPI")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/StreamingAPI")
link_libraries(StreamingAPI)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/SocketNetworking")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/SocketNetworking/socket_net/include")
link_libraries(socket_net)

add_executable(OpenCV_CAM "${CMAKE_CURRENT_SOURCE_DIR}/opencv_camera.cpp")
add_executable(OpenCV_FFMPEG_CAM "${CMAKE_CURRENT_SOURCE_DIR}/opencv_ffmpeg_conversion_camera.cpp"  )

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ServerClientMulticastStreaming")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ServerClientMulticastStreaming")
link_libraries(H264_FRAME_PKG)

find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/QtGuiMcastClient")
