#pragma once
#ifndef _HK_CAMERA_H_ 
#define _HK_CAMERA_H_

#include<HCNetSDK.h>
#include<plaympeg4.h>
#include<PlayM4.h>    //此头文件需要按照下面第二步调试Bug中的方法去添加
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
	bool Init();                  //初始化
	//bool Login(char* sDeviceAddress, char* sUserName, char* sPassword, WORD wPort);            //登陆
	bool Login(const char* sDeviceAddress,const char* sUserName,const char* sPassword, WORD wPort);            //登陆（VS2017版本）
	void show();                  //显示图像

private:
	LONG lUserID;
};
#endif;