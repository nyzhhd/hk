#pragma once
#ifndef _HK_CAMERA_H_ 
#define _HK_CAMERA_H_

#include<HCNetSDK.h>
#include<plaympeg4.h>
#include<PlayM4.h>    //��ͷ�ļ���Ҫ��������ڶ�������Bug�еķ���ȥ���
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;
using namespace std;

class HK_camera
{
public:
	HK_camera(void);
	~HK_camera(void);

public:
	bool Init();                  //��ʼ��
	//bool Login(char* sDeviceAddress, char* sUserName, char* sPassword, WORD wPort);            //��½
	bool Login(const char* sDeviceAddress,const char* sUserName,const char* sPassword, WORD wPort);            //��½��VS2017�汾��
	void show();                  //��ʾͼ��

private:
	LONG lUserID;
};
#endif;