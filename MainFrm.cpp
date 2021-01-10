// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "FlowWatcher.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define		WM_MY_TRAY_NOTIFICATION		(WM_USER+0)		//托盘消息

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_MY_TRAY_NOTIFICATION, OnTrayNotification)
	ON_WM_SETFOCUS()
	ON_WM_TIMER()
	ON_COMMAND(ID_NEWFEATURE, &CMainFrame::OnNewfeature)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

const double CMainFrame::m_dSizeDiv = 0.8;					//程序窗口与桌面大小比值
// CMainFrame 构造/析构

CMainFrame::CMainFrame():m_TrayIcon(IDR_TRAYMENU)
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
	m_TrayIcon.SetIcon(NULL);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// 创建一个视图以占用框架的工作区
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("未能创建视图窗口\n");
		return -1;
	}
	
	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("未能创建工具栏\n");
	//	return -1;      // 未能创建
	//}

	// TODO: 如果不需要可停靠工具栏，则删除这三行
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	// 设置托盘图标
	m_TrayIcon.SetNotificationWnd(this, WM_MY_TRAY_NOTIFICATION);
	m_TrayIcon.SetIcon(IDI_TRAYICON);
	//m_TrayIcon.SetIcon(NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	//cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
	//	;

	//cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	//获取屏幕工作区域矩形
	SystemParametersInfo(SPI_GETWORKAREA,   0,   (PVOID)   &m_screenWorkRect,   0);

	int width = m_screenWorkRect.right - m_screenWorkRect.left;
	int height = m_screenWorkRect.bottom - m_screenWorkRect.top;
	cs.x = width * (1 - gWinDiv) / 2;
	cs.y = height *(1 - gWinDiv) / 2;
	cs.cx = width * gWinDiv;
	cs.cy = height * gWinDiv;

	m_workRect.left = 0;
	m_workRect.right = cs.cx;
	m_workRect.top = 0;
	m_workRect.bottom = cs.cy;

	 cs.style&=~(WS_THICKFRAME|WS_MAXIMIZEBOX|WS_MINIMIZEBOX); 
	//设置窗口大小不可改变
	return TRUE;
}


// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// 将焦点前移到视图窗口
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 让视图第一次尝试该命令
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 否则，执行默认处理
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//托盘消息处理
LRESULT CMainFrame::OnTrayNotification(WPARAM uID, LPARAM lEvent)
{
	return m_TrayIcon.OnTrayNotification(uID, lEvent);
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
		//监控计时器
	case IDC_WATCH_TIMER:
		{
//			theApp.m_cWinInfoCtl.NewWatchTime();
			theApp.NewWatchTime();
			break;
		}
		//定时日志计时器
	case IDC_LOG_TIMER:
		{
			//theApp.m_cWinInfoCtl.WriteLog();
			//theApp.m_cWinInfoCtl.LogActiveGraphInFile();
			//theApp.m_cWinInfoCtl.LogActiveGraphInDB();
			break;
		}
		//最小化计时器
	case IDC_MIN_TIMER:
		{
			KillTimer(IDC_MIN_TIMER);
			this->ShowWindow(SW_HIDE);
			break;
		}
	case IDC_HOOK_INSTALL:
		{
			MessageBox("安装钩子", "安装钩子", MB_OK);
			KillTimer(IDC_HOOK_INSTALL);
			if(theApp.InstallHook())
			{
				MessageBox("钩子安装成功");
			}
			else
			{
				//AfxMessageBox("钩子安装失败");
			}
			break;
		}
	case IDC_UPDATE:
		{
			//确保数据传输在更新之前
//			theApp.m_cWinInfoCtl.WriteLog();
			theApp.CommitRecord();
			Sleep(5000);
			//更新
			theApp.Update();
//			theApp.StopWorkThread();
			break;
		}
	case IDC_FIRST_UPDATE:
		{
			KillTimer(IDC_FIRST_UPDATE);
			theApp.CommitRecord();
			Sleep(5000);
			theApp.Update();
		}
#ifdef DB_LOG
	case IDC_DB_RECONNECT:
		{
			//AfxMessageBox("DB lost");
			break;
			if(theApp.ReConnectDB())
			{
				//连接成功就删除重连计时器
				KillTimer(IDC_DB_RECONNECT);
			}
			else
			{
				//没有连接成功就继续连接
			}
			break;
		}
#endif // DB_LOG
	}

	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnNewfeature()
{
	// TODO: 在此添加命令处理程序代码
	AfxMessageBox("使用figtree库高斯核密度聚类\r\n聚类窗口调整");
}

// 隐藏TrayIcon
void CMainFrame::DisableTrayIcon(void)
{
	m_TrayIcon.SetIcon(NULL);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}

void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
	CFrameWnd::OnSizing(fwSide, pRect);

	// TODO: 在此处添加消息处理程序代码
}

void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//CFrameWnd::OnClose();
	::ShowWindow(m_hWnd, SW_HIDE);
}
