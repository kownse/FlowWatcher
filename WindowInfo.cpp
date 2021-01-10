#include "StdAfx.h"
#include "WindowInfo.h"
#include <math.h>

#define  SHOW FALSE

void WindowInfo::Init()
{
	//m_tDestory = new CTime();
#ifdef DATA_CACHE
	title_list = new list<TitleNode>;
	local_list = new list<LocalNode>;								//位移列表
	keyType_list = new list<KeyTypeNode>;					//键入概率列表
	mouseClic_list = new list<MouseClickNode>;			//鼠标点击列表
//	active_list = new list<ActiveNode>;							//激活列表 
#endif // DATA_CACHE

	m_tLastAttTime = CTime::GetCurrentTime();												
	m_iTotalTime = 0;													
	m_iTotalAttTime = 0;		
	m_dFocusBalance = 0.0;
	m_iLastAttTime = 0;
	m_iLastNoAttTime = 0;

	m_tLastLocalTime = m_tLastAttTime;
	m_iMouseCount = 0;												
	m_iKeyCount = 0;	

	m_iAttRate = 0;
	m_fImportance = 0.0f;

	m_iInDegree = 0;
	m_iOutDegree = 0;
}

//显示销毁对象
void WindowInfo::Destroy()
{
#ifdef DATA_CACHE
	local_list->clear();
	keyType_list->clear();
	mouseClic_list->clear();
//	active_list->clear();
	title_list->clear();
	delete local_list;
	delete keyType_list;
	delete mouseClic_list;
//	delete active_list;
	delete title_list;
#endif // DATA_CACHE

	//if(m_tDestory != NULL)
	//	delete m_tDestory;
}

WindowInfo::WindowInfo(HWND hWnd, const CString& title, const CTime& tCreate, bool active)
	:m_hWnd(hWnd),m_tCreate(tCreate)
{
	Init();
}

WindowInfo::WindowInfo(void)// : m_bContact(false)
{
	Init();
	//map<CTime, CRect>::
}

//预计在对象销毁的时候写日志文件
WindowInfo::~WindowInfo(void)
{

}

//带有创建时间的构造函数
WindowInfo::WindowInfo(const ATL::CTime& tCreate)
:m_tCreate(tCreate)
{
	Init();
	this->m_tCreate = tCreate;
}

//带有创建时间
WindowInfo::WindowInfo(const ATL::CTime& tCreate, const RECT& lRect)
:m_tCreate(tCreate)
{
	Init();
	//this->m_tCreate = tCreate;
	//this->local_list.push_back(lRect);
}

//应该是最实用的构造函数
WindowInfo::WindowInfo(HWND hWnd, const CString& title, const RECT& lRect, bool active)// : m_bContact(false)
:m_tCreate(CTime::GetCurrentTime()),m_hWnd(hWnd)
{
	Init();
	//m_tCreate = CTime::GetCurrentTime();
	//this->m_hWnd = hWnd;
	//this->m_sIniTitle = title;
#ifdef DATA_CACHE
	this->title_list->push_back(TitleNode(m_tCreate, title));
	this->local_list->push_back(LocalNode(m_tCreate,lRect));
//	this->active_list->push_back(ActiveNode(m_tCreate, active));
#endif
	
	//if(SHOW)
	//{
	//	CString msg;
	//	msg.Format("New Window %s %d", title_list->begin()->title, m_tCreate.GetSecond());
	//	AfxMessageBox(msg);
	//}
}

//带类名的构造函数
WindowInfo::WindowInfo(HWND hWnd, const CString& title, const RECT& lRect, bool active, const CString& className)
:m_tCreate(CTime::GetCurrentTime()),m_hWnd(hWnd),m_csclassName(className)
{
	Init();
	//m_tCreate = CTime::GetCurrentTime();
	//this->m_hWnd = hWnd;
	//this->m_sIniTitle = title;
	//this->className = className;

#ifdef DATA_CACHE
	this->title_list->push_back(TitleNode(m_tCreate, title));
	this->local_list->push_back(LocalNode(m_tCreate,lRect));
//	this->active_list->push_back(ActiveNode(m_tCreate, active));
#endif
}

//从句柄得到初始化信息
WindowInfo::WindowInfo(HWND hWnd)
:m_hWnd(hWnd),m_tCreate(CTime::GetCurrentTime())
{
	Init();
	//this->m_hWnd = hWnd;									//设置句柄
	//m_tCreate = CTime::GetCurrentTime();				//设置创建时间
	TCHAR title[TITLE_SIZE];
	//得到标题
	SendMessage(hWnd, WM_GETTEXT, TITLE_SIZE, (LPARAM)title);
	//m_sIniTitle.SetString(title);									//设置标题
	
	//得到窗口位置
	RECT rect;
	GetWindowRect(hWnd, &rect);
#ifdef DATA_CACHE
	local_list->push_back(LocalNode(m_tCreate,CRect(rect)));								//设置位置
//	active_list->push_back(ActiveNode(m_tCreate, true));
	title_list->push_back(TitleNode(m_tCreate, CString(title)));
#endif

}

//新的位置
void WindowInfo::NewRect(const RECT& rect)
{
	CTime time = CTime::GetCurrentTime();
	this->local_list->push_back(LocalNode(time,rect));
}

#ifdef DB_LOG
//键盘输入事件
void WindowInfo::OnKeyType(const CTime& watchTime, CAbstractDBManager *db)
{
	//当前键盘事件计数++
	m_iKeyCount++;

	//如果键盘键入列表为空，就建立新的列表
	if(keyType_list->empty())
	{
		keyType_list->push_back(KeyTypeNode(watchTime, 1));
		return;

		//AfxMessageBox("建立新的键入节点");
	}

	//已经有节点存在,得到最后一个节点
	list<KeyTypeNode>::iterator it = --keyType_list->end();
	//如果最后个节点的监控时间就是当前时间，则增加计数器
	if(it->time == watchTime)
	{
		(it->count)++;

		//AfxMessageBox("原键入节点自增");
	}
	//如果不等于，就插入新的节点
	else
	{
		//写数据库
		db->NewKeyType(wnd_id, (unsigned int)(it->count), it->time);
		//创建新的结点
		keyType_list->push_back(KeyTypeNode(watchTime, 1));	
		//AfxMessageBox("建立新的键入节点");
	}
}
#else
//键盘输入事件
void WindowInfo::OnKeyType(const CTime& watchTime)
{
	//当前键盘事件计数++
	m_iKeyCount++;

	//如果键盘键入列表为空，就建立新的列表
	if(keyType_list->empty())
	{
		keyType_list->push_back(KeyTypeNode(watchTime, 1));
		return;

		//AfxMessageBox("建立新的键入节点");
	}

	//已经有节点存在,得到最后一个节点
	list<KeyTypeNode>::iterator it = --keyType_list->end();
	//如果最后个节点的监控时间就是当前时间，则增加计数器
	if(it->time == watchTime)
	{
		(it->count)++;

		//AfxMessageBox("原键入节点自增");
	}
	//如果不等于，就插入新的节点
	else
	{
		//创建新的结点
		keyType_list->push_back(KeyTypeNode(watchTime, 1));	
		//AfxMessageBox("建立新的键入节点");
	}
}

#endif // DB_LOG

#ifdef DB_LOG
//检测鼠标点击事件
void WindowInfo::OnMouseClick(const CTime& watchTime, CAbstractDBManager *db)
{
	//当前鼠标事件计数++
	m_iMouseCount++;

	//如果键盘键入列表为空，就建立新的列表
	if(mouseClic_list->empty())
	{
		mouseClic_list->push_back(MouseClickNode(watchTime, 1));
		return;

		//AfxMessageBox("建立新的鼠标节点");
	}

	//已经有节点存在,得到最后一个节点
	list<MouseClickNode>::iterator it = --mouseClic_list->end();
	//如果最后个节点的监控时间就是当前时间，则增加计数器
	if(it->time == watchTime)
	{
		(it->count)++;

		//AfxMessageBox("原鼠标节点自增");
	}
	//如果不等于，就插入新的节点
	else
	{
		//写数据库
		db->NewMouseClick(wnd_id, (unsigned int)(it->count), it->time);
		//创建新节点
		mouseClic_list->push_back(MouseClickNode(watchTime, 1));
		//AfxMessageBox("建立新的鼠标节点");
	}
}

#else
//检测鼠标点击事件
void WindowInfo::OnMouseClick(const CTime& watchTime)
{
	//当前鼠标事件计数++
	m_iMouseCount++;

	//如果键盘键入列表为空，就建立新的列表
	if(mouseClic_list->empty())
	{
		mouseClic_list->push_back(MouseClickNode(watchTime, 1));
		return;

		//AfxMessageBox("建立新的鼠标节点");
	}

	//已经有节点存在,得到最后一个节点
	list<MouseClickNode>::iterator it = --mouseClic_list->end();
	//如果最后个节点的监控时间就是当前时间，则增加计数器
	if(it->time == watchTime)
	{
		(it->count)++;

		//AfxMessageBox("原鼠标节点自增");
	}
	//如果不等于，就插入新的节点
	else
	{
		//创建新节点
		mouseClic_list->push_back(MouseClickNode(watchTime, 1));
		//AfxMessageBox("建立新的鼠标节点");
	}
}
#endif // DB_LOG

#ifdef DB_LOG
//窗口激活
void WindowInfo::WindowActive(const CTime& time, CAbstractDBManager *db)
{
	//重新开始记录关注度参数
	NewAtt(time);
	//CTime time = CTime::GetCurrentTime();
//	active_list->push_back(ActiveNode(time, false));

	//获取失去焦点的时间
	CTimeSpan timeElipse = time - m_tLastAttTime;
	LONGLONG timeCount = timeElipse.GetTotalSeconds();
	m_iTotalTime += timeCount;
	//计算失去焦点时间
	m_iLastNoAttTime = timeCount;
	if(m_iTotalTime != 0)
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
	m_tLastAttTime = time;

	//判断标题和以前的一样不
	TCHAR buffer[TITLE_SIZE];
	//得到标题
	SendMessage(m_hWnd, WM_GETTEXT,TITLE_SIZE, (LPARAM)buffer);
	CString title(buffer);
	list<TitleNode>::const_iterator title_it = --title_list->end();
	//如果不相等，就添加新的节点
	if(title.Compare(title_it->title) != 0)
	{
		title_list->push_back(TitleNode(time, title));
		//写数据库
		db->WindowAddTitle(wnd_id, title, time);
	}
	//记录焦点持续时间
	db->WindowAttTime(wnd_id, m_iLastNoAttTime, m_iLastAttTime, false, m_dFocusBalance, time);
}

#else
//窗口激活,无数据库版本
void WindowInfo::WindowActive(const CTime& time)
{
	//重新开始记录关注度参数
	NewAtt(time);
	//CTime time = CTime::GetCurrentTime();
	active_list->push_back(ActiveNode(time, false));

	//获取失去焦点的时间
	CTimeSpan timeElipse = m_tLastAttTime - time;
	int timeCount = timeElipse.GetTotalSeconds();
	m_iTotalTime += timeCount;
	//计算失去焦点时间
	m_iLastNoAttTime = timeCount;
	if(m_iTotalTime != 0)
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
	m_tLastAttTime = time;

	//判断标题和以前的一样不
	TCHAR buffer[TITLE_SIZE];
	//得到标题
	SendMessage(m_hWnd, WM_GETTEXT,TITLE_SIZE, (LPARAM)buffer);
	CString title(buffer);
	list<TitleNode>::const_iterator title_it = --title_list->end();
	//如果不相等，就添加新的节点
	if(title.Compare(title_it->title) != 0)
	{
		title_list->push_back(TitleNode(time, title));
	}
}
#endif // DB_LOG

#ifdef DB_LOG
//窗口失去激活
void WindowInfo::WindowUnActive(const CTime& time, int desktopArea, CAbstractDBManager *db)
{
//	this->active_list->push_back(ActiveNode(time, false));

	//计算焦点获取时间
	CTimeSpan timeElipse = time - m_tLastAttTime;
	LONGLONG timeCount = timeElipse.GetTotalSeconds();
	m_iLastAttTime = timeCount;
	m_iTotalAttTime += timeCount;
	m_iTotalTime += timeCount;
	if(m_iTotalTime != 0)
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
	m_tLastAttTime = time;

	//写记录
	db->WindowAttTime(wnd_id, m_iLastAttTime, m_iLastNoAttTime, true, m_dFocusBalance, time);

	//对关注度进行一个清算
	//计算键盘和鼠标事件部分
	m_iAttRate = m_iAttRate + static_cast<size_t>(m_iKeyCount * Key_Span + m_iMouseCount * Mouse_Span);

	CTimeSpan elipseTime = time - m_tLastLocalTime;
	LONGLONG seconds = elipseTime.GetTotalSeconds();
//	ASSERT(seconds <= 300);
	//计算窗口大小与桌面大小比例
	list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	int width = rit->rect.right - rit->rect.left;
	int height = rit->rect.bottom - rit->rect.top;
	int windowArea = width * height;
	//ASSERT(windowArea >= 0);
	float windowBalance = static_cast<float>(windowArea) / desktopArea;
	//ASSERT(windowBalance <= 1.05f);
	//计算窗口焦点时间
	m_iAttRate += static_cast<size_t>(windowBalance * seconds * Area_Span);

	m_tLastLocalTime = time;
}

void WindowInfo::WindowUnActive(const CTime& time, int desktopArea, long keyCount, long mouseCount, CAbstractDBManager *db)
{
	//this->active_list->push_back(ActiveNode(time, false));

	//出度增加
	m_iOutDegree++;

	//交互数据清零
	float fAvgKey = 0.0f;
	float fAvgMouse = 0.0f;

	//计算焦点获取时间
	CTimeSpan timeElipse = time - m_tLastAttTime;
	LONGLONG timeCount = timeElipse.GetTotalSeconds();
	m_iLastAttTime = timeCount;
	m_iTotalAttTime += timeCount;
	m_iTotalTime += timeCount;

	//没到1秒就近似为一半
	fAvgKey = static_cast<float>(keyCount) / 2;
	fAvgMouse = static_cast<float>(mouseCount) / 2;
	if(m_iTotalTime != 0)
	{
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
		fAvgKey = static_cast<float>(keyCount) / m_iLastAttTime;
		fAvgMouse = static_cast<float>(mouseCount) / m_iLastAttTime;
	}
	m_tLastAttTime = time;

	//写记录
	db->WindowAttTime(wnd_id, m_iLastAttTime, m_iLastNoAttTime, true, m_dFocusBalance,fAvgKey,fAvgMouse, time);

	//对关注度进行一个清算
	//计算键盘和鼠标事件部分
	//m_iAttRate = m_iAttRate + static_cast<size_t>(m_iKeyCount * Key_Span + m_iMouseCount * Mouse_Span);

	//CTimeSpan elipseTime = time - m_tLastLocalTime;
	//LONGLONG seconds = elipseTime.GetTotalSeconds();
	//ASSERT(seconds <= 300);
	////计算窗口大小与桌面大小比例
	//list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	//int width = rit->rect.right - rit->rect.left;
	//int height = rit->rect.bottom - rit->rect.top;
	//int windowArea = width * height;
	//ASSERT(windowArea >= 0);
	//float windowBalance = static_cast<float>(windowArea) / desktopArea;
	//ASSERT(windowBalance <= 1.05f);
	////计算窗口焦点时间
	//m_iAttRate += static_cast<size_t>(windowBalance * seconds * Area_Span);

	m_tLastLocalTime = time;
}

void WindowInfo::WindowActive(const CTime& time, int desktopArea, long keyCount, long mouseCount, CAbstractDBManager *db)
{
	//增加入度
	m_iInDegree++;
	//平均交互清零
	float fAvgKey = 0.0f;
	float fAvgMouse = 0.0f;

	//获取失去焦点的时间
	CTimeSpan timeElipse = time - m_tLastAttTime;
	LONGLONG timeCount = timeElipse.GetTotalSeconds();
	m_iTotalTime += timeCount;
	//计算失去焦点时间
	m_iLastNoAttTime = timeCount;
	if(m_iTotalTime != 0)
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
	m_tLastAttTime = time;

	//没到1秒就近似为一半
	fAvgKey = static_cast<float>(keyCount) / 2;
	fAvgMouse = static_cast<float>(mouseCount) / 2;
	if(m_iTotalTime != 0)
	{
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
		fAvgKey = static_cast<float>(keyCount) / m_iLastAttTime;
		fAvgMouse = static_cast<float>(mouseCount) / m_iLastAttTime;
	}
	m_tLastAttTime = time;

	//写记录
	db->WindowAttTime(wnd_id, m_iLastNoAttTime, m_iLastAttTime, false, m_dFocusBalance,fAvgKey,fAvgMouse, time);
}
#else
//窗口失去激活
void WindowInfo::WindowUnActive(const CTime& time, int desktopArea)
{
	this->active_list->push_back(ActiveNode(time, false));

	//计算焦点获取时间
	CTimeSpan timeElipse = m_tLastAttTime - time;
	int timeCount = timeElipse.GetTotalSeconds();
	m_iLastAttTime = timeCount;
	m_iTotalAttTime += timeCount;
	m_iTotalTime += timeCount;
	if(m_iTotalTime != 0)
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;


	//对关注度进行一个清算
	//计算键盘和鼠标事件部分
	m_iAttRate = m_iAttRate + static_cast<size_t>(m_iKeyCount * Key_Span + m_iMouseCount * Mouse_Span);

	CTimeSpan elipseTime = time - m_tLastLocalTime;
	LONGLONG seconds = elipseTime.GetTotalSeconds();
	ASSERT(seconds <= 300);
	//计算窗口大小与桌面大小比例
	list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	int width = rit->rect.right - rit->rect.left;
	int height = rit->rect.bottom - rit->rect.top;
	int windowArea = width * height;
	ASSERT(windowArea >= 0);
	float windowBalance = static_cast<float>(windowArea) / desktopArea;
	ASSERT(windowBalance <= 1.05f);
	//计算窗口焦点时间
	m_iAttRate += static_cast<size_t>(windowBalance * seconds * Area_Span);

	m_tLastLocalTime = time;
}
#endif

//计算关注度
void WindowInfo::CalculateAttRate(const CTime& time,int desktopArea)
{
	//CTime time = CTime::GetCurrentTime();
	//计算键盘和鼠标事件部分
	m_iAttRate = m_iAttRate + static_cast<size_t>(m_iKeyCount * Key_Span + m_iMouseCount * Mouse_Span);

	if(local_list->size() > 0)
	{
		CTimeSpan elipseTime = time - m_tLastLocalTime;
		LONGLONG seconds = elipseTime.GetTotalSeconds();
		//计算窗口大小与桌面大小比例
		list<LocalNode>::reverse_iterator rit = local_list->rbegin();
		int width = rit->rect.right - rit->rect.left;
		int height = rit->rect.bottom - rit->rect.top;
		int windowArea = width * height;
		//ASSERT(windowArea >= 0);
		float windowBalance = static_cast<float>(windowArea) / desktopArea;
//		ASSERT(seconds <= 300);
		//ASSERT(windowBalance <= 1.05f);
		//计算窗口焦点时间
		m_iAttRate += static_cast<size_t>(windowBalance * seconds * Area_Span);
		//由于窗口的生存期可能在1秒以内，那将导致其关注度为0，这会对后面的计算造成严重影响，所以要避免
		if(m_iAttRate <= 0)
			m_iAttRate = 1;

		m_tLastLocalTime = time;
	}
}

//计算重要性
void WindowInfo::CalculateImportance()
{
	//以偏移的Sigmoid函数来计算重要性
	m_fImportance = 1.0f / (1.0f + exp(-(static_cast<int>(m_iAttRate) - 2000.0f) / 600.0f));
}

//得到当前位置
const CRect WindowInfo::GetCurrentLocal() const
{
	list<LocalNode>::const_reverse_iterator it = local_list->rbegin();
	return it->rect;
}

//窗口移动
void WindowInfo::OnWindowMove(const CRect& rect, int desktopArea)
{
	CTime time = CTime::GetCurrentTime();
	//计算前一个窗口位置大小对关注度的影响
	CTimeSpan elipseTime = time - m_tLastLocalTime;
	LONGLONG seconds = elipseTime.GetTotalSeconds();
	//计算窗口大小与桌面大小比例
	list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	int width = rit->rect.right - rit->rect.left;
	int height = rit->rect.bottom - rit->rect.top;
	int windowArea = width * height;
	//ASSERT(windowArea >= 0);
	float windowBalance = static_cast<float>(windowArea) / desktopArea;
	//ASSERT(windowBalance <= 1.05f);
	//计算窗口焦点时间
	m_iAttRate += static_cast<int>(windowBalance * seconds * Area_Span);

	//设置当前时间为新的位置时间
	m_tLastLocalTime = time;

	local_list->push_back(LocalNode(time, rect));
}

#ifdef DB_LOG
//窗口销毁
void WindowInfo::OnWindowDestory(const CTime& time, CAbstractDBManager *db)
{
	//计算焦点获取时间
	CTimeSpan timeElipse = time - m_tLastAttTime;
	LONGLONG timeCount = timeElipse.GetTotalSeconds();
	m_iLastAttTime = timeCount;
	m_iTotalAttTime += timeCount;
	m_iTotalTime += timeCount;
	if(m_iTotalTime != 0)
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;

	//写记录
	db->WindowAttTime(wnd_id, m_iLastAttTime, m_iLastNoAttTime, true, m_dFocusBalance, time);
//	active_list->push_back(ActiveNode(time, false));
}

void WindowInfo::OnWindowDestory(const CTime& time, int desktopArea, long keyCount, long mouseCount, CAbstractDBManager *db)
{
	//计算焦点获取时间
	float fAvgKey = 0.0f;
	float fAvgMouse = 0.0f;

	//计算焦点获取时间
	CTimeSpan timeElipse = time - m_tLastAttTime;
	LONGLONG timeCount = timeElipse.GetTotalSeconds();
	m_iLastAttTime = timeCount;
	m_iTotalAttTime += timeCount;
	m_iTotalTime += timeCount;

	//没到1秒就近似为一半
	fAvgKey = static_cast<float>(keyCount) / 2;
	fAvgMouse = static_cast<float>(mouseCount) / 2;
	if(m_iTotalTime != 0)
	{
		m_dFocusBalance = static_cast<double>(m_iTotalAttTime) / m_iTotalTime;
		fAvgKey = static_cast<float>(keyCount) / m_iLastAttTime;
		fAvgMouse = static_cast<float>(mouseCount) / m_iLastAttTime;
	}
	m_tLastAttTime = time;

	//写记录
	db->WindowAttTime(wnd_id, m_iLastAttTime, m_iLastNoAttTime, true, m_dFocusBalance,fAvgKey,fAvgMouse, time);

}
#else
//窗口销毁
void WindowInfo::OnWindowDestory(const CTime& time)
{
	
	active_list->push_back(ActiveNode(time, false));
}
#endif // DB_LOG

//最小化
void WindowInfo::OnMin(int desktopArea)
{
	CTime time = CTime::GetCurrentTime();
	//计算前一个窗口位置大小对关注度的影响
	CTimeSpan elipseTime = time - m_tLastLocalTime;
	LONGLONG seconds = elipseTime.GetTotalSeconds();
//	ASSERT(seconds <= 300);
	//计算窗口大小与桌面大小比例
	list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	int width = rit->rect.right - rit->rect.left;
	int height = rit->rect.bottom - rit->rect.top;
	int windowArea = width * height;
	//ASSERT(windowArea >= 0);
	float windowBalance = static_cast<float>(windowArea) / desktopArea;
	//ASSERT(windowBalance <= 1.05f);
	//计算窗口焦点时间
	m_iAttRate += static_cast<int>(windowBalance * seconds * Area_Span);

	//设置当前时间为新的位置时间
	m_tLastLocalTime = time;

	//local_list->push_back(LocalNode(time, rect));
	local_list->push_back(LocalNode(CTime::GetCurrentTime(), CRect(0, 0, 0, 0)));
}

//最大化
void WindowInfo::OnMax(int desktopArea, const RECT& deskArea)
{
	CTime time = CTime::GetCurrentTime();
	//计算前一个窗口位置大小对关注度的影响
	CTimeSpan elipseTime = time - m_tLastLocalTime;
	LONGLONG seconds = elipseTime.GetTotalSeconds();
	//计算窗口大小与桌面大小比例
	list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	int width = rit->rect.right - rit->rect.left;
	int height = rit->rect.bottom - rit->rect.top;
	int windowArea = width * height;
	//ASSERT(windowArea >= 0);
	float windowBalance = static_cast<float>(windowArea) / desktopArea;
	//ASSERT(windowBalance <= 1.05f);
	//计算窗口焦点时间
	m_iAttRate += static_cast<int>(windowBalance * seconds * Area_Span);

	//设置当前时间为新的位置时间
	m_tLastLocalTime = time;

	local_list->push_back(LocalNode(CTime::GetCurrentTime(), static_cast<CRect>(deskArea)));
}

//恢复大小
void WindowInfo::OnRestore(int desktopArea, HWND hWnd)
{
	CTime time = CTime::GetCurrentTime();
	//计算前一个窗口位置大小对关注度的影响
	CTimeSpan elipseTime = time - m_tLastLocalTime;
	LONGLONG seconds = elipseTime.GetTotalSeconds();
	//计算窗口大小与桌面大小比例
	list<LocalNode>::reverse_iterator rit = local_list->rbegin();
	int width = rit->rect.right - rit->rect.left;
	int height = rit->rect.bottom - rit->rect.top;
	int windowArea = width * height;
	//ASSERT(windowArea >= 0);
	float windowBalance = static_cast<float>(windowArea) / desktopArea;
	//ASSERT(windowBalance <= 1.05f);
	//计算窗口焦点时间
	m_iAttRate += static_cast<int>(windowBalance * seconds * Area_Span);

	//设置当前时间为新的位置时间
	m_tLastLocalTime = time;

	RECT rect;
	::GetWindowRect(hWnd, &rect);	
	local_list->push_back(LocalNode(CTime::GetCurrentTime(), rect));
}

//返回是否与该窗口发生过交互
BOOL WindowInfo::Contacted()
{
	if((!mouseClic_list->empty() || !keyType_list->empty()) && m_csclassName != "Shell_TrayWnd" && title_list->front().title != "")
		return true;
	else
		return false;
}

//得到初始化标题
const CString WindowInfo::getInitTitle() const
{
	if(title_list->size() >0)
		return ((TitleNode)*(title_list->begin())).title;
	else
		return CString("N/A");
}

//重新获取焦点，初始化几个计算关注度的变量
void WindowInfo::NewAtt(const CTime &attTime)
{
	m_tLastLocalTime = attTime;
	m_iMouseCount = 0;												
	m_iKeyCount = 0;													
}

//正则化关注度
void WindowInfo::RefreshAttRate()
{
	m_iAttRate = 0;
	m_fImportance = 0.0f;
}

//返回入度
const size_t WindowInfo::GetInDegree() const
{
	return m_iInDegree;
}

//返回出度
const size_t WindowInfo::GetOutDegree() const
{
	return m_iOutDegree;
}

//返回关注度
const size_t WindowInfo::GetAttRate() const
{
	return m_iAttRate;
}

//返回重要性
const float WindowInfo::GetImportance() const
{
	return m_fImportance;
}

//该函数范围一个巨大的CString对象，在某些情况下可能导致程序崩溃
//预计将该函数该为直接写日志文件
const CString WindowInfo::toString()
{
	CString msg;
	msg.Format("Window:\t%s\r\n---------------------------------------------\r\n", title_list->begin()->title);													//标题
	msg.Append("Create: " + m_tCreate.Format("%H:%M:%S") + "\r\n");					//创建时间
	//if(m_tDestory != NULL)
	//	msg.Append("Destory: " + (*m_tDestory).Format("%H:%M:%S") + "\r\n");				//销毁时间
	msg.Append("Titles:\r\n");																				//标题
	if(!title_list->empty())
	{
		list<TitleNode>::const_iterator title_it = title_list->begin();
		while(title_it != title_list->end())
		{
			CString tMsg = (title_it->time).Format("%H:%M:%S");
			CString title = title_it->title;
			msg.Append(tMsg + "\t" + title +  "\r\n");

			++title_it;
		}
	}
	
	msg.Append("Local and Size:\r\n");																		//位置信息
//	map<CTime, CRect>::const_iterator local_it = local_list->begin();
	if(!local_list->empty())
	{
		list<LocalNode>::const_iterator local_it = local_list->begin();
		while(local_it != local_list->end())
		{
			CString tStr = local_it->time.Format("%H:%M:%S");
			CRect rect = local_it->rect;
			CString rStr;
			rStr.Format("\t[%d-%d,%d-%d]", rect.left, rect.right, rect.top, rect.bottom);
			if(local_it->type == LocalNode.MOVE)
			{
				CRect rect = local_it->rect;
				CString rStr;
				rStr.Format("\t[%d-%d,%d-%d]", rect.left, rect.right, rect.top, rect.bottom);
				msg.Append(tStr + rStr + "\r\n");
			}
			else if(local_it->type == LocalNode.MAX)
			{
				msg.Append(tStr + "\tMAX\r\n");
			}
			else if(local_it->type == LocalNode.MIN)
			{
				msg.Append(tStr + "\tMIN\r\n");
			}
			else if(local_it->type == LocalNode.RESTORE)
			{
				msg.Append(tStr + "\tRESTORE\r\n");
			}

			++local_it;
		}
	}

	//msg.Append("Active:\r\n");																				//活动信息
	//if(!active_list->empty())
	//{
	//	list<ActiveNode>::const_iterator active_it = active_list->begin();
	//	while(active_it != active_list->end())
	//	{
	//		CString tMsg = (active_it->time).Format("%H:%M:%S");
	//		bool flag = active_it->flag;
	//		if(flag)
	//			msg.Append(tMsg + "\t Active\r\n");
	//		else
	//			msg.Append(tMsg + "\t UnActive\r\n");

	//		++active_it;
	//	}
	//}

	msg.Append("KeyTyped:\r\n");																				//活动信息
	if(!keyType_list->empty())
	{
		list<KeyTypeNode>::const_iterator key_it = keyType_list->begin();
		while(key_it != keyType_list->end())
		{
			int count = key_it->count;
			CString tMsg = (key_it->time).Format("%H:%M:%S");
			tMsg.Format("%s \t%d", tMsg, count);
			msg.Append(tMsg + "\r\n");

			++key_it;
		}
	}

	msg.Append("MouseClick:\r\n");																				//活动信息
	if(!mouseClic_list->empty())
	{
		list<MouseClickNode>::const_iterator mouse_it = mouseClic_list->begin();
		while(mouse_it != mouseClic_list->end())
		{
			int count = mouse_it->count; 
			//CString nullForma ="";
			CString tMsg = (mouse_it->time).Format("%H:%M:%S");
			//if (tMsg!=nullForma)
			//{
				tMsg.Format("%s \t%d", tMsg, count);
				msg.Append(tMsg + "\r\n");
			//}
			
			++mouse_it;
		}
	}

	msg.Append("\r\n");
//	 m_tCreate.
	return msg;
}

//写日志文件
bool WindowInfo::WriteLog(CFile& file)
{
	try
	{
		CString msg;
		msg.Format("Window:\t%s\r\n---------------------------------------------\r\n", title_list->begin()->title);													//标题
		msg.Append("Create: ");
		msg += m_tCreate.Format("%Y-%m-%d %H:%M:%S");
		//msg += m_tCreate.Format("%Y:%m:%d");
		msg += "\r\n";					//创建时间
		//if(m_tDestory != NULL)
		//	msg.Append("Destory: " + (*m_tDestory).Format("%H:%M:%S") + "\r\n");				//销毁时间

		msg.Append("Local and Size:\r\n");																		//位置信息
		//	map<CTime, CRect>::const_iterator local_it = local_list->begin();
		list<LocalNode>::const_iterator local_it = local_list->begin();
		while(local_it != local_list->end())
		{
			CString tStr = local_it->time.Format("%H:%M:%S");
			if(local_it->type == LocalNode.MOVE)
			{
				CRect rect = local_it->rect;
				CString rStr;
				rStr.Format("\t[%d-%d,%d-%d]", rect.left, rect.right, rect.top, rect.bottom);
				//msg.Append(tStr+ rStr + "\r\n"););
				msg += tStr;
				msg += rStr;
				msg += "\r\n";
				
			}
			else if(local_it->type == LocalNode.MAX)
			{
				msg.Append(tStr + "\tMAX\r\n");
			}
			else if(local_it->type == LocalNode.MIN)
			{
				msg.Append(tStr + "\tMIN\r\n");
			}
			else if(local_it->type == LocalNode.RESTORE)
			{
				msg.Append(tStr + "\tRESTORE\r\n");
			}

			++local_it;
		}
		
		//(*file).Write(msg, msg.GetLength());	//CString m_data
		//(*file).Flush();									//刷新，有点写点
		file.Write(msg, msg.GetLength());	//CString m_data
		file.Flush();									//刷新，有点写点
		msg.Empty();									//清空

		msg.Append("Titles:\r\n");																				//标题
		if(!title_list->empty())
		{
			list<TitleNode>::const_iterator title_it = title_list->begin();
			while(title_it != title_list->end())
			{
				CString tMsg = (title_it->time).Format("%H:%M:%S");
				CString title = title_it->title;
				//msg.Append(tMsg + "\t" + title +  "\r\n");
				msg += tMsg;
				msg += "\t";
				msg += title;
				msg += "\r\n";

				++title_it;
			}
		}

		//(*file).Write(msg, msg.GetLength());	//CString m_data
		//(*file).Flush();									//刷新，有点写点
		file.Write(msg, msg.GetLength());	//CString m_data
		file.Flush();									//刷新，有点写点
		msg.Empty();									//清空

		//msg.Append("Active:\r\n");																				//活动信息
		//list<ActiveNode>::const_iterator active_it = active_list->begin();
		//while(active_it != active_list->end())
		//{
		//	CString tMsg = (active_it->time).Format("%H:%M:%S");
		//	bool flag = active_it->flag;
		//	if(flag)
		//	{
		//		//msg.Append(tMsg + "\t Active\r\n");
		//		msg += tMsg;
		//		msg += "\t Active\r\n";
		//	}
		//	else
		//	{
		//		//msg.Append(tMsg + "\t UnActive\r\n");
		//		msg += tMsg;
		//		msg += "\t UnActive\r\n";
		//	}

		//	++active_it;
		//}

		//(*file).Write(msg, msg.GetLength());	//CString m_data
		//(*file).Flush();									//刷新，有点写点
		file.Write(msg, msg.GetLength());	//CString m_data
		file.Flush();									//刷新，有点写点
		msg.Empty();									//清空

		msg.Append("KeyTyped:\r\n");																				//活动信息
		list<KeyTypeNode>::const_iterator key_it = keyType_list->begin();
		while(key_it != keyType_list->end())
		{
			int count = key_it->count;
			CString tMsg = (key_it->time).Format("%H:%M:%S");
			tMsg.Format("%s \t%d", tMsg, count);
			//msg.Append(tMsg + "\r\n");
			msg += tMsg;
			msg += "\r\n";

			++key_it;
		}

		//(*file).Write(msg, msg.GetLength());	//CString m_data
		//(*file).Flush();									//刷新，有点写点
		file.Write(msg, msg.GetLength());	//CString m_data
		file.Flush();									//刷新，有点写点
		msg.Empty();									//清空

		msg.Append("MouseClick:\r\n");																				//活动信息
		list<MouseClickNode>::const_iterator mouse_it = mouseClic_list->begin();
		while(mouse_it != mouseClic_list->end())
		{
			int count = mouse_it->count;
			CString tMsg = (mouse_it->time).Format("%H:%M:%S");
			tMsg.Format("%s \t%d", tMsg, count);
			//msg.Append(tMsg + "\r\n");
			msg += tMsg;
			msg += "\r\n";

			++mouse_it;
		}

		msg.Append("\r\n");

		//(*file).Write(msg, msg.GetLength());	//CString m_data
		//(*file).Flush();									//刷新，有点写点
		file.Write(msg, msg.GetLength());	//CString m_data
		file.Flush();									//刷新，有点写点
		msg.Empty();									//清空
		//	 m_tCreate.
		return true;
	}
	catch (CMemoryException* e)
	{
		//AfxMessageBox("内存出错");
		return false;
	}
	catch (CFileException* e)
	{
		CString str;
		str.Format("写入失败的原因是:%d",e->m_cause);
		//AfxMessageBox("str");
//		(*file).Abort();
		file.Abort();
		e->Delete();
		return false;
	}
	catch (CException* e)
	{
		//AfxMessageBox("未知错误");
		return false;
	}
}

// 根据IsWindow清理hwnd_info结构
BOOL WindowInfo::ExamMap(void)
{
	return 0;
}
