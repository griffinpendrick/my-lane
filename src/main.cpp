#include <opencv2/opencv.hpp>

cv::Mat ApplyCanny(cv::Mat Frame)
{
    cv::Mat GrayFrame;
    cv::cvtColor(Frame, GrayFrame, cv::COLOR_BGR2GRAY);

    cv::Mat BlurredFrame;
    cv::GaussianBlur(GrayFrame, BlurredFrame, cv::Size(5, 5), 0);

    cv::Mat Canny;
    cv::Canny(BlurredFrame, Canny, 50, 150);

    return(Canny);
}

int main(int ArgCount, char** Args)
{
    cv::VideoCapture Capture("video1.mp4");

    if(Capture.isOpened())
    {
		cv::Mat Frame;
		for(;;)
		{
			if(!Capture.read(Frame))
			{
				break;
			}

			cv::Mat Canny = ApplyCanny(Frame);
			cv::imshow("my-lane", Canny);

			if (cv::waitKey(30) >= 0)
			{
				break;
			}
		}
    }

    Capture.release();
    cv::destroyAllWindows();
    return(0);
}
