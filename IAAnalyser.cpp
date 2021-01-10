#include "StdAfx.h"
#include "IAAnalyser.h"

//WinInfoCtl CIAAnalyser::m_cWinInfoCtl;

BOOL CIAAnalyser::m_bWorkFlag = TRUE;
BOOL CIAAnalyser::m_bStopFlag = FALSE;

CIAAnalyser::CIAAnalyser(void)
{
	m_cWinInfoCtl = new WinInfoCtl();
	m_cGesRecognizer = new CGestureRecognizer();
}

CIAAnalyser::CIAAnalyser(HWND hSelf)
{
	m_cWinInfoCtl = new WinInfoCtl();
	m_cGesRecognizer = new CGestureRecognizer();
	m_hSelf = hSelf;
}

CIAAnalyser::~CIAAnalyser(void)
{
	delete m_cWinInfoCtl;
}

//��Ϣ���ຯ��
DWORD CIAAnalyser::ThreadTranslateMessage(LPVOID param)
{
	//CIAAnalyser **ptrAny = static_cast<CIAAnalyser *>(param);
	CIAAnalyser *pAnalyer = static_cast<CIAAnalyser *>(param);
		//new CIAAnalyser();
	//*ptrAny = pAnalyer;
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while(CIAAnalyser::m_bWorkFlag && GetMessage(&msg, NULL, 0U, 0U))
	{
		//if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		//{
			switch(msg.message)
			{
				//�ƶ���Ϣ
			case SUB_WINDOW_MOVE:
				{
					pAnalyer->OnSubWindowMove(msg.wParam, msg.lParam);
					break;
				}
			case SUB_WINDOW_DESTORY:
				{
					pAnalyer->OnSubWindowDestory(msg.wParam, msg.lParam);
					break;
				}
			case SUB_MAX_MIN:
				{
					pAnalyer->OnSubMinMax(msg.wParam, msg.lParam);
					break;
				}
			case SUB_WINDOW_OPEN:
				{
					pAnalyer->OnSubWindowCreate(msg.wParam, msg.lParam);
					break;
				}
			case SUB_WINDOW_ACTIVE:
				{
					pAnalyer->OnSubWindowActive(msg.wParam, msg.lParam);
					break;
				}
			case SUB_KEY_TYPE:
				{
					pAnalyer->OnSubKeyType(msg.wParam, msg.lParam);
					break;
				}
			case SUB_MOUSE_CLICK:
				{
					pAnalyer->OnSubMouseClick(msg.wParam, msg.lParam);
					break;
				}
			case SUB_COMMIT_RECORD:
				{
					pAnalyer->CommitRecord();
					////���������ݺ�ɾ���������ݿ�
					//pAnalyer->OnSubDropLocalDB();
					//pAnalyer->OnRebuildLocalDB();
					break;
				}
			case SUB_START_TRACK:
				{
					//CString msg;
					//msg.Format("��ʼ׷���� %d %d\n", msg.wParam, msg.lParam);
					//AfxMessageBox("��ʼ������");
					pAnalyer->StartTraceMouse(msg.wParam, msg.lParam);
					break;
				}
			case SUB_END_TRACK:
				{
					pAnalyer->EndTraceMouse(msg.wParam, msg.lParam);
					break;
				}
			case  SUB_TRACK_POINT:
				{
					pAnalyer->InTraceMouse(msg.wParam, msg.lParam);
					break;
				}
			}
		//}
	}
	//��ָ��ɾ����
	delete pAnalyer;
	pAnalyer = NULL;
	m_bStopFlag = TRUE;

	return 0;
}

//ֹͣ�����Ǹ�ѭ��
BOOL CIAAnalyser::StopWork()
{
	m_bWorkFlag = FALSE;

	//int count =0;
	//while(m_bStopFlag == FALSE)
	//{
	//	if(++count > 10)
	//		return FALSE;
	//	Sleep(500);
	//}
	return TRUE;
}

//��������
LRESULT CIAAnalyser::ExportData()
{
	return m_cWinInfoCtl->ExportData();
}

//�������������Ϣ���������Ӧ����һ���߳�
BOOL CIAAnalyser::CommitRecord()
{
	//m_cWinInfoCtl->WriteLog();
	m_cWinInfoCtl->WriteLog();

	//�½��̵߳�ǰ����Ƕ��ʽmysql֧�ֶ�����
	//DWORD dCommitID = NULL;
	//::CreateThread(NULL, 0, &CIAAnalyser::CommitInNewThread, NULL, 0, &dCommitID);
	//if(dCommitID == NULL)
	//{
	//	AfxMessageBox("Commit�̴߳���ʧ��");
	//	return FALSE;
	//}
	return TRUE;
}

//�����߳����������
DWORD CIAAnalyser::CommitInNewThread(LPVOID param)
{
	CEmbeddedMySql eMysql;

	if(eMysql.ConnectExistDB() == FALSE)
		return FALSE;

	eMysql.CommitNewRecord();
	eMysql.FreeEmbededConnect();
	return TRUE;
}

//�ؽ��������ݿ�
LRESULT CIAAnalyser::RebuildLocalDB()
{
	return m_cWinInfoCtl->RebuildLocalDB();
}

//�µĹ۲�ʱ��
LRESULT CIAAnalyser::NewWatchTime()
{
	m_cWinInfoCtl->NewWatchTime();
	return 0;
}

//��DLL����������ȡ�ô��ڴ�����Ϣ��Ϣ��ʾ����
LRESULT CIAAnalyser::OnSubWindowCreate(WPARAM wParam, LPARAM lParam)
{

	//������Ȼ���Եõ�HWND������ʵ��������û�б���ʼ�����ò������ı���
	HWND hTarget = (HWND)wParam;
	if(hTarget == m_hSelf)
		return 0;

	//�õ�����
	TCHAR title[TITLE_SIZE];
	::SendMessage(hTarget, WM_GETTEXT, TITLE_SIZE, (LPARAM)title);
	//�õ�����λ��
	RECT rect;
	::GetWindowRect(hTarget, &rect);

	//�������յ����ڼ������Ϣ������������û�б�Ҫ�ٴ��������ˣ��������������������ڴ���Ƭ
	//�õ�����
	TCHAR className[TITLE_SIZE] = "";
	if(GetClassName(hTarget, className, TITLE_SIZE) == 0)
		m_cWinInfoCtl->NewWindow(hTarget, title, "N/A", rect, true);
	else
		m_cWinInfoCtl->NewWindow(hTarget, title, className, rect, true);
	return 0;
}

//��DLL����������ȡ�ô���������Ϣ��Ϣ��ʾ����
LRESULT CIAAnalyser::OnSubWindowDestory(WPARAM wParam, LPARAM lParam)
{
	HWND hTarget = (HWND)wParam;

	m_cWinInfoCtl->WindowDestory(hTarget);
	return 0;
}

//��DLL����������ȡ�ô��ڻ�ȡ������Ϣ��ʾ����
LRESULT CIAAnalyser::OnSubWindowActive(WPARAM wParam, LPARAM lParam)
{

	HWND hTarget = (HWND)wParam;
	if(hTarget == m_hSelf)
		return 0;
	m_cWinInfoCtl->WindowActive(hTarget);

	return 0;
}

//��DLL����������ȡ�ô��ڻ�ȡ������Ϣ��ʾ����
LRESULT CIAAnalyser::OnSubWindowMove(WPARAM wParam, LPARAM lParam)
{
	HWND hTarget = (HWND)wParam;
	//�õ�����λ��
	RECT rect;
	::GetWindowRect(hTarget, &rect);
	m_cWinInfoCtl->WindowMove(hTarget, CRect(rect));

	return 0;
}

//���̼�����Ϣ
LRESULT CIAAnalyser::OnSubKeyType(WPARAM wParam, LPARAM lParam)
{
	m_cWinInfoCtl->OnKeyType();

	return 0;
}

//�������Ϣ
LRESULT CIAAnalyser::OnSubMouseClick(WPARAM wParam, LPARAM lParam)
{
	m_cWinInfoCtl->OnMouseClick();

	return 0;
}

//�����С����Ϣ
LRESULT CIAAnalyser::OnSubMinMax(WPARAM wParam, LPARAM lParam)
{
	HWND hTarget = (HWND)wParam;

	switch(lParam)
	{
	case SW_MAXIMIZE:
		{
			m_cWinInfoCtl->OnMax(hTarget);
			break;
		}
	case SW_MINIMIZE:
		{
			m_cWinInfoCtl->OnMin(hTarget);
			break;
		}
	case SW_RESTORE:
		{
			m_cWinInfoCtl->OnRestore(hTarget);
			break;
		}
	}
	return 0;
}

//�յ������ı�����Ϣ
LRESULT CIAAnalyser::OnSubSetText(WPARAM WPARAM, LPARAM lParam)
{
	MSG *msg;
	msg = (MSG *)lParam;

	return 0;
}

//��ձ������ݿ�
LRESULT CIAAnalyser::DropLocalDB()
{
	return m_cWinInfoCtl->DropLocalDB();
}

//�����������洰����Ϣ
LRESULT CIAAnalyser::WinsLocal()
{
	return m_cWinInfoCtl->OnSetWinsLocal();
}

//ȫ�־���
LRESULT CIAAnalyser::GlobleClique()
{
	return m_cWinInfoCtl->getGlobeClique();
}

//��ʾ��һ�����ര�ڼ���
LRESULT CIAAnalyser::ShowNextClique()
{
	return m_cWinInfoCtl->OnShowNextClique();
}

LRESULT CIAAnalyser::ScreenShot()
{
	return m_cWinInfoCtl->OnScreenShot();
}

//�õ�hwnd_info
BOOL CIAAnalyser::GetWindowLocals(vector<sHWNDAtt> &result)
{
	return m_cWinInfoCtl->GetWindowLocals(result);
	//return NULL;
}

//�õ�HWND
BOOL CIAAnalyser::GetHWND(vector<HWND> &result)
{
	return m_cWinInfoCtl->GetHWND(result);
}

// ƽ���������д���
BOOL CIAAnalyser::OnPlainLayout(void)
{
	return m_cWinInfoCtl->OnPlainLayout();
}

// ��ʼ�������켣
void CIAAnalyser::StartTraceMouse(int x, int y)
{
	m_lLocus.clear();
	m_lLocus.push_back(CPoint(x, y));
	//return TRUE;
}

// ��ֹ�������켣
void CIAAnalyser::EndTraceMouse(int x, int y)
{
	m_lLocus.push_back(CPoint(x, y));

	if (m_lLocus.size() <5)
	{
		m_lLocus.clear();
		AfxMessageBox("������̫��");
		return;
	}

	//�Բ��������Ԥ�������������
	for(list<CPoint>::iterator it = m_lLocus.begin(); it != m_lLocus.end(); it++)
	{
		list<CPoint>::iterator ij = it;
		ij++;
		for(; ij != m_lLocus.end();)
		{
			int dis = (it->x - ij->x) * (it->x - ij->x) + (it->y - ij->y) * (it->y - ij->y);
			if(dis < 4)
			{
				ij =m_lLocus.erase(ij);
				continue;
			}
			ij++;
		}
	}

	//AfxMessageBox("�������");
	CFeature *pfeature = new CFeature(&m_lLocus);
	CGestureRecognizer::eGesType result = m_cGesRecognizer->Recogenize(pfeature);
	switch(result)
	{
	case CGestureRecognizer::eGesType::LAST:
		{
			//AfxMessageBox("ȫ���ر�");
//			AllClose();
			this->ShowNextClique();
			break;
		}
	case  CGestureRecognizer::eGesType::NEXT:
		{
			//AfxMessageBox("ȫ����С��");
		//	AllMinimal();
			this->ShowNextClique();
			break;
		}
	case CGestureRecognizer::eGesType::PLAIN:
		{
			//AfxMessageBox("ƽ������");
		//	PlainLayout();
			this->OnPlainLayout();
			break;
		}
	case CGestureRecognizer::eGesType::CLIQUE:
		{
			//AfxMessageBox("��סQQ����");
//			DisableQQ();
			this->GlobleClique();
			break;
		}
	case CGestureRecognizer::eGesType::NONE:
		{
			AfxMessageBox("�ϲ���������");
			break;
		}
	default:
		{
			AfxMessageBox("�����ʶ��");
		}
	}

	m_lLocus.clear();
	delete pfeature;
}

// ���ڸ������켣
void CIAAnalyser::InTraceMouse(int x, int y)
{
	//�ڴ˶Խػ񵽵ĵ�������
	static int count = 0;
	if(++count >= 3)
	{
		m_lLocus.push_back(CPoint(x, y));
		count = 0;
	}

	//return TRUE;
}
