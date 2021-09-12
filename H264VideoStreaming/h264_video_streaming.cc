#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <ctime> 

#include "cudp_mcast_server.h"
#include "H264EncConfig.h"
#include "AvH264_Encoder.h"
#include "H264_Frame.hpp"
#include "frame_package.h"

#include <opencv2/opencv.hpp>

#define LINK 0
#define WINDOW_INPUT_NAME "OpenCV_CAM INPUT"

#ifndef OUTPUT_FILE_PATH
#error "There are not OUTPUT FILE PATH"
#else

#define VIDEO1_NAME "video_1.avi"
#define VIDEO2_NAME "video_2.avi"

#define VIDEO1_PATH OUTPUT_FILE_PATH "/" VIDEO1_NAME
#define VIDEO2_PATH OUTPUT_FILE_PATH "/" VIDEO2_NAME

static constexpr const char* const cVideoNames[2] =
{
    VIDEO1_NAME,
    VIDEO2_NAME
};

static constexpr const char* const cVideoPaths[2] =
{
    VIDEO1_PATH,
    VIDEO2_PATH
};

#endif

namespace
{
    static int generateVideoFilename(cv::VideoWriter& writer, const cv::Size& size, double fps, bool& can_stream) noexcept(false)
    {
        static bool elapsed = false;
        static const int fourcc = cv::VideoWriter::fourcc('D', 'I', 'V', 'X');
        static int id = 1;
        id = !id;
        std::remove(cVideoPaths[id]);

        if (writer.isOpened())
        {
            writer.release();
        }
        if (!writer.open(cVideoPaths[id], fourcc, fps, size, true /*isColor*/))
        {
            std::stringstream ss;
            ss << "Failed to open file " << cVideoPaths[id] << ".";
            throw std::runtime_error(ss.str());
        }
        can_stream = elapsed;
        elapsed = true;
        return id;
    }
}

int main()
{
    /* Check OpenCV version */
    std::cout << "OpenCV version : " << CV_VERSION << std::endl;
    std::cout << "Major version : " << CV_MAJOR_VERSION << std::endl;
    std::cout << "Minor version : " << CV_MINOR_VERSION << std::endl;
    std::cout << "Subminor version : " << CV_SUBMINOR_VERSION << std::endl << std::endl;

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

    /* video handling variables */
    cv::VideoCapture webcamCapture(LINK);   // webcam reader
    cv::VideoCapture videoCapture;          // video reader
    cv::VideoWriter videoWriter;            // video writer

    if (!webcamCapture.isOpened())
    {
        std::cerr << "Opening of webcamCapture with id = " << LINK << " has been failed\n.";
        return -1;
    }

    /* video paths */
    std::string currentVideo;
    std::string potentialVideo;

    /* cv frame handling variables */
    cv::Mat cvWebcamFrame;
    cv::Mat cvVideoFrame;

    /* helper checkers variables */
    char key;
    bool bStream = false;               // start MCAST streaming
    int recordingId = -1;

    /* H264 utils variables */
    H264EncConfig conf;
    AvH264_Encoder h264encoder;
    AVPacket* ffmpegAvPacket;
    H264_Frame h264Frame;
    FramePackage package(&h264Frame);

    /* Init H264 encoder */
    conf.bit_rate = 1024;
    conf.width = (int)webcamCapture.get(cv::CAP_PROP_FRAME_WIDTH);
    conf.height = (int)webcamCapture.get(cv::CAP_PROP_FRAME_HEIGHT);
    conf.gop_size = 60;
    conf.max_b_frames = 0;
    conf.frame_rate = 12;

    if (h264encoder.open(conf) < 0)
    {
        std::cerr << "Error while opening H264 FFMPEG encoder.\n";
        return -1;
    }

    /* Video writing utils constants/variables */
    const cv::Size outputSize(conf.width, conf.height);
    const double fps = 30.0f;
    uint16_t counter = 0;
    std::time_t currTimestamp;
    std::string currTimestampString;
    const std::string cSpace = " ";

    while (true)
    {
        if (!counter)
        {
            try
            {
                recordingId = ::generateVideoFilename(videoWriter, outputSize, fps, bStream);
                potentialVideo = cVideoPaths[!recordingId];
            }
            catch (const std::runtime_error& e)
            {
                std::cerr << e.what() << std::endl;
                return -1;
            }
        }
        counter = (counter+1) % 1024;

        webcamCapture >> cvWebcamFrame;         // read frame from Webcam
        if (cvWebcamFrame.empty())
        {
            break;
        }

        /* get timestamp and put it into frame */
        currTimestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        currTimestampString = std::ctime(&currTimestamp);
        currTimestampString.resize(currTimestampString.size() - 1);
        currTimestampString += cSpace + cVideoNames[recordingId];

        cv::putText(cvWebcamFrame, currTimestampString, cv::Point(30, 30),
            cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(200, 200, 250), 1, cv::LINE_AA);

        videoWriter << cvWebcamFrame;           // write frame into .avi video file
        
        if (bStream && currentVideo != potentialVideo)
        {
            /* open new video */
            currentVideo = potentialVideo;
            if (videoCapture.isOpened())
            {
                videoCapture.release();
            }
            videoCapture.open(currentVideo);
            if (!videoCapture.isOpened())
            {
                std::cerr << "videoCapture failed to open file: " << currentVideo << std::endl;
                return -1;
            }
        }

        if (bStream)
        {
            videoCapture >> cvVideoFrame;       // read frame from .avi video file
            ffmpegAvPacket = h264encoder.encode(cvVideoFrame);
            if (!ffmpegAvPacket)
            {
                std::cerr << "Encode of cvVideoFrame failed.\n";
                break;
            }
            h264Frame.setFrame(ffmpegAvPacket->data, (int)ffmpegAvPacket->size);
            try
            {
                server << &package;             // send frame through MCAST stream
            }
            catch (const CSocketException& e)
            {
                std::cerr << e.what() << std::endl;
                break;
            }
        }
        cv::imshow(WINDOW_INPUT_NAME, cvWebcamFrame);
        key = cv::waitKey(1);
        if (key == 27 || key == 'q' || key == 'Q')
        {
            break;
        }
    }
    return 0;
}
