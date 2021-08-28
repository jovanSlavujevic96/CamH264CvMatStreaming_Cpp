#include <iostream>

#include <opencv2/opencv.hpp>

#include "cudp_mcast_client.h"

#include "AvH264_Decoder.h"
#include "H264_Frame.hpp"
#include "frame_package.h"

#define WINDOW_OUTPUT_NAME "OpenCV_CAM OUTPUT"
#define MAX_UDP_FRAME 65535u

int main()
{
	/* Multicast IP addresses (224.0.0.0 to 239.255.255.255) */
	CUdpMcastClient client("224.0.0.251", 9094);
	try
	{
		client.initClient();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return -1;
	}
	AvH264_Decoder h264decoder;
	H264_Frame h264Frame(MAX_UDP_FRAME);
	FramePackage package(&h264Frame);
	cv::Mat cvOutputFrame;
	char key;
	int32_t rcvBytes;
	int decodeRet;

	if (h264decoder.init() < 0)
	{
		std::cerr << "Error while opening H264 FFMPEG decoder.\n";
		return -1;
	}

	while (true)
	{
		cvOutputFrame = cv::Mat();
		try
		{
			rcvBytes = client >> &package;
		}
		catch (const CSocketException& e)
		{
			std::cerr << e.what() << std::endl;
			break;
		}
#ifdef OpenCV_FFMPEG_CAM_DBG
		std::cout << "H264 frame size: " << (int)h264Frame.getFrameSize() << std::endl;
#endif // OpenCV_FFMPEG_CAM_DBG
		decodeRet = h264decoder.decode((uint8_t*)h264Frame.getData(), (int)rcvBytes, cvOutputFrame);
		if (decodeRet < 0)
		{
			std::cout << "Skipped this frame\n";
			continue;
		}
		if (cvOutputFrame.empty())
		{
			break;
		}
		cv::imshow(WINDOW_OUTPUT_NAME, cvOutputFrame);
		key = cv::waitKey(1);
		if (key == 27 || key == 'q' || key == 'Q')
		{
			break;
		}
	}
	return 0;
}
