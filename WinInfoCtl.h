#pragma once

#include "stdafx.h"
#include <map>
#include "WindowInfo.h"
#include "DBManager.h"
#include "WndActiveGraph.h"
#include "AbstractDBManager.h"
#include "EmbeddedMySql.h"
#include <vector>

//窗口创建计数结构体
typedef struct _wndcreate
{
	CTime time;
	int count;
	bool recorded;
	_wndcreate(CTime t, int c):time(t),count(c),recorded(false)
	{
		//this->time = time;
		//this->count = count;
		//this->recorded = false;
	}
}WndCreateNode;

class WinInfoCtl
{
public:
	explicit WinInfoCtl(void);
	~WinInfoCtl(void);
	
	void NewWindow(HWND hWnd);																		//新窗口创建
	void NewWindow(HWND hWnd, const CString& title, const CString& className, const RECT& rect, bool active);				//新窗口创建
	void WindowActive(HWND hWnd);																	//窗口激活
	//void WindowUnActive(HWND hWnd);															//窗口失去激活
	void WindowMove(HWND hWnd, const CRect& rect);													//窗口移动
	void WindowDestory(HWND hWnd);																//窗口销毁
	void OnKeyType();																								//键盘敲击消息
	void OnMouseClick();																						//鼠标点击
	void OnMin(HWND hWnd);																				//最小化
	void OnMax(HWND hWnd);																				//最大化
	void OnRestore(HWND hWnd);																			//恢复大小

	void Log();																										//构建一个巨大的CString来写日志，有时候会由于内存错误崩溃
	void WriteLog();																								//逐步写日志文件
	void NewWatchTime();																						//产生新的监控时间
	void LogActiveGraphInFile();																				//将活动图在日志文件中写下来
	void LogActiveGraphInDB();																				//将挥动图文件写在数据库中
	void CalculateImportance();																				//集体计算重要性

	/////////////////////////////////////////////////////////////////
	//Getters
	size_t GetMaxAtt();																								//得到最大关注度
	size_t GetTotalAtt();																								//得到总的关注度
	float GetNormalBase();																							//得到正交底
	const int GetWorkArea();																							//得到工作区面积
	BOOL GetWindowLocals(vector<sHWNDAtt> &result);																//得到窗口位置
	BOOL GetHWND(vector<HWND> &result);																//得到HWND

	//////////////////////////////////////////////////////////////////////////
	// 聚类相关
	void GetBKCliqueWinInfo(vector<vector<const WindowInfo* const>> &result);			//得到BK聚类结果以WIndowInfo表示
	BOOL getGlobeClique(void);																							// 用高斯核和K-MEANS计算全局聚类

	////////////////////////////////
	//操作窗口
	///////////////////////////////
	BOOL OnSetWinsLocal(void);																			        //设置窗口排列
	BOOL OnShowNextClique(void);																			//显示下一个聚类窗口集合
	BOOL OnScreenShot(void);																					//截图
	BOOL OnPlainLayout(void	);																					//平铺桌面

	//////////////////////////////////////////////////////////////////////////
	// 比较函数
	static bool sHWNDAttCompare(const sHWNDAtt &a, const sHWNDAtt & b);

	//////////////////////////////////////////////////////////////////////////
	// 清理函数
	//////////////////////////////////////////////////////////////////////////
	BOOL ExamMaps();																								//清理hwnd_info

#ifdef DB_LOG
	BOOL ConnectDB();																								//连接数据库
	BOOL ReConnect();																								//重新连接
	BOOL DropLocalDB();																							//放弃本地数据库
	BOOL RebuildLocalDB();																						//重建本地数据库
	BOOL ExportData();																								//导出数据
#endif // DB_LOG

private:
	CString GetVersion();																	//获取版本信息
	

private:
	map<HWND, WindowInfo> hwnd_info;											//HWND与其信息的map,记录当前活动窗口的信息
	CWndActiveGraph activeGraph;														//窗口切换图结构
	list<WndCreateNode> wnd_create_count_list;									//窗口创建计数列表
	list<WindowInfo> history_list;															//记录已经关闭窗口的历史信息
	vector<vector<pair<HWND, float>>> m_vGlobelClique;		//全局聚类
	HWND m_hCurrentActiveWnd;															//上一个激活窗口，用于标记窗口失去激活太
	CTime m_tWatchTime;																		//当前监控时间,用于监控窗口接受的交互信息频率计算
	unsigned int tickCount;																		//计数器
	int m_iDeskArea;																				//桌面面积
	RECT m_cDeskRect;																			//桌面RECT
	
	CTime rec_time;																				//当前记录时间
	long m_lKeyCount;																			//键盘计数
	long m_lMouseCount;																		//鼠标计数

	//////////////////////////////////////////////////////////////////////////
	// 窗口操作
	int m_iCurClique;																				//当前显示的聚类标号

	//////////////////////////////////////////////////////////////////////////
	//常量
private:
	static const int MIN_WINDOW_WIDTH = 500;
	static const int MIN_WINDOW_HEIGHT = 450;

#ifdef DB_LOG
	TCHAR *computer;																			//计算机名
	unsigned int rec_id;																			//在数据库中的记录号 
	//DBManager	m_remoteMySql;																//mysql数据库连接实例
	CEmbeddedMySql m_embMySql;														//嵌入式mysql
	CAbstractDBManager *m_mysql;
	BOOL m_bDBConnect;																		//数据库连接标志
#endif


};
