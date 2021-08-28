#include <cstdint>
#include <iostream>

#include <opencv2/opencv.hpp>

#include "H264EncConfig.h"
#include "AvH264_Encoder.h"
#include "AvH264_Decoder.h"
#include "H264_Frame.hpp"

#define LINK 0
#define WINDOW_INPUT_NAME "OpenCV_CAM INPUT"
#define WINDOW_OUTPUT_NAME "OpenCV_CAM OUTPUT"

int main()
{
    cv::VideoCapture capture(LINK);
    if (!capture.isOpened())
    {
        std::cerr << "Opening of capture with id = " << LINK << " has been failed\n.";
        std::cerr << "Exit app.\n";
        return -1;
    }
    H264EncConfig conf;
    AvH264_Encoder h264encoder;
    AvH264_Decoder h264decoder;
    AVPacket* ffmpegAvPacket;
    H264_Frame h264Frame;
    cv::Mat cvInputFrame;
    cv::Mat cvOutputFrame;
    cv::Mat cvCopyFrame;
    char key;

    conf.bit_rate = 1024;
    conf.width = 640; //(int)capture.get(cv::CAP_PROP_FRAME_WIDTH); //1280 
    conf.height = 480; // (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT); //720
    conf.gop_size = 60;
    conf.max_b_frames = 0;
    conf.frame_rate = 12;

    if (h264encoder.open(conf) < 0)
    {
        std::cerr << "Error while opening H264 FFMPEG encoder.\n";
        return -1;
    }
    if (h264decoder.init() < 0)
    {
        std::cerr << "Error while opening H264 FFMPEG decoder.\n";
        return -1;
    }

    while (true)
    {
        cvOutputFrame = cv::Mat();
        capture >> cvInputFrame;
        if (cvInputFrame.empty())
        {
            break;
        }
        cvCopyFrame = cvInputFrame.clone();
        ffmpegAvPacket = h264encoder.encode(cvCopyFrame);
        if (!ffmpegAvPacket)
        {
            break;
        }
        h264Frame.setFrame(ffmpegAvPacket->data, ffmpegAvPacket->size);
#ifdef OpenCV_FFMPEG_CAM_DBG
        std::cout << "H264 frame size: " << (int)h264Frame.getFrameSize() << std::endl;
#endif // OpenCV_FFMPEG_CAM_DBG
        h264decoder.decode((uint8_t*)h264Frame.getData(), (int)h264Frame.getFrameSize(), cvOutputFrame);
        if (cvOutputFrame.empty())
        {
            break;
        }
        cv::imshow(WINDOW_INPUT_NAME, cvInputFrame);
        cv::imshow(WINDOW_OUTPUT_NAME, cvOutputFrame);
        key = cv::waitKey(1);
        if (key == 27 || key == 'q' || key == 'Q')
        {
            break;
        }
    }
    return 0;
}
