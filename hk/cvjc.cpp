//#include "pch.h"
#include <opencv2/opencv.hpp>
#include <opencv2\imgproc\types_c.h>
#include <iostream>

using namespace std;
using namespace cv;

int main2()
{
	VideoCapture cap;//读取视频
	Mat frame, gray, small;//视频中的帧，灰度图，缩略图
	CascadeClassifier faceclassifier;//检测人脸的分类器
	vector<Rect> faceRect;//存储检测矩形
	double scale = 2.0;//缩放比例（缩小图像是为了减小运算量）
	int choice;
	cout << "1.本地视频人脸检测" << endl
		<< "2.摄像头人脸检测" << endl;
	cin >> choice;
	switch (choice)
	{
	case 1:cap.open("D:\\研\\会议录制\\边缘过滤会议.mp4"); break;
	case 2:cap.open(0); break;
	default:
		break;
	}
	if (!cap.isOpened())
	{
		if (choice == 1)
		{
			cout << "本地读取失败！" << endl;
			return -1;
		}
		else
		{
			cout << "打开摄像头失败！" << endl;
			return -1;
		}
	}
	while (1)
	{
		cap >> frame;//从视频中读取帧
		cvtColor(frame, gray, CV_BGR2GRAY);//转为灰度图
		equalizeHist(gray, gray);//直方图均衡化
		resize(gray, small, Size(), 1.0 / scale, 1.0 / scale, INTER_LINEAR_EXACT);//缩小图像（为了提高速度）
		if (!faceclassifier.load("D:\\adavance\\opencv识别模型\\haarcascade_upperbody.xml"))
		{
			cout << "加载人脸分类器失败！" << endl;
			return -1;
		}
		faceclassifier.detectMultiScale(small, faceRect, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
		for (size_t i = 0; i < faceRect.size(); i++)//绘制所有检测到的目标
		{
			Rect r = faceRect[i];
			//参数：图像，顶点1，顶点2，颜色，线条粗细，线条类型
			rectangle(frame, Point(r.x, r.y)*scale, Point(r.x + r.width, r.y + r.height)*scale, Scalar(0, 255, 0), 2, 8);
		}
		imshow("人脸检测", frame);
		char ch = waitKey(1);
		if (ch == 27)//按ESC退出
			break;
	}
}