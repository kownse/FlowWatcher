// FlowWatcher.cpp : ����Ӧ�ó��������Ϊ��
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


// CFlowWatcherApp ����

CFlowWatcherApp::CFlowWatcherApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	//m_pAnalyser = new CIAAnalyser;
}

CFlowWatcherApp::~CFlowWatcherApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	delete m_pAnalyser;
}


// Ψһ��һ�� CFlowWatcherApp ����

CFlowWatcherApp theApp;


// CFlowWatcherApp ��ʼ��

BOOL CFlowWatcherApp::InitInstance()
{
	CWinApp::InitInstance();
	//�����Զ�����
	SetAutoRun();

	////���Ӱ�װʧ��Ҫ�Ƴ�
	//if(!InstallHook())
	//{
	//	return FALSE;
	//}


	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	// ��Ҫ���������ڣ��˴��뽫�����µĿ�ܴ���
	// ����Ȼ��������ΪӦ�ó���������ڶ���
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	
	// ���������ؿ�ܼ�����Դ
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	//m_pMainWnd = pFrame;
	//���ü�ʱ��
	::SetTimer(m_pMainWnd->m_hWnd,IDC_WATCH_TIMER, 60000, NULL);
	::SetTimer(m_pMainWnd->m_hWnd,IDC_LOG_TIMER, 3599990, NULL);
	::SetTimer(m_pMainWnd->m_hWnd,IDC_UPDATE, 3599990, NULL);
	::SetTimer(m_pMainWnd->m_hWnd,IDC_FIRST_UPDATE, 5000, NULL);
	m_eUpdateState = NO_ACTION;

	m_pAnalyser = new CIAAnalyser(m_pMainWnd->m_hWnd);
	//���������߳�
	m_hAnalyseThreadHandle = ::CreateThread(NULL, 0, &CIAAnalyser::ThreadTranslateMessage, m_pAnalyser, 0, &m_dAnalyseThreadID);
	if(m_dAnalyseThreadID == NULL)
	{
		//AfxMessageBox("�����̴߳���ʧ�ܣ�����ϴ����");
		PostQuitMessage(0);
		return FALSE;
	}
	else
	{
		//��װ����,��Ϣ���͵������߳�
		if(installHook(m_dAnalyseThreadID) == FALSE)
		{
			//��װ����ʧ��
			//AfxMessageBox("��װ����ʧ��");
			PostQuitMessage(0);
			return FALSE;
		}
	}

	//�ȸ���һ��
	//Update();
	//pFrame->SetWindowPos()
	//m_pMainWnd->SetWindowPos(m_pMainWnd, 100, 100, 200, 200,SWP_HIDEWINDOW);
	m_pMainWnd->SetWindowPos(m_pMainWnd, 100, 100, 200, 200,SWP_DRAWFRAME);
	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	pFrame->ShowWindow(SW_HIDE);
	pFrame->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	return TRUE;
}


// CFlowWatcherApp ��Ϣ�������

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void CFlowWatcherApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CFlowWatcherApp ��Ϣ�������

//�����߳���Ϣ
BOOL CFlowWatcherApp::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case SUB_UPDATE_CLOSE:
		{
			//���¹ر���Ϣ
			//AfxMessageBox("���¹ر�");
			OnSubUpdateClose();
			break;
		}
	case SUB_UPDATE_APP_CLOSE:
		{
			//���³���ʱ�ر�
			m_eUpdateState = NO_ACTION;
			break;
		}
	}

	return CWinApp::PreTranslateMessage(pMsg);
}


//��װ����
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
		//AfxMessageBox("�����Զ�����ʧ��");
		RegCloseKey(hKey);   
		return FALSE;
	}
	else
	{
		//AfxMessageBox("�����Զ����гɹ�");
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
//ʧȥ�����˾�����
void CFlowWatcherApp::LostDBConnect()
{
	::SetTimer(m_pMainWnd->m_hWnd, IDC_DB_RECONNECT, 10000, NULL);
}

//�����������ݿ�
BOOL CFlowWatcherApp::ReConnectDB()
{
//	return m_cWinInfoCtl.ReConnect();
	return TRUE;
}
#endif


// //���ڸ�����Ҫ�رյ���Ϣ
BOOL CFlowWatcherApp::OnSubUpdateClose(void)
{
	//AfxMessageBox("�յ��ر���Ϣ");
	//���б�־λ�õ���static����������������Ӧ�û����ܹ��رչ����̵߳�
	//CIAAnalyser temp;
	if(CIAAnalyser::StopWork() == FALSE)
	{
		//����������ݡ�����
	}

	//����Ҫ����ж�ع���
	BOOL flag = uninstallHook();
	if(flag)
	{
		//AfxMessageBox("����ж�سɹ����رճ���");
		//����ж�سɹ����͹رճ���
//		Sleep(1000);
		//CString id;
		//id.Format("%d", m_dUpdateID);
		//		AfxMessageBox("���½���ID = " + id);
		PostQuitMessage(0);
		//���͹ر�ȷ����Ϣ
		int dSendCount = 0;
		while(!::PostThreadMessage(m_dUpdateID, SUB_CLOSE_EXACT, m_nThreadID, m_nThreadID))
		{
			//�������ʧ�ܣ��͵ȴ�0.5���ٷ���
			Sleep(500);
			if(dSendCount++ > 10)
			{
				//50�뻹û���ͳɹ����˳�
				//AfxMessageBox("�ر���Ϣ����ʧ��");
				return flag;
			}
		}
		//AfxMessageBox("�ر���Ϣ���ͳɹ�");
		m_eUpdateState = CLOSE_EXACT;
		//PostQuitMessage(0);
	}
	else
	{
		//AfxMessageBox("����ж��ʧ�ܣ��������");
		//���ɹ�������
	}

	//�ر�TrayIcon
	m_pMainWnd->~CWnd();

	return flag;
}

//��ȡ����汾
CString CFlowWatcherApp::GetVersion()
{
	TCHAR szVersionBuffer[8192] = _T("");   
	DWORD dwVerSize;   
	DWORD dwHandle;
	CString sLocalVersion;

	dwVerSize = GetFileVersionInfoSize("FlowWatcher.exe", &dwHandle);   
	if (dwVerSize == 0)   
	{
		//AfxMessageBox(remoteFileName + "�����ڣ���Ҫ����");
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

//����
BOOL CFlowWatcherApp::Update()
{
	//AfxMessageBox("����");
	static int dFaileCount = 0;
	//����Ѿ����ڸ���״̬���Ͳ�����Ӧ�����Ϣ?
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
	//�����������ʧ�ܣ���ô�͵ȴ�һ���ٴ���
	//��ȡ��ǰ����Ŀ¼���Ծ���·������updater
	TCHAR fullName[MAX_PATH];
	//ȡ�õ�ǰӦ�ó������·��
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
		//AfxMessageBox("���г���ʧ��");
		if(count > 5)
			break;
		count++;

		////���������Ϣ
		//			DWORD nErrorNo = GetLastError ( ); // �õ��������
		//			LPSTR lpBuffer;    
		//			FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER  | 
		//				FORMAT_MESSAGE_IGNORE_INSERTS  | 
		//				FORMAT_MESSAGE_FROM_SYSTEM,
		//				NULL,
		//				nErrorNo, // ���˴�����룬ͨ���ڳ����п��� GetLastError()��֮
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
		//AfxMessageBox("�������½��̳ɹ�");
		Sleep(1000);
		m_dUpdateID = pStructInfo->dwThreadId;
		//CString id;
		//id.Format("%d", m_dUpdateID);
//		AfxMessageBox("���½���ID = " + id);
		//���͸�����Ϣ
		int dSendCount = 0;
		while(!::PostThreadMessage(m_dUpdateID, SUB_START_UPDATE, m_nThreadID, m_nThreadID))
		{
			//�������ʧ�ܣ��͵ȴ�0.5���ٷ���
			Sleep(500);
			if(dSendCount++ > 10)
			{
				//50�뻹û���ͳɹ����˳�
				break;
			}
		}
		m_eUpdateState = START_UPDATE;
		//AfxMessageBox("���͸�����Ϣ�ɹ�");
		return TRUE;
	}
	//AfxMessageBox("�������½���ʧ��");
	return FALSE;	
}

//�õ����ڴ�С
BOOL CFlowWatcherApp::GetScreenWinLocals(vector<sHWNDAtt> &result)
{
	return m_pAnalyser->GetWindowLocals(result);
}

//�õ�HWND
BOOL CFlowWatcherApp::GetHWND(vector<HWND> &result)
{
	return m_pAnalyser->GetHWND(result);
}

void CAboutDlg::OnTrayAbout()
{
	// �������жԻ����Ӧ�ó�������
	//CAboutDlg aboutDlg;
	//aboutDlg.DoModal();
}

void CFlowWatcherApp::OnAppclose()
{
	// TODO: �ڴ���������������
	//AfxMessageBox("��Ҫ�ر��Ұ�");
	//PostQuitMessage(0);
	((CMainFrame *)m_pMainWnd)->DisableTrayIcon();
	m_pAnalyser->StopWork();
	::TerminateThread(m_hAnalyseThreadHandle, 0);

	PostMessage(m_pMainWnd->m_hWnd, WM_QUIT, 0, 0);
	return;
}

//ֹͣ�����߳�
BOOL CFlowWatcherApp::StopWorkThread()
{
	//return ((CIAAnalyser *)*m_pAnalyser)->StopWork();
	return m_pAnalyser->StopWork();
}

//�������������Ϣ
BOOL CFlowWatcherApp::CommitRecord()
{
	//return m_pAnalyser->CommitRecord();
	::PostThreadMessage(m_dAnalyseThreadID, SUB_COMMIT_RECORD, 0, 0);
	return TRUE;
}

//void CAboutDlg::OnNewfeature()
//{
//	// TODO: �ڴ���������������
//	AfxMessageBox("Ƕ��ʽMySQL\r\n���̼߳�¼\r\nBUG������");
//}

void CFlowWatcherApp::OnExportData()
{
	// TODO: �ڴ���������������
	//::PostThreadMessage(m_dAnalyseThreadID, SUB_EXPORT_DATA, 0, 0);
	m_pAnalyser->ExportData();
//	return TRUE;
}

//�����������д��ڵ�λ��
void CFlowWatcherApp::OnSetWinsLocal()
{
	// TODO: �ڴ���������������
	//���ô�������
	//AfxMessageBox("PostMessage");
	//::PostThreadMessage(m_dAnalyseThreadID, SUB_SET_WINS_LOCAL, 0, 0);
	m_pAnalyser->WinsLocal();

	//AfxMessageBox("ֱ�ӵ���");
	//((CIAAnalyser *)m_pAnalyser)->OnSubWinsLocal(0,0);
}

//����ȫ�־���
void CFlowWatcherApp::OnGlbClique()
{
	// TODO: �ڴ���������������
	//::PostThreadMessage(m_dAnalyseThreadID, SUB_GLOBLE_CLIQUE, 0, 0);
	m_pAnalyser->GlobleClique();
}

//��ʾ��һ�����ര�ڼ���
void CFlowWatcherApp::OnShowWin()
{
	// TODO: �ڴ���������������
	//::PostThreadMessage(m_dAnalyseThreadID, SUB_SHOW_NEXT_CLIQUE, 0, 0);
	m_pAnalyser->ShowNextClique();
}

//�Ե�ǰ���㴰�ڽ�ͼ
void CFlowWatcherApp::OnScrShot()
{
	// TODO: �ڴ���������������
	//::PostThreadMessage(m_dAnalyseThreadID, SUB_SCREEN_SHOT, 0, 0);
	m_pAnalyser->ScreenShot();
}

void CFlowWatcherApp::NewWatchTime()
{
	m_pAnalyser->NewWatchTime();
}

//ƽ���������д���
void CFlowWatcherApp::OnPlainLayout()
{
	// TODO: �ڴ���������������
	m_pAnalyser->OnPlainLayout();
}
