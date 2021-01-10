#pragma once
#include "stdafx.h"
#include <list>
#include <ctime>
#include <map>
#include "DBManager.h"
//#include "CMD_Globle.h"

using namespace std;

//活动结构体
typedef struct _activenode
{
	CTime time;
	bool flag;
	_activenode(const CTime& t, bool f) : time(t), flag(f)
	{
		//this->time = time; 
		//this->flag = flag;
	}
}ActiveNode;
//ActiveNode
//class ActiveNode
//{
//public:
//	ActiveNode(CTime time, bool flag)
//	{
//		this->time = time;
//		this->flag = flag;
//	}
//	CTime time;
//	bool flag;
//};

//位置结构体
typedef struct _localnode
{
	enum LOCALTYPE {MOVE, MAX, MIN, RESTORE};
	CTime time;
	CRect rect;
	LOCALTYPE type;
	_localnode(const CTime& t, const CRect& r): time(t), rect(r), type(MOVE)
	{
		//this->time = time;
		//this->rect = rect;
		//type = MOVE;
	}

	_localnode(const CTime& t, LOCALTYPE l):time(t),type(l)
	{
		//this->time = time;
		//this->type = type;
	}
}LocalNode;
//LocalNode::_localnode(ATL::CTime time, CRect rect)
//{
//	this->time = time;
//	this->rect = rect;
//}


//键盘输入结构体
typedef struct _keytypenode
{
	CTime time;
	int count;
	_keytypenode(const CTime& t, int c):time(t),count(c)
	{
		//this->time = time;
		//this->count = count;
	}
}KeyTypeNode;
//KeyTypeNode::_keytypenode(ATL::CTime time, int count)
//{
//	this->time = time;
//	this->count = count;
//}

//鼠标点击结构体
typedef struct _mouseclicknode
{
	CTime time;
	int count;
	_mouseclicknode(const CTime& t, int c):time(t),count(c)
	{
		//this->time = time;
		//this->count = count;
	}
}MouseClickNode;
//MouseClickNode::_mouseclicknode(CTime time, int count)
//{
//	this->time = time;
//	this->count = count;
//}

//标题结构体
typedef struct _titlenode
{
	CTime time;
	CString title;
	_titlenode(const CTime& t, const CString& ti):time(t),title(ti)
	{
		//this->time = time;
		//this->title = title;
	}
}TitleNode;

//记录窗口中无法从HWND得到的相关信息
class WindowInfo
{

public:
	WindowInfo(void);
	~WindowInfo(void);

	explicit WindowInfo(const CTime& tCreate);															//带创建时间的构造函数
	explicit WindowInfo(const CTime& tCreate, const RECT& lRect);										//带时间和形状矩阵的构造函数
	explicit WindowInfo(HWND hWnd);															//从句柄得到初始化信息应该是最方便的了
	explicit WindowInfo(HWND hWnd, const CString& title,const RECT& lRect, bool active);				//具有具体消息的构造函数
	explicit WindowInfo(HWND hWnd, const CString& title, const CTime& tCreate, bool active);		//窗口创建
	explicit WindowInfo(HWND hWnd, const CString& title, const RECT& lRect, bool active, const CString& className);
	void NewRect(const RECT& rect);																//新的位置和大小
	void OnDestory(const CTime& tDestory);													//窗口销毁
	void OnWindowMove(const CRect& rect, int deskArea);												//窗口移动
	void OnMin(int desktopArea);																				//最小化
	void OnMax(int desktopArea, const RECT& deskArea);																				//最大化
	void OnRestore(int desktopArea, HWND hWnd);																			//恢复大小
	BOOL Contacted();																								//是否与该窗口发生过交互
	const CString getInitTitle() const;																								//得到初始化标题
	const size_t GetInDegree() const;																					//得到入度
	const size_t GetOutDegree() const;																					//得到出度
	const size_t GetAttRate() const;																						//返回关注度

	void NewAtt(const CTime &attTime);																				//重新获取焦点
	void RefreshAttRate();																							//正则化
	const float GetImportance() const;																					//返回重要性
	void CalculateAttRate(const CTime& time,int desktopArea);												//计算关注度
	void CalculateImportance();																					//计算重要度
	const CRect GetCurrentLocal() const;																						//得到当前位置

#ifdef DB_LOG
	void OnKeyType(const CTime& watchTime, CAbstractDBManager *db);												//键盘键入事件
	void OnMouseClick(const CTime& watchTime, CAbstractDBManager *db);											//鼠标点击事件
	void WindowActive(const CTime& time, CAbstractDBManager *db);														//窗口激活
	void WindowActive(const CTime& time, int desktopArea, long keyCount, long mouseCount, CAbstractDBManager *db);
	void WindowUnActive(const CTime& time, int desktopArea, CAbstractDBManager *db);						//窗口失去激活
	void WindowUnActive(const CTime& time, int desktopArea, long keyCount, long mouseCount, CAbstractDBManager *db);				//窗口失去激活，记录活动期间键盘和鼠标平均计数
	void OnWindowDestory(const CTime& time, CAbstractDBManager *db);																//窗口销毁
	void OnWindowDestory(const CTime& time, int desktopArea, long keyCount, long mouseCount, CAbstractDBManager *db);
#else
	void WindowActive(const CTime& time);																				//窗口激活，无数据库版本
	void OnKeyType(const CTime& watchTime);																			//键盘键入事件,无数据库版本
	void OnMouseClick(const CTime& watchTime);																		//鼠标点击事件,无数据库版本
	void WindowUnActive(const CTime& time, int desktopArea);																//窗口失去激活
	void OnWindowDestory(const CTime& time);																//窗口销毁
#endif

	const CString toString();																			//打印信息
	bool WriteLog(CFile &file);																//写日志文件
	void Destroy();																				//对象销毁

private:
	void Init();																						//初始化各种变量


private:
	HWND m_hWnd;														//窗口句柄
	//CString	m_sIniTitle;												//初始化窗口名
	CTime m_tCreate;														//创建时间
	//CTime *m_tDestory;												//销毁时间 很恶心
	CString m_csclassName;													//类名

	//在内存中缓存的信息////////////////////////////////////////////////////
#ifdef DATA_CACHE
	list<TitleNode> *title_list;											//标题列表
	list<LocalNode> *local_list;										//位移列表
	list<KeyTypeNode> *keyType_list;								//键入概率列表
	list<MouseClickNode> *mouseClic_list;						//鼠标点击列表
	//list<ActiveNode> *active_list;									//激活列表
#endif // DATA_CACHE
	//////////////////////////////////////////////////////////////////////////

	//计算关注度的用量//////////////////////////////////////////////////////
	size_t m_iAttRate;														//窗口关注度，每分钟进行一次正则化
	size_t m_iMouseCount;												//即时鼠标事件计数
	size_t m_iKeyCount;													//即时键盘事件计数
	CTime m_tLastLocalTime;											//前一个窗口移动时间 
	float m_fImportance;												//窗口重要性，范围[0,1]
	float m_fImpDecSpeed;												//重要性衰减速度
	//////////////////////////////////////////////////////////////////////////

	//焦点时间统计相关//////////////////////////////////////////////////////
	CTime m_tLastAttTime;												//前一个获取焦点的时间
	size_t m_iTotalTime;													//总生存时间
	size_t m_iTotalAttTime;												//总焦点时间
	double m_dFocusBalance;											//激活时间比
	int m_iLastAttTime;													//上一个焦点时间
	int m_iLastNoAttTime;												//上一个失去焦点时间
	//////////////////////////////////////////////////////////////////////////

	//出入度统计/////////////////////////////////////////////////////////////
	size_t m_iInDegree;													//入度
	size_t m_iOutDegree;												//出度
	//////////////////////////////////////////////////////////////////////////

public:
	unsigned int wnd_id;													//数据库中的窗口ID
	// 根据IsWindow清理hwnd_info结构
	BOOL ExamMap(void);
};


