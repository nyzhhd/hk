//#include "stdafx.h" //VS2017����Ҫ��Ӵ�Ԥ����ͷ�ļ�
#include"HK_camera.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2\imgproc\types_c.h>
#include <stdio.h>
#include "Windows.h"
#include <time.h>


using namespace std;
using namespace cv;

// ȫ�ֱ���
CascadeClassifier faceclassifier;//��������ķ�����
LONG UserID;
int iChannel = 1;//�豸ͨ����

// ȫ�ֱ��������ڴ洢�������ĵ����ʷλ��
std::vector<Point> faceCenters;

//ȫ�ֱ���
LONG g_nPort;
Mat g_BGRImage;

//���ݽ���ص�������
//���ܣ���YV_12��ʽ����Ƶ������ת��Ϊ�ɹ�opencv�����BGR���͵�ͼƬ���ݣ���ʵʱ��ʾ��  ��������ͷ����faceĿ���
void CALLBACK DecCBFun(long nPort, char* pBuf, long nSize, FRAME_INFO* pFrameInfo, long nUser, long nReserved2)
{
	if (pFrameInfo->nType == T_YV12)
	{
		std::cout << "the frame information is T_YV12" << std::endl;
		if (g_BGRImage.empty())
		{
			g_BGRImage.create(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC3);
		}
		Mat YUVImage(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, (unsigned char*)pBuf);
		cvtColor(YUVImage, g_BGRImage, COLOR_YUV2BGR_YV12);

		Mat frame, gray, small;
		vector<Rect> faceRect;
		double scale = 2.0; // ���ű���

		frame = g_BGRImage;
		cvtColor(frame, gray, CV_BGR2GRAY);
		equalizeHist(gray, gray);
		resize(gray, small, Size(), 1.0 / scale, 1.0 / scale, INTER_LINEAR_EXACT);

		faceclassifier.detectMultiScale(small, faceRect, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

		// ��������������
		Rect largestFace;
		int maxArea = 0;
		for (size_t i = 0; i < faceRect.size(); i++)
		{
			Rect r = faceRect[i];
			int area = r.width * r.height;
			if (area > maxArea)
			{
				maxArea = area;
				largestFace = r;
			}
		}

		if (maxArea > 0) // �����⵽������
		{
			// ��������������
			rectangle(frame, Point(largestFace.x, largestFace.y) * scale, Point(largestFace.x + largestFace.width, largestFace.y + largestFace.height) * scale, Scalar(0, 255, 0), 2, 8);
			putText(frame, "face", Point(largestFace.x * scale, largestFace.y * scale - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);

			// �����������ĵ��ͼ�����ĵ�
			Point faceCenter(largestFace.x + largestFace.width / 2, largestFace.y + largestFace.height / 2);
			faceCenter *= scale;

			// ���µ����ĵ���ӵ���ʷλ���б���
			faceCenters.push_back(faceCenter);

			// �����������ĵ����ʷ�켣
			for (size_t i = 1; i < faceCenters.size(); i++)
			{
				line(frame, faceCenters[i - 1], faceCenters[i], Scalar(255, 0, 0), 2);
			}

			Point imageCenter(frame.cols / 2, frame.rows / 2);

			// ��������ͷ�ƶ�
			bool needToMove = false;
			int command = 0;
			int deltaX = abs(faceCenter.x - imageCenter.x);
			int deltaY = abs(faceCenter.y - imageCenter.y);
			const int centerTolerance = 30;

			if (deltaX > centerTolerance || deltaY > centerTolerance)
			{
				needToMove = true;
				// ����X����ƫ��ѡ������
				if (deltaX > centerTolerance)
				{
					command = (faceCenter.x < imageCenter.x) ? PAN_LEFT : PAN_RIGHT;
				}
				// ����Y����ƫ��ѡ������
				if (deltaY > centerTolerance)
				{
					if (faceCenter.y < imageCenter.y)
					{
						command = (command == PAN_LEFT) ? UP_LEFT : (command == PAN_RIGHT) ? UP_RIGHT : TILT_UP;
					}
					else
					{
						command = (command == PAN_LEFT) ? DOWN_LEFT : (command == PAN_RIGHT) ? DOWN_RIGHT : TILT_DOWN;
					}
				}
			}

			if (needToMove)
			{
				DWORD dwStop = 0;
				NET_DVR_PTZControl_Other(UserID, iChannel, command, dwStop);
				Sleep(std::max(deltaX, deltaY) / 10); // Sleepʱ����ƫ��ɱ���
				dwStop = 1;
				NET_DVR_PTZControl_Other(UserID, iChannel, command, dwStop);
			}
		}

		g_BGRImage = frame;
		imshow("RGBImage1", g_BGRImage);
		waitKey(15);
		YUVImage.release();
	}
}



//TILT_UP            21    /* ��̨��SS���ٶ����� */
//TILT_DOWN        22    /* ��̨��SS���ٶ��¸� */
//PAN_LEFT        23    /* ��̨��SS���ٶ���ת */
//PAN_RIGHT        24    /* ��̨��SS���ٶ���ת */
//UP_LEFT            25    /* ��̨��SS���ٶ���������ת */
//UP_RIGHT        26    /* ��̨��SS���ٶ���������ת */
//DOWN_LEFT        27    /* ��̨��SS���ٶ��¸�����ת */
//DOWN_RIGHT        28    /* ��̨��SS���ٶ��¸�����ת */
//PAN_AUTO        29    /* ��̨��SS���ٶ������Զ�ɨ�� */

//ʵʱ��Ƶ�������ݻ�ȡ �ص�����
void CALLBACK g_RealDataCallBack_V30(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser)
{
	if (dwDataType == NET_DVR_STREAMDATA)//��������
	{
		if (dwBufSize > 0 && g_nPort != -1)
		{
			if (!PlayM4_InputData(g_nPort, pBuffer, dwBufSize))
			{
				std::cout << "fail input data" << std::endl;
			}
			else
			{
				std::cout << "success input data" << std::endl;
			}

		}
	}
}
//���캯��
HK_camera::HK_camera(void)
{

}
//��������
HK_camera::~HK_camera(void)
{
}
//��ʼ��������������ʼ��״̬���
bool HK_camera::Init()
{
	if (NET_DVR_Init())
	{
		return true;
	}
	else
	{
		return false;
	}
}

//��¼��������������ͷid�Լ����������¼
//bool HK_camera::Login(char* sDeviceAddress, char* sUserName, char* sPassword, WORD wPort)
bool HK_camera::Login(const char* sDeviceAddress,const char* sUserName,const char* sPassword, WORD wPort)        //��½��VS2017�汾��
{
	NET_DVR_USER_LOGIN_INFO pLoginInfo = { 0 };
	NET_DVR_DEVICEINFO_V40 lpDeviceInfo = { 0 };

	pLoginInfo.bUseAsynLogin = 0;     //ͬ����¼��ʽ
	strcpy_s(pLoginInfo.sDeviceAddress, sDeviceAddress);
	strcpy_s(pLoginInfo.sUserName, sUserName);
	strcpy_s(pLoginInfo.sPassword, sPassword);
	pLoginInfo.wPort = wPort;

	lUserID = NET_DVR_Login_V40(&pLoginInfo, &lpDeviceInfo);
	UserID = lUserID;
	faceclassifier.load("D:\\adavance\\opencvʶ��ģ��\\haarcascade_frontalface_alt2.xml");//haarcascade_upperbody

	if (lUserID < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//��Ƶ����ʾ����
void HK_camera::show()
{
	if (PlayM4_GetPort(&g_nPort))            //��ȡ���ſ�ͨ����
	{
		if (PlayM4_SetStreamOpenMode(g_nPort, STREAME_REALTIME))      //������ģʽ
		{
			if (PlayM4_OpenStream(g_nPort, NULL, 0, 1024 * 1024))         //����
			{
				if (PlayM4_SetDecCallBackExMend(g_nPort, DecCBFun, NULL, 0, NULL))
				{
					if (PlayM4_Play(g_nPort, NULL))
					{
						std::cout << "success to set play mode" << std::endl;
					}
					else
					{
						std::cout << "fail to set play mode" << std::endl;
					}
				}
				else
				{
					std::cout << "fail to set dec callback " << std::endl;
				}
			}
			else
			{
				std::cout << "fail to open stream" << std::endl;
			}
		}
		else
		{
			std::cout << "fail to set stream open mode" << std::endl;
		}
	}
	else
	{
		std::cout << "fail to get port" << std::endl;
	}
	//����Ԥ�������ûص�������
	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	struPlayInfo.hPlayWnd = NULL; //����Ϊ�գ��豸SDK������ֻȡ��
	struPlayInfo.lChannel = 1; //Channel number �豸ͨ��
	struPlayInfo.dwStreamType = 0;// �������ͣ�0-��������1-��������2-����3��3-����4, 4-����5,5-����6,7-����7,8-����8,9-����9,10-����10
	struPlayInfo.dwLinkMode = 0;// 0��TCP��ʽ,1��UDP��ʽ,2���ಥ��ʽ,3 - RTP��ʽ��4-RTP/RTSP,5-RSTP/HTTP 
	struPlayInfo.bBlocked = 1; //0-������ȡ��, 1-����ȡ��, �������SDK�ڲ�connectʧ�ܽ�����5s�ĳ�ʱ���ܹ�����,���ʺ�����ѯȡ������.

	if (NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, g_RealDataCallBack_V30, NULL))
	{
		namedWindow("RGBImage2");
	}
}