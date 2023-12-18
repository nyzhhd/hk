//#include "stdafx.h" //VS2017中需要添加此预编译头文件
#include"HK_camera.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2\imgproc\types_c.h>


using namespace std;
using namespace cv;

// 全局变量
CascadeClassifier faceclassifier;//检测人脸的分类器

//全局变量
LONG g_nPort;
Mat g_BGRImage;

//数据解码回调函数，
//功能：将YV_12格式的视频数据流转码为可供opencv处理的BGR类型的图片数据，并实时显示。
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

		Mat frame, gray, small;//视频中的帧，灰度图，缩略图
		vector<Rect> faceRect;//存储检测矩形
		double scale = 2.0;//缩放比例（缩小图像是为了减小运算量）
		frame = g_BGRImage;
		cvtColor(frame, gray, CV_BGR2GRAY);//转为灰度图
		equalizeHist(gray, gray);//直方图均衡化
		resize(gray, small, Size(), 1.0 / scale, 1.0 / scale, INTER_LINEAR_EXACT);//缩小图像（为了提高速度）

		faceclassifier.detectMultiScale(small, faceRect, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));


		for (size_t i = 0; i < faceRect.size(); i++)//绘制所有检测到的目标
		{
			Rect r = faceRect[i];
			//参数：图像，顶点1，顶点2，颜色，线条粗细，线条类型
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

//实时视频码流数据获取 回调函数
void CALLBACK g_RealDataCallBack_V30(LONG lPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void* pUser)
{
	if (dwDataType == NET_DVR_STREAMDATA)//码流数据
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
//构造函数
HK_camera::HK_camera(void)
{

}
//析构函数
HK_camera::~HK_camera(void)
{
}
//初始化函数，用作初始化状态检测
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

//登录函数，用作摄像头id以及密码输入登录
//bool HK_camera::Login(char* sDeviceAddress, char* sUserName, char* sPassword, WORD wPort)
bool HK_camera::Login(const char* sDeviceAddress,const char* sUserName,const char* sPassword, WORD wPort)        //登陆（VS2017版本）
{
	NET_DVR_USER_LOGIN_INFO pLoginInfo = { 0 };
	NET_DVR_DEVICEINFO_V40 lpDeviceInfo = { 0 };

	pLoginInfo.bUseAsynLogin = 0;     //同步登录方式
	strcpy_s(pLoginInfo.sDeviceAddress, sDeviceAddress);
	strcpy_s(pLoginInfo.sUserName, sUserName);
	strcpy_s(pLoginInfo.sPassword, sPassword);
	pLoginInfo.wPort = wPort;

	lUserID = NET_DVR_Login_V40(&pLoginInfo, &lpDeviceInfo);
	faceclassifier.load("D:\\adavance\\opencv识别模型\\haarcascade_frontalface_alt2.xml");//haarcascade_upperbody


	if (lUserID < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//视频流显示函数
void HK_camera::show()
{
	if (PlayM4_GetPort(&g_nPort))            //获取播放库通道号
	{
		if (PlayM4_SetStreamOpenMode(g_nPort, STREAME_REALTIME))      //设置流模式
		{
			if (PlayM4_OpenStream(g_nPort, NULL, 0, 1024 * 1024))         //打开流
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
	//启动预览并设置回调数据流
	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	struPlayInfo.hPlayWnd = NULL; //窗口为空，设备SDK不解码只取流
	struPlayInfo.lChannel = 1; //Channel number 设备通道
	struPlayInfo.dwStreamType = 0;// 码流类型，0-主码流，1-子码流，2-码流3，3-码流4, 4-码流5,5-码流6,7-码流7,8-码流8,9-码流9,10-码流10
	struPlayInfo.dwLinkMode = 0;// 0：TCP方式,1：UDP方式,2：多播方式,3 - RTP方式，4-RTP/RTSP,5-RSTP/HTTP 
	struPlayInfo.bBlocked = 1; //0-非阻塞取流, 1-阻塞取流, 如果阻塞SDK内部connect失败将会有5s的超时才能够返回,不适合于轮询取流操作.

	if (NET_DVR_RealPlay_V40(lUserID, &struPlayInfo, g_RealDataCallBack_V30, NULL))
	{
		namedWindow("RGBImage2");
	}
}