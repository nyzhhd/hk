//#include "stdafx.h" //VS2017����Ҫ��Ӵ�Ԥ����ͷ�ļ�
#include"HK_camera.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2\imgproc\types_c.h>


using namespace std;
using namespace cv;

// ȫ�ֱ���
CascadeClassifier faceclassifier;//��������ķ�����

//ȫ�ֱ���
LONG g_nPort;
Mat g_BGRImage;

//���ݽ���ص�������
//���ܣ���YV_12��ʽ����Ƶ������ת��Ϊ�ɹ�opencv�����BGR���͵�ͼƬ���ݣ���ʵʱ��ʾ��
void CALLBACK DecCBFun(long nPort, char* pBuf, long nSize, FRAME_INFO* pFrameInfo, long nUser, long nReserved2)
{
	if (pFrameInfo->nType == T_YV12)
	{
		std::cout << "the frame infomation is T_YV12" << std::endl;
		if (g_BGRImage.empty())
		{
			g_BGRImage.create(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC3);
		}
		Mat YUVImage(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, (unsigned char*)pBuf);
		cvtColor(YUVImage, g_BGRImage, COLOR_YUV2BGR_YV12);

		Mat frame, gray, small;//��Ƶ�е�֡���Ҷ�ͼ������ͼ
		vector<Rect> faceRect;//�洢������
		double scale = 2.0;//���ű�������Сͼ����Ϊ�˼�С��������
		frame = g_BGRImage;
		cvtColor(frame, gray, CV_BGR2GRAY);//תΪ�Ҷ�ͼ
		equalizeHist(gray, gray);//ֱ��ͼ���⻯
		resize(gray, small, Size(), 1.0 / scale, 1.0 / scale, INTER_LINEAR_EXACT);//��Сͼ��Ϊ������ٶȣ�

		faceclassifier.detectMultiScale(small, faceRect, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));


		for (size_t i = 0; i < faceRect.size(); i++)//�������м�⵽��Ŀ��
		{
			Rect r = faceRect[i];
			//������ͼ�񣬶���1������2����ɫ��������ϸ����������
			rectangle(frame, Point(r.x, r.y)*scale, Point(r.x + r.width, r.y + r.height)*scale, Scalar(0, 255, 0), 2, 8);
			// Add a label to the detected face
			putText(frame, "face", Point(r.x * scale, r.y * scale - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
		}

		g_BGRImage= frame ;


		imshow("RGBImage1", g_BGRImage);
		waitKey(15);
		YUVImage.~Mat();
	}
}

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