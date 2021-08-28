#include <iostream>

#include <opencv2/opencv.hpp>

#include "cudp_mcast_server.h"

#include "H264EncConfig.h"
#include "AvH264_Encoder.h"
#include "H264_Frame.hpp"
#include "frame_package.h"

#define LINK 0
#define WINDOW_INPUT_NAME "OpenCV_CAM INPUT"

int main()
{
    /* Multicast IP addresses (224.0.0.0 to 239.255.255.255) */
    CUdpMcastServer server("224.0.0.251", 9094);
    try
    {
        server.initServer();
        
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
    cv::VideoCapture capture(LINK);
    if (!capture.isOpened())
    {
        std::cerr << "Opening of capture with id = " << LINK << " has been failed\n.";
        std::cerr << "Exit app.\n";
        return -1;
    }
    H264EncConfig conf;
    AvH264_Encoder h264encoder;
    AVPacket* ffmpegAvPacket;
    H264_Frame h264Frame;
    FramePackage package(&h264Frame);
    cv::Mat cvInputFrame;
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

    while (true)
    {
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
        h264Frame.setFrame(ffmpegAvPacket->data, (int)ffmpegAvPacket->size);
        try
        {
            server << &package;
        }
        catch (const CSocketException& e)
        {
            std::cerr << e.what() << std::endl;
            break;
        }
#ifdef OpenCV_FFMPEG_CAM_DBG
        std::cout << "H264 frame size: " << (int)h264Frame.getFrameSize() << std::endl;
#endif // OpenCV_FFMPEG_CAM_DBG
        cv::imshow(WINDOW_INPUT_NAME, cvInputFrame);
        key = cv::waitKey(1);
        if (key == 27 || key == 'q' || key == 'Q')
        {
            break;
        }
    }
	return 0;
}
