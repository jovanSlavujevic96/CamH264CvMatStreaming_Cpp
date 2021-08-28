#pragma once

#include <cstdint>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavdevice/avdevice.h>
}

#include <opencv2/opencv.hpp>

struct H264EncConfig;

class AvH264_Encoder
{
private:
    AVCodec* cdc_ = NULL;
    AVCodecContext* cdc_ctx_ = NULL;
    AVFrame* avf_ = NULL;
    AVPacket* avp_ = NULL;
    cv::Size size = cv::Size(0, 0);
    int32_t frame_size_ = 0;
    int32_t pts_ = 0;
public:
    AvH264_Encoder() = default;
    ~AvH264_Encoder();

    int open(const H264EncConfig& h264_config);
    void close();
    AVPacket* encode(cv::Mat& mat);
};
