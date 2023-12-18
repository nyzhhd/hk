#include <stdio.h>
#include <iostream>
#include "Windows.h"
#include "HCNetSDK.h"
#include <time.h>
using namespace std;

void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
	char tempbuf[256] = { 0 };
	switch (dwType)
	{
	case EXCEPTION_RECONNECT:    //预览时重连
		printf("----------reconnect--------%d\n", time(NULL));
		break;
	default:
		break;
	}
}

void main2223() {

	//---------------------------------------
	// 初始化
	NET_DVR_Init();
	//设置连接时间与重连时间
	NET_DVR_SetConnectTime(2000, 1);
	NET_DVR_SetReconnect(10000, true);

	//---------------------------------------
	//设置异常消息回调函数
	NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack, NULL);

	//---------------------------------------
	// 注册设备
	LONG lUserID;

	//登录参数，包括设备地址、登录用户、密码等
	NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
	struLoginInfo.bUseAsynLogin = 0; //同步登录方式
	strcpy(struLoginInfo.sDeviceAddress, "169.254.25.95"); //设备IP地址
	struLoginInfo.wPort = 8000; //设备服务端口
	strcpy(struLoginInfo.sUserName, "admin"); //设备登录用户名
	strcpy(struLoginInfo.sPassword, "gyb18800"); //设备登录密码

	//设备信息, 输出参数
	NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };

	lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
	if (lUserID < 0)
	{
		printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return;
	}

	int iChannel = 1;//设备通道号
	DWORD dwPTZCommand = PAN_LEFT; //云台左转
	DWORD dwStop = 0; //开始转动
	if (!NET_DVR_PTZControl_Other(lUserID, iChannel, PAN_LEFT, dwStop))
	{
		printf("PAN_LEFT start failed, error code: %d\n", NET_DVR_GetLastError());
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup();
		return;
	}
	Sleep(5000); //云台控制接口下发命令之后是云台持续运动的，持续转动5秒之后再调用接口下发停止转动命令

	dwStop = 1; //停止转动
	if (!NET_DVR_PTZControl_Other(lUserID, iChannel, PAN_LEFT, dwStop))
	{
		printf("PAN_LEFT stop failed, error code: %d\n", NET_DVR_GetLastError());
	}

	//注销用户
	NET_DVR_Logout(lUserID);

	//释放SDK资源
	NET_DVR_Cleanup();

	return;
}

