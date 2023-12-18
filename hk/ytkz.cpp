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
	case EXCEPTION_RECONNECT:    //Ԥ��ʱ����
		printf("----------reconnect--------%d\n", time(NULL));
		break;
	default:
		break;
	}
}

void main2223() {

	//---------------------------------------
	// ��ʼ��
	NET_DVR_Init();
	//��������ʱ��������ʱ��
	NET_DVR_SetConnectTime(2000, 1);
	NET_DVR_SetReconnect(10000, true);

	//---------------------------------------
	//�����쳣��Ϣ�ص�����
	NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack, NULL);

	//---------------------------------------
	// ע���豸
	LONG lUserID;

	//��¼�����������豸��ַ����¼�û��������
	NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
	struLoginInfo.bUseAsynLogin = 0; //ͬ����¼��ʽ
	strcpy(struLoginInfo.sDeviceAddress, "169.254.25.95"); //�豸IP��ַ
	struLoginInfo.wPort = 8000; //�豸����˿�
	strcpy(struLoginInfo.sUserName, "admin"); //�豸��¼�û���
	strcpy(struLoginInfo.sPassword, "gyb18800"); //�豸��¼����

	//�豸��Ϣ, �������
	NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };

	lUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
	if (lUserID < 0)
	{
		printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return;
	}

	int iChannel = 1;//�豸ͨ����
	DWORD dwPTZCommand = PAN_LEFT; //��̨��ת
	DWORD dwStop = 0; //��ʼת��
	if (!NET_DVR_PTZControl_Other(lUserID, iChannel, PAN_LEFT, dwStop))
	{
		printf("PAN_LEFT start failed, error code: %d\n", NET_DVR_GetLastError());
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup();
		return;
	}
	Sleep(5000); //��̨���ƽӿ��·�����֮������̨�����˶��ģ�����ת��5��֮���ٵ��ýӿ��·�ֹͣת������

	dwStop = 1; //ֹͣת��
	if (!NET_DVR_PTZControl_Other(lUserID, iChannel, PAN_LEFT, dwStop))
	{
		printf("PAN_LEFT stop failed, error code: %d\n", NET_DVR_GetLastError());
	}

	//ע���û�
	NET_DVR_Logout(lUserID);

	//�ͷ�SDK��Դ
	NET_DVR_Cleanup();

	return;
}

