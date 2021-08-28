#include <opencv2/opencv.hpp>

#define LINK 0
#define WINDOW_NAME "OpenCV_CAM"

int main()
{
	cv::VideoCapture capture(LINK);
    if (!capture.isOpened())
    {
        std::cerr << "Opening of capture with id = " << LINK << " has been failed\n.";
        std::cerr << "Exit app.\n";
        return -1;
    }
    cv::Mat frame;
    char key;
    while (true)
    {
        capture >> frame;
        if (frame.empty())
        {
            break;
        }
        cv::imshow(WINDOW_NAME, frame);
        key = cv::waitKey(1);
        if (key == 27 || key == 'q' || key == 'Q')
        {
            break;
        }
    }
	return 0;
}
