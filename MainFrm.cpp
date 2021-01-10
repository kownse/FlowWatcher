// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "FlowWatcher.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define		WM_MY_TRAY_NOTIFICATION		(WM_USER+0)		//������Ϣ

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

const double CMainFrame::m_dSizeDiv = 0.8;					//���򴰿��������С��ֵ
// CMainFrame ����/����

CMainFrame::CMainFrame():m_TrayIcon(IDR_TRAYMENU)
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
}

CMainFrame::~CMainFrame()
{
	m_TrayIcon.SetIcon(NULL);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// ����һ����ͼ��ռ�ÿ�ܵĹ�����
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("δ�ܴ�����ͼ����\n");
		return -1;
	}
	
	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("δ�ܴ���������\n");
	//	return -1;      // δ�ܴ���
	//}

	// TODO: �������Ҫ��ͣ������������ɾ��������
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	// ��������ͼ��
	m_TrayIcon.SetNotificationWnd(this, WM_MY_TRAY_NOTIFICATION);
	m_TrayIcon.SetIcon(IDI_TRAYICON);
	//m_TrayIcon.SetIcon(NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	//cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
	//	;

	//cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	//��ȡ��Ļ�����������
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
	//���ô��ڴ�С���ɸı�
	return TRUE;
}


// CMainFrame ���

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


// CMainFrame ��Ϣ�������

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// ������ǰ�Ƶ���ͼ����
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// ����ͼ��һ�γ��Ը�����
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// ����ִ��Ĭ�ϴ���
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//������Ϣ����
LRESULT CMainFrame::OnTrayNotification(WPARAM uID, LPARAM lEvent)
{
	return m_TrayIcon.OnTrayNotification(uID, lEvent);
}


void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch(nIDEvent)
	{
		//��ؼ�ʱ��
	case IDC_WATCH_TIMER:
		{
//			theApp.m_cWinInfoCtl.NewWatchTime();
			theApp.NewWatchTime();
			break;
		}
		//��ʱ��־��ʱ��
	case IDC_LOG_TIMER:
		{
			//theApp.m_cWinInfoCtl.WriteLog();
			//theApp.m_cWinInfoCtl.LogActiveGraphInFile();
			//theApp.m_cWinInfoCtl.LogActiveGraphInDB();
			break;
		}
		//��С����ʱ��
	case IDC_MIN_TIMER:
		{
			KillTimer(IDC_MIN_TIMER);
			this->ShowWindow(SW_HIDE);
			break;
		}
	case IDC_HOOK_INSTALL:
		{
			MessageBox("��װ����", "��װ����", MB_OK);
			KillTimer(IDC_HOOK_INSTALL);
			if(theApp.InstallHook())
			{
				MessageBox("���Ӱ�װ�ɹ�");
			}
			else
			{
				//AfxMessageBox("���Ӱ�װʧ��");
			}
			break;
		}
	case IDC_UPDATE:
		{
			//ȷ�����ݴ����ڸ���֮ǰ
//			theApp.m_cWinInfoCtl.WriteLog();
			theApp.CommitRecord();
			Sleep(5000);
			//����
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
				//���ӳɹ���ɾ��������ʱ��
				KillTimer(IDC_DB_RECONNECT);
			}
			else
			{
				//û�����ӳɹ��ͼ�������
			}
			break;
		}
#endif // DB_LOG
	}

	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnNewfeature()
{
	// TODO: �ڴ���������������
	AfxMessageBox("ʹ��figtree���˹���ܶȾ���\r\n���ര�ڵ���");
}

// ����TrayIcon
void CMainFrame::DisableTrayIcon(void)
{
	m_TrayIcon.SetIcon(NULL);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
}

void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
	CFrameWnd::OnSizing(fwSide, pRect);

	// TODO: �ڴ˴������Ϣ����������
}

void CMainFrame::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	//CFrameWnd::OnClose();
	::ShowWindow(m_hWnd, SW_HIDE);
}
