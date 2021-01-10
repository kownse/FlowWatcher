// MainFrm.h : CMainFrame ��Ľӿ�
//
#define IDC_WATCH_TIMER					1
#define IDC_LOG_TIMER						2
#define IDC_MIN_TIMER						3
#define IDC_DB_RECONNECT				4
#define IDC_HOOK_INSTALL					5
#define IDC_UPDATE							6
#define IDC_FIRST_UPDATE					7

#pragma once

#include "ChildView.h"
#include "trayicon.h"


class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// ����
public:

// ����
public:

	//BOOL __declspec(dllimport)__stdcall installHook(HWND hWnd);


// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CToolBar    m_wndToolBar;
	CChildView    m_wndView;
	CTrayIcon		m_TrayIcon;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg LRESULT OnTrayNotification(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
private:
	CRect m_screenWorkRect;					//���湤���ռ�
	CRect m_workRect;							//�����ռ�

public:
	static const double m_dSizeDiv;					//���򴰿��������С��ֵ
	// ʵ��
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNewfeature();
	// ����TrayIcon
	void DisableTrayIcon(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnClose();
};


