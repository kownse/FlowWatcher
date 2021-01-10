// keyhook.h : keyhook DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

#pragma comment(lib, "windowhook.lib")

#pragma data_seg("SharedVar")
//extern "C" __declspec(dllexport) HHOOK hHook=NULL;
//HHOOK __declspec(dllexport)__stdcall  hHook=NULL;
static HHOOK hCbtHook = NULL;				//CBT����
static HHOOK hShellHook = NULL;			//SHELL����
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
// �йش���ʵ�ֵ���Ϣ������� keyhook.cpp
//

class __declspec(dllimport)   CWindowhookApp : public CWinApp
{
public:
	CWindowhookApp();
	~CWindowhookApp();



// ��д
public:
	virtual BOOL InitInstance();

	BOOL CALLBACK installHook(HWND hWnd);		//��װ����
	BOOL uninstallHook();											//ж�ع���
	BOOL getCreateMsg(char **pText);						//�õ�������Ϣ
	BOOL getActiveMsg(char **pText);						//�õ����Ϣ

	DECLARE_MESSAGE_MAP()

private:
	static LRESULT CALLBACK CbtProc(int nCode, WPARAM wParam, LPARAM lParam);													//CBT���Ӵ�����
	static LRESULT CALLBACK CWindowhookApp::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);			//���̹��Ӵ�����
	static LRESULT CALLBACK ShellProc(int nCode, WPARAM wParam, LPARAM lParam);												//SHELL���Ӵ�����
};
