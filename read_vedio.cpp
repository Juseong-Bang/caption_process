#include<iostream>
#include<fstream>
#include <string>
#include "srtparser.h"
#include<opencv2\opencv.hpp>
using namespace std;
using namespace cv;

void put_string(Mat &frame, String text, Point pt) {
	int width = frame.cols;
	int hight = frame.rows;

	pt += Point(width / 2 - text.length() * 11 / 2, hight*0.95);

	Point shade = pt + Point(2, 2);
	int font = FONT_HERSHEY_SIMPLEX;
	putText(frame, text, shade, font, 0.7, Scalar(0, 0, 0), 2);
	putText(frame, text, pt, font, 0.7, Scalar(255, 255, 255), 2);
}

int main() {


	VideoCapture capture;
	capture.open("data\\Marvel Studios' Black Panther - Official Trailer.avi");
	//capture.open("data\\videoplayback.mp4");

	SubtitleParserFactory *subParserFactory = new SubtitleParserFactory("data\\Marvel Studios' Black Panther - Official Trailer.srt");
	//SubtitleParserFactory *subParserFactory = new SubtitleParserFactory("data\\Ma.srt");

	SubtitleParser *parser = subParserFactory->getParser();
	std::vector<SubtitleItem*> sub = parser->getSubtitles();

	CV_Assert(capture.isOpened());

	double frame_rate = capture.get(CV_CAP_PROP_FPS);

	double delay = 1000 / frame_rate;
	int frame_cnt = 0;
	Mat frame;
	int i = 0;
	SubtitleItem * element = sub.front();
	long st = element->getStartTime();
	long en = element->getEndTime();
	String dia = element->getDialogue();
	Mat hsv, tem, hsv_arr[3], tem3;

	while (capture.read(frame))
	{
		frame_cnt++;
		if (frame_cnt < 309)
			continue;

		if (waitKey(delay) >= 0)
			break;
		
		cvtColor(frame, hsv, CV_BGR2HSV);//BGR->> HSV
		split(hsv, hsv_arr);//split H,s,v factor

		int a[] = { 0 };
		int b[] = { 256 };
		float d[] = { 0,(float)256 };
		const float * c[] = { d };
		calcHist(&hsv_arr[2], 1, a, Mat(), tem, 1, b, c);

		Mat hist_img = Mat(Size(256, 200), CV_8U, Scalar(255));
		float bin = (float)hist_img.cols / tem.rows;
		normalize(tem, tem, 0, hist_img.rows, NORM_MINMAX);
		for (int t = 0; t < tem.rows; t++)
		{
			float start_x = t*bin;
			float end_x = (t + 1)*bin;
			Point2f pt1(start_x, 0);
			Point2f pt2(end_x, tem.at<float>(t));
			if (pt2.y > 0)
				rectangle(hist_img, pt1, pt2, Scalar(0), -1);

		}
		flip(hist_img, hist_img, 0);
		equalizeHist(hsv_arr[2], hsv_arr[2]);//eq using v

		merge(hsv_arr, 3, hsv);//merge 
		cvtColor(hsv, tem3, CV_HSV2BGR);//hsv-> BGR
		
		if (frame_cnt > 309)
			if (frame_cnt % 10 == 0)
			{
				imwrite("capture\\frame" + to_string(frame_cnt) + "_1.jpg", frame);
				imwrite("capture\\frame" + to_string(frame_cnt) + "_3.jpg", hist_img);
				imwrite("capture\\frame" + to_string(frame_cnt) + "_2.jpg", tem3);
			}
		// save frame to image 

		if ((st < delay*frame_cnt) && (delay*frame_cnt < en))
			put_string(frame, dia, Point(0, 0));//include caption 

		imshow("read movie", frame);// after histo eq

		if (en < delay*frame_cnt)
		{
			element = sub.at(++i);
			st = element->getStartTime();
			en = element->getEndTime();
			dia = element->getDialogue();
		}
		//	frame_cnt++;
	}
	return 0;
}

void draw_histo(const Mat & image, Mat &hist, int bins, int range_max = 256) 
{

}