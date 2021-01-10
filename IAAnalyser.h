#pragma once
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
//这个类处理所有的hook信息
//////////////////////////////////////////////////////////////////////////
class CIAAnalyser
{
public:
	CIAAnalyser(void);
	CIAAnalyser(HWND hSelf);
	~CIAAnalyser(void);

	//////////////////////////////////////////////////////////////////////////
	//以下为处理具体消息的函数
	LRESULT OnSubWindowCreate(WPARAM wParam, LPARAM lParam);			//接到CBT钩子截获的窗口创建消息
	LRESULT OnSubWindowActive(WPARAM wParam, LPARAM lParam);			//接到窗口获取焦点消息
	LRESULT OnSubWindowMove(WPARAM wParam, LPARAM lParam);			//接到窗口移动消息
	LRESULT OnSubWindowDestory(WPARAM wParam, LPARAM lParam);			//接到窗口销毁消息
	LRESULT OnSubKeyType(WPARAM wParam, LPARAM lParam);						//接到键盘消息
	LRESULT OnSubMouseClick(WPARAM wParam, LPARAM lParam);				//接到鼠标点击消息
	LRESULT OnSubMinMax(WPARAM wParam, LPARAM lParam);						//接到最大最小化消息
	LRESULT OnSubSetText(WPARAM wParam, LPARAM lParam);					//接到设置文本消息
	
	///////////////////////////////////////////////////////////////////////////
	//数据库操作
	LRESULT DropLocalDB();																	//清空本地数据库
	LRESULT RebuildLocalDB();																	//重建本地数据库
	LRESULT ExportData();																			//导出数据
	LRESULT NewWatchTime();																//新的观察时间

	//////////////////////////////////////////////////////////////////////////
	//界面操作，可独立于线程
	LRESULT WinsLocal();					//重新排列桌面窗口
	LRESULT GlobleClique();						//全局聚类
	LRESULT ShowNextClique();						//全局聚类
	LRESULT ScreenShot();						//测试截图

	//////////////////////////////////////////////////////////////////////////
	//控制函数
	static BOOL StopWork();																						//停止线程工作
	BOOL CommitRecord();																				//向服务器传递信息
	static DWORD WINAPI CommitInNewThread(LPVOID param);																	//在新线程中向服务器传递信息
	static DWORD WINAPI ThreadTranslateMessage(LPVOID param);					//这是这个类分类hook消息并调用处理函数的地方，应当作为ThreadFunc调用之

	///////////////////////////////////////////////////////////////////////////
	//Getters
	BOOL  GetWindowLocals(vector<sHWNDAtt> &result);									//得到HWND和最后记录的位置信息
	BOOL GetHWND(vector<HWND> &result);													//得到所有检测到的HWND

private:
	WinInfoCtl					*m_cWinInfoCtl;					//窗口信息管理器
	CGestureRecognizer		*m_cGesRecognizer;					//手势识别器

//线程控制
private:
	static BOOL m_bWorkFlag;			//该值为真，线程继续运行
	static BOOL m_bStopFlag;				//该值为真，线程停止成功
	HWND m_hSelf;							//FlowWatcher自己的句柄
	list<CPoint> m_lLocus;				//鼠标轨迹
public:
	// 平铺桌面现有窗口
	BOOL OnPlainLayout(void);
	// 开始跟踪鼠标轨迹
	void StartTraceMouse(int x, int y);
	// 终止跟踪鼠标轨迹
	void EndTraceMouse(int x, int y);
	// 正在跟踪鼠标轨迹
	void InTraceMouse(int x, int y);
};
