//#include "pch.h"
#include <opencv2/opencv.hpp>
#include <opencv2\imgproc\types_c.h>
#include <iostream>

using namespace std;
using namespace cv;

int main2()
{
	VideoCapture cap;//��ȡ��Ƶ
	Mat frame, gray, small;//��Ƶ�е�֡���Ҷ�ͼ������ͼ
	CascadeClassifier faceclassifier;//��������ķ�����
	vector<Rect> faceRect;//�洢������
	double scale = 2.0;//���ű�������Сͼ����Ϊ�˼�С��������
	int choice;
	cout << "1.������Ƶ�������" << endl
		<< "2.����ͷ�������" << endl;
	cin >> choice;
	switch (choice)
	{
	case 1:cap.open("D:\\��\\����¼��\\��Ե���˻���.mp4"); break;
	case 2:cap.open(0); break;
	default:
		break;
	}
	if (!cap.isOpened())
	{
		if (choice == 1)
		{
			cout << "���ض�ȡʧ�ܣ�" << endl;
			return -1;
		}
		else
		{
			cout << "������ͷʧ�ܣ�" << endl;
			return -1;
		}
	}
	while (1)
	{
		cap >> frame;//����Ƶ�ж�ȡ֡
		cvtColor(frame, gray, CV_BGR2GRAY);//תΪ�Ҷ�ͼ
		equalizeHist(gray, gray);//ֱ��ͼ���⻯
		resize(gray, small, Size(), 1.0 / scale, 1.0 / scale, INTER_LINEAR_EXACT);//��Сͼ��Ϊ������ٶȣ�
		if (!faceclassifier.load("D:\\adavance\\opencvʶ��ģ��\\haarcascade_upperbody.xml"))
		{
			cout << "��������������ʧ�ܣ�" << endl;
			return -1;
		}
		faceclassifier.detectMultiScale(small, faceRect, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
		for (size_t i = 0; i < faceRect.size(); i++)//�������м�⵽��Ŀ��
		{
			Rect r = faceRect[i];
			//������ͼ�񣬶���1������2����ɫ��������ϸ����������
			rectangle(frame, Point(r.x, r.y)*scale, Point(r.x + r.width, r.y + r.height)*scale, Scalar(0, 255, 0), 2, 8);
		}
		imshow("�������", frame);
		char ch = waitKey(1);
		if (ch == 27)//��ESC�˳�
			break;
	}
}