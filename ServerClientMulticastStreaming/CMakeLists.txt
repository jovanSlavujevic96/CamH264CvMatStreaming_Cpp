project(ServerClientMulticastStreaming)

add_library(H264_FRAME_PKG
	"${CMAKE_CURRENT_SOURCE_DIR}/frame_package.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/frame_package.cc"
)

link_libraries(H264_FRAME_PKG)

add_executable(H264_MCAST_SERVER
	"${CMAKE_CURRENT_SOURCE_DIR}/h264_mcast_server.cpp"
)

add_executable(H264_MCAST_CLIENT
	"${CMAKE_CURRENT_SOURCE_DIR}/h264_mcast_client.cpp"
)