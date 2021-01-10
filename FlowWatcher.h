// FlowWatcher.h : FlowWatcher 应用程序的主头文件
//
#pragma once

#pragma comment(lib, "windowhook.lib")
BOOL __declspec(dllimport)__stdcall installHook(DWORD threadID);
BOOL __declspec(dllimport)__stdcall uninstallHook();

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "IAAnalyser.h"


// CFlowWatcherApp:
// 有关此类的实现，请参阅 FlowWatcher.cpp
//

class CFlowWatcherApp : public CWinApp
{
public:
	CFlowWatcherApp();
	~CFlowWatcherApp();
	BOOL CommitRecord();																							//向服务器传递信息
	BOOL Update();																										//更新
	BOOL StopWorkThread();																					//停止工作线程
	void NewWatchTime();																						//新的观察时间

	BOOL InstallHook();																						//安装钩子
#ifdef DB_LOG
	void LostDBConnect();																					//失去了数据库连接
	BOOL ReConnectDB();																					//重新连接数据库
#endif // DB_LOG


// 重写
public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);											//在预处理中处理PostThreadMessage发来的消息

// 实现

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	//DWORD WINAPI ThreadTranslateMessage(LPVOID param);
	BOOL SetAutoRun();																								//设置自动运行
	CString GetVersion();																								//得到程序版本
	DWORD m_dUpdateID;																							//更新进程ID
	UpdateState m_eUpdateState;																				//更新状态
	DWORD m_dAnalyseThreadID;																				//hook信息分析线程ID
	HANDLE m_hAnalyseThreadHandle;																		//hook信息分析线程句柄
	CIAAnalyser *m_pAnalyser;																					//hook信息分析类

public:
	BOOL GetScreenWinLocals(vector<sHWNDAtt> &result);													//得到当前桌面窗口程序的大小
	BOOL GetHWND(vector<HWND> &result);																		//得到HWND
public:
	//WinInfoCtl m_cWinInfoCtl;					//窗口信息管理器
	//CString 
	// //由于更新需要关闭的消息
	BOOL OnSubUpdateClose(void);
	afx_msg void OnAppclose();
	afx_msg void OnExportData();
	afx_msg void OnSetWinsLocal();
	afx_msg void OnGlbClique();
	afx_msg void OnShowWin();
	afx_msg void OnScrShot();
	afx_msg void OnPlainLayout();
};

extern CFlowWatcherApp theApp;