// FlowWatcher.h : FlowWatcher Ӧ�ó������ͷ�ļ�
//
#pragma once

#pragma comment(lib, "windowhook.lib")
BOOL __declspec(dllimport)__stdcall installHook(DWORD threadID);
BOOL __declspec(dllimport)__stdcall uninstallHook();

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "IAAnalyser.h"


// CFlowWatcherApp:
// �йش����ʵ�֣������ FlowWatcher.cpp
//

class CFlowWatcherApp : public CWinApp
{
public:
	CFlowWatcherApp();
	~CFlowWatcherApp();
	BOOL CommitRecord();																							//�������������Ϣ
	BOOL Update();																										//����
	BOOL StopWorkThread();																					//ֹͣ�����߳�
	void NewWatchTime();																						//�µĹ۲�ʱ��

	BOOL InstallHook();																						//��װ����
#ifdef DB_LOG
	void LostDBConnect();																					//ʧȥ�����ݿ�����
	BOOL ReConnectDB();																					//�����������ݿ�
#endif // DB_LOG


// ��д
public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);											//��Ԥ�����д���PostThreadMessage��������Ϣ

// ʵ��

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	//DWORD WINAPI ThreadTranslateMessage(LPVOID param);
	BOOL SetAutoRun();																								//�����Զ�����
	CString GetVersion();																								//�õ�����汾
	DWORD m_dUpdateID;																							//���½���ID
	UpdateState m_eUpdateState;																				//����״̬
	DWORD m_dAnalyseThreadID;																				//hook��Ϣ�����߳�ID
	HANDLE m_hAnalyseThreadHandle;																		//hook��Ϣ�����߳̾��
	CIAAnalyser *m_pAnalyser;																					//hook��Ϣ������

public:
	BOOL GetScreenWinLocals(vector<sHWNDAtt> &result);													//�õ���ǰ���洰�ڳ���Ĵ�С
	BOOL GetHWND(vector<HWND> &result);																		//�õ�HWND
public:
	//WinInfoCtl m_cWinInfoCtl;					//������Ϣ������
	//CString 
	// //���ڸ�����Ҫ�رյ���Ϣ
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