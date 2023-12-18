//#include "stdafx.h" //VS2017中需要添加此预编译头文件
#include"HK_camera.h"
#include <iostream>
#include <Windows.h>

using namespace std;

int main()
{
	HK_camera camera;
	if (camera.Init())
	{
		cout << "init success" << endl;
		if (camera.Login("169.254.104.194", "admin", "gyb18800", 8000))//用户名以及密码，根据此系列博客一中的方法查看或设置
		{
			cout << "login successfully" << endl;
			camera.show();
		}
		else
		{
			cout << "login fail" << endl;
		}
	}
	else
	{
		cout << "init fail" << endl;
	}

	while (1)
	{


	}
	return 0;
}
