// FlowWatcher.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "FlowWatcher.h"
#include "MainFrm.h"
#include "ScreenHelper.h"
#include "IAAnalyser.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFlowWatcherApp

BEGIN_MESSAGE_MAP(CFlowWatcherApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CFlowWatcherApp::OnAppAbout)
	ON_COMMAND(ID_APPCLOSE, &CFlowWatcherApp::OnAppclose)
	ON_COMMAND(ID_EXPORT_DATA, &CFlowWatcherApp::OnExportData)
	ON_COMMAND(ID_PAILIE, &CFlowWatcherApp::OnSetWinsLocal)
	ON_COMMAND(ID_GLB_CLIQUE, &CFlowWatcherApp::OnGlbClique)
	ON_COMMAND(ID_SHOW_WIN, &CFlowWatcherApp::OnShowWin)
	ON_COMMAND(ID_SCR_SHOT, &CFlowWatcherApp::OnScrShot)
	ON_COMMAND(ID_PLAIN_LAYOUT, &CFlowWatcherApp::OnPlainLayout)
END_MESSAGE_MAP()


// CFlowWatcherApp 构造

CFlowWatcherApp::CFlowWatcherApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	//m_pAnalyser = new CIAAnalyser;
}

CFlowWatcherApp::~CFlowWatcherApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	delete m_pAnalyser;
}


// 唯一的一个 CFlowWatcherApp 对象

CFlowWatcherApp theApp;


// CFlowWatcherApp 初始化

BOOL CFlowWatcherApp::InitInstance()
{
	CWinApp::InitInstance();
	//设置自动运行
	SetAutoRun();

	////钩子安装失败要推出
	//if(!InstallHook())
	//{
	//	return FALSE;
	//}


	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	
	// 创建并加载框架及其资源
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	//m_pMainWnd = pFrame;
	//设置计时器
	::SetTimer(m_pMainWnd->m_hWnd,IDC_WATCH_TIMER, 60000, NULL);
	::SetTimer(m_pMainWnd->m_hWnd,IDC_LOG_TIMER, 3599990, NULL);
	::SetTimer(m_pMainWnd->m_hWnd,IDC_UPDATE, 3599990, NULL);
	::SetTimer(m_pMainWnd->m_hWnd,IDC_FIRST_UPDATE, 5000, NULL);
	m_eUpdateState = NO_ACTION;

	m_pAnalyser = new CIAAnalyser(m_pMainWnd->m_hWnd);
	//创建分析线程
	m_hAnalyseThreadHandle = ::CreateThread(NULL, 0, &CIAAnalyser::ThreadTranslateMessage, m_pAnalyser, 0, &m_dAnalyseThreadID);
	if(m_dAnalyseThreadID == NULL)
	{
		//AfxMessageBox("分析线程创建失败，程序洗白了");
		PostQuitMessage(0);
		return FALSE;
	}
	else
	{
		//安装钩子,消息发送到监听线程
		if(installHook(m_dAnalyseThreadID) == FALSE)
		{
			//安装钩子失败
			//AfxMessageBox("安装钩子失败");
			PostQuitMessage(0);
			return FALSE;
		}
	}

	//先更新一次
	//Update();
	//pFrame->SetWindowPos()
	//m_pMainWnd->SetWindowPos(m_pMainWnd, 100, 100, 200, 200,SWP_HIDEWINDOW);
	m_pMainWnd->SetWindowPos(m_pMainWnd, 100, 100, 200, 200,SWP_DRAWFRAME);
	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	pFrame->ShowWindow(SW_HIDE);
	pFrame->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand 之后发生
	return TRUE;
}


// CFlowWatcherApp 消息处理程序

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTrayAbout();
	//afx_msg void OnNewfeature();
	//afx_msg void OnSetWinsLocal();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_COMMAND(ID_32771, &CAboutDlg::OnTrayAbout)
	//ON_COMMAND(ID_NEWFEATURE, &CAboutDlg::OnNewfeature)
//	ON_COMMAND(ID_PAILIE, &CAboutDlg::OnSetWinsLocal)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CFlowWatcherApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CFlowWatcherApp 消息处理程序

//处理线程消息
BOOL CFlowWatcherApp::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case SUB_UPDATE_CLOSE:
		{
			//更新关闭消息
			//AfxMessageBox("更新关闭");
			OnSubUpdateClose();
			break;
		}
	case SUB_UPDATE_APP_CLOSE:
		{
			//更新程序超时关闭
			m_eUpdateState = NO_ACTION;
			break;
		}
	}

	return CWinApp::PreTranslateMessage(pMsg);
}


//安装钩子
BOOL CFlowWatcherApp::InstallHook()
{
	return installHook(m_nThreadID);
}


BOOL CFlowWatcherApp::SetAutoRun()
{
	HKEY   hKey;   
	LONG   lResult;   
	BYTE   szPathName[MAX_PATH];   
	CString   KeyTree = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	GetModuleFileName(NULL,(char   *)szPathName,MAX_PATH);   
	if(szPathName[0]=='\\')   
		return FALSE;   
	lResult=RegOpenKeyEx(HKEY_CURRENT_USER,KeyTree,0,KEY_ALL_ACCESS,&hKey);   
	//lResult=RegOpenKeyEx(HKEY_CURRENT_USER, )
	if(lResult!=ERROR_SUCCESS)   
		return FALSE;   
	//if(m_bAutoStart)   
	//{   
	long ret=::RegSetValueEx(hKey,"WinWatcher.EXE",0,REG_SZ,szPathName,sizeof(szPathName));   
	if(ret != ERROR_SUCCESS)
	{
		//AfxMessageBox("设置自动运行失败");
		RegCloseKey(hKey);   
		return FALSE;
	}
	else
	{
		//AfxMessageBox("设置自动运行成功");
	}
	//}   
	//else   
	//{   
	//	RegDeleteValue(hKey,"Mon.EXE");   
	//}   
	RegCloseKey(hKey);   

	return TRUE;
}
#ifdef DB_LOG
//失去连接了就重连
void CFlowWatcherApp::LostDBConnect()
{
	::SetTimer(m_pMainWnd->m_hWnd, IDC_DB_RECONNECT, 10000, NULL);
}

//重新连接数据库
BOOL CFlowWatcherApp::ReConnectDB()
{
//	return m_cWinInfoCtl.ReConnect();
	return TRUE;
}
#endif


// //由于更新需要关闭的消息
BOOL CFlowWatcherApp::OnSubUpdateClose(void)
{
	//AfxMessageBox("收到关闭消息");
	//运行标志位用的是static变量，所以在这里应该还是能够关闭工作线程的
	//CIAAnalyser temp;
	if(CIAAnalyser::StopWork() == FALSE)
	{
		//这个错还不好容。。。
	}

	//最重要的是卸载钩子
	BOOL flag = uninstallHook();
	if(flag)
	{
		//AfxMessageBox("钩子卸载成功，关闭程序");
		//钩子卸载成功，就关闭程序
//		Sleep(1000);
		//CString id;
		//id.Format("%d", m_dUpdateID);
		//		AfxMessageBox("更新进程ID = " + id);
		PostQuitMessage(0);
		//发送关闭确切消息
		int dSendCount = 0;
		while(!::PostThreadMessage(m_dUpdateID, SUB_CLOSE_EXACT, m_nThreadID, m_nThreadID))
		{
			//如果发送失败，就等待0.5秒再发送
			Sleep(500);
			if(dSendCount++ > 10)
			{
				//50秒还没发送成功就退出
				//AfxMessageBox("关闭消息发送失败");
				return flag;
			}
		}
		//AfxMessageBox("关闭消息发送成功");
		m_eUpdateState = CLOSE_EXACT;
		//PostQuitMessage(0);
	}
	else
	{
		//AfxMessageBox("钩子卸载失败，程序继续");
		//不成功不动作
	}

	//关闭TrayIcon
	m_pMainWnd->~CWnd();

	return flag;
}

//获取程序版本
CString CFlowWatcherApp::GetVersion()
{
	TCHAR szVersionBuffer[8192] = _T("");   
	DWORD dwVerSize;   
	DWORD dwHandle;
	CString sLocalVersion;

	dwVerSize = GetFileVersionInfoSize("FlowWatcher.exe", &dwHandle);   
	if (dwVerSize == 0)   
	{
		//AfxMessageBox(remoteFileName + "不存在，需要更新");
		return NULL;   
	}

	if (GetFileVersionInfo("FlowWatcher.exe", 0, dwVerSize, szVersionBuffer))   
	{   
		VS_FIXEDFILEINFO * pInfo;   
		unsigned int nInfoLen;   

		if (VerQueryValue(szVersionBuffer, _T("\\"), (void**)&pInfo, &nInfoLen))
		{
			sLocalVersion.Format(_T("%d.%d.%d.%d"),   
				HIWORD(pInfo->dwFileVersionMS), LOWORD(pInfo->dwFileVersionMS),   
				HIWORD(pInfo->dwFileVersionLS), LOWORD(pInfo->dwFileVersionLS));  

			//AfxMessageBox(sLocalVersion);
			return sLocalVersion;
		}   
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
	return NULL;
}

//更新
BOOL CFlowWatcherApp::Update()
{
	//AfxMessageBox("更新");
	static int dFaileCount = 0;
	//如果已经处于更新状态，就不再响应这个消息?
	if(m_eUpdateState == START_UPDATE)
	{
		if(dFaileCount++ > 3)
		{
			m_eUpdateState = NO_ACTION;
			dFaileCount = 0;
		}
		else
		{
			return FALSE;
		}
	}

	PROCESS_INFORMATION info;
	memset(&info, 0, sizeof(PROCESS_INFORMATION));
	STARTUPINFO startInfo;
	memset(&startInfo, 0, sizeof(STARTUPINFO));
	LPPROCESS_INFORMATION pStructInfo = &info;
	//memset(&structInfo, 0, sizeof(PROCESS_INFORMATION));
	//TCHAR szAppName[100] = gWorkAppName;
	//如果创建进程失败，那么就等待一下再创建
	//获取当前工作目录，以绝对路径开启updater
	TCHAR fullName[MAX_PATH];
	//取得当前应用程序绝对路径
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL,szFileName,MAX_PATH);
	char szDrive[MAX_PATH]={0};
	char szDir[MAX_PATH]={0};
	char szPath[MAX_PATH]={0};
	_tsplitpath(szFileName,szDrive,szDir,NULL,NULL);
	sprintf(fullName,"%s%s%s",szDrive,szDir,"FlowUpdate.exe");
	//strcat(fullName, "\\FlowUpdate.exe"); 
	//AfxMessageBox(fullName);
	int count = 0;
	while(0 ==CreateProcess(fullName, NULL, NULL, NULL,FALSE, 0, NULL, NULL, &startInfo, pStructInfo))
	{
		//AfxMessageBox("运行程序失败");
		if(count > 5)
			break;
		count++;

		////输出错误信息
		//			DWORD nErrorNo = GetLastError ( ); // 得到错误代码
		//			LPSTR lpBuffer;    
		//			FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER  | 
		//				FORMAT_MESSAGE_IGNORE_INSERTS  | 
		//				FORMAT_MESSAGE_FROM_SYSTEM,
		//				NULL,
		//				nErrorNo, // 此乃错误代码，通常在程序中可由 GetLastError()得之
		//				LANG_NEUTRAL,
		//				(LPTSTR) & lpBuffer,
		//				0 ,
		//				NULL );
		//			//CString strErrorCause=lpBuffer?_T(lpBuffer):_T( " Sorry, cannot find this error info. " );
		//			CString strErrorCause = lpBuffer? CString(lpBuffer) : CString("can not find error info");
		////			UpdateData (FALSE);
		//			//  Free the buffer. 
		//			LocalFree (lpBuffer);
		//			AfxMessageBox(strErrorCause);

		Sleep(1000);
	}

	if(count < 10)
	{
		//AfxMessageBox("创建更新进程成功");
		Sleep(1000);
		m_dUpdateID = pStructInfo->dwThreadId;
		//CString id;
		//id.Format("%d", m_dUpdateID);
//		AfxMessageBox("更新进程ID = " + id);
		//发送更新消息
		int dSendCount = 0;
		while(!::PostThreadMessage(m_dUpdateID, SUB_START_UPDATE, m_nThreadID, m_nThreadID))
		{
			//如果发送失败，就等待0.5秒再发送
			Sleep(500);
			if(dSendCount++ > 10)
			{
				//50秒还没发送成功就退出
				break;
			}
		}
		m_eUpdateState = START_UPDATE;
		//AfxMessageBox("发送更新消息成功");
		return TRUE;
	}
	//AfxMessageBox("创建更新进程失败");
	return FALSE;	
}

//得到窗口大小
BOOL CFlowWatcherApp::GetScreenWinLocals(vector<sHWNDAtt> &result)
{
	return m_pAnalyser->GetWindowLocals(result);
}

//得到HWND
BOOL CFlowWatcherApp::GetHWND(vector<HWND> &result)
{
	return m_pAnalyser->GetHWND(result);
}

void CAboutDlg::OnTrayAbout()
{
	// 用于运行对话框的应用程序命令
	//CAboutDlg aboutDlg;
	//aboutDlg.DoModal();
}

void CFlowWatcherApp::OnAppclose()
{
	// TODO: 在此添加命令处理程序代码
	//AfxMessageBox("不要关闭我啊");
	//PostQuitMessage(0);
	((CMainFrame *)m_pMainWnd)->DisableTrayIcon();
	m_pAnalyser->StopWork();
	::TerminateThread(m_hAnalyseThreadHandle, 0);

	PostMessage(m_pMainWnd->m_hWnd, WM_QUIT, 0, 0);
	return;
}

//停止工作线程
BOOL CFlowWatcherApp::StopWorkThread()
{
	//return ((CIAAnalyser *)*m_pAnalyser)->StopWork();
	return m_pAnalyser->StopWork();
}

//向服务器传递信息
BOOL CFlowWatcherApp::CommitRecord()
{
	//return m_pAnalyser->CommitRecord();
	::PostThreadMessage(m_dAnalyseThreadID, SUB_COMMIT_RECORD, 0, 0);
	return TRUE;
}

//void CAboutDlg::OnNewfeature()
//{
//	// TODO: 在此添加命令处理程序代码
//	AfxMessageBox("嵌入式MySQL\r\n多线程记录\r\nBUG超级多");
//}

void CFlowWatcherApp::OnExportData()
{
	// TODO: 在此添加命令处理程序代码
	//::PostThreadMessage(m_dAnalyseThreadID, SUB_EXPORT_DATA, 0, 0);
	m_pAnalyser->ExportData();
//	return TRUE;
}

//测试设置所有窗口的位置
void CFlowWatcherApp::OnSetWinsLocal()
{
	// TODO: 在此添加命令处理程序代码
	//设置窗口排列
	//AfxMessageBox("PostMessage");
	//::PostThreadMessage(m_dAnalyseThreadID, SUB_SET_WINS_LOCAL, 0, 0);
	m_pAnalyser->WinsLocal();

	//AfxMessageBox("直接调用");
	//((CIAAnalyser *)m_pAnalyser)->OnSubWinsLocal(0,0);
}

//计算全局聚类
void CFlowWatcherApp::OnGlbClique()
{
	// TODO: 在此添加命令处理程序代码
	//::PostThreadMessage(m_dAnalyseThreadID, SUB_GLOBLE_CLIQUE, 0, 0);
	m_pAnalyser->GlobleClique();
}

//显示下一个聚类窗口集合
void CFlowWatcherApp::OnShowWin()
{
	// TODO: 在此添加命令处理程序代码
	//::PostThreadMessage(m_dAnalyseThreadID, SUB_SHOW_NEXT_CLIQUE, 0, 0);
	m_pAnalyser->ShowNextClique();
}

//对当前焦点窗口截图
void CFlowWatcherApp::OnScrShot()
{
	// TODO: 在此添加命令处理程序代码
	//::PostThreadMessage(m_dAnalyseThreadID, SUB_SCREEN_SHOT, 0, 0);
	m_pAnalyser->ScreenShot();
}

void CFlowWatcherApp::NewWatchTime()
{
	m_pAnalyser->NewWatchTime();
}

//平铺桌面现有窗口
void CFlowWatcherApp::OnPlainLayout()
{
	// TODO: 在此添加命令处理程序代码
	m_pAnalyser->OnPlainLayout();
}
