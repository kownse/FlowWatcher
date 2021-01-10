// keyhook.h : keyhook DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#pragma comment(lib, "windowhook.lib")

#pragma data_seg("SharedVar")
//extern "C" __declspec(dllexport) HHOOK hHook=NULL;
//HHOOK __declspec(dllexport)__stdcall  hHook=NULL;
static HHOOK hCbtHook = NULL;				//CBT钩子
static HHOOK hShellHook = NULL;			//SHELL钩子
static HINSTANCE hins = NULL;
static int g_iCount = 0;
static HWND g_hWnd = NULL;
static TCHAR msgCreate[512] = {0};
static TCHAR msgActive[512] = {0};
static HWND g_Target = NULL;
//HHOOK hHook=NULL;
#pragma data_seg()

//#pragma comment(linker, "/section:SharedVar,rws")

//extern "C" __declspec(dllexport)
//LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
//BOOL __declspec(dllexport)__stdcall CALLBACK installHook(HWND hWnd);
//BOOL __declspec(dllexport)__stdcall uninstallHook();
//BOOL __declspec(dllexport)__stdcall getCreateMsg(char **pText);
//BOOL __declspec(dllexport)__stdcall getActiveMsg(char **pText);



// CkeyhookApp
// 有关此类实现的信息，请参阅 keyhook.cpp
//

class __declspec(dllimport)   CWindowhookApp : public CWinApp
{
public:
	CWindowhookApp();
	~CWindowhookApp();



// 重写
public:
	virtual BOOL InitInstance();

	BOOL CALLBACK installHook(HWND hWnd);		//安装钩子
	BOOL uninstallHook();											//卸载钩子
	BOOL getCreateMsg(char **pText);						//得到创建消息
	BOOL getActiveMsg(char **pText);						//得到活动消息

	DECLARE_MESSAGE_MAP()

private:
	static LRESULT CALLBACK CbtProc(int nCode, WPARAM wParam, LPARAM lParam);													//CBT钩子处理函数
	static LRESULT CALLBACK CWindowhookApp::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);			//键盘钩子处理函数
	static LRESULT CALLBACK ShellProc(int nCode, WPARAM wParam, LPARAM lParam);												//SHELL钩子处理函数
};
