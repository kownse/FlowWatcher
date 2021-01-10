#include "StdAfx.h"
#include "WinInfoCtl.h"
#include "figtree.h"
#include "ScreenHelper.h"
#include "AnimatinoHelper.h"
#include <math.h>
#include <algorithm>

//CEmbeddedMySql  WinInfoCtl::m_embMySql = CEmbeddedMySql();												//嵌入式mysql
//CAbstractDBManager *WinInfoCtl::m_mysql = &WinInfoCtl::m_embMySql;

WinInfoCtl::WinInfoCtl(void)
:m_hCurrentActiveWnd(NULL),m_tWatchTime(CTime::GetCurrentTime())
{
	//m_hCurrentActiveWnd = NULL;
	//m_tWatchTime = CTime::GetCurrentTime();

	//得到记录创建时间
	rec_time = m_tWatchTime;
	//计数器设置为0
	tickCount = 0;
	//窗口创建计数开始
	wnd_create_count_list.push_back(WndCreateNode(rec_time, 0));

	//获取桌面面积
	RECT rWorkArea;
	BOOL bResult = SystemParametersInfo(SPI_GETWORKAREA, sizeof(RECT), &rWorkArea, 0);
	int width = rWorkArea.right - rWorkArea.left;
	int height = rWorkArea.bottom - rWorkArea.top;
	m_iDeskArea = width * height;
	m_cDeskRect = rWorkArea;

	m_lKeyCount = 0;
	m_lMouseCount = 0;

	//m_mysql = &m_remoteMySql;
	m_mysql = &m_embMySql;

	//当前显示的聚类窗口集合标号为0
	m_iCurClique = 0;


#ifdef DB_LOG
	//连接数据库
	ConnectDB();
#endif
}

#ifdef DB_LOG
//连接数据库
BOOL WinInfoCtl::ConnectDB()
{
	//得到计算机名
	TCHAR buffer[256] = TEXT("");
	int cnf = 0;
	DWORD dwSize = sizeof(buffer);
	//依次取得能取得所有计算机标示符
	for (cnf = 0; cnf < ComputerNameMax; cnf++)
	{
		if (!GetComputerNameEx((COMPUTER_NAME_FORMAT)cnf, buffer, &dwSize))
		{
			continue;
			//_tprintf(TEXT("GetComputerNameEx failed (%d)\n"), GetLastError());
			//return;
		}
		else
		{
			computer = buffer;
			//ZeroMemory(buffer, dwSize);
			break;
		}		
	}

	if(!m_mysql->Connect())
	//if(!m_embMySql.Connect())
	{
		m_bDBConnect = FALSE;
		return FALSE;
		//AfxMessageBox("数据库连接失败");
		//theApp.LostDBConnect();
	}
	else
	{
		m_bDBConnect = TRUE;
		CString sVersion = GetVersion();
		if(sVersion != "NONE")
		{
			rec_id = m_mysql->NewRecord(computer, rec_time, sVersion);
		}
		else
		{
			rec_id = m_mysql->NewRecord(computer, rec_time);
		}	
		return TRUE;
	}
}
#endif

WinInfoCtl::~WinInfoCtl(void)
{
	//在外面显示销毁WindowInfo结构
	map<HWND, WindowInfo>::const_iterator map_it = hwnd_info.begin();
	while(map_it != hwnd_info.end())
	{
		((WindowInfo)map_it->second).Destroy();
		++map_it;
	}
	hwnd_info.clear();
	history_list.clear();
}

//创建了一个新窗口
void WinInfoCtl::NewWindow(HWND hWnd)
{							//设置句柄
	CTime createTime = CTime::GetCurrentTime();				//设置创建时间
	TCHAR titlebuffer[TITLE_SIZE];
	//得到标题
	SendMessage(hWnd, WM_GETTEXT, TITLE_SIZE, (LPARAM)titlebuffer);
	CString title(titlebuffer);
	
	//得到窗口位置
	RECT rect;
	GetWindowRect(hWnd, &rect);
	//调用另外一个函数建立新窗口

	//得到类名
	TCHAR classNameBuffer[TITLE_SIZE] = "";
	
	if(GetClassName(hWnd, classNameBuffer, TITLE_SIZE)==0)
		NewWindow(hWnd, title, "N/A", rect, true);
	else
		NewWindow(hWnd, title, classNameBuffer, rect, true);
	//CString className(classNameBuffer);
	
}

//建立了一个新窗口
void WinInfoCtl::NewWindow(HWND hWnd, const CString& title, const CString& className, const RECT& rect, bool active)
{
	//现在map中查找那个窗口句柄
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
	//找到了那个句柄,就退出，无需创建
	if(it != hwnd_info.end())
	{
		return;
		//(*wif).WindowActive();
	}
	//没找到,应该为该窗口创建实例
	else
	{
		//获取当前时间
		CTime creat_time = CTime::GetCurrentTime();
		//当前时间段窗口计数++
		//list<WndCreateNode>::reverse_iterator rit = wnd_create_count_list.rbegin();
		////如果最后个节点的监控时间就是当前时间，则增加计数器
		//if(rit != wnd_create_count_list.rend())
		//{
		//	if( rit->time == m_tWatchTime)
		//	{
		//		(rit->count)++;
		//		(rit->recorded) = false;
		//		//AfxMessageBox("原键入节点自增");
		//	}
		//	else
		//	{
		//		wnd_create_count_list.push_back(WndCreateNode(creat_time, 1));
		//	}
		//}

		WindowInfo newInfo(hWnd, title, rect, active, className);
		
		//向数据库添加记录
#ifdef DB_LOG
		newInfo.wnd_id = m_mysql->NewWnd(creat_time, rec_id, title, className, rect);
		//newInfo.WindowActive(creat_time, m_mysql);
#endif // DB_LOG
		//if(tempInfo.wnd_id == 0)
		//{
		//	m_mysql->DisConnect();
		//	theApp.LostDBConnect();
		//}
		hwnd_info.insert(make_pair(hWnd, newInfo));

		//找到失去激活态的窗口，添加记录信息
		//判断上一个激活窗口不为空
		if(m_hCurrentActiveWnd != NULL)
		{

			//数据处理
			map<HWND, WindowInfo>::iterator itl = hwnd_info.find(m_hCurrentActiveWnd);
			WindowInfo *tempInfo;
			if(itl != hwnd_info.end())
			{
				tempInfo = &hwnd_info[m_hCurrentActiveWnd];

#ifdef DB_LOG
				tempInfo->WindowUnActive(creat_time, m_iDeskArea, m_lKeyCount, m_lMouseCount, m_mysql);
				m_lKeyCount = 0;
				m_lMouseCount = 0;
				//m_mysql->WindowActive(tempInfo->wnd_id, false, creat_time);
#else
				tempInfo->WindowUnActive(creat_time, m_iDeskArea);
#endif // DB_LOG

				//判断在该窗口上是否有鼠标或者键盘的交互，有交互再添加边信息
				//这样做的目的是为了避免对那些干扰窗口建立节点
				if(tempInfo->Contacted())
				{
					//activeGraph.insertDelay(m_hCurrentActiveWnd, hWnd);
					activeGraph.insert(m_hCurrentActiveWnd, hWnd);
					m_mysql->LogWindowSwitch(rec_id, tempInfo->wnd_id,newInfo.wnd_id, creat_time);
				}

				//记录当前窗口为活动窗口
				m_hCurrentActiveWnd = hWnd;
			}
		}
		else
		{
			//没有前一个活动节点,就在切换图中插入当前这个节点
			m_lKeyCount = 0;
			m_lMouseCount = 0;
			//activeGraph.insertDelay(hWnd);
			activeGraph.insert(hWnd);
		}
		//记录当前窗口为活动窗口
		m_hCurrentActiveWnd = hWnd;
	}
}

//窗口被激活
void WinInfoCtl::WindowActive(HWND hWnd)
{
	CTime time = CTime::GetCurrentTime();
	//当前激活窗口不是上一个
	if(hWnd != m_hCurrentActiveWnd)
	{
		//WindowInfo *wif;
		//找到要激活的窗口，添加记录信息
		map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
		WindowInfo* targetInfo;
		//找到了那个句柄
		if(it != hwnd_info.end())
		//if(targetInfo.getInitTitle() != "")
		{
			targetInfo = &hwnd_info[hWnd];
			//targetInfo = (WindowInfo)it->second;
			//((WindowInfo)it->second).WindowActive(time, &m_mysql);

#ifdef DB_LOG
			//targetInfo->WindowActive(time, m_mysql);
			targetInfo->WindowActive(time, m_iDeskArea, m_lKeyCount, m_lMouseCount, m_mysql);
			m_lKeyCount = 0;
			m_lMouseCount = 0;
			//写数据库
			//m_mysql->WindowActive(targetInfo->wnd_id, true, time);
#else
			targetInfo->WindowActive(time);
#endif
		}
		else
		{
			//没找到实例，说明程序有错，退出
			return;
		}
		//找到失去激活态的窗口，添加记录信息
		//判断上一个激活窗口不为空
		if(m_hCurrentActiveWnd != NULL)
		{
			
			//数据处理
			map<HWND, WindowInfo>::iterator itl = hwnd_info.find(m_hCurrentActiveWnd);
			if(itl != hwnd_info.end())
			{
			//只有这样才能真正修改到数据
			WindowInfo *tempInfo = &hwnd_info[m_hCurrentActiveWnd];
			//if(tempInfo->getInitTitle() != "N/A")
			//{
#ifdef DB_LOG
				//tempInfo->WindowUnActive(time, m_iDeskArea, m_mysql);
				//m_mysql->WindowActive(tempInfo->wnd_id, false, time);
				tempInfo->WindowUnActive(time, m_iDeskArea, m_lKeyCount, m_lMouseCount, m_mysql);
				m_lKeyCount = 0;
				m_lMouseCount = 0;
#else
				tempInfo->WindowUnActive(time, m_iDeskArea);
#endif // DB_LOG

				//判断在该窗口上是否有鼠标或者键盘的交互，有交互再添加边信息
				//这样做的目的是为了避免对那些干扰窗口建立节点
				if(tempInfo->Contacted())
				{
					//activeGraph.insertDelay(m_hCurrentActiveWnd, hWnd);
					activeGraph.insert(m_hCurrentActiveWnd, hWnd);
					m_mysql->LogWindowSwitch(rec_id, tempInfo->wnd_id,targetInfo->wnd_id, time);
				}

				//记录当前窗口为活动窗口
				m_hCurrentActiveWnd = hWnd;
			}
			else
			{
				//AfxMessageBox("上一个活动窗口已经洗白了");
				//NewWindow(hWnd);
			}
			//CString msg
		}
		else
		{
			//记录当前窗口为活动窗口
			m_hCurrentActiveWnd = hWnd;
			//写一个节点
			if(targetInfo->Contacted())
				//activeGraph.insertDelay(hWnd);
				activeGraph.insert(hWnd);
		}
	}
}

//窗口移动
void WinInfoCtl::WindowMove(HWND hWnd, const CRect& rect)
{
	//找到移动的窗口，添加记录信息
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
	//找到了那个句柄
	if(it != hwnd_info.end())
	{
		WindowInfo *tempInfo = &hwnd_info[hWnd];
		//WindowInfo tempInfo = ((WindowInfo)it->second);
		tempInfo->OnWindowMove(rect, m_iDeskArea);

#ifdef DB_LOG
		m_mysql->WindowMove(tempInfo->wnd_id, CTime::GetCurrentTime(), (RECT)rect);
#endif // DB_LOG
		//(*wif).WindowActive();
	}
	////没找到,应该为该窗口创建实例
	//else
	//{
	//	NewWindow(hWnd);
	//}
}

//窗口洗白
void WinInfoCtl::WindowDestory(HWND hWnd)
{
	CTime time = CTime::GetCurrentTime();
	//如果当前窗口时活动窗口，先消除活动窗口变量
	if(hWnd == m_hCurrentActiveWnd)
		m_hCurrentActiveWnd = NULL;
	//找到关闭的窗口，添加记录信息
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
	//找到了那个句柄
	if(it != hwnd_info.end())
	{
		WindowInfo *tempInfo = &hwnd_info[hWnd];

		
		//删除图节点
		if(it->second.Contacted())
		{
			//不论是否有接触，都要从历史中删除这个窗口
			activeGraph.RemoveFromClusterHistory(hWnd);
			activeGraph.remove(hWnd);
		}

		//WindowInfo tempInfo = (WindowInfo)it->second;

#ifdef DB_LOG
		//tempInfo->OnWindowDestory(time, m_mysql);
		tempInfo->OnWindowDestory(time, m_iDeskArea, m_lKeyCount, m_lMouseCount, m_mysql);
		m_lKeyCount = 0;
		m_lMouseCount = 0;
		m_mysql->WindowDestory(tempInfo->wnd_id, time);
#else
		tempInfo.OnWindowDestory(time);
#endif

		//(*wif).WindowActive();
		hwnd_info.erase(it);
	}
	

	//将该窗口信息加入历史信息列表			节约内存，不保存以前的窗口了
	//history_list.push_back(it->second);
	//(it->second).Destroy();
	//将该窗口移出活动记录
	
	
}

//键盘敲击
void WinInfoCtl::OnKeyType()
{
	if(m_hCurrentActiveWnd == NULL)
		return;

	m_lKeyCount++;
	//AfxMessageBox("Key Typed");
	//找到关闭的窗口，添加记录信息
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(m_hCurrentActiveWnd);
	//找到了那个句柄
	if(it != hwnd_info.end())
	{
		WindowInfo *temp = &hwnd_info[m_hCurrentActiveWnd];
#ifdef DB_LOG
		//((WindowInfo)it->second).OnKeyType(m_tWatchTime, &m_mysql);
		temp->OnKeyType(m_tWatchTime, m_mysql);
#else
		//((WindowInfo)it->second).OnKeyType(m_tWatchTime);
		temp->OnKeyType(m_tWatchTime);
#endif // DB_LOG
	}
}   

//鼠标点击
void WinInfoCtl::OnMouseClick()
{
	if(m_hCurrentActiveWnd == NULL)
		return;

	m_lMouseCount++;
	//找到那个句柄
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(m_hCurrentActiveWnd);
	//找到句柄
	if(it != hwnd_info.end())
	{
		WindowInfo *temp = &hwnd_info[m_hCurrentActiveWnd];
#ifdef DB_LOG
//		((WindowInfo)it->second).OnMouseClick(m_tWatchTime, &m_mysql);
		temp->OnMouseClick(m_tWatchTime, m_mysql);
#else
//		((WindowInfo)it->second).OnMouseClick(m_tWatchTime);
		temp->OnMouseClick(m_tWatchTime);
#endif // DB_LOG
	}
}

//最大最小化
void WinInfoCtl::OnMin(HWND hWnd)
{
	//找到那个句柄
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
	WindowInfo *tempInfo;
	//找到句柄
	if(it != hwnd_info.end())
	{
		//WindowInfo tempInfo = (WindowInfo)it->second;
		tempInfo = &hwnd_info[hWnd];
		tempInfo->OnMin(m_iDeskArea);
#ifdef DB_LOG
		m_mysql->WindowMove(tempInfo->wnd_id, CTime::GetCurrentTime(), (RECT)CRect(-1,-1,-1,-1));
#endif // DB_LOG
	}
}

//最大最小化
void WinInfoCtl::OnMax(HWND hWnd)
{
	//找到那个句柄
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
	//找到句柄
	if(it != hwnd_info.end())
	{
		//WindowInfo tempInfo = (WindowInfo)it->second;
		WindowInfo *tempInfo = &hwnd_info[hWnd];
		tempInfo->OnMax(m_iDeskArea, m_cDeskRect);
		//((WindowInfo)it->second).OnMax(m_iDeskArea, m_cDeskRect);
#ifdef DB_LOG
		m_mysql->WindowMove(tempInfo->wnd_id, CTime::GetCurrentTime(), (RECT)CRect(-2,-2,-2,-2));
#endif // DB_LOG
	}
}

//恢复大小
void WinInfoCtl::OnRestore(HWND hWnd)
{
	//找到那个句柄
	map<HWND, WindowInfo>::iterator it = hwnd_info.find(hWnd);
	//找到句柄
	if(it != hwnd_info.end())
	{
		WindowInfo *tempInfo = &hwnd_info[hWnd];
		tempInfo->OnRestore(m_iDeskArea,hWnd);
		//WindowInfo tempInfo = (WindowInfo)it->second;
		//((WindowInfo)it->second).OnRestore(m_iDeskArea,hWnd);
#ifdef DB_LOG
		m_mysql->WindowMove(tempInfo->wnd_id, CTime::GetCurrentTime(), (RECT)CRect(-3,-3,-3,-3));
#endif // DB_LOG  
	}
}

//产生新的监控时间
//该函数需要由MFC调用触发
void WinInfoCtl::NewWatchTime()
{
	//清理hwnd_info
	ExamMaps();
	m_tWatchTime = CTime::GetCurrentTime();
	
	//只有当前活动窗口才进行关注度计算
	//其他窗口只做关注度正则化
	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		if(it->first == m_hCurrentActiveWnd)
		{
			it->second.CalculateAttRate(m_tWatchTime, m_iDeskArea);
			//it->second.NewAtt(m_tWatchTime);
		}
		it->second.NewAtt(m_tWatchTime);
	}

	//设置活动图观察时间
	if(tickCount >= 5)
	{
		tickCount = 0;
		//刷新监控时间，同时，刷新当前活动窗口的活动时间
		activeGraph.newFixTime(m_hCurrentActiveWnd);
		//移除那些闲置5分钟以上的节点
		activeGraph.RemoveFadeNodeEdge(hwnd_info);
		activeGraph.ClusterInHistory(hwnd_info);
		//LogActiveGraphInFile();
		//清空所有WindowInfo的关注度
		for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
		{
			it->second.RefreshAttRate();
		}
	}
	else
	{
		tickCount++;
	}
#ifdef DB_LOG
	//设置数据库重连
	//if(m_mysql->IsConnected() == FALSE)
	//{
	//	//m_mysql->Connect();
	//	ConnectDB();
	//}
	//////////////////////////////////////////////////////////////////////////
	//不写窗口创建计数了，没得意思
	//没有断线，就写入窗口创建计数
	//else
	//{
	//	list<WndCreateNode>::reverse_iterator rit = wnd_create_count_list.rbegin();
	//	if(rit != wnd_create_count_list.rend())
	//	{
	//		if(rit->recorded == false)
	//		{
	//			m_mysql->RecordWndCreateCount(rec_id, rit->time, rit->count);
	//			rit->recorded = true;
	//		}
	//	}	
	//}
#endif // DB_LOG
	//添加新的窗口创建计数节点
	//wnd_create_count_list.push_back(WndCreateNode(m_tWatchTime, 0));
	
//	AfxMessageBox("new watch Time");
}

//得到工作区面积
const int WinInfoCtl::GetWorkArea()
{
	return m_iDeskArea;
}

//得到桌面显示的窗口的位置
BOOL WinInfoCtl::GetWindowLocals(vector<sHWNDAtt> &result)																
{
	//清理结构，保证返回的都是正确的
	ExamMaps();
	//计算重要性
	int sumAtt = 0;
	for(map<HWND, WindowInfo>::const_iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		if(!::IsIconic(it->first))
		{
			sumAtt += it->second.GetAttRate();
		}
	}

	if(sumAtt <= 0)
		sumAtt = 1;

	//vector<sWinLocal> tmpV;
	for(map<HWND, WindowInfo>::const_iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		if(!::IsIconic(it->first))
		{
			sHWNDAtt tmpS;
			tmpS.hwnd = it->first;
			//tmpS.rect = it->second.GetCurrentLocal();
			::GetWindowRect(it->first, &tmpS.rect);
			tmpS.attRate = it->second.GetAttRate();
			tmpS.importance = (tmpS.attRate > 0) ? tmpS.attRate / (float)sumAtt : 0.5f;
			//tmpV.push_back(tmpS);
			result.push_back(tmpS);
		}
	}

	sort(result.begin(), result.end(), &WinInfoCtl::sHWNDAttCompare);

	return TRUE;
}

//得到HWND
BOOL WinInfoCtl::GetHWND(vector<HWND> &result)
{
	//vector<sWinLocal> tmpV;
	for(map<HWND, WindowInfo>::const_iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		result.push_back(it->first);
	}
	return TRUE;
}

//随机设置窗口位置
BOOL  WinInfoCtl::OnSetWinsLocal(void)
{
	//getGlobeClique();
	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		//如果没有最小化，就改变其位置
		//if(IsIconic(it->first) == FALSE)
		//{
		::ShowWindow(it->first, SW_RESTORE);
			int x = rand() % 1000;
			int y = rand() % 700;
			int width = rand() % 400 + 300;
			int height = rand() % 400 + 300;
			CRect rect(x, y, x+width, y+height);
			::SetWindowPos(it->first, HWND_TOP, x, y, width, height, SWP_NOZORDER | SWP_DRAWFRAME | SWP_SHOWWINDOW);
		//}
	}
	return TRUE;
}

//显示下一个聚类窗口集合
BOOL WinInfoCtl::OnShowNextClique(void)
{
	//确保至少有一个聚类
	if(m_vGlobelClique.size() <= 0)
	{
		return FALSE;
	}

	m_iCurClique = ++m_iCurClique % m_vGlobelClique.size();
	for(map<HWND,WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		//::ShowWindow(it->first, SW_SHOWMINIMIZED);
		bool bFound = FALSE;
		for(vector<pair<HWND, float>>::const_iterator ij = m_vGlobelClique[m_iCurClique].begin(); ij != m_vGlobelClique[m_iCurClique].end(); ij++)
		{
			if(::IsWindow(ij->first) == TRUE)
			{
				//该窗口在该聚类中出现了，就按照原大小位置显示
				if(it->first == ij->first)
				{
					::ShowWindow(it->first, SW_RESTORE);
					bFound = true;
					break;
				}
			}
		}
		//如果没有找到就最小化
		if(bFound == FALSE)
		{
			::ShowWindow(it->first, SW_SHOWMINIMIZED);
		}
	}
	return TRUE;
}

//写记录日志
//构建一个巨大的CString来写日志
void WinInfoCtl::Log()
{
	//AfxMessageBox("写日志");
	CString msg;
	msg.Append("***********Active Window Infomation************\r\n");
	map<HWND, WindowInfo>::const_iterator map_it = hwnd_info.begin();
	while(map_it != hwnd_info.end())
	{
		msg.Append(((WindowInfo)map_it->second).toString());

		++map_it;
	}

	msg.Append("\r\n\r\n**************History Window Infomation****************\r\n");
	list<WindowInfo>::const_iterator list_it = history_list.begin();
	while (list_it != history_list.end())
	{
		msg.Append(((WindowInfo)*list_it).toString());
		++list_it;
	}

	//AfxMessageBox("遍历完成");

	CFile file;
	CString FileName="WindowInfo.log";
	CTime date = CTime::GetCurrentTime();
	CString dateMsg = date.Format("%y_%m_%d");
	FileName.Format("%s_%s", dateMsg, FileName);
	try
	{
		file.Open(FileName,CFile::modeCreate|CFile::modeWrite);
		file.SeekToBegin();
		file.Write(msg, msg.GetLength());
		file.Flush();
		file.Close();
		//MessageBox("写入成功！");
	}
	catch(CFileException *e)
	{
		CString str;
		str.Format("写入失败的原因是:%d",e->m_cause);
		//AfxMessageBox("str");
		file.Abort();
		e->Delete();
	}
	//AfxMessageBox("日志写完");

}

//逐步写日志文件
void WinInfoCtl::WriteLog()
{
	if(m_mysql->Log() == FALSE)
	{
		//AfxMessageBox("传输信息失败");
	}
	//m_mysql->Log();
	//CFile file;
	//CString FileName;
	//CTime date = CTime::GetCurrentTime();
	//CString dateMsg = date.Format("%y_%m_%d");
	//FileName.Format("%s_windowlog.log", dateMsg, FileName);
	//try
	//{
	//	file.Open(FileName,CFile::modeCreate|CFile::modeWrite);
	//	file.SeekToBegin();

	//	//以下是写日志的代码
	//	{
	//		CString msg;
	//		msg.Append("***********Active Window Infomation************\r\n");
	//		file.Write(msg, msg.GetLength());
	//		msg.Empty();
	//		map<HWND, WindowInfo>::const_iterator map_it = hwnd_info.begin();
	//		while(map_it != hwnd_info.end())
	//		{
	//			((WindowInfo)map_it->second).WriteLog(file);

	//			++map_it;
	//		}

	//		msg.Append("\r\n\r\n**************History Window Infomation****************\r\n");
	//		file.Write(msg, msg.GetLength());
	//		msg.Empty();
	//		list<WindowInfo>::const_iterator list_it = history_list.begin();
	//		while (list_it != history_list.end())
	//		{
	//			((WindowInfo)*list_it).WriteLog(file);
	//			++list_it;
	//		}
	//	}

	//	file.Flush();
	//	file.Close();
	//	//MessageBox("写入成功！");
	//}
	//catch(CFileException *e)
	//{
	//	CString str;
	//	str.Format("写入失败的原因是:%d",e->m_cause);
	//	//AfxMessageBox("str");
	//	file.Abort();
	//	e->Delete();
	//	return;
	//}
	//AfxMessageBox("日志写完");
}

#ifdef DB_LOG
//重新连接数据库
BOOL WinInfoCtl::ReConnect()
{
	//先断开连接
	m_mysql->DisConnect();

	//重连
	if(!m_bDBConnect)
	{
		if(m_mysql->Connect())
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
}

//放弃本地数据库
BOOL WinInfoCtl::DropLocalDB()
{
	return  m_embMySql.DropLocalDB();
}

//重建本地数据库
BOOL WinInfoCtl::RebuildLocalDB()
{
	if(m_embMySql.RebuildLocalDB() == TRUE)
	{
		//重新注册记录
		TCHAR buffer[256] = TEXT("");
		int cnf = 0;
		DWORD dwSize = sizeof(buffer);
		//依次取得能取得所有计算机标示符
		for (cnf = 0; cnf < ComputerNameMax; cnf++)
		{
			if (!GetComputerNameEx((COMPUTER_NAME_FORMAT)cnf, buffer, &dwSize))
			{
				continue;
				//_tprintf(TEXT("GetComputerNameEx failed (%d)\n"), GetLastError());
				//return;
			}
			else
			{
				computer = buffer;
				//ZeroMemory(buffer, dwSize);
				break;
			}		
		}

		CString sVersion = GetVersion();
		if(sVersion != "NONE")
		{
			rec_id = m_mysql->NewRecord(computer, rec_time, sVersion);
		}
		else
		{
			rec_id = m_mysql->NewRecord(computer, rec_time);
		}	
		return TRUE;
	}
	return FALSE;
}

//导出数据
BOOL WinInfoCtl::ExportData()
{
//	return m_embMySql.ExportAllTable();
	return TRUE;
}
#endif // DB_LOG

//获取程序版本
CString WinInfoCtl::GetVersion()
{
	TCHAR szVersionBuffer[8192] = _T("");   
	DWORD dwVerSize;   
	DWORD dwHandle;
	CString sLocalVersion;

	dwVerSize = GetFileVersionInfoSize("FlowWatcher.exe", &dwHandle);   
	if (dwVerSize == 0)   
	{
		//AfxMessageBox(remoteFileName + "不存在，需要更新");
		return "NONE";   
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
			return "NONE";
		}
	}
	else
	{
		return "NONE";
	}
	return "NONE";
}

//集体计算重要性
void WinInfoCtl::CalculateImportance()
{
	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		it->second.CalculateImportance();
	}
}

//得到最大关注度
size_t WinInfoCtl::GetMaxAtt()
{
	size_t max = 0;
	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		size_t tmp = it->second.GetAttRate();
		if(tmp >= max)
			max = tmp;
	}
	return max;
}

//关注度总量
size_t WinInfoCtl::GetTotalAtt()
{
	size_t total = 0;
	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		size_t tmp = it->second.GetAttRate();
		total += tmp;
	}
	//确认total是大于0的，如果没有大于0，说明有BUG
	ASSERT(total > 0);
	return total;
}

//得到正交底
float WinInfoCtl::GetNormalBase()
{
	float base = 0.0f;
	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		float tmp = static_cast<float>(it->second.GetAttRate());
		base += pow(tmp,2);
	}
	//确认total是大于0的，如果没有大于0，说明有BUG
	//貌似是长期没交互系统屏幕保护后会出现这个BUG
	ASSERT(base > 0);
	return sqrt(base);

}

void WinInfoCtl::GetBKCliqueWinInfo(vector<vector<const WindowInfo* const>> &result)
{
	//得到聚类后的窗口句柄
	vector<vector<const HWND>> resHWND;
	activeGraph.GetBKCliqueHWND(resHWND);

	//通过句柄查找得到WindowInfo引用
	for(vector<vector<const HWND>>::const_iterator it = resHWND.begin(); it != resHWND.end(); it++)
	{
		vector<const WindowInfo* const> clique;
		for(vector<const HWND>::const_iterator ij = it->begin(); ij != it->end(); ij++)
		{
			clique.push_back(static_cast<const WindowInfo* const>(&hwnd_info[*ij]));
		}
		result.push_back(clique);
	}
}

//将活动图记录在数据库中
void WinInfoCtl::LogActiveGraphInDB()
{
	//没有内容就退出
	if(activeGraph.hasSomething() == false)
		return;

	//集体计算重要性
	CalculateImportance();
	
	CTime time = CTime::GetCurrentTime();
	CString strT = time.Format("%Y-%m-%d %H:%M:%S\\n");
	//得到图结构字符串
	CString gMsg;
	gMsg.Append(strT);
	gMsg.Append(activeGraph.log());

	//查询重要性等记录
	CString dataStr;
	vector<HWND> hWndList = activeGraph.getHWNDs();
	//写节点标题
	int i = 0;
	for(vector<HWND>::iterator it = hWndList.begin(); it != hWndList.end(); it++, i++)
	{
		map<HWND, WindowInfo>::iterator itm = hwnd_info.find(*it);
		if(itm != hwnd_info.end())
		{
			//计算重要性
			//itm->second.CalculateImportance();
			CString msg;
			msg.Format("%d\t%d\t%.2f\t%s\r", i, static_cast<int>(itm->second.GetAttRate()), itm->second.GetImportance(),itm->second.getInitTitle());
			dataStr.Append(msg);
		}
	}
	//最终记录文档
	CString logStr = gMsg + dataStr;
	//将记录写入DB
	//if(m_mysql->LogCliqueResult(rec_id, time, logStr) == FALSE)
	//{
	//}
		//AfxMessageBox("日志写入数据库失败");
}

//将活动图记录在日志文件中
void WinInfoCtl::LogActiveGraphInFile()
{
	//没有内容就退出
	if(activeGraph.hasSomething() == false)
		return;

	//集体计算重要性
	//CalculateImportance();


	CFile file;
	CString FileName;
	CTime date = CTime::GetCurrentTime();
	CString dateMsg = date.Format("%y_%m_%d");
	FileName.Format("%s_activelog.log", dateMsg, FileName);
	try
	{
		file.Open(FileName,CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate);
		//file.SeekToBegin();
		file.SeekToEnd();

		//以下是写日志的代码
		{
			CString msg;
			msg.Append("\r\n**Active Window Information**");
			CTime time = CTime::GetCurrentTime();
			CString strT = time.Format("%Y-%m-%d %H:%M:%S****\r\n");
			msg.Append(strT);
			file.Write(msg, msg.GetLength());
			msg.Empty();
			
			const vector<vector<vector<pair<HWND, float>>>> clusterHistory = activeGraph.GetClusterHistory();

			for(vector<vector<vector<pair<HWND, float>>>>::const_iterator it = clusterHistory.begin(); it != clusterHistory.end(); it++)
			{
				//一个集合
				for(vector<vector<pair<HWND, float>>>::const_iterator ij = it->begin(); ij != it->end(); ij++)
				{
					//一行
					for(map<HWND, WindowInfo>::const_iterator il = hwnd_info.begin(); il != hwnd_info.end(); il++)
					{
						bool found = false;
						CString wnd;
						for(vector<pair<HWND, float>>::const_iterator ik = ij->begin(); ik != ij->end(); ik++)
						{		
							//每个窗口都应该有一个值
							if(ik->first == il->first)
							{
								wnd.Format("%.3f, ",static_cast<float>(ik->second));
								msg.Append(wnd);
								found = true;
								break;
							}
						}
						//如果没有这个窗口的聚类信息，就写个0
						if(found == false)
						{
							wnd.Format("%.3f, ", 0.0f);
							msg.Append(wnd);
						}
					}
					msg.Append("\r\n");
				}
				msg.Append("\r\n");
			}
			file.Write(msg, msg.GetLength());
		}

		file.Flush();
		file.Close();
		//MessageBox("写入成功！");
	}
	catch(CFileException *e)
	{
		CString str;
		str.Format("写入失败的原因是:%d",e->m_cause);
		//AfxMessageBox("str");
		file.Abort();
		e->Delete();
		return;
	}
}

// 用高斯核和K-Center计算全局聚类
BOOL WinInfoCtl::getGlobeClique(void)
{
	const vector<vector<vector<pair<HWND, float>>>> clusterHistory = activeGraph.GetClusterHistory();

	int cliqueNum = 0;
	for(vector<vector<vector<pair<HWND, float>>>>::const_iterator it = clusterHistory.begin(); it != clusterHistory.end(); it++)
	{
		cliqueNum += it->size();
	}

	//由于要和figtree兼容，这里实际上用的是一维数组
	int maxNum = cliqueNum * hwnd_info.size();
	double *history = new double[maxNum];
	//用下面这个index来标记一维数组中的坐标
	int index=0;
	for(vector<vector<vector<pair<HWND, float>>>>::const_iterator it = clusterHistory.begin(); it != clusterHistory.end(); it++)
	{
		//对每个集合创建一个新数组	

		//当前阶段只考虑处理每个集合中的第一个分量
		//vector<vector<pair<HWND, float>>>::const_iterator ij = it->begin();
		for(vector<vector<pair<HWND, float>>>::const_iterator ij = it->begin(); ij != it->end(); ij++)
		{
			//array[i] = new double[hwnd_info.size()];

			//一行
			//int j=0;
			for(map<HWND, WindowInfo>::const_iterator il = hwnd_info.begin(); il != hwnd_info.end(); il++)
			{
				bool found = false;
				CString wnd;
				for(vector<pair<HWND, float>>::const_iterator ik = ij->begin(); ik != ij->end(); ik++)
				{		
					//每个窗口都应该有一个值
					if(ik->first == il->first)
					{
						history[index++] = static_cast<float>(ik->second);
						
						found = true;
						break;
					}
				}
				//如果没有这个窗口的聚类信息，就写个0
				if(found == false)
				{
					history[index++] = 0.0f;
				}
			}
		}
	}

	//CString msg;
	//for(int i=0; i<index; i++)
	//{
	//	CString tmp;
	//	tmp.Format("%.3f\t", history[i]);
	//	msg.Append(tmp);

	//	if(i % hwnd_info.size() == 0)
	//		msg.Append("\r\n");
	//}
	//AfxMessageBox(msg);

	int d = hwnd_info.size();		//需要聚类的维度
	int N = cliqueNum;				//数据数量
	int M = 10;
	double h = .9;						//聚类宽度
	double epsilon =  0.9;			//聚类偏移量

	double y[] = {
		1.01f, 1.01f, 1.02f, 1.03f, 1.02f, 1.44f, 0.98f,
		1.07f, 1.01f, 1.02f, 1.03f, 1.02f, 1.44f, 0.98f};
	double q[] = {0.2280, 0.4496, 0.1722, 0.9688, 0.3557, 0.0490, 0.7553, 0.8948, 0.2861, 0.2512};
	int W = 1;

	double * g_ifgt_nu = new double[W*M];
	memset( g_ifgt_nu    , 0, sizeof(double)*W*M );

	//这个结果是index和重要性的组合集合
	std::vector<std::vector<std::pair<int, double>>*> *result = new std::vector<std::vector<std::pair<int, double>>*>;
	std::vector<int> *count = new std::vector<int>;
	figtreeKCenter(d, N, M, W, history, h, q, y, epsilon, g_ifgt_nu, result, count, FIGTREE_EVAL_IFGT_TREE, FIGTREE_PARAM_NON_UNIFORM, 1 );
	//NND，这这个数组不要了~
	delete [] history;

	//清除以前的聚类
	m_vGlobelClique.clear();
	//将索引转换为HWND并存放在m_vGlobelClique中
	//vector<int>::iterator cit = count->begin();
	for(std::vector<std::vector<std::pair<int, double>>*>::const_iterator it = result->begin(); it != result->end(); it++)
	{
		vector<pair<HWND, float>> oneClique;
		//只添加那些总重要性大于某阈值的聚类
		float sumAtt = 0.0f;
		for(std::vector<std::pair<int, double>>::const_iterator ij = (*it)->begin(); ij != (*it)->end(); ij++)
		{
			int i = 0;
			for(map<HWND, WindowInfo>::const_iterator il = hwnd_info.begin(); il != hwnd_info.end(); il++,i++)
			{
				if (i != ij->first)
				{
					continue;
				}
				else
				{
					oneClique.push_back(make_pair(il->first, ij->second));
					sumAtt += ij->second;
				}
			}
		}
		if(sumAtt >= 0.3f)
		{
			m_vGlobelClique.push_back(oneClique);
		}
		else
		{
			//删除计数
//			count->erase(cit);
		}
	}

	//生成聚类信息
	CString totalMsg;
	int indexClique = 0;
	for(vector<vector<pair<HWND, float>>>::const_iterator it = m_vGlobelClique.begin(); it != m_vGlobelClique.end(); it++,indexClique++)
	{
		CString cliqueMsg;
		for(vector<pair<HWND, float>>::const_iterator ij = it->begin(); ij != it->end(); ij++)
		{
			TCHAR titlebuffer[TITLE_SIZE];
			//得到标题
			SendMessage(ij->first, WM_GETTEXT, TITLE_SIZE, (LPARAM)titlebuffer);
			CString title(titlebuffer);
			cliqueMsg.Append(title + "\r\n");
		}
		CString countStr;
		countStr.Format("%d", count->at(indexClique));
		totalMsg.Append(cliqueMsg + "\r\n" + countStr + "\r\n");
	}
	AfxMessageBox(totalMsg);

	//清除历史数据
	count->clear();
	for(std::vector<std::vector<std::pair<int, double>>*>::iterator it = result->begin(); it != result->end(); it++)
	{
		(*it)->clear();
	}
	result->clear();

	return 0;
}

// 截图
BOOL WinInfoCtl::OnScreenShot(void)
{
	for (map<HWND, WindowInfo>::const_iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		CScreenHelper::ScreenshotHWNDFile(it->first);
	}

	return 0;
}

//平铺桌面
BOOL WinInfoCtl::OnPlainLayout(void )
{
	vector<sHWNDAtt> hwnds;
	GetWindowLocals(hwnds);

	int iWinNum = hwnds.size();

	if(iWinNum > 0)
	{
		CRect screenRect;
		SystemParametersInfo(SPI_GETWORKAREA,   0,   (PVOID)   &screenRect,   0);
		double balance = screenRect.Width() / (double)screenRect.Height();
		int base = (int)sqrt((double)iWinNum);
		int column;
		int row;

		if((base * base) < iWinNum)
		{
			if(balance > 1)
			{
				column = (iWinNum == 1) ? 1 : base + 1;
				row = ceil(iWinNum / (double) column);
			}
			else
			{
				row = base + 1;
				column = ceil(iWinNum / (double) row);
			}
		}
		else
		{
			column = row = base;
		}

		int width = max( screenRect.Width()  / column, MIN_WINDOW_WIDTH);
		int height = max( screenRect.Height() / row, MIN_WINDOW_HEIGHT);
		int stepX;
		int stepY;

		if((screenRect.Width()  / column) < MIN_WINDOW_WIDTH)
		{
			stepX = (column > ceil(balance)) ? (screenRect.Width() - MIN_WINDOW_WIDTH) / (column - 1) : (screenRect.Width() / column );
		}
		else
		{
			stepX = width;
		}
		
		if((screenRect.Height() / row) < MIN_WINDOW_HEIGHT)
		{
			stepY = (row > ceil((double)(1 / balance))) ? (screenRect.bottom - MIN_WINDOW_HEIGHT) / (row - 1) : (screenRect.Height()  / row);
		}
		else
		{
			stepY = height;
		}
		

		int index = iWinNum - 1;
		AnimatinoHelper::Begin();
		for(vector<sHWNDAtt>::const_iterator it = hwnds.begin(); it != hwnds.end(); it++, index--)
		{
			//////////////////////////////////////////////////////////////////////////
			// QQ2010 最小窗口大小 宽538 高500

			int x = stepX * (index % column);
			int y = stepY * (index / column);
			// 
			::ShowWindow(it->hwnd, SW_RESTORE);
			// 			::SetWindowPos(it->hwnd, HWND_TOPMOST, x, y, width, height, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS | SWP_SHOWWINDOW);
			CRect srcRect;
			::GetWindowRect(it->hwnd, &srcRect);
			AnimatinoHelper::AddMove(it->hwnd, srcRect, CRect(x, y, x + width, y + height));
		}
		AnimatinoHelper::MoveAnimation();
		AnimatinoHelper::End();
	}

	return TRUE;
}

//清理hwnd_info
BOOL WinInfoCtl::ExamMaps()
{
	vector<HWND> vDeadList;				//将被删除的无效句柄

	for(map<HWND, WindowInfo>::iterator it = hwnd_info.begin(); it != hwnd_info.end(); it++)
	{
		//如果这个东西不是窗口，则说明他已经被洗白了
		if(!::IsWindow(it->first))
		{
			vDeadList.push_back(it->first);
		}
	}

	for(vector<HWND>::iterator it = vDeadList.begin(); it != vDeadList.end(); it++)
	{
		WindowDestory(*it);
	}
	return TRUE;
}

//sHWNDAtt比较
bool WinInfoCtl::sHWNDAttCompare(const sHWNDAtt &a, const sHWNDAtt & b)
{
	return a.attRate > b.attRate ? true : false;
}
