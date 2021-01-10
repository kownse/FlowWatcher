// MainFrm.h : CMainFrame 类的接口
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

// 属性
public:

// 操作
public:

	//BOOL __declspec(dllimport)__stdcall installHook(HWND hWnd);


// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CToolBar    m_wndToolBar;
	CChildView    m_wndView;
	CTrayIcon		m_TrayIcon;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg LRESULT OnTrayNotification(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
private:
	CRect m_screenWorkRect;					//桌面工作空间
	CRect m_workRect;							//工作空间

public:
	static const double m_dSizeDiv;					//程序窗口与桌面大小比值
	// 实现
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNewfeature();
	// 隐藏TrayIcon
	void DisableTrayIcon(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnClose();
};


