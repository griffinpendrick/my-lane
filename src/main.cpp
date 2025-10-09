#include <opencv2/opencv.hpp>

#define SafeDivide(A, B) ((B) ? ((A)/(B)) : (0))

inline cv::Mat ApplyGrayscale(cv::Mat Frame)
{
    cv::Mat GrayFrame;
    cv::cvtColor(Frame, GrayFrame, cv::COLOR_BGR2GRAY);
	return(GrayFrame);
}

inline cv::Mat ApplyBlur(cv::Mat Frame)
{
    cv::Mat BlurredFrame;
    cv::GaussianBlur(Frame, BlurredFrame, cv::Size(5, 5), 0);
	return(BlurredFrame);
}

inline cv::Mat ApplyCanny(cv::Mat Frame)
{
    cv::Mat Canny;
    cv::Canny(Frame, Canny, 80, 200);
    return(Canny);
}

inline cv::Mat ApplyROIMask(cv::Mat Frame)
{
    const float ROIBottomWidth = 0.85f;
    const float ROITopWidth    = 0.03f;
    const float ROIHeight	   = 0.43f;

	std::vector<cv::Point> Points = {
		cv::Point((1 - ROIBottomWidth) * Frame.cols / 2, Frame.rows),
		cv::Point((1 - ROITopWidth) * Frame.cols / 2, Frame.rows * (1.0f - ROIHeight)),
		cv::Point(Frame.cols - (1 - ROITopWidth) * Frame.cols / 2, Frame.rows * (1.0f - ROIHeight)),
		cv::Point(Frame.cols - (1 - ROIBottomWidth) * Frame.cols / 2, Frame.rows)
	};

    cv::Mat Mask = cv::Mat::zeros(Frame.size(), CV_8UC1);
    cv::fillPoly(Mask, Points, cv::Scalar(255));

    cv::Mat MaskedFrame;
    cv::bitwise_and(Frame, Frame, MaskedFrame, Mask);

    return(MaskedFrame);
}

inline std::vector<cv::Vec4i> HoughLineTransform(cv::Mat CannyFrame, cv::Mat Frame)
{
	std::vector<cv::Vec4i> Lines;
	cv::HoughLinesP(CannyFrame, Lines, 2.0, CV_PI / 180, 30, 10, 20);
	return(Lines);
}

inline cv::Mat DrawLane(cv::Mat Frame, std::vector<cv::Vec4i> Lines)
{
	if(!Lines.empty())
	{
		std::vector<cv::Point> LeftPoints;
		std::vector<cv::Point> RightPoints;

		for(size_t i = 0; i < Lines.size(); ++i)
		{
			cv::Vec4i Line = Lines[i];

			float x1 = Line[0];
			float y1 = Line[1];
			float x2 = Line[2];
			float y2 = Line[3];

			float Slope = SafeDivide((y2 - y1), (x2 - x1));

			if(fabsf(Slope) > 0.5f)
			{
				if(Slope < 0.0f)
				{
					LeftPoints.emplace_back(x1, y1);
					LeftPoints.emplace_back(x2, y2);
				}
				else
				{
					RightPoints.emplace_back(x1, y1);
					RightPoints.emplace_back(x2, y2);
				}
			}
		}

		cv::Vec4f LeftLine;
		cv::Vec4f RightLine;
		cv::Vec2i LeftX;
		cv::Vec2i RightX;

		if(!LeftPoints.empty() || !RightPoints.empty())
		{
			cv::fitLine(LeftPoints, LeftLine, cv::DIST_L2, 0, 0.01, 0.01);
			cv::fitLine(RightPoints, RightLine, cv::DIST_L2, 0, 0.01, 0.01);

			LeftX[0] = (int)(LeftLine[2] + (Frame.rows - LeftLine[3]) * (LeftLine[0] / LeftLine[1]));
			LeftX[1] = (int)(LeftLine[2] + (Frame.rows * 0.60f - LeftLine[3]) * (LeftLine[0] / LeftLine[1]));

			RightX[0] = (int)(RightLine[2] + (Frame.rows - RightLine[3]) * (RightLine[0] / RightLine[1]));
			RightX[1] = (int)(RightLine[2] + (Frame.rows * 0.60f - RightLine[3]) * (RightLine[0] / RightLine[1]));
		}
		else
		{
			return(Frame);
		}

		std::vector<cv::Point> Points = {
			cv::Point(LeftX[0], Frame.rows),
			cv::Point(LeftX[1], Frame.rows * 0.60f),

			cv::Point(RightX[1], Frame.rows * 0.60f),
			cv::Point(RightX[0], Frame.rows)
		};

		cv::Mat Mask = cv::Mat::zeros(Frame.size(), Frame.type());
		cv::fillConvexPoly(Mask, Points, cv::Scalar(0, 255, 0));

		cv::Mat Result;
		cv::addWeighted(Frame, 1.0, Mask, 0.5, 0.0, Result);
		cv::line(Result, Points[0], Points[1], cv::Scalar(0, 255, 0), 2);
		cv::line(Result, Points[2], Points[3], cv::Scalar(0, 255, 0), 2);


		float LeftLaneSlope  = (Frame.rows * 0.60f - Frame.rows) / (LeftX[1] - LeftX[0]);
		float RightLaneSlope = (Frame.rows * 0.60f - Frame.rows) / (RightX[1] - RightX[0]);

		char T1[32];
		sprintf_s(T1, "Right lane slope: %f", RightLaneSlope);

		char T2[32];
		sprintf_s(T2, "Left lane slope: %f", LeftLaneSlope);

		cv::putText(Result, T1, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.60, cv::Scalar(15, 15, 15), 2);
		cv::putText(Result, T2, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.60, cv::Scalar(15, 15, 15), 2);

		return(Result);
	}

	return(Frame);
}

int main(int ArgCount, char** Args)
{
    cv::VideoCapture Capture("../sample.mp4");
	cv::namedWindow("my-lane", cv::WINDOW_NORMAL);

    if(Capture.isOpened())
    {
		cv::Mat Frame;
		cv::Mat OutputFrame;
		for(;;)
		{
			if(!Capture.read(Frame)) break;

			OutputFrame = Frame;
			OutputFrame = ApplyGrayscale(OutputFrame);
			OutputFrame = ApplyBlur(OutputFrame);
			OutputFrame = ApplyCanny(OutputFrame);
			OutputFrame = ApplyROIMask(OutputFrame);
			OutputFrame = DrawLane(Frame, HoughLineTransform(OutputFrame, Frame));
			
			cv::imshow("my-lane", OutputFrame);
			cv::waitKey(16);
		}
    }

    Capture.release();
	cv::destroyAllWindows();
    return(0);
}
