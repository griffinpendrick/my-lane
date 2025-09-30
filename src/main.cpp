
// Segment -> Hough -> Output Frame

#include <stdio.h>
#include <opencv2/opencv.hpp>

cv::Mat ApplyCanny(cv::Mat Image)
{
    cv::Mat GrayImage;
    cv::cvtColor(Image, GrayImage, cv::COLOR_RGB2GRAY);

    cv::Mat BlurredImage;
    cv::GaussianBlur(GrayImage, BlurredImage, cv::Size(5, 5), 0);

    cv::Mat Canny;
    cv::Canny(BlurredImage, Canny, 50, 150);

    return(Canny);
}

cv::Mat ApplySegment(cv::Mat Image)
{
    return(NULL);
}

int main(int ArgCount, char** Args)
{
    cv::namedWindow("mylane", cv::WINDOW_NORMAL);

    cv::Mat Image = cv::imread("image.png", cv::IMREAD_COLOR);

    cv::Mat Canny = ApplyCanny(Image);

    cv::imshow("mylane", Canny);

    cv::waitKey(0);
    cv::destroyAllWindows();

    return(0);
}