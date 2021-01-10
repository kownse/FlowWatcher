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

//消息分类函数
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
				//移动消息
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
					////传输完数据后删除本地数据库
					//pAnalyer->OnSubDropLocalDB();
					//pAnalyer->OnRebuildLocalDB();
					break;
				}
			case SUB_START_TRACK:
				{
					//CString msg;
					//msg.Format("开始追踪在 %d %d\n", msg.wParam, msg.lParam);
					//AfxMessageBox("开始跟踪在");
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
	//把指针删除了
	delete pAnalyer;
	pAnalyer = NULL;
	m_bStopFlag = TRUE;

	return 0;
}

//停止上面那个循环
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

//导出数据
LRESULT CIAAnalyser::ExportData()
{
	return m_cWinInfoCtl->ExportData();
}

//向服务器传递信息，这个东西应该另开一个线程
BOOL CIAAnalyser::CommitRecord()
{
	//m_cWinInfoCtl->WriteLog();
	m_cWinInfoCtl->WriteLog();

	//新建线程的前提是嵌入式mysql支持多连接
	//DWORD dCommitID = NULL;
	//::CreateThread(NULL, 0, &CIAAnalyser::CommitInNewThread, NULL, 0, &dCommitID);
	//if(dCommitID == NULL)
	//{
	//	AfxMessageBox("Commit线程创建失败");
	//	return FALSE;
	//}
	return TRUE;
}

//在新线程中向服务器
DWORD CIAAnalyser::CommitInNewThread(LPVOID param)
{
	CEmbeddedMySql eMysql;

	if(eMysql.ConnectExistDB() == FALSE)
		return FALSE;

	eMysql.CommitNewRecord();
	eMysql.FreeEmbededConnect();
	return TRUE;
}

//重建本地数据库
LRESULT CIAAnalyser::RebuildLocalDB()
{
	return m_cWinInfoCtl->RebuildLocalDB();
}

//新的观察时间
LRESULT CIAAnalyser::NewWatchTime()
{
	m_cWinInfoCtl->NewWatchTime();
	return 0;
}

//从DLL共享数据中取得窗口创建消息消息显示出来
LRESULT CIAAnalyser::OnSubWindowCreate(WPARAM wParam, LPARAM lParam)
{

	//这里虽然可以得到HWND，但是实际上它还没有被初始化，得不到它的标题
	HWND hTarget = (HWND)wParam;
	if(hTarget == m_hSelf)
		return 0;

	//得到标题
	TCHAR title[TITLE_SIZE];
	::SendMessage(hTarget, WM_GETTEXT, TITLE_SIZE, (LPARAM)title);
	//得到窗口位置
	RECT rect;
	::GetWindowRect(hTarget, &rect);

	//由于先收到窗口激活的消息，所以在这里没有必要再创建窗口了，如果那样做反而会造成内存碎片
	//得到类名
	TCHAR className[TITLE_SIZE] = "";
	if(GetClassName(hTarget, className, TITLE_SIZE) == 0)
		m_cWinInfoCtl->NewWindow(hTarget, title, "N/A", rect, true);
	else
		m_cWinInfoCtl->NewWindow(hTarget, title, className, rect, true);
	return 0;
}

//从DLL共享数据中取得窗口销毁消息消息显示出来
LRESULT CIAAnalyser::OnSubWindowDestory(WPARAM wParam, LPARAM lParam)
{
	HWND hTarget = (HWND)wParam;

	m_cWinInfoCtl->WindowDestory(hTarget);
	return 0;
}

//从DLL共享数据中取得窗口获取焦点消息显示出来
LRESULT CIAAnalyser::OnSubWindowActive(WPARAM wParam, LPARAM lParam)
{

	HWND hTarget = (HWND)wParam;
	if(hTarget == m_hSelf)
		return 0;
	m_cWinInfoCtl->WindowActive(hTarget);

	return 0;
}

//从DLL共享数据中取得窗口获取焦点消息显示出来
LRESULT CIAAnalyser::OnSubWindowMove(WPARAM wParam, LPARAM lParam)
{
	HWND hTarget = (HWND)wParam;
	//得到窗口位置
	RECT rect;
	::GetWindowRect(hTarget, &rect);
	m_cWinInfoCtl->WindowMove(hTarget, CRect(rect));

	return 0;
}

//键盘键入消息
LRESULT CIAAnalyser::OnSubKeyType(WPARAM wParam, LPARAM lParam)
{
	m_cWinInfoCtl->OnKeyType();

	return 0;
}

//鼠标点击消息
LRESULT CIAAnalyser::OnSubMouseClick(WPARAM wParam, LPARAM lParam)
{
	m_cWinInfoCtl->OnMouseClick();

	return 0;
}

//最大化最小化消息
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

//收到设置文本的消息
LRESULT CIAAnalyser::OnSubSetText(WPARAM WPARAM, LPARAM lParam)
{
	MSG *msg;
	msg = (MSG *)lParam;

	return 0;
}

//清空本地数据库
LRESULT CIAAnalyser::DropLocalDB()
{
	return m_cWinInfoCtl->DropLocalDB();
}

//重新排列桌面窗口消息
LRESULT CIAAnalyser::WinsLocal()
{
	return m_cWinInfoCtl->OnSetWinsLocal();
}

//全局聚类
LRESULT CIAAnalyser::GlobleClique()
{
	return m_cWinInfoCtl->getGlobeClique();
}

//显示下一个聚类窗口集合
LRESULT CIAAnalyser::ShowNextClique()
{
	return m_cWinInfoCtl->OnShowNextClique();
}

LRESULT CIAAnalyser::ScreenShot()
{
	return m_cWinInfoCtl->OnScreenShot();
}

//得到hwnd_info
BOOL CIAAnalyser::GetWindowLocals(vector<sHWNDAtt> &result)
{
	return m_cWinInfoCtl->GetWindowLocals(result);
	//return NULL;
}

//得到HWND
BOOL CIAAnalyser::GetHWND(vector<HWND> &result)
{
	return m_cWinInfoCtl->GetHWND(result);
}

// 平铺桌面现有窗口
BOOL CIAAnalyser::OnPlainLayout(void)
{
	return m_cWinInfoCtl->OnPlainLayout();
}

// 开始跟踪鼠标轨迹
void CIAAnalyser::StartTraceMouse(int x, int y)
{
	m_lLocus.clear();
	m_lLocus.push_back(CPoint(x, y));
	//return TRUE;
}

// 终止跟踪鼠标轨迹
void CIAAnalyser::EndTraceMouse(int x, int y)
{
	m_lLocus.push_back(CPoint(x, y));

	if (m_lLocus.size() <5)
	{
		m_lLocus.clear();
		AfxMessageBox("采样点太少");
		return;
	}

	//对采样点进行预处理，消除相近点
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

	//AfxMessageBox("跟踪完成");
	CFeature *pfeature = new CFeature(&m_lLocus);
	CGestureRecognizer::eGesType result = m_cGesRecognizer->Recogenize(pfeature);
	switch(result)
	{
	case CGestureRecognizer::eGesType::LAST:
		{
			//AfxMessageBox("全部关闭");
//			AllClose();
			this->ShowNextClique();
			break;
		}
	case  CGestureRecognizer::eGesType::NEXT:
		{
			//AfxMessageBox("全部最小化");
		//	AllMinimal();
			this->ShowNextClique();
			break;
		}
	case CGestureRecognizer::eGesType::PLAIN:
		{
			//AfxMessageBox("平铺桌面");
		//	PlainLayout();
			this->OnPlainLayout();
			break;
		}
	case CGestureRecognizer::eGesType::CLIQUE:
		{
			//AfxMessageBox("锁住QQ窗口");
//			DisableQQ();
			this->GlobleClique();
			break;
		}
	case CGestureRecognizer::eGesType::NONE:
		{
			AfxMessageBox("认不到的手势");
			break;
		}
	default:
		{
			AfxMessageBox("错误的识别");
		}
	}

	m_lLocus.clear();
	delete pfeature;
}

// 正在跟踪鼠标轨迹
void CIAAnalyser::InTraceMouse(int x, int y)
{
	//在此对截获到的点做处理
	static int count = 0;
	if(++count >= 3)
	{
		m_lLocus.push_back(CPoint(x, y));
		count = 0;
	}

	//return TRUE;
}
