project(H264VideoStreaming)

add_executable(H264VideoStreaming "${CMAKE_CURRENT_SOURCE_DIR}/h264_video_streaming.cc")
target_compile_definitions(H264VideoStreaming PRIVATE -DOUTPUT_FILE_PATH="${CMAKE_CURRENT_BINARY_DIR}")
